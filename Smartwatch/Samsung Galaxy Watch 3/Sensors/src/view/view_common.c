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
#include "view/view_common.h"
#include "view/view_indicator.h"
#include "data.h"
#include "view_defines.h"

/**
 * @brief Creates a layout object with an indicator object as a swallow. The new layout is added to the parent naviframe widget.
 * @param parent The parent naviframe.
 * @param file_name The edc file to use.
 * @param group_name The group to use.
 * @return A naviframe item containing the created layout.
 */
Elm_Object_Item *view_create_layout(Evas_Object *parent, char *file_name, char *group_name)
{
	Evas_Object *indicator = NULL;
	Elm_Object_Item *item = NULL;
	char *edj_path = NULL;
	Evas_Object *layout = elm_layout_add(parent);
	if (!layout) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] s_info.layout == NULL", __FILE__, __LINE__);
		return NULL;
	}

	indicator = view_indicator_create(layout);
	if (!indicator) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] indicator == NULL", __FILE__, __LINE__);
		return NULL;
	}

	edj_path = data_create_resource_path(file_name);
	if (!elm_layout_file_set(layout, edj_path, group_name)) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] elm_layout_file_set() failed", __FILE__, __LINE__);
		return NULL;
	}

	elm_layout_content_set(layout, PART_INDICATOR, indicator);
	item = elm_naviframe_item_simple_push(parent, layout);
	if (!item) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] item == NULL", __FILE__, __LINE__);
		return NULL;
	}

	view_set_indicator_dot(item, 0);
	evas_object_show(layout);
	return item;
}

/**
 * @brief Sets the indicator's highlighted dot and changes the displayed title.
 * @param navi_item The naviframe containing a layout with the indicator.
 * @param position The position the indicator will be set to.
 */
void view_set_indicator_dot(Elm_Object_Item *navi_item, int position)
{
	Edje_Message_Int msg = {0, };
	Evas_Object *layout = elm_object_item_content_get(navi_item);
	Evas_Object *indicator = elm_layout_content_get(layout, PART_INDICATOR);
	char *titles[SENSOR_COUNT] = {
		"ACCELEROMETER",
		"GRAVITY",
		"LINEAR ACCELERATION",
		"MAGNETIC",
		"ROTATION VECTOR",
		"ORIENTATION",
		"GYROSCOPE",
		"LIGHT",
		"PROXIMITY",
		"PRESSURE",
		"ULTRAVIOLET",
		"TEMPERATURE",
		"HUMIDITY",
		"HRM",
	};

	msg.val = position;

	edje_object_message_send(elm_layout_edje_get(indicator), EDJE_MESSAGE_INT, MSG_ID_INDICATOR_CHANGED, &msg);
	elm_layout_text_set(indicator, PART_TITLE, titles[position]);
}

/**
 * @brief Increases or decreases the position indicated by an indicator.
 * @param is_clockwise true - increase, false decrease.
 * @param pos Pointer to the position that will be changed.
 */
void view_set_selected_sensor(bool is_clockwise, int *pos)
{
	if (is_clockwise && *pos < SENSOR_COUNT - 1)
		(*pos)++;
	else if (!is_clockwise && *pos > 0)
		(*pos)--;
}

/**
 * @brief Creates path to the given resource file by concatenation of the basic resource path and the given file_name.
 * @param[file_name]: File name or path relative to the resource directory.
 * @return: The absolute path to the resource with given file_name is returned.
 */
char *data_create_resource_path(const char *file_name)
{
	static char res_path_buff[PATH_MAX] = {0,};
	char *res_path = NULL;

	res_path = app_get_resource_path();
	if (res_path == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get resource path.");
		return NULL;
	}

	snprintf(res_path_buff, PATH_MAX, "%s%s", res_path, file_name);
	free(res_path);

	return &res_path_buff[0];
}
