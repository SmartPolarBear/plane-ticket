#pragma once

#include "framework.h"

#include <stdint.h>

#define ARRAY_SIZE(arr) (sizeof((arr))/sizeof((arr)[0]))

/// @brief compute the hash of a string
/// @return 64bit hashing of the string
uint64_t hashing(wchar_t* str);