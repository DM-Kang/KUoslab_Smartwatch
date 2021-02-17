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

#include <sensors.h>
#include <stdlib.h>
#include "view.h"
#include "data.h"
#include "view_defines.h"
#include "view/view_main.h"
#include "view/view_information.h"
#include "view/view_data.h"
#include "view/view_example.h"
#include "view/view_indicator.h"

#define VISIBLE_CHARTS_MAX 4

static struct view_info {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *naviframe;
	Evas_Object *options;
	int chart_count;
	int view_mode;
} s_info = {
	.win = NULL,
	.conform = NULL,
	.naviframe = NULL,
	.options = NULL,
	.chart_count = 0,
	.view_mode = VIEW_MODE_MAIN,
};

static bool _create_naviframe(void);
static void _win_delete_request_cb(void *data , Evas_Object *obj , void *event_info);
static void _back_button_pressed_cb(void *data, Evas_Object *obj, void *event_info);
static Eina_Bool _rotary_cb(void *data, Eext_Rotary_Event_Info *info);

/**
 * @brief Create Essential Object window, conformant and layout
 */
Eina_Bool view_create(void)
{
	/* Create window */
	s_info.win = view_create_win(PACKAGE);
	if (s_info.win == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to create a window.");
		return EINA_FALSE;
	}

	/* Create conformant */
	s_info.conform = view_create_conformant_without_indicator(s_info.win);
	if (s_info.conform == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to create a conformant");
		return EINA_FALSE;
	}

	if (!_create_naviframe()) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] _create_naviframe() failed", __FILE__, __LINE__);
		return EINA_FALSE;
	}

	if (!view_main_create(s_info.naviframe)) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] view_main_create() failed", __FILE__, __LINE__);
		return EINA_FALSE;
	}

	if (!view_information_create(s_info.naviframe)) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] view_information_create() failed", __FILE__, __LINE__);
		return EINA_FALSE;
	}

	if (!view_data_create(s_info.naviframe)) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] view_data_create() failed", __FILE__, __LINE__);
		return EINA_FALSE;
	}

	if (!view_example_create(s_info.naviframe)) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] view_example_create() failed", __FILE__, __LINE__);
		return EINA_FALSE;
	}

	view_main_show();
	eext_rotary_event_handler_add(_rotary_cb, NULL);

	/* Show window after main view is set up */
	evas_object_show(s_info.win);
	return EINA_TRUE;
}

/**
 * @brief Make a basic window named package
 * @param[pkg_name]: Name of the window
 */
Evas_Object *view_create_win(const char *pkg_name)
{
	Evas_Object *win = NULL;

	/*
	 * Window
	 * Create and initialize elm_win.
	 * elm_win is mandatory to manipulate window
	 */
	win = elm_win_util_standard_add(pkg_name, pkg_name);
	elm_win_conformant_set(win, EINA_TRUE);
	elm_win_autodel_set(win, EINA_TRUE);

	/* Rotation setting */
	if (elm_win_wm_rotation_supported_get(win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(win, (const int *)(&rots), 4);
	}

	evas_object_smart_callback_add(win, "delete,request", _win_delete_request_cb, NULL);
	eext_object_event_callback_add(win, EEXT_CALLBACK_BACK, _back_button_pressed_cb, NULL);

	return win;
}

/**
 * @brief Make a conformant without indicator for wearable app
 * @param[win]: The object to which you want to set this conformant
 * Conformant is mandatory for base GUI to have proper size
 */
Evas_Object *view_create_conformant_without_indicator(Evas_Object *win)
{
	/*
	 * Conformant
	 * Create and initialize elm_conformant.
	 * elm_conformant is mandatory for base GUI to have proper size
	 * when indicator or virtual keypad is visible.
	 */
	Evas_Object *conform = NULL;

	if (win == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "window is NULL.");
		return NULL;
	}

	conform = elm_conformant_add(win);
	evas_object_size_hint_weight_set(conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(win, conform);

	evas_object_show(conform);

	return conform;
}


/**
 * @brief Destroy window and free important data to finish this application
 */
void view_destroy(void)
{
	if (s_info.win == NULL)
		return;

	evas_object_del(s_info.win);
}

/**
 * @brief Creates the naviframe object and puts it as the content of the conformant object.
 * @return true on success or false on fail.
 */
static bool _create_naviframe(void)
{
	s_info.naviframe = elm_naviframe_add(s_info.conform);
	if (!s_info.naviframe) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] s_info.naviframe == NULL", __FILE__, __LINE__);
		return false;
	}

	elm_object_content_set(s_info.conform, s_info.naviframe);
	evas_object_show(s_info.naviframe);

	return true;
}

/**
 * @brief Changes the displayed view.
 * @param view_mode The view that will be displayed.
 */
void view_switch_view(int view_mode)
{
	s_info.view_mode = view_mode;

	switch (view_mode) {
	case VIEW_MODE_DATA:
		view_data_show();
		break;
	case VIEW_MODE_INFORMATION:
		view_information_show();
		break;
	case VIEW_MODE_EXAMPLE:
		view_example_show();
		break;
	default:
		view_main_show();
		break;
	}
}

/**
 * @brief Callback function to be invoked on main window close.
 * @param[data]: Custom data passed to the callback attachment function.
 * @param[obj]: Object to which the handler is attached.
 * @param[event_info]: The information on the event occurred.
 */
static void _win_delete_request_cb(void *data , Evas_Object *obj , void *event_info)
{
	ui_app_exit();
}

/**
 * @brief Callback invoked when the user presses the hw back button.
 * @param data The user data.
 * @param obj The window object.
 * @param event_info The event information.
 */
static void _back_button_pressed_cb(void *data, Evas_Object *obj, void *event_info)
{
	if (s_info.view_mode == VIEW_MODE_MAIN) {
		elm_win_lower(s_info.win);
	} else {

		if (s_info.view_mode == VIEW_MODE_DATA)
			view_data_hide();
		if (s_info.view_mode == VIEW_MODE_EXAMPLE)
			view_example_hide();

		s_info.view_mode = VIEW_MODE_MAIN;
		view_main_show();
	}
}

/**
 * @brief Callback invoked when the rotary is moved.
 * @param data The user data.
 * @param info The rotation data.
 * @return The callback priority control.
 */
static Eina_Bool _rotary_cb(void *data, Eext_Rotary_Event_Info *info)
{
	switch (s_info.view_mode) {
	case VIEW_MODE_MAIN:
		view_main_rotary_pos_changed(info->direction == EEXT_ROTARY_DIRECTION_CLOCKWISE);
		break;
	case VIEW_MODE_DATA:
		view_data_rotary_pos_changed(info->direction == EEXT_ROTARY_DIRECTION_CLOCKWISE);
		break;
	case VIEW_MODE_EXAMPLE:
		view_example_rotary_pos_changed(info->direction == EEXT_ROTARY_DIRECTION_CLOCKWISE);
		break;
	case VIEW_MODE_INFORMATION:
		view_information_rotary_pos_changed(info->direction == EEXT_ROTARY_DIRECTION_CLOCKWISE);
		break;
	default:
		dlog_print(DLOG_WARN, LOG_TAG, "[%s:%d] Unknown view mode", __FILE__, __LINE__);
		break;
	}

	return EINA_TRUE;
}
