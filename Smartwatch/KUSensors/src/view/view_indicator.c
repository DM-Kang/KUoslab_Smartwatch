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

#include <stdbool.h>
#include <Elementary.h>
#include <efl_extension.h>
#include <sensors.h>
#include "data.h"
#include "view/view_indicator.h"
#include "view/view_common.h"
#include "view_defines.h"

#define DATA_VIEW_ROTARY_CB "data_view_rotray_cb"

/**
 * @brief Creates the indicator object's layout.
 * @param parent The parent layout that the indicator will placed into.
 * @return The indicator object or NULL on error.
 */
Evas_Object *view_indicator_create(Evas_Object *parent)
{
	char *edj_path = NULL;
	Evas_Object *indicator = elm_layout_add(parent);
	if (!indicator) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] s_info.layout == NULL", __FILE__, __LINE__);
		return NULL;
	}

	edj_path = data_create_resource_path(EDJ_INDICATOR);
	if (!elm_layout_file_set(indicator, edj_path, GRP_INDICATOR)) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] Function elm_layout_file_set() failed", __FILE__, __LINE__);
		return NULL;
	}

	return indicator;
}
