#include <stdio.h>

#include "score/console/console.h"
#include "score/string/string.h"

#include "weather/weather.h"

int main() {
	printf("Sweather!\n");

	Weather_Retriever weather_retriever = {0};
	if(!weather_init_retriever(&weather_retriever)) {
		printf("Failed to initialize weather-retriever.\n");
		return -1;
	}

	while(true) {
		printf("Type the name of a city.\n");

		char buf[32];
		if(!score_console_read(&buf[0], 32)) {
			break;
		}

		{ /* Check if we should exit the program. */
			if(score_string_compare(&buf[0], "q", false) == Score_String_Compare_Result_Equal) {
				break;
			}
			if(score_string_compare(&buf[0], "exit", false) == Score_String_Compare_Result_Equal) {
				break;
			}
			if(score_string_compare(&buf[0], "quit", false) == Score_String_Compare_Result_Equal) {
				break;
			}
		}

		printf("Read '%s'.\n", buf);

		Score_String_Compare_Result compare_result = score_string_compare(&buf[0], "abc", true);
		if(compare_result != Score_String_Compare_Result_Equal) {
			printf("Not equal. Result: %i.\n", compare_result);
			continue;
		}

		printf("Equal!\n");

		Geographical_Coordinate coords = {0};
		coords.latitude = 55.476168;
		coords.longitude= 13.494602;

		Open_Meteo_Data_Request weather_data_request = {
			.coordinate = coords,
			.forecast_days = 1,
			.current_flags = 0,
			.hourly_flags = OPENMETEO_HOURLY_FLAG_RAIN,
			.daily_flags = 0,
		};

		Weather_Report report;
		if(weather_get_report(&weather_retriever, &weather_data_request, &report)) {

		}
	}

	printf("Program shutting down ...\n");

	weather_dispose_retriever(&weather_retriever);

	return 0;
}