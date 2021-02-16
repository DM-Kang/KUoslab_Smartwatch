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

#if !defined(_VIEW_CHART_H_)
#define _VIEW_CHART_H_

#include <stdbool.h>
#include <Elementary.h>
#include <efl_extension.h>

bool view_chart_create(Evas_Object *parent);
void view_chart_prepare(int value_count, float min, float max);
void view_chart_add_data(float *values);

#endif
