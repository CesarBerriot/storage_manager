//
// Created by César B. on 1/18/2024.
//

#pragma once

#include <stdint.h>

void sqrt_table_init(double step, uint32_t max);

void sqrt_table_destroy();

double sqrt_table_approximate(double value);
