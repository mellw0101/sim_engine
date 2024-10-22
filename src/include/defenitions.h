#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdio.h>

#define FALSE 0
#define TRUE  1

#define Uchar  unsigned char
#define Ushort unsigned short
#define Uint   unsigned int
#define Ulong  unsigned long

#define GRAVITY           9.81
#define FPS               120.00
#define FRAMETIME_S       (1.00 / FPS)
#define TIME_STEP         10
#define TIME_STEP_S       (FRAMETIME_S / TIME_STEP)
#define PIXELS_PER_METER  100.00
#define PIXEL_TO_M(pixel) (pixel / PIXELS_PER_METER)
#define M_TO_PIXEL(m)     (m * PIXELS_PER_METER)

/* Some def`s for calculating things. */
#define ACCEL(velocity, accelaration, delta_t) velocity = velocity + (accelaration * delta_t)
#define POS(position, velocity, delta_t)       position = position + (velocity * delta_t);
#define CLAMP(value, min, max)                 (value = (value < min) ? min : (value > max) ? max : value)

/* Def`s to help with objects. */

/* If condition is 'TRUE' then set value to 0.00. */
#define CONSTRAIN_VEL_IF(condition, value) (condition) ? (value = 0.00) : 0;
/* This macro is used to confine an object within a reqtangle. */
#define HIT_WALL(object, min_x, max_x, min_y, max_y)                                        \
  CLAMP((object)->pos.x, min_x, max_x);                                                     \
  ((object)->pos.x == min_x) ? ((object)->vel.x < 0.00) ? ((object)->vel.x = 0.00) : 0 : 0; \
  ((object)->pos.x == max_x) ? ((object)->vel.x > 0.00) ? ((object)->vel.x = 0.00) : 0 : 0; \
  CLAMP((object)->pos.y, min_y, max_y);                                                     \
  ((object)->pos.y == min_y) ? ((object)->vel.y < 0.00) ? ((object)->vel.y = 0.00) : 0 : 0; \
  ((object)->pos.y == max_y) ? ((object)->vel.y > 0.00) ? ((object)->vel.y = 0.00) : 0 : 0

/* clang-format off */
#define PRINT_VEL_POS(vel_x, vel_y, pos_x, pos_y) \
  printf("Vel:(%lf m/s, %lf m/s) Pos:(%lf x, %lf y).\n", vel_x, vel_y, pos_x, pos_y)

#define PRINT_OBJ_VEL_POS(object) \
  PRINT_VEL_POS((object)->vel.x, (object)->vel.y, (object)->pos.x, (object)->pos.y)

#define PRINT_ACCEL_POINT(point) \
  printf("Point:(%lf pix/s, %lf pix/s, time: %lf).\n", point->vel.x, point->vel.y, point->time)

#define OBJ_L(o) ((o)->pos.x)
#define OBJ_R(o) ((o)->pos.x + (o)->width)
#define OBJ_T(o) ((o)->pos.y)
#define OBJ_B(o) ((o)->pos.y + (o)->height)

#define OBJECTS_COLLIDING(o, so) \
  OBJ_R(o) >= OBJ_L(so) && \
  OBJ_L(o) <= OBJ_R(so) && \
  OBJ_B(o) >= OBJ_T(so) && \
  OBJ_T(o) <= OBJ_B(so)

#define OBJ_OVERLAP_L(o, so)  (OBJ_R(o)  - OBJ_L(so))
#define OBJ_OVERLAP_R(o, so) (OBJ_R(so) - OBJ_L(o))
#define OBJ_OVERLAP_T(o, so)   (OBJ_B(o)    - OBJ_T(so))
#define OBJ_OVERLAP_B(o, so)   (OBJ_B(so)   - OBJ_T(o))

#define PRINT_OVERLAP(object, so) \
  printf("OBJ_OVERLAP_L: %f, OBJ_OVERLAP_R: %f,                 \
          OBJ_OVERLAP_T: %f, OBJ_OVERLAP_B: %f.\n",             \
          OBJ_OVERLAP_L(object, so), OBJ_OVERLAP_R(object, so), \
          OBJ_OVERLAP_T(object, so), OBJ_OVERLAP_B(object, so))

#define OBJ_OVERLAP_LEAST_L(o, so) \
  OBJ_OVERLAP_L(o, so) < OBJ_OVERLAP_R(o, so) && \
  OBJ_OVERLAP_L(o, so) < OBJ_OVERLAP_T(o, so) && \
  OBJ_OVERLAP_L(o, so) < OBJ_OVERLAP_B(o, so)

#define OBJ_OVERLAP_LEAST_R(o, so) \
  OBJ_OVERLAP_R(o, so) < OBJ_OVERLAP_L(o, so) &&  \
  OBJ_OVERLAP_R(o, so) < OBJ_OVERLAP_T(o, so) && \
  OBJ_OVERLAP_R(o, so) < OBJ_OVERLAP_B(o, so)

#define OBJ_OVERLAP_LEAST_T(o, so) \
  OBJ_OVERLAP_T(o, so) < OBJ_OVERLAP_B(o, so) && \
  OBJ_OVERLAP_T(o, so) < OBJ_OVERLAP_L(o, so) && \
  OBJ_OVERLAP_T(o, so) < OBJ_OVERLAP_R(o, so)

#define OBJ_OVERLAP_LEAST_B(o, so) \
  OBJ_OVERLAP_B(o, so) < OBJ_OVERLAP_T(o, so) && \
  OBJ_OVERLAP_B(o, so) < OBJ_OVERLAP_L(o, so) && \
  OBJ_OVERLAP_B(o, so) < OBJ_OVERLAP_R(o, so)
/* clang-format off */

#define IS_ON_STATIC_OBJECT(o) \
  for (Object *so = object_head; so; so = so->next) { \
    if ((o == so) || (so->state & STATIC) == FALSE) {   \
      continue;                                         \
    }                                                   \
    if (OBJECTS_COLLIDING(o, so)) {                     \
      if (OBJ_OVERLAP_LEAST_T(o, so)) {                 \
        ((o)->state |= ON_STATIC_OBJECT);               \
      }                                                 \
      else if (OBJ_OVERLAP_LEAST_B(o, so)) {            \
        (o)->state &= ~ON_STATIC_OBJECT;                \
      }                                                 \
      else if (OBJ_OVERLAP_LEAST_L(o, so)) {         \
        (o)->state &= ~ON_STATIC_OBJECT;                \
      }                                                 \
      else if (OBJ_OVERLAP_LEAST_R(o, so)) {            \
        (o)->state &= ~ON_STATIC_OBJECT;                \
      }                                                 \
    }                                                   \
  }

#define NO_STATE              0
#define STATIC                (1 << 0)
#define ON_STATIC_OBJECT      (1 << 1)
#define INCREACE_MAX_SPEED    (1 << 2)
#define DECREACE_MAX_SPEED    (1 << 3)
#define INCREACE_ACCELERATION (1 << 4)
#define DECREACE_ACCELERATION (1 << 5)
#define FLYING_ENABLED        (1 << 6)
#define JUMP                  (1 << 7)

typedef struct {
  double x;
  double y;
} Vector;

typedef struct AccelPoint {
  Vector                vel;
  double                time;
  struct AccelPoint *next;
} AccelPoint;

typedef struct Object {
  Vector pos;
  Vector vel;
  Vector accel;
  Vector max_speed;
  Ushort width;
  Ushort height;
  Uint   state;
  struct Object *next;
} Object;
