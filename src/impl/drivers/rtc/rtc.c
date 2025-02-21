/**
 * Real Time Clock (RTC) Implementation
 * NansOS Driver System
 * Copyright (c) 2025 NansStudios
 */

#include "rtc.h"
#include "../port_io/port.h"
#include "../pic/pic.h"

/* RTC IRQ number */
#define RTC_IRQ 8

/* Internal callback */
static rtc_callback_t rtc_callback = 0;

/* Internal RTC time */
static struct RTCTime current_time = {0};

/* Read RTC register */
static uint8_t rtc_read_register(uint8_t reg) {
    port_byte_out(RTC_INDEX_PORT, reg);
    return port_byte_in(RTC_DATA_PORT);
}

/* Write RTC register */
static void rtc_write_register(uint8_t reg, uint8_t value) {
    port_byte_out(RTC_INDEX_PORT, reg);
    port_byte_out(RTC_DATA_PORT, value);
}

uint8_t rtc_bcd_to_bin(uint8_t bcd) {
    return ((bcd & 0xF0) >> 4) * 10 + (bcd & 0x0F);
}

uint8_t rtc_bin_to_bcd(uint8_t bin) {
    return ((bin / 10) << 4) | (bin % 10);
}

int rtc_update_in_progress(void) {
    return rtc_read_register(RTC_STATUS_A) & 0x80;
}

void rtc_read_time(struct RTCTime* time) {
    /* Wait until RTC is not updating */
    while (rtc_update_in_progress());

    /* Read all time values in BCD format */
    time->second = rtc_bcd_to_bin(rtc_read_register(RTC_SECONDS));
    time->minute = rtc_bcd_to_bin(rtc_read_register(RTC_MINUTES));
    time->hour = rtc_bcd_to_bin(rtc_read_register(RTC_HOURS));
    time->day = rtc_bcd_to_bin(rtc_read_register(RTC_DAY));
    time->month = rtc_bcd_to_bin(rtc_read_register(RTC_MONTH));
    time->year = rtc_bcd_to_bin(rtc_read_register(RTC_YEAR)) + 2000;
    time->weekday = rtc_bcd_to_bin(rtc_read_register(RTC_WEEKDAY));
}

void rtc_init(void) {
    uint8_t status;

    /* Read Status Register B */
    status = rtc_read_register(RTC_STATUS_B);

    /* Set 24-hour mode, binary mode */
    status |= 0x02;  /* 24 hour mode */
    status |= 0x04;  /* Binary mode */

    /* Write back Status Register B */
    rtc_write_register(RTC_STATUS_B, status);

    /* Enable RTC interrupt */
    pic_clear_mask(RTC_IRQ);

    /* Read initial time */
    rtc_read_time(&current_time);
}

void rtc_enable_interrupt(void) {
    uint8_t status;

    /* Read Status Register B */
    status = rtc_read_register(RTC_STATUS_B);

    /* Enable periodic interrupt */
    status |= 0x40;

    /* Write back Status Register B */
    rtc_write_register(RTC_STATUS_B, status);

    /* Read Status Register C to clear any pending interrupts */
    rtc_read_register(RTC_STATUS_C);
}

void rtc_disable_interrupt(void) {
    uint8_t status;

    /* Read Status Register B */
    status = rtc_read_register(RTC_STATUS_B);

    /* Disable periodic interrupt */
    status &= ~0x40;

    /* Write back Status Register B */
    rtc_write_register(RTC_STATUS_B, status);
}

void rtc_handler(void) {
    /* Read Status Register C to acknowledge interrupt */
    rtc_read_register(RTC_STATUS_C);

    /* Update current time */
    rtc_read_time(&current_time);

    /* Call callback if registered */
    if (rtc_callback) {
        rtc_callback(&current_time);
    }

    /* Send EOI to PIC */
    pic_send_eoi(RTC_IRQ);
}

void rtc_set_callback(rtc_callback_t callback) {
    rtc_callback = callback;
} 