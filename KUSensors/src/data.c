/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <Eina.h>
#include <app_preference.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sensor.h>
#include <sensors.h>
#include "data.h"
#include "view_defines.h"

#define MAX_GYRO_VALUE 571.0
#define MAX_HRM_VALUE 220.0
// modified by dmkang
//#define LISTENER_TIMEOUT 1000
#define LISTENER_TIMEOUT 1000
#define LISTENER_TIMEOUT_FINAL ((LISTENER_TIMEOUT != 0) ? LISTENER_TIMEOUT : 100)

typedef struct _sensor_data {
	sensor_h handle;
	sensor_listener_h listener;
} sensor_data_t;

static struct data_info {
	sensor_type_e current_sensor;
	sensor_data_t sensors[SENSOR_COUNT];
	Update_Sensor_Values_Cb sensor_update_cb;
	Ecore_Timer *timer;
} s_info = {
	.sensors = { {0}, },
	.current_sensor = 0,
	.sensor_update_cb = NULL,
	.timer = NULL,
};

static void _initialize_sensors(void);
static void _sensor_event_cb(sensor_h sensor, sensor_event_s *event, void *data);
static void _timer_stop(void);

/**
 * @brief Function that initializes the data module.
 * @param sensor_update_cb Callback used to update the data displayed in the data view.
 * @return True on success or false on error.
 */
bool data_initialize(Update_Sensor_Values_Cb sensor_update_cb)
{
	_initialize_sensors();
	s_info.sensor_update_cb = sensor_update_cb;

	return true;
}

/**
 * @brief Function used to destroy the sensor listeners. Should be invoked when the app is terminated.
 */
void data_finalize(void)
{
	int ret = SENSOR_ERROR_NONE;
	int i;

	for (i = 0; i < SENSOR_COUNT; ++i) {
		ret = sensor_destroy_listener(s_info.sensors[i].listener);
		if (ret != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] sensor_get_default_sensor() error: %s", __FILE__, __LINE__, get_error_message(ret));
			continue;
		}
	}
}

/**
 * @brief Checks if the given sensor type is supported by the device.
 * @param type The sensor's type.
 * @return true - sensor is supported, false - sensor is not supported.
 */
bool data_get_sensor_support(sensor_type_e type)
{
	bool supported = false;
	int ret;

	ret = sensor_is_supported(type, &supported);
	if (ret != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] sensor_is_supported() error: %s", __FILE__, __LINE__, get_error_message(ret));
		return false;
	}

	return supported;
}

/**
 * @brief Checks the minimal and maximal values that the given sensor supports.
 * @param type The sensor type.
 * @param[out] min The minimal value.
 * @param[out] max The maximal value.
 */
void data_get_sensor_range(sensor_type_e type, float *min, float *max)
{
	*min = 0;
	*max = 0;

	if (type == SENSOR_GYROSCOPE) {
		*min = -MAX_GYRO_VALUE;
		*max = MAX_GYRO_VALUE;
		return;
	} else if (type == SENSOR_HRM) {
		*min = 0.0;
		*max = MAX_HRM_VALUE;
		return;
	}

	int ret = sensor_get_min_range(s_info.sensors[type].handle, min);
	if (ret != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] sensor_get_min_range() error: %s", __FILE__, __LINE__, get_error_message(ret));
		return;
	}

	ret = sensor_get_max_range(s_info.sensors[type].handle, max);
	if (ret != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] sensor_get_min_range() error: %s", __FILE__, __LINE__, get_error_message(ret));
		return;
	}
}

/**
 * @brief Checks the sensor's resolution.
 * @param type The sensor type.
 * @return The sensor's resolution.
 */
float data_get_sensor_resolution(sensor_type_e type)
{
	int ret;
	float resolution = 0.0;

	ret = sensor_get_resolution(s_info.sensors[type].handle, &resolution);
	if (ret != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] sensor_get_resolution() error: %s", __FILE__, __LINE__, get_error_message(ret));
		return 0.0;
	}

	return resolution;
}

/**
 * @brief Checks the sensor's vendor.
 * @param type The sensor type.
 * @return The sensor's vendor (the returned value should be freed).
 */
char *data_get_sensor_vendor(sensor_type_e type)
{
	int ret;
	char *vendor = NULL;

	ret = sensor_get_vendor(s_info.sensors[type].handle, &vendor);
	if (ret != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] sensor_get_vendor() error: %s", __FILE__, __LINE__, get_error_message(ret));
		return strdup("");
	}

	return vendor;
}

/**
 * @brief Stops the current listener.
 */
void data_stop_sensor(void)
{
	int ret = sensor_listener_stop(s_info.sensors[s_info.current_sensor].listener);
	if (ret != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] sensor_listener_stop() error: %s", __FILE__, __LINE__, get_error_message(ret));
		return;
	}

	_timer_stop();
}

/**
 * @brief Sets the current sensor.
 * @param type The sensor to be selected as the current one.
 */
void data_set_selected_sensor(sensor_type_e type)
{
	int ret;
	data_stop_sensor();

	ret = sensor_listener_start(s_info.sensors[type].listener);
	if (ret != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] sensor_listener_start() error: %s", __FILE__, __LINE__, get_error_message(ret));
		return;
	}

	s_info.current_sensor = type;
}

/**
 * @brief Initializes the sensors. The function sets the handles and creates a listener for each of the sensors.
 */
static void _initialize_sensors(void)
{
	int ret;
	int i;
	for (i = 0; i < SENSOR_COUNT; ++i) {

		ret = sensor_get_default_sensor(i, &s_info.sensors[i].handle);
		if (ret != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] sensor_get_default_sensor() error: %s", __FILE__, __LINE__, get_error_message(ret));
			continue;
		}

		ret = sensor_create_listener(s_info.sensors[i].handle, &s_info.sensors[i].listener);
		if (ret != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] sensor_create_listener() error: %s", __FILE__, __LINE__, get_error_message(ret));
			continue;
		}

		ret = sensor_listener_set_event_cb(s_info.sensors[i].listener, LISTENER_TIMEOUT, _sensor_event_cb, (void *)i);
		if (ret != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] sensor_listener_set_event_cb() error: %s", __FILE__, __LINE__, get_error_message(ret));
			continue;
		}
	}
}

/**
 * @brief A ecore timer callback used when the proximity timer is the current one. This is needed because the proximity sensor works differently than most of the other sensors.
 * @param data
 * @return
 */
static Eina_Bool _proximity_timer_cb(void *data)
{
	float value = (int)data;
	s_info.sensor_update_cb(1, &value);

	return ECORE_CALLBACK_RENEW;
}

/**
 * @brief Function used to delete the proximity timer.
 */
static void _timer_stop(void)
{
	if (s_info.timer) {
		ecore_timer_del(s_info.timer);
		s_info.timer = NULL;
	}
}

/**
 * @brief Function used to create the proximity timer.
 * @param value Value provided by the proximity sensor.
 */
static void _timer_start(int value)
{
	//s_info.timer = ecore_timer_add(LISTENER_TIMEOUT_FINAL / 1000.0, _proximity_timer_cb, (void *)value);
	// added by dmkang
	s_info.timer = ecore_timer_add(LISTENER_TIMEOUT_FINAL / 1000.0, _proximity_timer_cb, (void *)value);

}

/**
 * @brief Function used when the hrm sensor is the current one. The hrm sensor works differently than most of the other sensors.
 * @param event The event object storing the sensor data.
 */
static void _set_hrm_values(sensor_event_s *event)
{
	static int timeout = 0;
	static int draw_phase = 0;
	float min = 0;
	float max = 0;

	data_get_sensor_range(SENSOR_HRM, &min, &max);

	event->value_count = 2;

	if (timeout >= (int)event->values[2]) {
		timeout = 0;
		draw_phase = 2;
	} else {
		timeout += LISTENER_TIMEOUT_FINAL;
	}

	if (draw_phase == 2) {
		event->values[1] = max;
		draw_phase--;
	} else if (draw_phase == 1) {
		event->values[1] = (max + min) / 4.0;
		draw_phase--;
	} else {
		event->values[1] = (max + min) / 2.0;
	}
}

/**
 * @brief Reads the data provided by the given sensor.
 * @param type The sensor to read the data from.
 */
void data_get_sensor_data(sensor_type_e type)
{
	sensor_event_s event;

	sensor_listener_read_data(s_info.sensors[s_info.current_sensor].listener, &event);
	_timer_stop();

	if (type == SENSOR_HRM)
		_set_hrm_values(&event);
	else if (type == SENSOR_PRESSURE)
		event.value_count = 1;
	else if (type == SENSOR_PROXIMITY)
		_timer_start((int)event.values[0]);

	s_info.sensor_update_cb(event.value_count, &event.values[0]);
}

/**
 * @brief Callback invoked by a sensor's listener.
 * @param sensor The sensor's handle.
 * @param event The event data.
 * @param data The user data.
 */
static void _sensor_event_cb(sensor_h sensor, sensor_event_s *event, void *data)
{
	sensor_type_e type = (sensor_type_e)data;

	_timer_stop();

	if (type == SENSOR_HRM)
		_set_hrm_values(event);
	else if (type == SENSOR_PRESSURE)
		event->value_count = 1;
	else if (type == SENSOR_PROXIMITY)
		_timer_start((int)event->values[0]);

	s_info.sensor_update_cb(event->value_count, &event->values[0]);
}
