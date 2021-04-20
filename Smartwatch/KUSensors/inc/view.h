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

#if !defined(_VIEW_H)
#define _VIEW_H

#include <Elementary.h>
#include <efl_extension.h>
#include "view/view_indicator.h"

typedef void (*Update_Sensor_Values_Cb)(int count, float *values);

Eina_Bool view_create(void);
Evas_Object *view_create_win(const char *pkg_name);
Evas_Object *view_create_conformant_without_indicator(Evas_Object *win);
Evas_Object *view_create_layout_for_conformant(Evas_Object *parent, const char *file_path, const char *group_name, Eext_Event_Cb cb_function, void *user_data);
void view_destroy(void);
void view_add_chart(void);
void view_add_option_item(const char *opt_name, const int id, const bool checked);
void view_show_sensor(int sensor_id, const char *name);
void view_hide_sensor(int sensor_id);
void view_update_chart(double sensor_value, const int id);
void view_switch_view(int view_mode);

#endif
