#ifndef GSM_H
#define GSM_H

#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <string.h>
#include <stdio.h>

// Define UART pins and settings for the SIM900 module
#define GSM_UART_ID uart1
#define GSM_TX_PIN 4 // Pico TX pin connected to SIM900 RX
#define GSM_RX_PIN 5 // Pico RX pin connected to SIM900 TX
#define GSM_BAUDRATE 9600

// Function prototypes
void setup_gsm(void);
void send_sms(const char *phone_number, const char *message);
void gsm_send_command(const char *command, float sleep_time);
void read_sms(char *sms);

#endif // GSM_H