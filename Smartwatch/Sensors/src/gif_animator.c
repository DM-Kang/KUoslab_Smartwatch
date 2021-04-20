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
#include "data.h"
#include "gif_animator.h"

#define DATA_KEY_ANIM "data_key_anim"

typedef struct _anim_data {
	int frame_count;
	int cur_frame;
	double frame_duration;
	Eina_Bool repeat;
	Eina_Bool play;
	Ecore_Timer *anim_timer;
	Evas_Object *img;
} anim_data;

/**
 * @brief Callback invoked when an animated image object is going to be deleted.
 * @param data The user data.
 * @param e The canvas the object is placed on.
 * @param obj The object to be deleted.
 * @param event_info The event information.
 */
static void _animated_data_deleted_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	struct anim_data *d = evas_object_data_del(obj, DATA_KEY_ANIM);
	if (d != NULL)
		free(d);

	evas_object_event_callback_del(obj, EVAS_CALLBACK_DEL, _animated_data_deleted_cb);
}

/**
 * @brief Callback invoked by the animation timer.
 * @param user_data The user data.
 * @return ECORE_CALLBACK_RENEW - The timer should restart the countdown. ECORE_CALLBACK_CANCEL - The timer should be destroyed.
 */
static Eina_Bool _animated_cb(void *user_data)
{
	anim_data *data = (anim_data *)user_data;
	if (!data->play)
		return ECORE_CALLBACK_CANCEL;

	data->cur_frame++;
	if (data->cur_frame > data->frame_count) {
		if (!data->repeat) {
			data->play = EINA_FALSE;
			return ECORE_CALLBACK_CANCEL;
		}
		data->cur_frame = data->cur_frame % data->frame_count;
	}
	evas_object_image_animated_frame_set(data->img, data->cur_frame);
	data->frame_duration = evas_object_image_animated_frame_duration_get(data->img, data->cur_frame, 0);

	if (data->frame_duration > 0)
		ecore_timer_interval_set(data->anim_timer, data->frame_duration);

	return ECORE_CALLBACK_RENEW;
}

/**
 * @brief Function used to store the data needed by an animated image object.
 * @param obj The image object.
 * @param play true - start the animation.
 */
static void _animated_set(Evas_Object *obj, Eina_Bool play)
{
	anim_data *data = NULL;
	Evas_Object *img = NULL;

	if (strcmp("elm_image", evas_object_type_get(obj)))
		return;

	data = evas_object_data_get(obj, DATA_KEY_ANIM);

	img = elm_image_object_get(obj);
	if (!evas_object_image_animated_get(img))
		return;

	if (play) {
		if (!data)
			data = calloc(1, sizeof(anim_data));

		if (!data) {
			dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] data == NULL", __FILE__, __LINE__);
			return;
		}

		evas_object_event_callback_del(obj, EVAS_CALLBACK_DEL, _animated_data_deleted_cb);
		evas_object_event_callback_add(obj, EVAS_CALLBACK_DEL, _animated_data_deleted_cb, NULL);

		data->img = img;
		data->frame_count = evas_object_image_animated_frame_count_get(img);
		data->cur_frame = 1;
		data->frame_duration = evas_object_image_animated_frame_duration_get(img, data->cur_frame, 0);
		evas_object_image_animated_frame_set(img, data->cur_frame);
		evas_object_data_set(obj, DATA_KEY_ANIM, data);
	} else {
		if (data != NULL) {
			evas_object_event_callback_del(obj, EVAS_CALLBACK_DEL, _animated_data_deleted_cb);
			free(data);
		}
	}
}

/**
 * @brief Creates the animated image.
 * @param parent The parent object.
 * @param path The path to the gif file.
 * @return Pointer to the animated image object or NULL on error.
 */
Evas_Object *gif_animator_create_animated_image(Evas_Object *parent, char *path)
{
	const char *buf = NULL;
	Evas_Object *anim = elm_image_add(parent);
	if (!anim) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] anim == NULL", __FILE__, __LINE__);
		return NULL;
	}

	buf = eina_stringshare_nprintf(PATH_MAX, "%s/%s", app_get_resource_path(), path);

	if (!elm_image_file_set(anim, buf, NULL)) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] Failed to set file: %s", __FILE__, __LINE__, buf);
		evas_object_del(anim);
		eina_stringshare_del(buf);
		return NULL;
	}

	eina_stringshare_del(buf);
	evas_object_size_hint_weight_set(anim, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(anim, EVAS_HINT_FILL, EVAS_HINT_FILL);

	if (!elm_image_animated_available_get(anim))
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] No animation available in image: %s", __FILE__, __LINE__, buf);
	else
		_animated_set(anim, true);

	evas_object_show(anim);
	return anim;
}

/**
 * @brief Sets options of the animation playback.
 * @param obj The animated object.
 * @param play Start the animation.
 * @param repeat Repeat the animation when the last frame is reached.
 * @param reset Set the first frame as the current one.
 */
void gif_animator_play_set(Evas_Object *obj, Eina_Bool play, Eina_Bool repeat, Eina_Bool reset)
{
	anim_data *data = NULL;

	if (strcmp("elm_image", evas_object_type_get(obj)))
			return;

	data = evas_object_data_get(obj, DATA_KEY_ANIM);
	if (data == NULL)
		return;

	data->repeat = repeat;

	if (data->play == play)
		return;

	if (reset) {
		data->frame_count = evas_object_image_animated_frame_count_get(data->img);
		data->cur_frame = 1;
		data->frame_duration = evas_object_image_animated_frame_duration_get(data->img, data->cur_frame, 0);
		evas_object_image_animated_frame_set(data->img, data->cur_frame);
	}

	if (play) {
		if (data->anim_timer) {
			ecore_timer_del(data->anim_timer);
			data->anim_timer = NULL;
		}

		data->anim_timer = ecore_timer_add(data->frame_duration, _animated_cb, data);
	} else {
		if (data->anim_timer) {
			ecore_timer_del(data->anim_timer);
			data->anim_timer = NULL;
		}
	}
	data->play = play;
}
