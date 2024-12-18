#ifndef GSM_H
#define GSM_H

#include <stdio.h>
#include <string.h>

#include "hardware/uart.h"
#include "pico/stdlib.h"

// Define UART pins and settings for the SIM900 module
#define GSM_UART_ID uart1
#define GSM_TX_PIN 8  // Pico TX pin connected to SIM900 RX
#define GSM_RX_PIN 9  // Pico RX pin connected to SIM900 TX
#define GSM_BAUDRATE 9600

// Function prototypes
void setup_gsm(void);
void send_sms(const char *message);
void gsm_send_command(const char *command, float sleep_time);
void read_sms(char *sms);
void update_number(char *new_number);

#endif  // GSM_H