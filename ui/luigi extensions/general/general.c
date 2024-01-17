//
// Created by César B. on 1/17/2024.
//

#include "general.h"
#include <assert.h>
#include <math.h>
#include <malloc.h>
#include "data structures/vec2/vec2.h"

void UIDrawCircle(UIPainter * painter, size_t x, size_t y, size_t radius, size_t thickness, uint32_t color)
{
	enum
	{
		CIRCLE_POINTS_COUNT = 360,
		STEP = 1,
		POINTS_COUNT = CIRCLE_POINTS_COUNT / STEP
	};

	static_assert(!(CIRCLE_POINTS_COUNT % STEP));

	// allocating buffers for interior and exterior vertex arrays
	struct
	{
		struct vec2 * inner_vertices;
		struct vec2 * outer_vertices;
		size_t len; // amount of vec2's in there, not length in bytes
	} vertex_arrays;
	vertex_arrays.len = CIRCLE_POINTS_COUNT / STEP;
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
		UIDrawTriangle(painter, prev_inner.x, prev_inner.y, prev_outer.x, prev_outer.y, i_inner.x, i_inner.y, color);
		UIDrawTriangle(painter, i_outer.x, i_outer.y, i_inner.x, i_inner.y, prev_outer.x, prev_outer.y, color);
	}

	// freeing the vertex arrays
	free(vertex_arrays.inner_vertices);
	free(vertex_arrays.outer_vertices);
}
