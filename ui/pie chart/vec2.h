//
// Created by César B. on 1/16/2024.
//

#pragma once

#include <stdint.h>
#include <assert.h>
#include <math.h>

struct vec2
{

	union
	{
		struct
		{
			double x, y;
		};
		double xy[2];
	};
};

// @formatter:off
__forceinline struct vec2 vec2_create_2(double x, double y) { struct vec2 v = { x, y }; return v; }
__forceinline struct vec2 vec2_create(double value) { return vec2_create_2(value, value); }
__forceinline struct vec2 vec2_negate(struct vec2 vec) { vec.x = -vec.x; vec.y = -vec.y; return vec; }
__forceinline struct vec2 vec2_mult(struct vec2 vec, double factor) { vec.x *= factor; vec.y *= factor; return vec; }
__forceinline struct vec2 vec2_mult_2(struct vec2 vec, struct vec2 other) { vec.x *= other.x; vec.y *= other.y; return vec; }
__forceinline struct vec2 vec2_div(struct vec2 vec, double factor) { assert(factor); return vec2_mult(vec, 1 / factor); }
__forceinline struct vec2 vec2_add(struct vec2 vec, double value) { vec.x += value; vec.y += value; return vec; }
__forceinline struct vec2 vec2_add_2(struct vec2 vec, struct vec2 other) { vec.x += other.x; vec.y += other.y; return vec; }
__forceinline struct vec2 vec2_sub(struct vec2 vec, double value) { return vec2_add(vec, -value); }
__forceinline struct vec2 vec2_sub_2(struct vec2 vec, struct vec2 other) { return vec2_add_2(vec, vec2_negate(other)); }
__forceinline double vec2_len(struct vec2 vec) { return sqrt(pow(vec.x, 2) + pow(vec.y, 2)); }
__forceinline struct vec2 vec2_normalize(struct vec2 vec) { double len = vec2_len(vec); return len ? vec2_div(vec, len) : vec2_create(0); }
// @formatter:on
