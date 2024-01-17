//
// Created by César B. on 1/17/2024.
//

#include "general.h"
#include <assert.h>
#include <math.h>
#include <malloc.h>
#include <memory.h>

// @formatter:off
void UIDrawCircleEx(UIPainter * painter, size_t x, size_t y, size_t radius, size_t thickness
					, uint32_t color, bool free_vertex_array_buffers
					, struct vec2 ** out_inner_vertices, struct vec2 ** out_outer_vertices
					, size_t * out_vertex_count)
// @formatter:on
{
	struct vec2 pixel;
	size_t squared_radius = pow(radius, 2);
	size_t squared_thickness = pow(thickness, 2);
#pragma openacc parallel loop
	for(size_t iy = 0; iy <= painter->height; ++iy)
		for(size_t ix = 0; ix <= painter->width; ++ix)
		{
				size_t len = vec2_len(vec2_sub_2(vec2_create_2(x, y), vec2_create_2(ix, iy)));
				if(len < radius + thickness && len > radius - thickness)
					painter->bits[iy * painter->width + ix] = color;
		}
	/*enum
	{
		CIRCLE_POINTS_MAX = 360,
		STEP = 1,
		POINTS_COUNT = CIRCLE_POINTS_MAX / STEP
	};

	static_assert(!(CIRCLE_POINTS_MAX % STEP));

	// allocating buffers for interior and exterior vertex arrays
	struct
	{
		struct vec2 * inner_vertices;
		struct vec2 * outer_vertices;
		size_t len; // amount of vec2's in there, not length in bytes
	} vertex_arrays;
	vertex_arrays.len = CIRCLE_POINTS_MAX / STEP;
	vertex_arrays.inner_vertices = malloc(vertex_arrays.len * sizeof(struct vec2));
	vertex_arrays.outer_vertices = malloc(vertex_arrays.len * sizeof(struct vec2));

	// filling the vertex arrays
	for(register uint16_t i = 0; i < POINTS_COUNT; ++i)
	{
#pragma push_macro("TO_RAD")
#define TO_RAD(x) (M_PI * 2 * (x) / 360.)
		double i_rad = TO_RAD(i * STEP);
#pragma pop_macro("TO_RAD")
#pragma push_macro("POS")
		struct vec2 point = {
			x + radius * cos(i_rad)
			, y + radius * sin(i_rad)
		};
#pragma pop_macro("POS")
		struct vec2 dir = vec2_normalize(vec2_sub_2(point, vec2_create_2(x, y)));
		vertex_arrays.inner_vertices[i] = vec2_sub_2(point, vec2_mult(dir, thickness / 2.));;
		vertex_arrays.outer_vertices[i] = vec2_add_2(point, vec2_mult(dir, thickness / 2.));
	}

	// draw the circle
	for(register uint16_t prev = POINTS_COUNT - 1, i = 0; i < POINTS_COUNT; prev = i++)
	{
		struct vec2 prev_inner = vertex_arrays.inner_vertices[prev];
		struct vec2 i_inner = vertex_arrays.inner_vertices[i];
		struct vec2 prev_outer = vertex_arrays.outer_vertices[prev];
		struct vec2 i_outer = vertex_arrays.outer_vertices[i];
		UIDrawTriangle(painter, prev_inner.x, prev_inner.y, prev_outer.x, prev_outer.y, i_outer.x, i_outer.y, color);
		UIDrawTriangle(painter, i_outer.x, i_outer.y, i_inner.x, i_inner.y, prev_inner.x, prev_inner.y, color);
		// normally the above 2 draws are enough to fill the whole thing
		// but the ui library leaves a gap in-between the triangles
		//UIDrawTriangle(painter, prev_inner.x, prev_inner.y, prev_outer.x, prev_outer.y, i_inner.x, i_inner.y, color);
		//UIDrawTriangle(painter, i_outer.x, i_outer.y, i_inner.x, i_inner.y, prev_outer.x, prev_outer.y, color);
	}

	// freeing the vertex arrays
	if(free_vertex_array_buffers)
	{
		free(vertex_arrays.inner_vertices);
		free(vertex_arrays.outer_vertices);
	}
	else
	{
		assert(out_inner_vertices && out_outer_vertices && out_vertex_count);
		*out_inner_vertices = vertex_arrays.inner_vertices;
		*out_outer_vertices = vertex_arrays.outer_vertices;
		*out_vertex_count = vertex_arrays.len;
	}*/
}

void UIAntiAlias(UIPainter * painter, UIRectangle bounds, uint8_t pixel_area_size, float weight)
{
	size_t pixels_count = painter->height * painter->width;
	size_t buffer_len = pixels_count * sizeof(uint32_t);
	uint32_t * buffer_backup = malloc(buffer_len);
	memcpy(buffer_backup, painter->bits, buffer_len);
	for(size_t y = bounds.t; y <= bounds.b; ++y)
	{
		uint32_t * row = painter->bits + y * painter->width;
		for(size_t x = bounds.l; x <= bounds.r; ++x)
		{
			//row[x] = RGB8(20, 0, 20);
			size_t count = 0;
			struct { size_t r, g, b; } total;
			total.r = total.g = total.b = 0;
			for(register uint8_t i = 0; i < pixel_area_size; ++i)
			{
				// disponibility flags
				struct { uint8_t right : 1, left : 1, up : 1, down : 1; } f;
				f.left = x;
				f.right = x < bounds.r;
				f.up = y;
				f.down = y < bounds.b;
				uint32_t color;
				if(f.left)
				{
					color = x ? row[x - 1] : row[-painter->width];
					total.r += UI_COLOR_RED(color);
					total.g += UI_COLOR_GREEN(color);
					total.b += UI_COLOR_BLUE(color);
					++count;
				}
				if(f.right)
				{
					color = x < painter->width - 1 ? row[x + 1] : row[painter->width];
					total.r += UI_COLOR_RED(color);
					total.g += UI_COLOR_GREEN(color);
					total.b += UI_COLOR_BLUE(color);
					++count;
				}
				if(f.up)
				{
					color = row[x - painter->width];
					total.r += UI_COLOR_RED(color);
					total.g += UI_COLOR_GREEN(color);
					total.b += UI_COLOR_BLUE(color);
					++count;
				}
				if(f.down)
				{
					color = row[x + painter->width];
					total.r += UI_COLOR_RED(color);
					total.g += UI_COLOR_GREEN(color);
					total.b += UI_COLOR_BLUE(color);
					++count;
				}
			}
			if(!count)
				continue;
			total.r /= count;
			total.g /= count;
			total.b /= count;
			//row[x] = RGB8(total.r, total.g, total.b);
			struct color_rgb8 old_color = color_to_struct(row[x]);
			total.r += old_color.r;
			total.g += old_color.g;
			total.b += old_color.b;
			total.r /= 2;
			total.g /= 2;
			total.b /= 2;
			row[x] = struct_to_color(total.r, total.g, total.b);
		}
	}
}
