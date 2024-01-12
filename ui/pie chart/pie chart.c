//
// Created by César B. on 1/12/2024.
//

#include <assert.h>
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
			UIPainter * painter = dp;
			UIRectangle bounds = g_ui.pie_chart_element->e.bounds;
			RGB
			UIDrawBlock(painter, bounds, (20 << 16) | (0 << 8) | 20);
			UIDrawLine(painter, bounds.l, bounds.t, bounds.r, bounds.b)
			break;
		case UI_MSG_GET_WIDTH:
			return element->bounds.r - element->bounds.l;
		case UI_MSG_GET_HEIGHT:
			return element->bounds.b - element->bounds.t;
	}
	return 0;
}
