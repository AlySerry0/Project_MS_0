#include <math.h>
#include <stdio.h>
#include <time.h>

#include "Drivers/Buzzer/Buzzer.h"
#include "Drivers/GPS/gps.h"
#include "Drivers/GSM/gsm.h"
#include "Drivers/LED/led.h"
#include "Drivers/Motor/motor.h"
#include "Drivers/Vibration/Vibration.h"
#include "Wifi/wifi.h"

GpsCoordinates locked_coords;
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

// Shared state and queues
static bool device_locked = false;
static bool is_theft = false;
static QueueHandle_t commandQueue;
static QueueHandle_t alarmQueue;
static SemaphoreHandle_t stateMutex;

typedef enum {
    VIBRATION_ALARM,
    GPS_ALARM
} AlarmType;

typedef struct {
    AlarmType type;
    GpsCoordinates coords;
    int value;
} AlarmEvent;

// Constants
#define VIBRATION_THRESHOLD 800   // Adjust based on your sensor
#define MOVEMENT_THRESHOLD 0.001  // 1 meters in km

// Command message type
typedef struct {
    char command[32];
    void *data;
} CommandMessage;

// Helper functions
double calculate_distance(GpsCoordinates a, GpsCoordinates b) {
    double lat1 = a.latitude;
    double lon1 = a.longitude;
    double lat2 = b.latitude;
    double lon2 = b.longitude;

    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;

    double a_ = pow(sin(dlat / 2), 2) +
                cos(lat1) * cos(lat2) * pow(sin(dlon / 2), 2);
    double c = 2 * atan2(sqrt(a_), sqrt(1 - a_));
    return 6371 * c;
}

// Command handlers
void lock_car() {
    motorLeftBrake();
    motorRightBrake();
    led_on();
    locked_coords = get_current_coords();
    device_locked = true;
    is_theft = false;
}

void unlock_car() {
    motorLeftCoast();
    motorRightCoast();
    led_off();
    device_locked = false;
    is_theft = false;
}

void checkTheft() {
    if (is_theft) {
        char message[100];
        snprintf(message, sizeof(message), "The car is being stolen! Corrdinates: %f, %f", locked_coords.latitude, locked_coords.longitude);
        send_sms(message);
    } else {
        char message[100];
        snprintf(message, sizeof(message), "The car is safe!");
        send_sms(message);
    }
}

void vibration_alarm() {
    is_theft = true;
    char message[100];
    snprintf(message, sizeof(message), "");

    snprintf(message, sizeof(message), "Vibration detected!");
    send_sms(message);
    led_alarm();
    buzzer_tone(2000, 1000);
}

void gps_alarm() {
    is_theft = true;
    char message[100];
    snprintf(message, sizeof(message), "Car is moving!");
    send_sms(message);
    led_alarm();
    buzzer_tone(2000, 1000);
}

// Task definitions
void vWebSocketTask(void *pvParameters) {
    printf("WebSocket task started.\n");

    const TickType_t xDelay = pdMS_TO_TICKS(100);

    while (1) {
        // Debug message to confirm the task is running
        printf("WebSocket task running.\n");

        // Replace with simple test logic
        printf("Simulating message processing...\n");

        vTaskDelay(xDelay);
    }
}

// Update task functions to remove setup calls:
void vSensorTask(void *pvParameters) {
    printf("Starting Sensor task...\n");
    const TickType_t xDelay = pdMS_TO_TICKS(100);

    while (1) {
        bool is_locked;
        xSemaphoreTake(stateMutex, portMAX_DELAY);
        is_locked = device_locked;
        xSemaphoreGive(stateMutex);

        if (is_locked) {
            // Check vibration
            int vibration_value = readVibrationSensorValue();
            printf("Vibration: %d\n", vibration_value);
            if (vibration_value > VIBRATION_THRESHOLD) {
                AlarmEvent evt = {
                    .type = VIBRATION_ALARM,
                    .value = vibration_value};
                xQueueSend(alarmQueue, &evt, portMAX_DELAY);
            }

            // Check GPS position
            GpsCoordinates current_pos = get_current_coords();
            double distance = calculate_distance(locked_coords, current_pos);

            if (distance > MOVEMENT_THRESHOLD) {
                AlarmEvent evt = {
                    .type = GPS_ALARM,
                    .coords = current_pos,
                    .value = (int)(distance * 1000)  // Convert to meters
                };
                xQueueSend(alarmQueue, &evt, portMAX_DELAY);
            }
        }

        vTaskDelay(xDelay);
    }
}

void vAlarmTask(void *pvParameters) {
    printf("Starting Alarm task...\n");
    const TickType_t xDelay = pdMS_TO_TICKS(100);

    while (1) {
        AlarmEvent evt;
        if (xQueueReceive(alarmQueue, &evt, xDelay) == pdTRUE) {
            switch (evt.type) {
                case VIBRATION_ALARM:
                    vibration_alarm();
                    break;
                case GPS_ALARM:
                    gps_alarm();
                    break;
            }
        }
    }
}

void test_wifi(void) {
    printf("Starting WiFi test...\n");

    // Initialize peripherals
    stdio_init_all();
    setup_wifi();

    // Wait for initialization
    sleep_ms(2000);

    while (1) {
        // Check for new messages
        if (has_messages()) {
            receive_message();
            char message[256];
            if (dequeue_message(message, sizeof(message))) {
                printf("Received message: %s\n", message);
            }
        }

        sleep_ms(100);
    }
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    printf("Stack overflow in task: %s\n", pcTaskName);
    while (1) {
    }  // Halt the system for debugging
}

void vApplicationMallocFailedHook(void) {
    printf("Malloc failed! Increase configTOTAL_HEAP_SIZE.\n");
    while (1) {
    }
}

int main() {
    // Initialize system
    stdio_init_all();

    // Initialize all peripherals first
    setup_gps();
    setup_wifi();
    printf("Peripherals initialized\n");
    setup_led();
    setup_gsm();
    buzzer_init();
    initVibrationSensor();

    // Wait for peripherals to stabilize
    sleep_ms(2000);

    // Create synchronization primitives
    commandQueue = xQueueCreate(5, sizeof(CommandMessage));
    alarmQueue = xQueueCreate(5, sizeof(AlarmEvent));
    stateMutex = xSemaphoreCreateMutex();

    // Check if synchronization primitives were created successfully
    if (commandQueue == NULL) {
        printf("Failed to create commandQueue.\n");
    }
    if (alarmQueue == NULL) {
        printf("Failed to create alarmQueue.\n");
    }
    if (stateMutex == NULL) {
        printf("Failed to create stateMutex.\n");
    }

    if (xTaskCreate(vWebSocketTask, "WebSocket", 512, NULL, 5, NULL) != pdPASS) {
        printf("Failed to create WebSocket task.\n");
    } else {
        printf("WebSocket task created successfully.\n");
    }
    printf("Creating Sensor task...\n");
    if (xTaskCreate(vSensorTask, "Sensors", 512, NULL, 2, NULL) != pdPASS) {
        printf("Failed to create Sensor task.\n");
    } else {
        printf("Sensor task created successfully.\n");
    }
    if (xTaskCreate(vAlarmTask, "Alarm", 512, NULL, 3, NULL) != pdPASS) {
        printf("Failed to create Alarm task.\n");
    } else {
        printf("Alarm task created successfully.\n");
    }

    printf("Starting FreeRTOS scheduler...\n");
    vTaskStartScheduler();
    printf("Scheduler failed to start.\n");

    while (1) {
    }
    return 0;
}