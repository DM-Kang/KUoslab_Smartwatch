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
#include "view/view_example.h"
#include "view/view_indicator.h"
#include "view/view_main.h"
#include "view/view_common.h"
#include "data.h"
#include "view_defines.h"
#include "gif_animator.h"

typedef struct _page_data {
	char *sensor_name;
	Evas_Object *animation;
} page_data_t;

static struct view_example_info {
	Elm_Object_Item *naviframe_item;
	int position;
	Evas_Object *page_data[SENSOR_COUNT];
} s_info = {
	.naviframe_item = NULL,
	.position = 0,
	.page_data = { 0,},
};

static void _initialize_images(void);
static void _set_displayed_page(void);
static void _unset_displayed_page(void);
static void _play_button_clicked_cb(void *data, Evas_Object *obj, const char *emission, const char *source);

/**
 * @brief Creates the example view layout.
 * @param parent The parent naviframe.
 * @return true on success, false on fail.
 */
bool view_example_create(Evas_Object *parent)
{
	s_info.naviframe_item = view_create_layout(parent, EDJ_EXAMPLE, GRP_EXAMPLE);
	if (!s_info.naviframe_item) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] s_info.layout == NULL", __FILE__, __LINE__);
		return false;
	}

	_initialize_images();
	elm_layout_signal_callback_add(elm_object_item_content_get(s_info.naviframe_item), SIGNAL_BUTTON_CLICKED, "", _play_button_clicked_cb, NULL);


	return true;
}

/**
 * @brief Displays the example view.
 */
void view_example_show(void)
{
	elm_naviframe_item_promote(s_info.naviframe_item);
}

/**
 * @brief Stops the gif animation. Should be invoked when the example view is going to be hid.
 */
void view_example_hide(void)
{
	gif_animator_play_set(s_info.page_data[s_info.position], EINA_FALSE, EINA_FALSE, EINA_TRUE);
}

/**
 * @brief Function invoked by the rotary cb. Changes the position of the indicator displayed in the example view.
 * @param is_clockwise Rotation direction.
 */
void view_example_rotary_pos_changed(bool is_clockwise)
{
	_unset_displayed_page();
	view_set_selected_sensor(is_clockwise, &s_info.position);
	view_set_indicator_dot(s_info.naviframe_item, s_info.position);
	_set_displayed_page();
}

/**
 * @brief Loads the gif images.
 */
static void _initialize_images(void)
{
	Evas_Object *layout = elm_object_item_content_get(s_info.naviframe_item);
	int i = 0;
	char *animation_file_names[SENSOR_COUNT] = {
			"images/examples/animations/animation_accelerometr.gif",
			"images/examples/animations/animation_gravity.gif",
			"images/examples/animations/animation_linear_acceleration.gif",
			"images/examples/animations/animation_magnetic.gif",
			"images/examples/animations/animation_rotation_vector.gif",
			"images/examples/animations/animation_orientation.gif",
			"images/examples/animations/animation_gyroscope.gif",
			"images/examples/animations/animation_light.gif",
			"images/examples/animations/animation_proximity.gif",
			"images/examples/animations/animation_pressure.gif",
			"images/examples/animations/animation_UV.gif",
			"images/examples/animations/animation_temperature.gif",
			"images/examples/animations/animation_humidity.gif",
			"images/examples/animations/animation_hrm.gif",
	};

	for (i = 0; i < SENSOR_COUNT; ++i) {
		s_info.page_data[i] = gif_animator_create_animated_image(layout, animation_file_names[i]);
		if (!s_info.page_data[i]) {
			dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] s_info.page_data[i].animation == NULL", __FILE__, __LINE__);
			continue;
		}
	}

	_set_displayed_page();
}

/**
 * @brief Hides and stops the gif animation played on the current page. This function is invoked before the page is changed.
 */
static void _unset_displayed_page(void)
{
	Evas_Object *layout = elm_object_item_content_get(s_info.naviframe_item);
	Evas_Object *old_anim = elm_layout_content_unset(layout, PART_ANIM);
	evas_object_hide(old_anim);

	gif_animator_play_set(s_info.page_data[s_info.position], EINA_FALSE, EINA_FALSE, EINA_TRUE);
}

/**
 * @brief Sets an image as the content of the example view.
 */
static void _set_displayed_page(void)
{
	Evas_Object *layout = elm_object_item_content_get(s_info.naviframe_item);
	elm_layout_content_set(layout, PART_ANIM, s_info.page_data[s_info.position]);
}

/**
 * @brief Callback invoked when the 'Play' button is clicked.
 * @param data User data.
 * @param obj The layout object.
 * @param emission The emitted signal.
 * @param source The signal's source.
 */
static void _play_button_clicked_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	gif_animator_play_set(s_info.page_data[s_info.position], EINA_TRUE, EINA_TRUE, EINA_TRUE);
}
