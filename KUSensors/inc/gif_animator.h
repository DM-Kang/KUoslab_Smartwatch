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

#if !defined(_GIF_ANIMATOR_H_)
#define _GIF_ANIMATOR_H_

#include <stdbool.h>
#include <Elementary.h>
#include <efl_extension.h>

void gif_animator_play_set(Evas_Object *obj, Eina_Bool play, Eina_Bool repeat, Eina_Bool reset);
Evas_Object *gif_animator_create_animated_image(Evas_Object *parent, char *path);

#endif
