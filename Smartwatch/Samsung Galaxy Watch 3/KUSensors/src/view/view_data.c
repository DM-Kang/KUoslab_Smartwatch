/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd
 *
 * Licensed under the Flora License, Version 1.1 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <sensors.h>
#include "view/view_data.h"
#include "view/view_indicator.h"
#include "view/view_main.h"
#include "view/view_common.h"
#include "view_chart.h"
#include "data.h"
#include "view_defines.h"
// added by dmkang
#include "mqtt.h"
#include <system_info.h>

typedef struct _sensor_text_format {
	char *param_name[MAX_VALUES_PER_SENSOR];
	char *value_format[MAX_VALUES_PER_SENSOR];
} sensor_text_format_t;


static struct view_info {
	Elm_Object_Item *naviframe_item;
	int position;
	int values_per_sensor;
	sensor_text_format_t text_formats[SENSOR_COUNT];
} s_info = {
		.naviframe_item = NULL,
		.position = 0,
		.values_per_sensor = 0,
		.text_formats = {
				{
						{"x", "y", "z", ""}, /* acceleration */
						{"% 5.2fm/s²", "% 5.2fm/s²", "% 5.2fm/s²", ""},
				},
				{
						{"x", "y", "z", ""}, /* gravity */
						{"% 5.2fm/s²", "% 5.2fm/s²", "% 5.2fm/s²", ""},
				},
				{
						{"x", "y", "z", ""}, /* linear acc */
						{"% 5.2fm/s²", "% 5.2fm/s²", "% 5.2fm/s²", ""},
				},
				{
						{"x", "y", "z", ""}, /* magnetic */
						{"% 7.2fµT", "% 7.2fµT", "% 7.2fµT", ""},
				},
				{
						{"x", "y", "z", "v"}, /* rot vector */
						{"% 5.2f", "% 5.2f", "% 5.2f", "% 5.2f"},
				},
				{
						{"x", "y", "z", ""}, /* orientation */
						{"% 5.2f", "% 5.2f", "% 5.2f", ""},
				},
				{
						{"x", "y", "z", ""}, /* gyroscope */
						{"% 7.2f°/s", "% 7.2f°/s", "% 7.2f°/s", ""},
				},
				{
						{"lux", "", "", ""}, /* light */
						{"% 4.0fLux", "", "", ""},
				},
				{
						{"proximity", "", "", ""}, /* proximity */
						{"% 1.0f", "", "", ""},
				},
				{
						{"hPa", "", "", ""}, /* pressure */
						{"% 6.2fhPa", "", "", ""},
				},
				{
						{"UV", "", "", ""}, /* UV */
						{"% 4.2f", "", "", ""},
				},
				{
						{"temperature", "", "", ""}, /* temp */
						{"% 5.2f°", "", "", ""},
				},
				{
						{"humidity", "", "", ""}, /* humidity */
						{"", "", "", ""},
				},
				{
						//{"<con>Heart Rate</con>", "<con>Peek-to-Peek</con>", "", ""}, /* hrm */
						{"HeartRate", "P2P", "", ""}, /* hrm */
						{"% 6.2f", "%4.0fms", "", ""},
				},
		},
};

static void _set_values_per_sensor(int count);

/**
 * @brief Creates the data view layout.
 * @param parent The parent naviframe.
 * @return true on success, false on fail.
 */
bool view_data_create(Evas_Object *parent)
{
	s_info.naviframe_item = view_create_layout(parent, EDJ_DATA, GRP_DATA);
	if (!s_info.naviframe_item) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] s_info.layout == NULL", __FILE__, __LINE__);
		return false;
	}

	if (!view_chart_create(elm_object_item_content_get(s_info.naviframe_item))) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] function view_chart_create() failed", __FILE__, __LINE__);
		return false;
	}

	return true;
}

/**
 * @brief Displays the data view.
 */
void view_data_show(void)
{
	float min = 0;
	float max = 0;

	data_set_selected_sensor(s_info.position);
	data_get_sensor_data(s_info.position);
	data_get_sensor_range(s_info.position, &min, &max);
	view_chart_prepare(s_info.values_per_sensor, min, max);

	elm_naviframe_item_promote(s_info.naviframe_item);
}

/**
 * @brief Stops the sensor listeners. Should be called when to data view is going to be hid
 */
void view_data_hide(void)
{
	data_stop_sensor();
}

/**
 * @brief Function invoked by the rotary cb. Changes the position of the indicator displayed in the data view.
 * @param is_clockwise Rotation direction.
 */
void view_data_rotary_pos_changed(bool is_clockwise)
{
	float min = 0;
	float max = 0;

	if ((is_clockwise && s_info.position == SENSOR_COUNT - 1) ||
			(!is_clockwise && s_info.position == 0))
		return;


	view_set_selected_sensor(is_clockwise, &s_info.position);
	view_set_indicator_dot(s_info.naviframe_item, s_info.position);

	data_set_selected_sensor(s_info.position);
	data_get_sensor_data(s_info.position);
	data_get_sensor_range(s_info.position, &min, &max);

	view_chart_prepare(s_info.values_per_sensor, min, max);
}
/**
 * @brief Invoked by a sensor's listener callback. Updates the displayed values and the chart.
 * @param count Number of values provided by the current sensor.
 * @param values The values array.
 */
void view_data_update_sensor_values(int count, float *values)
{
	static int current_sensor_count = 0;

	int i;
	char name_part[NAME_MAX];
	char value_part[NAME_MAX];
	char name_string[NAME_MAX];
	char value_string[NAME_MAX];
	// added by dmkang
	char mqtt_string[1024] = {0,};
	char mqtt_string_buf[1024] = {0,};
	char temp[128] = {0,};
	static int transactionId = 0;
	time_t timestamp = 0;


	if (current_sensor_count != count) {
		current_sensor_count = count;
		_set_values_per_sensor(count);
	}

	view_chart_add_data(values);

	if (s_info.position == SENSOR_HRM)
		values[1] = values[2];

	// added by dmkang
	timestamp = time(NULL);
	snprintf(mqtt_string, sizeof(mqtt_string),
			"{\"transaction_id\":%d,"
			"\"timestamp\":%ld,"
			"\"sensor_type\":%d,"
			"\"sensor_data\":{", transactionId++, timestamp, s_info.position);

	for (i = 0; i < MAX_VALUES_PER_SENSOR; ++i) {
		snprintf(name_part, NAME_MAX, "%s%d", PART_DATA_PARAM_NAME, i);
		snprintf(value_part, NAME_MAX, "%s%d", PART_DATA_PARAM_VALUE, i);

		if (strlen(s_info.text_formats[s_info.position].param_name[i]) > 0) {
			snprintf(name_string, NAME_MAX, "%s=", s_info.text_formats[s_info.position].param_name[i]);
			// added by dmkang
			memcpy(mqtt_string_buf, mqtt_string, sizeof(mqtt_string_buf));
			snprintf(temp, sizeof(temp), "\"%s\":%0.2f,", s_info.text_formats[s_info.position].param_name[i], values[i]);
			snprintf(mqtt_string, sizeof(mqtt_string), "%s%s", mqtt_string_buf, temp);
			memset(temp, 0, sizeof(temp));
			memset(mqtt_string_buf, 0, sizeof(mqtt_string_buf));
		}
		else {
			snprintf(name_string, NAME_MAX, "");
		}

		snprintf(value_string, NAME_MAX, s_info.text_formats[s_info.position].value_format[i], values[i]);

		elm_layout_text_set(elm_object_item_content_get(s_info.naviframe_item), name_part, name_string);
		elm_layout_text_set(elm_object_item_content_get(s_info.naviframe_item), value_part, value_string);
	}

	// added by dmkang
#if 0
	memcpy(mqtt_string_buf, mqtt_string, sizeof(mqtt_string_buf));
	snprintf(temp, sizeof(temp), "\"BP\":{\"high\":120,\"low\":80},");
	snprintf(mqtt_string, sizeof(mqtt_string), "%s%s", mqtt_string_buf, temp);
#endif

	mqtt_string[strlen(mqtt_string)-1] = '}';
	mqtt_string[strlen(mqtt_string)] = '}';

	//TODO - porting JSON parser lib.
	mqttPublish(mqtt_string);
}

/**
 * @brief Updates the stored number of sensor values.
 * @param count The new value count.
 */
static void _set_values_per_sensor(int count)
{
	Evas_Object *layout = elm_object_item_content_get(s_info.naviframe_item);
	Edje_Message_Int msg = {count, };
	edje_object_message_send(elm_layout_edje_get(layout), EDJE_MESSAGE_INT, MSG_ID_DATA_DISPLAY_CHANGED, &msg);

	s_info.values_per_sensor = count;
}
