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

#if !defined(_DATA_H)
#define _DATA_H

#include <sensor.h>
#include "view.h"

void data_finalize(void);
bool data_initialize(Update_Sensor_Values_Cb callback);
void data_set_selected_sensor(sensor_type_e type);
bool data_get_sensor_support(sensor_type_e type);
void data_get_sensor_range(sensor_type_e type, float *min, float *max);
float data_get_sensor_resolution(sensor_type_e type);
char *data_get_sensor_vendor(sensor_type_e type);
void data_get_sensor_data(sensor_type_e type);
void data_stop_sensor(void);

#endif
