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

#if !defined(_VIEW_COMMON_H_)
#define _VIEW_COMMON_H_

#include <stdbool.h>
#include <Elementary.h>
#include <efl_extension.h>

Elm_Object_Item *view_create_layout(Evas_Object *parent, char *file_name, char *group_name);
void view_set_indicator_dot(Elm_Object_Item *navi_item, int position);
void view_set_selected_sensor(bool is_clockwise, int *pos);
char *data_create_resource_path(const char *file_name);

#endif
