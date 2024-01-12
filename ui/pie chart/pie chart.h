//
// Created by César B. on 1/12/2024.
//

#pragma once

#include "../ui.h"

typedef struct UIPieChart
{
	UIElement  e;
	struct { char * name; size_t size; uint8_t color_rgb; } * pieces;
	size_t pieces_len;
} UIPieChart;

UIPieChart * UIPieChartCreate(UIElement *parent, uint32_t flags);

int UIPieChartMessage(UIElement *element, UIMessage message, int di, void *dp);
