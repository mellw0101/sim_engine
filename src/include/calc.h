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

__constexpr_calc_ret(MVec2) calculate_bezier_point(const MVec2 &p1, const MVec2 &apex, const MVec2 &p2, float t) {
  return (p1 * ((1 - t) * (1 - t))) + (apex * (2 * (1 - t) * t)) + (p2 * (t * t));
}

__constexpr_calc_ret_void calculate_arc_points(const MVec2 &p1, const MVec2 &p2, const MVec2 &apex, SDL_FPoint *points, Uint num_points) {
  for (Uint i = 0; i < num_points; ++i) {
    float t = ((float)i / (num_points - 1));
    points[i] = calculate_bezier_point(p1, apex, p2, t);
  }
}

__constexpr_calc_ret(MVec2) calculate_center_of_two_points(float p1_x, float p1_y, float p2_x, float p2_y) {
  return {((p1_x + p2_x) / 2), ((p1_y + p2_y) / 2)};
}

__constexpr_calc_ret(MVec2) calculate_center_of_two_points_offset(const MVec2 &p1, const MVec2 &p2, const MVec2 &offset) {
  /* Calculate the center point. */
  MVec2 ret = MVec2::center(p1, p2);
  if (offset.x) {
    /* Calculate perpendiculat distance to offset. */
    const MVec2 distance = (p1.direction_to(p2, true).normalize() *= offset.x);
    ret += distance;
  }
  if (offset.y) {
    /* Calculate inline distance to offset. */
    const MVec2 distance = (p1.direction_to(p2).normalize() *= offset.y);
    ret += distance;
  }
  return ret;
}

__constexpr_calc_ret(float) calculate_air_density(float temp_celsius, float altitude_meters = 0.0f) {
  float temp_kelvin = C_TO_KELVIN(temp_celsius);
  float pressure = (SEA_LEVEL_PRESSURE * pow(1.0f - ((TEMPERATURE_LAPSE_RATE * altitude_meters) / SEA_LEVEL_TEMPERATURE),
                                                  (GRAVITY * MOLAR_MASS_AIR) / (GAS_CONSTANT_AIR * TEMPERATURE_LAPSE_RATE)));
  return (pressure / (GAS_CONSTANT_AIR * temp_kelvin));
}

__constexpr_calc_ret(float) calculate_cross_sectional_area_box(float width, float height) {
  return (width * height);
}

__constexpr_calc_ret(MVec2) calculate_air_resistance(const MVec2 &velocity, float drag_coefficient, float air_density, float cross_sectional_area, float mass) {
  float speed = velocity.magnitude();
  if (speed == 0.0f) {
    return {0.0f, 0.0f};
  }
  float drag_force = (0.5f * drag_coefficient * air_density * cross_sectional_area * speed * speed);
  MVec2 drag_acceleration = velocity.normalize() * (-drag_force / mass);
  return drag_acceleration;
}

#undef __constexpr_calc_ret