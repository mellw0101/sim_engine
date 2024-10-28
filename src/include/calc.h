#pragma once

#include "defenitions.h"

/* clang-format off */

#define __constexpr_calc_ret(type) \
  __inline__ constexpr type __attribute((__always_inline__, __nodebug__, __nothrow__, __warn_unused_result__, __const__))

#define __constexpr_calc_ret_void \
  __inline__ constexpr void __attribute((__always_inline__, __nodebug__, __nothrow__))

#define CALC_RAD_SIN_COS_F(radiant_name, sin_name, cos_name, angle) \
  const float radiant_name = (angle * (M_PI / 180.0f));             \
  const float sin_name = sin(radiant_name);                         \
  const float cos_name = cos(radiant_name)

__constexpr_calc_ret(float) calculate_angle_between_points(float x_1, float y_1, float x_2, float y_2) noexcept {
  const float radians = atan2((y_2 - y_1), (x_2 - x_1));
  const float degrees = (radians * (180.0f / M_PI));
  return degrees;
}

template <float Angle>
__constexpr_calc_ret(float) calculate_radiant(void) {
  return (Angle * (M_PI / 180.0f));
}

__constexpr_calc_ret(float) calculate_radiant(float angle) {
  return (angle * (M_PI / 180.0f));
}

template <float Angle>
__constexpr_calc_ret(float) calculate_sin(void) {
  return sin(calculate_radiant<Angle>());
}

template <float Angle>
__constexpr_calc_ret(float) calculate_cos(void) {
  return cos(calculate_radiant<Angle>);
}

__constexpr_calc_ret_void calculate_rotated_rect(const SDL_FRect *const rect, MArray<SDL_FPoint, 5> *points, float angle) {
  CALC_RAD_SIN_COS_F(radiant, sin_a, cos_a, angle);
  /* Caclculate half width and height */
  const float hw = (rect->w / 2);
  const float hh = (rect->h / 2);
  (*points).ARRIDX(0) = {(rect->x + ((-hw * cos_a) - (-hh * sin_a))), (rect->y + ((-hw *sin_a) + (-hh * cos_a)))};
  (*points).ARRIDX(1) = {(rect->x + (( hw * cos_a) - (-hh * sin_a))), (rect->y + (( hw *sin_a) + (-hh * cos_a)))};
  (*points).ARRIDX(2) = {(rect->x + (( hw * cos_a) - ( hh * sin_a))), (rect->y + (( hw *sin_a) + ( hh * cos_a)))};
  (*points).ARRIDX(3) = {(rect->x + ((-hw * cos_a) - ( hh * sin_a))), (rect->y + ((-hw *sin_a) + ( hh * cos_a)))};
  (*points).ARRIDX(4) = (*points).ARRIDX(0);
}

__constexpr_calc_ret(Vector) calculate_center_of_two_points(float p1_x, float p1_y, float p2_x, float p2_y) {
  return {((p1_x + p2_x) / 2), ((p1_y + p2_y) / 2)};
}


__constexpr_calc_ret(Vector) calculate_center_of_two_points(const Vector &p1, const Vector &p2, const Vector &distance) {
  /* Center point. */
  const Vector center = Vector::center(p1, p2);
  /* Return vector. */
  Vector ret = center;
  if (distance.x) {
    /* Directional vector. */
    const Vector dir_x = p1.direction_to(p2, true);
    /* Normalize directional vector. */
    Vector offset_x = dir_x.normalize();
    /* Calculate offset vector. */
    offset_x *= distance.x;
    ret += offset_x;
  }
  if (distance.y) {
    const Vector dir_y = p1.direction_to(p2, false);
    /* Normalize directional vector. */
    Vector offset_y = dir_y.normalize();
    /* Calculate offset vector. */
    offset_y *= distance.y;
    ret += offset_y;
  }
  return ret;
}

#undef __constexpr_calc_ret