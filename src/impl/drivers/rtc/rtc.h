/**
 * Real Time Clock (RTC) Driver
 * NansOS Driver System
 * Copyright (c) 2025 NansStudios
 */

#pragma once
#include <stdint.h>

/* RTC ports */
#define RTC_INDEX_PORT     0x70
#define RTC_DATA_PORT      0x71

/* RTC registers */
#define RTC_SECONDS        0x00
#define RTC_MINUTES        0x02
#define RTC_HOURS          0x04
#define RTC_WEEKDAY        0x06
#define RTC_DAY            0x07
#define RTC_MONTH          0x08
#define RTC_YEAR           0x09
#define RTC_STATUS_A       0x0A
#define RTC_STATUS_B       0x0B
#define RTC_STATUS_C       0x0C
#define RTC_STATUS_D       0x0D

/* Time structure */
struct RTCTime {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;
    uint8_t weekday;
};

/* Function declarations */
void rtc_init(void);                          /* Initialize RTC */
void rtc_read_time(struct RTCTime* time);     /* Read current time */
int rtc_update_in_progress(void);             /* Check if RTC update in progress */

/* Time format conversion functions */
uint8_t rtc_bcd_to_bin(uint8_t bcd);         /* Convert BCD to binary */
uint8_t rtc_bin_to_bcd(uint8_t bin);         /* Convert binary to BCD */

/* RTC interrupt handling */
void rtc_enable_interrupt(void);              /* Enable RTC interrupt */
void rtc_disable_interrupt(void);             /* Disable RTC interrupt */

/* Callback type for RTC updates */
typedef void (*rtc_callback_t)(struct RTCTime* time);

/* Set callback for RTC updates */
void rtc_set_callback(rtc_callback_t callback); 