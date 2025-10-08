#include "date.h"

#include <stdio.h>

bool json_to_date_time(const SCore_JSON_Object *json_object, Date_Time *out_date_time) {
    if(!score_json_is_string(json_object)) {
        return false;
    }

    (void)out_date_time;

    return true;
}

void print_date(const Date date) {
    printf("Date: %u-%u-%u.\n", date.year, date.month, date.day);
}

void print_time(const Time time) {
    printf("Time: %02u:%02u.\n", time.hour, time.minute);
}