//
// Created by César B. on 12/14/2023.
//

#pragma once

#define DO_WHILE_FALSE(...) \
do                          \
{                           \
    __VA_ARGS__             \
}                           \
while(false)

