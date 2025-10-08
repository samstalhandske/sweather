#include <stdio.h>

#include "score/types/types.h"
#include "score/console/console.h"
#include "score/string/string.h"

#include "weather/weather.h"

int main() {
	Weather_Retriever weather_retriever = {0};

	printf("Sweather!\n");

	if(!weather_init_retriever(&weather_retriever)) {
		printf("Failed to initialize weather-retriever.\n");
		return -1;
	}

	while(true) {
		printf("Type the name of a city.\n");

		{ /* Read from stdin. */
			char console_read_buffer[32];


			if(!score_console_read_from_stdin(&console_read_buffer[0], sizeof(console_read_buffer))) {
				break;
			}

			{ /* Check if we should exit the program. */
				if(score_string_compare(&console_read_buffer[0], "q", false) == SCore_String_Compare_Result_Equal) {
					break;
				}
				if(score_string_compare(&console_read_buffer[0], "exit", false) == SCore_String_Compare_Result_Equal) {
					break;
				}
				if(score_string_compare(&console_read_buffer[0], "quit", false) == SCore_String_Compare_Result_Equal) {
					break;
				}
			}

			printf("Read '%s'.\n", console_read_buffer);

			{ /* TEMP: SS - Compare against 'abc'. Might be city-name or something later. */
				SCore_String_Compare_Result compare_result = score_string_compare(&console_read_buffer[0], "abc", true);
				if(compare_result != SCore_String_Compare_Result_Equal) {
					printf("Not equal. Result: %i.\n", compare_result);
					continue;
				}
			}
		}

		printf("Equal!\n");

		{ /* Build a 'Open_Meteo_Data_Request' and send it off. */
			Open_Meteo_Data_Request weather_data_request = {0};

			weather_data_request.coordinate.latitude = 55.476168;
			weather_data_request.coordinate.longitude= 13.494602;
			weather_data_request.past_days = 0;
			weather_data_request.forecast_days = 2;
			weather_data_request.temperature_unit = Open_Meteo_Data_Request_Temperature_Unit_Celsius;
			weather_data_request.current_flags = 0;
			weather_data_request.hourly_flags = OPENMETEO_HOURLY_FLAG_RAIN | OPENMETEO_HOURLY_FLAG_TEMPERATURE_2M;
			weather_data_request.daily_flags = 0;

			{
				Weather_Report report;
				if(weather_get_report(&weather_retriever, &weather_data_request, &report)) {
					printf("Got a report!\n");
				}
				else {
					printf("Failed to get a report!\n");
				}
			}
		}
	}

	printf("Program shutting down ...\n");

	weather_dispose_retriever(&weather_retriever);

	return 0;
}