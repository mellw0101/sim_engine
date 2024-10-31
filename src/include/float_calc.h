#pragma once

#include <cmath>
#include <float.h>

#define __float \
  __inline__ constexpr float __attribute((__always_inline__, __nodebug__, __nothrow__, __warn_unused_result__, __const__))

__float square(float value) {
  return (value * value);
} 

__float calc_magnitude(float x, float y) {
  return sqrt((x * x) + (y * y));
}