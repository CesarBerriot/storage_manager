//
// Created by César B. on 1/17/2024.
//

#pragma once

#include "ui/ui.h"
#include "data structures/vec2/vec2.h"

struct color_rgb8 { uint32_t a : 8, r : 8, g : 8, b : 8; };

// @formatter:off
void UIDrawCircleEx(UIPainter * painter, size_t x, size_t y, size_t radius, size_t thickness
					, uint32_t color, bool free_vertex_array_buffers
					, struct vec2 ** out_inner_vertices, struct vec2 ** out_outer_vertices
					, size_t * out_vertex_count);
__forceinline void UIDrawCircle(UIPainter * painter, size_t x, size_t y, size_t radius, size_t thickness, uint32_t color) { UIDrawCircleEx(painter, x, y, radius, thickness, color, true, NULL, NULL, NULL); }
/**
 * @brief applies anti-aliasing across the whole painter using pre-filtering
 * @param pixel_area_size defines how large the area of pixels affecting a pixel's color is
 * @param weight defines how affected by it's area a pixel should be. from 0 to 1.
 */
void UIAntiAlias(UIPainter * painter, UIRectangle bounds, uint8_t pixel_area_size, float weight);

__forceinline struct color_rgb8 color_to_struct(uint32_t color) { struct color_rgb8 c = {255, UI_COLOR_RED(color), UI_COLOR_GREEN(color), UI_COLOR_BLUE(color)}; return c; }
__forceinline uint32_t struct_to_color(uint8_t r, uint8_t g, uint8_t b) { return RGB8(r, g, b); }
// @formatter:on
