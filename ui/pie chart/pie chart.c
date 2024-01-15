//
// Created by César B. on 1/12/2024.
//

#include <assert.h>
#include <math.h>
#include <malloc.h>
#include "pie chart.h"

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
	struct vec2 { size_t x, y; };

	// todo make this a variable ?
	enum { CIRCLE_POINTS_COUNT = 360 };

	// allocating buffers for interior and exterior vertex arrays
	struct
	{
		struct vec2 * interior_vertices;
		struct vec2 * exterior_vertices;
		size_t len; // amount of vec2's in there, not length in bytes
	} vertex_arrays;
	vertex_arrays.len = CIRCLE_POINTS_COUNT;
	vertex_arrays.interior_vertices = malloc(vertex_arrays.len * sizeof(struct vec2));
	vertex_arrays.exterior_vertices = malloc(vertex_arrays.len * sizeof(struct vec2));

	// filling the vertex arrays
	for(register uint16_t prev = CIRCLE_POINTS_COUNT - 1, i = 0; i <= CIRCLE_POINTS_COUNT; prev = i++)
	{
#pragma push_macro("TO_RAD")
#define TO_RAD(x) (M_PI * 2 * (x) / 360.)
		double prev_rad = TO_RAD(prev);
		double i_rad = TO_RAD(i);
#pragma pop_macro("TO_RAD")
#pragma push_macro("POS")
#define POS() (square_rect.l)
		struct vec2 from = {
			x + radius * cos(prev_rad)
			, y + radius * sin(prev_rad)
		};
		struct vec2 to = {
			x + radius * cos(i_rad)
			, y + radius * sin(i_rad)
		};
#pragma pop_macro("POS")
		UIDrawLine(painter, from.x, from.y, to.x, to.y, RGB8(0, 255, 0));
	}

	// freeing the vertex arrays
	free(vertex_arrays.interior_vertices);
	free(vertex_arrays.exterior_vertices);
}
