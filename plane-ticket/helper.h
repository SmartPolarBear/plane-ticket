#pragma once

#include "framework.h"

#include <stdint.h>

#define ARRAY_SIZE(arr) (sizeof((arr))/sizeof((arr)[0]))

uint64_t hashing(wchar_t* str);