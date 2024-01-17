//
// Created by César B. on 1/12/2024.
//

#include "pie chart.h"
#include <assert.h>
#include <math.h>
#include <malloc.h>
#include "ui/luigi extensions/general/general.h"
#include "data structures/vec2/vec2.h"

UIPieChart * UIPieChartCreate(UIElement * parent, uint32_t flags)
{
	return UIElementCreate(sizeof(UIPieChart), parent, flags, UIPieChartMessage, "Pie Chart");
}

int UIPieChartMessage(UIElement * element, UIMessage message, int di, void * dp)
{
	switch(message)
	{
		case UI_MSG_PAINT:
		{
			// a good-looking gnome-feeling dark red I found out randomly
			// while doing some testing with colors
			uint32_t gnome_red = (20 << 16) | (0 << 8) | 20;
			UIPainter * painter = dp;
			UIRectangle bounds = g_ui.pie_chart_element->e.bounds;
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
			//UIDrawBlock(painter, bounds, gnome_red);
			//UIDrawBlock(painter, square_rect, RGB8(0, 0, 255));
			{
				UIRectangle right_square_rect = { square_rect.r, bounds.r, bounds.t, bounds.b };
				UIRectangle left_square_rect = { bounds.l, square_rect.l, bounds.t, bounds.b };
				UIRectangle up_square_rect = { bounds.l, bounds.r, bounds.t, square_rect.t };
				UIRectangle down_square_rect = { bounds.l, bounds.r, square_rect.b, bounds.b };
				UIDrawBlock(painter, right_square_rect, gnome_red);
				UIDrawBlock(painter, left_square_rect, gnome_red);
				UIDrawBlock(painter, up_square_rect, gnome_red);
				UIDrawBlock(painter, down_square_rect, gnome_red);
			}
			size_t square_rect_width = square_rect.r - square_rect.l;
			size_t square_rect_width_half = square_rect_width / 2;
			struct { size_t x, y; } square_rect_center = {
				square_rect.l + square_rect_width_half
				, square_rect.t + square_rect_width_half
			};
			UIDrawCircle(painter, square_rect_center.x, square_rect_center.y, square_rect_width_half, 25, RGB8(0, 0, 0));
			// UIAntiAlias(painter, bounds, 1, .5f);
		}
			break;
		case UI_MSG_GET_WIDTH:
			return element->bounds.r - element->bounds.l;
		case UI_MSG_GET_HEIGHT:
			return element->bounds.b - element->bounds.t;
	}
	return 0;
}