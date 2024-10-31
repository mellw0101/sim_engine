#pragma once

/* clang-format off */
#include <SDL2/SDL_rect.h>
#include <cmath>

#include "float_calc.h"

namespace /* Vector helper defines. */ {
  /* Some calculation helpers. */
  #define RADIANT_F(angle) \
    (angle * (M_PI / 180.0f))
  /* Some attributes macro helpers. */
  #define __warn __attribute__((__warn_unused_result__))
  /* Internal return macro helpers. */
  #define __Vector_INLINE__(type) __inline__ constexpr type __attribute__((__always_inline__, __nodebug__, __nothrow__)) 
  #define __Vector_STATIC__(type) static __inline__ constexpr type __attribute__((__always_inline__, __nodebug__, __nothrow__))
  #define __Vector_copy        __Vector_INLINE__(Vector) __warn
  #define __Vector_STATIC_copy __Vector_STATIC__(Vector) __warn __attribute__((const))
  /* Macros to declare operator return and operator params. */
  #define __const_(type) __inline__ constexpr type __attribute((const, __always_inline__, __nodebug__, __nothrow__))
  #define __operator_double(_op) operator _op(const Vector &vec1, const Vector &vec2)
  #define __operator_single(_op) operator _op(const Vector &vec, float value)
  #define __internal_operator_Vector(_op) operator _op(const Vector &other)
  #define __internal_operator_double(_op) operator _op(float value)
  /* Macros to define internal methods returning a Vector ref. */
  #define __Vector_ref __Vector_INLINE__(Vector &)
  #define __Vector_ref_operator_(_op) \
    __Vector_ref __internal_operator_Vector(_op) { this->x _op other.x; this->y _op other.y; return *this; } \
    __Vector_ref __internal_operator_double(_op) { this->x _op value;   this->y _op value;   return *this; }
  /* Macros to declare and define Vector returning operators. */
  #define __const_Vector __const_(Vector) __warn
  #define __const_Vector_operator_decl(_op) \
    __const_Vector  __operator_double(_op); \
    __const_Vector __operator_single(_op)
  #define __const_Vector_operator_def(_op) \
    __const_Vector __operator_double(_op) { return {vec1.x _op vec2.x, vec1.y _op vec2.y}; } \
    __const_Vector __operator_single(_op) { return {vec.x  _op value,  vec.y  _op value};  }
  /* Macros to declare and define bool returning operators. */
  #define __const_bool __const_(bool) __warn
  #define __const_bool_operator_decl(_op) \
    __const_bool __operator_double(_op); \
    __const_bool __operator_single(_op)
  #define __const_bool_operator_def(_op) \
    __const_bool __operator_double(_op) { return (vec1.x _op vec2.x && vec1.y _op vec2.y); } \
    __const_bool __operator_single(_op) { return (vec.x  _op value  && vec.y  _op value);  }
}

/* Forward decl. */
typedef struct Vector Vector;

/* Operator decl. */
__const_Vector_operator_decl(+);
__const_Vector_operator_decl(-);
__const_Vector_operator_decl(*);
__const_Vector_operator_decl(/);
__const_bool_operator_decl(<);
__const_bool_operator_decl(>);
__const_bool_operator_decl(<=);
__const_bool_operator_decl(>=);
__const_bool_operator_decl(==);
__const_bool_operator_decl(!=);

/* -------------- */
/* <<- Vector ->> */
/* -------------- */
typedef struct Vector {
  /* ------------------- */
  /* <<- Vector data ->> */
  /* ------------------- */
  float x;
  float y;

  /* ------------------------- */
  /* <<- Vector operator`s ->> */
  /* ------------------------- */
  __Vector_ref_operator_(*=);
  __Vector_ref_operator_(+=);
  __Vector_ref_operator_(-=);
  __Vector_ref_operator_(=);

  /* ----------------------- */
  /* <<- Vector method`s ->> */
  /* ----------------------- */
  __Vector_ref set(float x, float y) {
    *this = {x, y};
    return *this;
  }

  __Vector_ref accel(float x, float y, float delta_t) {
    *this += {(x * delta_t), (y * delta_t)};
    return *this;
  }

  void accel_x(const float x, const float delta_t) noexcept;
  void accel_y(const float y, const float delta_t) noexcept;
  void constrain(bool positive, const Vector &vec) noexcept;
  void adjust_angle(float angle);

  __Vector_ref set_magnitude_and_angle(float magnitude, float angle) {
    const float radians = RADIANT_F(angle);
    x = (magnitude * cos(radians));
    y = (magnitude * sin(radians));
    return *this;
  }

  __const_(Vector) direction_to(const Vector &other, bool perp = false) const {
    return perp ? Vector{-(other.y - y), (other.x - x)} : Vector{(other.x - x), (other.y - y)};
  }

  __const_(float) magnitude(void) const {
    return sqrt((x * x) + (y * y));
  }

  __const_(float) dot(const Vector &other) const {
    return ((x * other.x) + (y * other.y)); 
  }

  __const_(Vector) normalize(void) const {
    return {*this / magnitude()};
  }

  __const_(SDL_FPoint) FPoint(void) const {
    return {x, y};
  }

  __Vector_STATIC_copy from(const SDL_FPoint &point) {
    return {point.x, point.y};
  }

  __Vector_STATIC_copy center(const Vector &vec1, const Vector &vec2) {
    return {(vec1 + vec2) / 2};
  }

  __Vector_STATIC_copy center(const SDL_FPoint &p1, const SDL_FPoint &p2) {
    return {(from(p1) + from(p2)) / 2};
  }

  __Vector_STATIC_copy center_with_offset(const Vector& p1, const Vector &p2, const Vector &offset) {
    /* Calculate center point. */
    Vector ret = center(p1, p2);
    if (offset.x) {
      const Vector direction = (p1.direction_to(p2, true).normalize() *= offset.x);
      ret += direction;
    }
    if (offset.y) {
      const Vector direction = (p1.direction_to(p2).normalize() *= offset.y);
      ret += direction; 
    }
    return ret;
  }

  __Vector_STATIC_copy center_with_offset(const SDL_FPoint& p1, const SDL_FPoint&p2, const Vector &offset) {
    Vector ret = center(p1, p2);
    const Vector vec1 = from(p1);
    const Vector vec2 = from(p2);
    if (offset.x) {
      const Vector direction = (vec1.direction_to(vec2, true).normalize() *= offset.x);
      ret += direction;
    }
    if (offset.y) {
      const Vector direction = (vec1.direction_to(vec2).normalize() *= offset.y);
      ret += direction; 
    }
    return ret;
  }
} Vector;

/* Operator def. */
__const_Vector_operator_def(+);
__const_Vector_operator_def(-);
__const_Vector_operator_def(*);
__const_Vector_operator_def(/);
__const_bool_operator_def(<);
__const_bool_operator_def(>);
__const_bool_operator_def(<=)
__const_bool_operator_def(>=)
__const_bool_operator_def(==)
__const_bool_operator_def(!=)