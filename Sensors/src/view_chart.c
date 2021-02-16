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

#include <cairo.h>
#include <sensors.h>
#include "view_chart.h"
#include "view_defines.h"

#define CHART_TRANSLATION 2
#define WIDTH 300
#define HEIGHT 158
#define CHART_MAX_POINT_COUNT 100
#define CHART_POINT_INTERVAL (WIDTH / CHART_MAX_POINT_COUNT)

typedef struct _chart_data_s {
	float color[4];
	float points[CHART_MAX_POINT_COUNT];
} chart_data_t;

static struct _view_char_info {
	Evas_Object *image;
	cairo_surface_t *cairo_surface;
	cairo_t *cairo;
	int width;
	int height;
	int current_point;
	bool area_end_reached;
	int value_count;
	float min;
	float max;
	chart_data_t charts_data[MAX_VALUES_PER_SENSOR];
} s_info = {
	.image = NULL,
	.cairo_surface = NULL,
	.cairo = NULL,
	.width = WIDTH,
	.height = HEIGHT,
	.current_point = 0,
	.area_end_reached = false,
	.value_count = 0,
	.min = 0,
	.max = 0,
	.charts_data = {
		{ .color = {1.0, 0.0, 0.0, 1.0}, .points = {0,} },
		{ .color = {0.0, 1.0, 0.0, 1.0}, .points = {0,} },
		{ .color = {0.0, 0.0, 1.0, 1.0}, .points = {0,} },
		{ .color = {0.0, 1.0, 1.0, 1.0}, .points = {0,} },
	},
};

static bool _create_image(Evas_Object *parent);
static bool _create_cairo(void);
static void _draw_chart_area(void);
static void _update_image(void);
static float _lerp(float val);
static float _horiz_pos_transform(float val);
static void _redraw_chart(float *values);
static void _redraw_point(int horiz_pos, int point_index, int chart_index);

/**
 * @brief Creates an chart object using the cairo framework.
 * @param parent The parent object.
 * @return
 */
bool view_chart_create(Evas_Object *parent)
{
	if (!_create_image(parent)) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] Function _create_image() failed", __FILE__, __LINE__);
		return false;
	}

	if (!_create_cairo()) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] Function _create_cairo() failed", __FILE__, __LINE__);
		return false;
	}

	return true;
}

/**
 * @brief Function used to store the displayed values count and the minimal and maximal available values.
 * @param value_count The value count.
 * @param min The minimal value.
 * @param max The maximal value.
 */
void view_chart_prepare(int value_count, float min, float max)
{
	s_info.min = min ;
	s_info.max = max;
	s_info.value_count = value_count;

	_draw_chart_area();
	s_info.current_point = 0;
	s_info.area_end_reached = false;
}

/**
 * @brief Adds new values to be displayed on the chart.
 * @param values The values array
 */
void view_chart_add_data(float *values)
{
	if (s_info.area_end_reached || s_info.current_point >= CHART_MAX_POINT_COUNT -1) {
		s_info.current_point %= CHART_MAX_POINT_COUNT ;
		s_info.area_end_reached = true;
	}

	_draw_chart_area();
	_redraw_chart(values);

	s_info.current_point++;
}

/**
 * @brief Draws the chart using the available points.
 * @param values The values array.
 */
static void _redraw_chart(float *values)
{
	int i;
	int j;
	int index = 0;
	int last = 0;
	int start = 0;

	index = (CHART_MAX_POINT_COUNT + s_info.current_point) % CHART_MAX_POINT_COUNT;

	if (s_info.area_end_reached) {
		last = CHART_MAX_POINT_COUNT;
		start = index + 1;
	} else {
		last = s_info.current_point + 1;
	}

	for (i = 0; i < s_info.value_count; ++i) {
		if (s_info.current_point < CHART_MAX_POINT_COUNT)
			s_info.charts_data[i].points[index] = values[i];
	}

	for (i = 0; i < last; ++i) {
		for (j = 0; j < s_info.value_count; ++j)
			_redraw_point(i, (i + start) % CHART_MAX_POINT_COUNT, j);
	}
}

/**
 * @brief Stores the new point and draws a line connecting the new point and a previously created one.
 * @param horiz_pos The horizontal position of the new point.
 * @param point_index The point index.
 * @param chart_index The chart index.
 */
static void _redraw_point(int horiz_pos, int point_index, int chart_index)
{
	float pos_prev_x = 0;
	float pos_curr_x = 0;
	float pos_prev_y = 0;
	float pos_curr_y = 0;

	if (horiz_pos == 0)
		pos_prev_x = -CHART_POINT_INTERVAL;
	else
		pos_prev_x = CHART_POINT_INTERVAL * (horiz_pos - 1);

	if (point_index == 0)
		pos_prev_y = s_info.charts_data[chart_index].points[CHART_MAX_POINT_COUNT - 1];
	else
		pos_prev_y = s_info.charts_data[chart_index].points[point_index - 1];

	pos_curr_x = CHART_POINT_INTERVAL * (horiz_pos);
	pos_curr_y = s_info.charts_data[chart_index].points[point_index];

	pos_prev_y = _horiz_pos_transform(pos_prev_y);
	pos_curr_y = _horiz_pos_transform(pos_curr_y);

	cairo_set_line_width(s_info.cairo, 2);
	cairo_set_source_rgba(s_info.cairo,
			s_info.charts_data[chart_index].color[0],
			s_info.charts_data[chart_index].color[1],
			s_info.charts_data[chart_index].color[2],
			s_info.charts_data[chart_index].color[3]);

	cairo_move_to(s_info.cairo, pos_prev_x, pos_prev_y);
	cairo_line_to(s_info.cairo, pos_curr_x, pos_curr_y);
	cairo_stroke(s_info.cairo);

	_update_image();
}

/**
 * @brief Transforms the given value to chart coordinates.
 * @param val The value to transform.
 * @return The transformed value.
 */
static inline float _lerp(float val)
{
	float max = s_info.max;

	if (s_info.min < 0) {
		val -= s_info.min;
		max -= s_info.min;
	}

	return s_info.height / max * val;
}

/**
 * @brief Creates a cairo handle and a cairo surface using an evas image.
 * @return true on success or false on fail.
 */
static bool _create_cairo(void)
{
	int row_stride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, s_info.width);
	unsigned char *imageData = (unsigned char*)evas_object_image_data_get(s_info.image, EINA_TRUE);

	s_info.cairo_surface = cairo_image_surface_create_for_data(imageData, CAIRO_FORMAT_ARGB32, s_info.width, s_info.height, row_stride);
	if (!s_info.cairo_surface) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] s_info.cairo_surface == NULL", __FILE__, __LINE__);
		return false;
	}

	s_info.cairo = cairo_create(s_info.cairo_surface);
	if (!s_info.cairo) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] s_info.cairo == NULL", __FILE__, __LINE__);
		cairo_surface_destroy(s_info.cairo_surface);
		return false;
	}

	cairo_translate(s_info.cairo, 0.0, CHART_TRANSLATION);
	cairo_scale(s_info.cairo, 1.0, 0.98);

	return true;
}

/**
 * @brief Creates an evas image.
 * @param parent The parent layout that the image will be added to.
 * @return true on success or false on fail.
 */
static bool _create_image(Evas_Object *parent)
{
	s_info.image = evas_object_image_filled_add(evas_object_evas_get(parent));
	if (!s_info.image) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] s_info.image == NULL", __FILE__, __LINE__);
		return false;
	}

	evas_object_image_content_hint_set(s_info.image, EVAS_IMAGE_CONTENT_HINT_DYNAMIC);
	evas_object_image_size_set(s_info.image, s_info.width, s_info.height);
	evas_object_image_colorspace_set(s_info.image, EVAS_COLORSPACE_ARGB8888);
	evas_object_image_alpha_set(s_info.image, EINA_FALSE);

	if (!elm_layout_content_set(parent, PART_CHART, s_info.image)) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] Function elm_layout_content_set() failed", __FILE__, __LINE__);
		return false;
	}

	return true;
}

/**
 * @brief Transforms a value to chart coordinates and inverts its value.
 * @param val The value to transform.
 * @return The new value.
 */
static inline float _horiz_pos_transform(float val)
{
	return s_info.height - _lerp(val);
}

/**
 * @brief draw the chart area background and the axis arrow.
 */
static void _draw_chart_area(void)
{
	cairo_set_source_rgba(s_info.cairo, 1.0, 1.0, 1.0, 1.0);
	cairo_paint(s_info.cairo);

	cairo_set_source_rgba(s_info.cairo, 0.0, 0.0, 0.0, 1.0);
	cairo_set_line_width(s_info.cairo, 3);

	cairo_move_to(s_info.cairo, 0, _horiz_pos_transform(0));
	cairo_line_to(s_info.cairo, s_info.width * 0.98, _horiz_pos_transform(0));
	cairo_rel_line_to(s_info.cairo, -5, -5);
	cairo_move_to(s_info.cairo, s_info.width * 0.98, _horiz_pos_transform(0));
	cairo_rel_line_to(s_info.cairo, -5, 5);
	cairo_stroke(s_info.cairo);
}

/**
 * @brief Update the evas image when all the cairo operations are completed.
 */
static void _update_image(void)
{
	cairo_surface_flush(s_info.cairo_surface);

	unsigned char *imageData = cairo_image_surface_get_data(cairo_get_target(s_info.cairo));
	evas_object_image_data_set(s_info.image, imageData);
	evas_object_image_data_update_add(s_info.image, 0, 0, s_info.width, s_info.height);
}
