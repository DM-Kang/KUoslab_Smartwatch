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
#include "view/view_information.h"
#include "view/view_indicator.h"
#include "view/view_main.h"
#include "view/view_common.h"
#include "view.h"
#include "data.h"
#include "view_defines.h"

#define FORMAT_INFO_SUPPORTED  "supported: <value>%s</value>"
#define FORMAT_INFO_RANGE      "range: <value>%.2f - %.2f</value>"
#define FORMAT_INFO_RESOLUTION "resolution: <value>%.2f</value>"
#define FORMAT_INFO_VENDOR     "vendor: <value>%s</value>"

static struct view_info {
	Elm_Object_Item *naviframe_item;
	int position;
} s_info = {
	.naviframe_item = NULL,
	.position = 0,
};

static void _update_information(void);
static void _page_switch_button_clicked_cb(void *data, Evas_Object *obj, const char *emission, const char *source);

/**
 * @brief Creates the information view's layout.
 * @param parent The parent naviframe.
 * @return true on success, false on fail.
 */
bool view_information_create(Evas_Object *parent)
{
	s_info.naviframe_item = view_create_layout(parent, EDJ_INFORMATION, GRP_INFORMATION);
	if (!s_info.naviframe_item) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] s_info.layout == NULL", __FILE__, __LINE__);
		return false;
	}

	elm_layout_signal_callback_add(elm_object_item_content_get(s_info.naviframe_item), SIGNAL_NEXT_CLICKED, "", _page_switch_button_clicked_cb, (void *)false);
	elm_layout_signal_callback_add(elm_object_item_content_get(s_info.naviframe_item), SIGNAL_PREV_CLICKED, "", _page_switch_button_clicked_cb, (void *)true);

	_update_information();
	return true;
}

/**
 * @brief Displays the information view.
 */
void view_information_show(void)
{
	elm_naviframe_item_promote(s_info.naviframe_item);
}

/**
 * @brief Function invoked by the rotary cb. Changes the position of the indicator displayed in the information view.
 * @param is_clockwise Rotation direction.
 */
void view_information_rotary_pos_changed(bool is_clockwise)
{
	view_set_selected_sensor(is_clockwise, &s_info.position);
	view_set_indicator_dot(s_info.naviframe_item, s_info.position);
	_update_information();
}

/**
 * @brief Sets the text displayed by the given part.
 * @param part A text part.
 * @param text The text that will be displayed by the given part.
 */
static inline void _set_info_text(char *part, char *text)
{
	Evas_Object *layout = elm_object_item_content_get(s_info.naviframe_item);
	elm_layout_text_set(layout, part, text);
}

/**
 * @brief Sets the text displayed in the 'supported' filed
 */
static inline void _set_info_supported(void)
{
	char buf[MAX_INPUT] = {0,};
	bool supported = data_get_sensor_support(s_info.position);
	snprintf(buf, MAX_INPUT, FORMAT_INFO_SUPPORTED, supported ? "YES" : "NO");
	_set_info_text(PART_INFO_SUPPORTED, buf);
}

/**
 * @brief Sets the text displayed in the 'range' filed
 */
static inline void _set_info_range(void)
{
	char buf[MAX_INPUT] = {0,};
	float min = 0;
	float max = 0;
	data_get_sensor_range(s_info.position, &min, &max);
	snprintf(buf, MAX_INPUT, FORMAT_INFO_RANGE, min, max);
	_set_info_text(PART_INFO_RANGE, buf);
}

/**
 * @brief Sets the text displayed in the 'resolution' filed
 */
static inline void _set_info_resoultion(void)
{
	char buf[MAX_INPUT] = {0,};
	float res = data_get_sensor_resolution(s_info.position);
	snprintf(buf, MAX_INPUT, FORMAT_INFO_RESOLUTION, res);
	_set_info_text(PART_INFO_RES, buf);
}

/**
 * @brief Sets the text displayed in the 'vendor' filed
 */
static inline void _set_info_vendor(void)
{
	char buf[MAX_INPUT] = {0,};
	char *vendor = data_get_sensor_vendor(s_info.position);
	snprintf(buf, MAX_INPUT, FORMAT_INFO_VENDOR, vendor);
	_set_info_text(PART_INFO_VENDOR, buf);
	free(vendor);
}

/**
 * @brief Sets the text displayed in the information field.
 */
static void _update_information(void)
{
	_set_info_supported();
	_set_info_range();
	_set_info_resoultion();
	_set_info_vendor();
}

/**
 * @brief Callback invoked when the 'next' or 'previous' buttons are clicked.
 * @param data The user data.
 * @param obj The layout object.
 * @param emission The emitted signal.
 * @param source The signal's source.
 */
static void _page_switch_button_clicked_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	view_information_rotary_pos_changed((bool)data);
}
