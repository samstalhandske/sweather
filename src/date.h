#ifndef DATE_H
#define DATE_H

#include "score/json/json.h"
#include <stdbool.h>

/* TODO: SS - Replace with score/time/time.h? */

typedef struct {
    uint16_t year;
    uint8_t month;
    uint8_t day;
} Date;

typedef struct {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} Time;

typedef struct {
    Date date;
    Time time;
} Date_Time;

bool json_to_date_time(const SCore_JSON_Object *json_object, Date_Time *out_date_time);

void print_date(const Date date);
void print_time(const Time time);

#endif