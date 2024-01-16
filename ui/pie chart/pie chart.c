//
// Created by César B. on 1/12/2024.
//

#include <assert.h>
#include <math.h>
#include <malloc.h>
#include "pie chart.h"
#include "vec2.h"

UIPieChart * UIPieChartCreate(UIElement * parent, uint32_t flags)
{
	return UIElementCreate(sizeof(UIPieChart), parent, flags, UIPieChartMessage, "Pie Chart");
}

int UIPieChartMessage(UIElement * element, UIMessage message, int di, void * dp)
{
	switch(message)
	{
		case UI_MSG_LAYOUT:
			//element->children = NULL;
			//UIElementMove(element, element->bounds, false);
			break;
		case UI_MSG_PAINT:
		{

			UIPainter * painter = dp;
			UIRectangle bounds = g_ui.pie_chart_element->e.bounds;
			UIDrawBlock(painter, bounds, (20 << 16) | (0 << 8) | 20);
			UIDrawLine(painter, bounds.l, bounds.t, bounds.r, bounds.b, RGB8(255, 255, 0));
			UIDrawGlyph(painter, bounds.l + 150, bounds.t + 150, 'f', RGB8(255, 255, 255));
			UIDrawTriangle(painter, bounds.l, bounds.b, bounds.l + (bounds.r - bounds.l) / 2, bounds.t, bounds.r, bounds.b, RGB8(255, 0, 0));
			UIRectangle square_rect;
			size_t horizontal_length = bounds.r - bounds.l;
			size_t vertical_length = bounds.b - bounds.t;
			// todo make this more generic
			if(vertical_length < horizontal_length)
			{
				square_rect.t = bounds.t;
				square_rect.b = bounds.b;
				size_t distHalf = vertical_length / 2;
				size_t mid_horizontal = bounds.r - horizontal_length / 2;
				square_rect.l = mid_horizontal - distHalf;
				square_rect.r = mid_horizontal + distHalf;
			}
			else
			{
				square_rect.l = bounds.l;
				square_rect.r = bounds.r;
				size_t distHalf = horizontal_length / 2;
				size_t mid_vertical = bounds.b - vertical_length / 2;
				square_rect.t = mid_vertical - distHalf;
				square_rect.b = mid_vertical + distHalf;
			}
			UIDrawBlock(painter, square_rect, RGB8(0, 0, 255));
			size_t square_rect_width = square_rect.r - square_rect.l;
			size_t square_rect_width_half = square_rect_width / 2;
			struct { size_t x, y; } square_rect_center = {
				square_rect.l + square_rect_width_half
				, square_rect.t + square_rect_width_half
			};
			UIDrawCircle(painter, square_rect_center.x, square_rect_center.y, square_rect_width_half, 25);
			UIRectangle bottom_left_invert_rect = { bounds.l, square_rect_center.x, square_rect_center.y, bounds.b };
			UIRectangle top_right_invert_rect = { square_rect_center.x, bounds.r, bounds.t, square_rect_center.y };
			UIDrawInvert(painter, bottom_left_invert_rect);
			UIDrawInvert(painter, top_right_invert_rect);
		}
			break;
		case UI_MSG_GET_WIDTH:
			return element->bounds.r - element->bounds.l;
		case UI_MSG_GET_HEIGHT:
			return element->bounds.b - element->bounds.t;
	}
	return 0;
}

void UIDrawCircle(UIPainter * painter, size_t x, size_t y, size_t radius, size_t thickness)
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
		UIDrawTriangle(painter, prev_inner.x, prev_inner.y, prev_outer.x, prev_outer.y, i_outer.x, i_outer.y, RGB8(255, 255, 0));
		UIDrawTriangle(painter, i_outer.x, i_outer.y, i_inner.x, i_inner.y, prev_inner.x, prev_inner.y, RGB8(255, 255, 0));
		// normally the above 2 draws are enough to fill the whole thing
		// but the ui library leaves a gap in-between the triangles
		UIDrawTriangle(painter, prev_inner.x, prev_inner.y, prev_outer.x, prev_outer.y, i_inner.x, i_inner.y, RGB8(255, 255, 0));
		UIDrawTriangle(painter, i_outer.x, i_outer.y, i_inner.x, i_inner.y, prev_outer.x, prev_outer.y, RGB8(255, 255, 0));
	}

	// freeing the vertex arrays
	free(vertex_arrays.inner_vertices);
	free(vertex_arrays.outer_vertices);
}
