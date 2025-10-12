#include "date.h"

#include <stdio.h>

bool json_to_date_time(const SCore_JSON_Object *json_object, Date_Time *out_date_time) {
    char *time_format = NULL;
    Date_Time date_time;

    if(!score_json_is_string(json_object)) {
        return false;
    }

    assert(score_json_as_string(json_object, &time_format));

    {
        uint32_t year = 0;
        uint32_t month = 0;
        uint32_t day = 0;

        uint32_t hour = 0;
        uint32_t minute = 0;

        int32_t result = sscanf(time_format,
            "%u-%u-%u"
            "T"
            "%u:%u",
            &year, &month, &day,
            &hour, &minute
        );
        if(result == 0) {
            return false;
        }

        date_time.date.year     = (uint16_t)year;
        date_time.date.month    = (uint8_t)month;
        date_time.date.day      = (uint8_t)day;
        date_time.time.hour     = (uint8_t)hour;
        date_time.time.minute   = (uint8_t)minute;
    }

    *out_date_time = date_time;

    return true;
}

void print_date(const Date date) {
    printf("Date: %u-%u-%u.\n", date.year, date.month, date.day);
}

void print_time(const Time time) {
    printf("Time: %02u:%02u.\n", time.hour, time.minute);
}