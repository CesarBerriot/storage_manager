//
// Created by César B. on 1/18/2024.
//

/// fast square root by approximation

#include <malloc.h>
#include <memory.h>
#include <math.h>
#include <assert.h>
#include "sqrt_table.h"

struct g_sqrt_table_struct
{
	double * arr;
	size_t len;
	double step;
	uint32_t max;
} g_sqrt_table;

void sqrt_table_init(double step, uint32_t max)
{
	sqrt_table_destroy();
	g_sqrt_table.step = step;
	g_sqrt_table.max = max;
	g_sqrt_table.len = round((1 / step) * max);
	g_sqrt_table.arr = malloc(g_sqrt_table.len * sizeof(double));
	uint32_t i = 0;
	for(register double value = 0; value < max; (value += step) && ++i)
		g_sqrt_table.arr[i] = sqrt(value);
}

void sqrt_table_destroy()
{
	free(g_sqrt_table.arr);
	memset(&g_sqrt_table, 0, sizeof(struct g_sqrt_table_struct));
}

double sqrt_table_approximate(double value)
{
	assert(value <= g_sqrt_table.max);
	return g_sqrt_table.arr[(size_t)round(value * (g_sqrt_table.len - 1) / g_sqrt_table.max)];
}
