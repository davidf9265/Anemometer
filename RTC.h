#ifndef RTC_H
#define RTC_H

#define RTC_ADDRESS 0x68 //B1101000

#include <Arduino.h>
#include <Wire.h>

byte get_RTC_data(byte reg_address, String name, bool debug);

byte get_RTC_secs();

byte get_RTC_mins();

byte get_RTC_hour();

bool set_RTC_time(int date, int month, int day, int hour);

bool set_RTC_alarm();

void flush_status();

#endif