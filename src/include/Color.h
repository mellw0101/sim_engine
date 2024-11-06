#pragma once

/* clang-format off */

#include <Mlib/def.h>
#include <SDL2/SDL_pixels.h>

#include <Mlib/float_calc.h>

namespace /* Defines. */ {
  #define __Color_OPERATOR(type) __inline__ constexpr operator type __attribute__((__const__, __always_inline__, __nothrow__, __nodebug__))
  #define __Color_OPERATOR_RET(ret, type) __inline__ constexpr ret operator type __attribute__((__const__, __always_inline__, __nothrow__, __nodebug__))
  #define __Color_CONST(type) __inline__ constexpr type __attribute__((__const__, __always_inline__, __nodebug__, __nothrow__))
  #define __Color_INLINE(type) __inline__ constexpr type __attribute__((__always_inline__, __nodebug__, __nothrow__)) 
  #define __Color_STATIC(type) static __Color_INLINE(type)
  #define __Color_REF __Color_INLINE(Color &)
  #define __Color_CONSTRUCTOR(...) __inline__ constexpr Color(__VA_ARGS__) __attribute__((__always_inline__, __nodebug__, __nothrow__))
  #define __Color_SET(chanel) __Color_REF set_##chanel(float value) { chanel = clampf(value, 0.0f, 255.0f);    return *this; }
  #define __Color_ADD(chanel) __Color_REF add_##chanel(float value) { chanel = maxf((chanel + value), 255.0f); return *this; }
  #define __Color_SUB(chanel) __Color_REF sub_##chanel(float value) { chanel = minf((chanel - value), 0.0f  ); return *this; }
}

typedef struct Color {
  float r, g, b, a;

  __Color_CONSTRUCTOR(void) : r(0), g(0), b(0), a(255) {}

  __Color_CONSTRUCTOR(Uchar r, Uchar g, Uchar b, Uchar a) :
    r(clampf(r, 0.0f, 255.0f)),
    g(clampf(g, 0.0f, 255.0f)),
    b(clampf(b, 0.0f, 255.0f)),
    a(clampf(a, 0.0f, 255.0f))
  {}

  __Color_OPERATOR(SDL_Color(void) const) {
    return {(Uchar)r, (Uchar)g, (Uchar)b, (Uchar)a};
  }

  __Color_REF set_from_temp(float temp) {
    if (temp <= 300.0f) {
      return *this;
    }
    float kelvin = (temp + 273.15f);
    set_r((kelvin - 300.0f) * 1.1f);
    return *this;
  }

  __Color_REF add_gray(float value) {
    if ((r != 255.0f) && (g != 255.0f) && (b != 255.0f)) {
      r = maxf((r + value), 255.0f);
      g = maxf((g + value), 255.0f);
      b = maxf((b + value), 255.0f);
    }
    return *this;
  }
  
  __Color_REF sub_gray(float value) {
    if ((r != 0.0f) && (g != 0.0f) && (b != 0.0f)) {
      r = minf((r - value), 0.0f);
      g = minf((g - value), 0.0f);
      b = minf((b - value), 0.0f);
    }
    return *this;  
  }

  __Color_SET(r);
  __Color_SET(g);
  __Color_SET(b);
  __Color_SET(a);

  __Color_ADD(r);
  __Color_ADD(g);
  __Color_ADD(b);
  __Color_ADD(a);

  __Color_SUB(r);
  __Color_SUB(g);
  __Color_SUB(b);
  __Color_SUB(a);
} Color;

namespace /* Undef defines. */ {
  #undef __Color_OPERATOR
  #undef __Color_OPERATOR_RET
  #undef __Color_CONST
  #undef __Color_INLINE
  #undef __Color_STATIC
  #undef __Color_REF
  #undef __Color_CONSTRUCTOR
}
