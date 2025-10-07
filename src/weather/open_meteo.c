#include "open_meteo.h"

#include "score/string/string.h"
#include <assert.h>
#include <stdbool.h>

const char *OPENMETEO_HOURLY_STRINGS[OPENMETEO_HOURLY_FLAG_MAX] = { /* NOTE: SS - ',' required at the moment. */
    "temperature_2m,",
    "relative_humidity_2m,",
    "dewpoint_2m,",
    "apparent_temperature,",
    "precipitation_probability,",
    "precipitation,",
    "rain,",
    "showers,",
    "snowfall,",
    "snow_depth,",
    "weather_code,",
    "pressure_msl,",
    "surface_pressure,",
    "cloud_cover,",
    "cloud_cover_low,",
    "cloud_cover_mid,",
    "cloud_cover_high,",
    "visibility,",
    "evapotranspiration,",
    "et0_fao_evapotranspiration,",
    "vapour_pressure_deficit,",
    "wind_speed_10m,",
    "wind_speed_80m,",
    "wind_speed_120m,",
    "wind_speed_180m,",
    "wind_direction_10m,",
    "wind_direction_80m,",
    "wind_direction_120m,",
    "wind_direction_180m,",
    "wind_gusts_10m,",
    "temperature_80m,",
    "temperature_120m,",
    "temperature_180m,",
    "soil_temperature_0cm,",
    "soil_temperature_6cm,",
    "soil_temperature_18cm,",
    "soil_temperature_54cm,",
    "soil_moisture_0_to_1cm,",
    "soil_moisture_1_to_3cm,",
    "soil_moisture_3_to_9cm,",
    "soil_moisture_9_to_27cm,",
    "soil_moisture_27_to_81cm,"
};

static bool open_meteo_write_if_flag_set(SCore_Buffer_Writer *writer, const uint64_t all_flags, const uint64_t flag, const uint64_t flag_index) {
    if(score_flag64_is_set(all_flags, flag)) {
        if(!score_buffer_writer_write_string(writer, OPENMETEO_HOURLY_STRINGS[flag_index])) {
            return false;
        }
    }

    return true;
}

void open_meteo_write_url_to_writer(SCore_Buffer_Writer *writer, const Open_Meteo_Data_Request *data_request) {
    assert(score_string_snprintf(writer,
        "https://api.open-meteo.com/v1/forecast?latitude=%f&longitude=%f"
        "&forecast_days=%u",
        data_request->coordinate.latitude, data_request->coordinate.longitude,
        data_request->forecast_days
    ));

    { /* Hourly. */
        uint64_t flags = data_request->hourly_flags;

        if(score_has_any_flag_set(flags)) {
            if(!score_buffer_writer_write_string(writer, "&hourly=")) {
                return;
            }

            u64 i;
            for (i = 0; i < OPENMETEO_HOURLY_FLAG_MAX; i++) {
                uint64_t flag = ((uint64_t)1 << i);
                if (!open_meteo_write_if_flag_set(writer, flags, flag, i)) {
                    return;
                }
            }
        }
    }

    { /* Daily. */
        uint64_t flags = data_request->daily_flags;

        if(score_has_any_flag_set(flags)) {
            if(!score_buffer_writer_write_string(writer, "&daily=")) {
                return;
            }
        }


    }

    { /* Current. */
        uint64_t flags = data_request->current_flags;

        if(score_has_any_flag_set(flags)) {
            if(!score_buffer_writer_write_string(writer, "&current=")) {
                return;
            }
        }


    }

}