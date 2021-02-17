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

#if !defined(_VIEW_DEFINES_H)
#define _VIEW_DEFINES_H

#define EDJ_MAIN "edje/main.edj"
#define EDJ_INDICATOR "edje/indicator.edj"
#define EDJ_INFORMATION "edje/information.edj"
#define EDJ_DATA "edje/data.edj"
#define EDJ_EXAMPLE "edje/example.edj"

#define GRP_MAIN "GRP_MAIN"
#define GRP_INDICATOR "GRP_INDICATOR"
#define GRP_INFORMATION "GRP_INFORMATION"
#define GRP_DATA "GRP_DATA"
#define GRP_EXAMPLE "GRP_EXAMPLE"

#define PART_TITLE "part,title"
#define PART_ANIM "part,anim"
#define PART_ROTARY_CLIPPER "part,main,rotary_clipper"
#define PART_INDICATOR "part,indicator"
#define PART_DATA_VALUES "part,data,values"
#define PART_INFO_SUPPORTED "part,info,supported"
#define PART_INFO_RANGE "part,info,range"
#define PART_INFO_RES "part,info,res_value"
#define PART_INFO_VENDOR "part,info,vendor_name"
#define PART_CHART "part,data,chart"

#define PART_INDICATOR_DOT_NAME "part,indicator,dot_"
#define PART_DATA_PARAM_NAME "part_data,text,name_"
#define PART_DATA_PARAM_VALUE "part_data,text,value_"

#define MSG_ID_ROTARY 1
#define MSG_ID_MAIN_MODE 2
#define MSG_ID_INDICATOR_CHANGED 3
#define MSG_ID_DATA_DISPLAY_CHANGED 4
#define MSG_ID_DEBUG 25

#define VIEW_MODE_MAIN 0
#define VIEW_MODE_INFORMATION 1
#define VIEW_MODE_DATA 2
#define VIEW_MODE_EXAMPLE 3

#define SENSOR_COUNT 14
#define MAX_VALUES_PER_SENSOR 4

#define COLOR_YELLOW_R 231
#define COLOR_YELLOW_G 214
#define COLOR_YELLOW_B 25
#define COLOR_YELLOW_A 255

#define SIGNAL_BUTTON_CLICKED "signal,button,clicked"
#define SIGNAL_NEXT_CLICKED "signal,button,next"
#define SIGNAL_PREV_CLICKED "signal,button,prev"

#define STATE_CUSTOM "custom"

#define M_SQRT1_2 0.70710678118654752440
#define BG_REL_MIN (0.5*(1.0 - 1.0*M_SQRT1_2))
#define BG_REL_MAX (1.0-BG_REL_MIN)

#endif
