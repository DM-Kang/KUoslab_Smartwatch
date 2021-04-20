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
#include "view/view_main.h"
#include "view/view_common.h"
#include "view.h"
#include "data.h"
#include "view_defines.h"

static struct view_info {
	Evas_Object *layout;
	Elm_Object_Item *navi_item;
	int rotary_position;
} s_info = {
	.layout = NULL,
	.navi_item = NULL,
	.rotary_position = VIEW_MODE_DATA,
};

static void _mode_changed_cb(void *data, Evas_Object *obj, Edje_Message_Type type, int id, void *msg);
static void _go_button_clicked(void *data, Evas_Object *obj, const char *emission, const char *source);
static void transition_finished_cb(void *data, Evas_Object *obj, void *event_info);

/**
 * @brief Crates the main view and pushes it into a naviframe.
 * @param parent The parent naviframe.
 * @return true on success or false on fail.
 */
bool view_main_create(Evas_Object *parent)
{
	char *edj_path = NULL;

	s_info.layout = elm_layout_add(parent);
	if (s_info.layout == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "layout is NULL.");
		return false;
	}

	edj_path = data_create_resource_path(EDJ_MAIN);
	if (!elm_layout_file_set(s_info.layout, edj_path, GRP_MAIN)) {
		evas_object_del(s_info.layout);
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to set layout edj file: %s.", edj_path);
		return false;
	}

	/* Layout size setting */
	evas_object_size_hint_weight_set(s_info.layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	edje_object_message_handler_set(elm_layout_edje_get(s_info.layout), _mode_changed_cb, NULL);
	elm_layout_signal_callback_add(s_info.layout, SIGNAL_BUTTON_CLICKED, "", _go_button_clicked, NULL);
	elm_object_content_set(parent, s_info.layout);
	evas_object_show(s_info.layout);
	evas_object_smart_callback_add(parent, "transition,finished", transition_finished_cb, parent);

	return true;
}

/**
 * @biref Displays the main view.
 */
void view_main_show(void)
{
	elm_naviframe_item_promote(s_info.navi_item);
}

/**
 * @brief Function invoked by the rotary cb. Rotates the menu.
 * @param is_clockwise Rotation direction.
 */
void view_main_rotary_pos_changed(bool is_clockwise)
{
	Edje_Message_Int msg = { 0 };
	msg.val = is_clockwise;
	edje_object_message_send(elm_layout_edje_get(s_info.layout), EDJE_MESSAGE_INT, MSG_ID_ROTARY, &msg);
}

/**
 * @brief Callback invoked when the 'Go' button is clicked. This will display the chosen view
 * @param data User data.
 * @param obj The layout object.
 * @param emission The emitted signal.
 * @param source The signal's source.
 */
static void _go_button_clicked(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	view_switch_view(s_info.rotary_position);
}

/**
 * @brief Callback invoked when transition of naviframe is finished.
 * @param data User data.
 * @param obj The layout object.
 * @param event_info Information attached to event.
 */
static void transition_finished_cb(void *data, Evas_Object *obj, void *event_info)
{
	if (!s_info.navi_item) {

		s_info.navi_item = elm_naviframe_item_simple_push((Evas_Object *)data, s_info.layout);
		if (!s_info.navi_item) {
			dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] s_info.navi_item == NULL", __FILE__, __LINE__);
			return;
		}
	}
}

/**
 * @brief Callback invoked when one of the menu options is selected.
 * @param data User data
 * @param obj The edje object used in the main view's layout.
 * @param type The edje message type.
 * @param id The message's id number.
 * @param msg The message received from the edje.
 */
static void _mode_changed_cb(void *data, Evas_Object *obj, Edje_Message_Type type, int id, void *msg)
{
	Edje_Message_Int *msg_int = (Edje_Message_Int *)msg;

	if (id != MSG_ID_MAIN_MODE)
		return;

	s_info.rotary_position = msg_int->val;
}
