#include <Mlib/Flag.h>
#include <Mlib/Vector.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <chrono>
#include <functional>
#include <map>
#include <stdbool.h>
#include <stdio.h>
#include <thread>

using std::function;
using std::map;
using std::milli;
using std::chrono::duration;
using std::chrono::high_resolution_clock;
using std::chrono::time_point;

#define FALSE 0
#define true  1

#define Uchar  unsigned char
#define Ushort unsigned short
#define Uint   unsigned int
#define Ulong  unsigned long

#define __attr_const __attribute((const))

#define GRAVITY           (9.806650000000001)
#define FPS               (100.00)
#define FRAMETIME_S       (1.00 / FPS)
#define TIME_STEP         (40)
#define TIME_STEP_S       (FRAMETIME_S / TIME_STEP)
#define PIXELS_PER_METER  (100.00)
#define PIXEL_TO_M(pixel) (pixel / PIXELS_PER_METER)
#define M_TO_PIXEL(m)     (m * PIXELS_PER_METER)

/* clang-format off */

/* Some def`s for calculating things. */
#define ACCEL(v, a, delta_t) \
  (v = v + (a * delta_t))

#define POS(p, v, delta_t) \
  (p = p + (v * delta_t))

#define MAX(val, max) \
  (val = (val > max) ? max : val)

#define MIN(val, min) \
  (val = (val < min) ? min : val)

#define CLAMP(val, min, max) \
  (val = (val < min) ? min : (val > max) ? max : val)

/* Def`s to help with objects. */

/* If condition is 'true' then set value to 0.00. */
#define CONSTRAIN_VEL_IF(cond, val) \
  ((cond) ? (val = 0.00) : 0);

/* This macro is used to confine an object within a reqtangle. */
#define HIT_WALL(o, min_x, max_x, min_y, max_y) \
  CLAMP((o)->pos.x, min_x, max_x);                                           \
  ((o)->pos.x == min_x) ? ((o)->vel.x < 0.00) ? ((o)->vel.x = 0.00) : 0 : 0; \
  ((o)->pos.x == max_x) ? ((o)->vel.x > 0.00) ? ((o)->vel.x = 0.00) : 0 : 0; \
  CLAMP((o)->pos.y, min_y, max_y);                                           \
  ((o)->pos.y == min_y) ? ((o)->vel.y < 0.00) ? ((o)->vel.y = 0.00) : 0 : 0; \
  ((o)->pos.y == max_y) ? ((o)->vel.y > 0.00) ? ((o)->vel.y = 0.00) : 0 : 0

#define PRINT_VEL_POS(vx, vy, px, py) \
  printf("Vel:(%lf m/s, %lf m/s) Pos:(%lf x, %lf y).\n", vx, vy, px, py)

#define PRINT_OBJ_VEL_POS(o) \
  PRINT_VEL_POS((o)->vel.x, (o)->vel.y, (o)->pos.x, (o)->pos.y)

#define PRINT_ACCEL_POINT(p) \
  printf("Point:(%lf pix/s, %lf pix/s, time: %lf).\n", p->vel.x, p->vel.y, p->time)

#define OBJ_L(o) ((o)->pos.x)
#define OBJ_R(o) ((o)->pos.x + (o)->width)
#define OBJ_T(o) ((o)->pos.y)
#define OBJ_B(o) ((o)->pos.y + (o)->height)

#define OBJECTS_COLLIDING(o, so) \
  OBJ_R(o) >= OBJ_L(so) && \
  OBJ_L(o) <= OBJ_R(so) && \
  OBJ_B(o) >= OBJ_T(so) && \
  OBJ_T(o) <= OBJ_B(so)

#define OBJ_OVERLAP_L(o, so) (OBJ_R(o)  - OBJ_L(so))
#define OBJ_OVERLAP_R(o, so) (OBJ_R(so) - OBJ_L(o))
#define OBJ_OVERLAP_T(o, so) (OBJ_B(o)  - OBJ_T(so))
#define OBJ_OVERLAP_B(o, so) (OBJ_B(so) - OBJ_T(o))

#define PRINT_OVERLAP(o, so) \
  printf("OBJ_OVERLAP_L: %f, OBJ_OVERLAP_R: %f, OBJ_OVERLAP_T: %f, OBJ_OVERLAP_B: %f.\n", \
          OBJ_OVERLAP_L(o, so), OBJ_OVERLAP_R(o, so), OBJ_OVERLAP_T(o, so), OBJ_OVERLAP_B(o, so))

#define OBJ_OVERLAP_LEAST_L(o, so) \
  OBJ_OVERLAP_L(o, so) < OBJ_OVERLAP_R(o, so) && \
  OBJ_OVERLAP_L(o, so) < OBJ_OVERLAP_T(o, so) && \
  OBJ_OVERLAP_L(o, so) < OBJ_OVERLAP_B(o, so)

#define OBJ_OVERLAP_LEAST_R(o, so) \
  OBJ_OVERLAP_R(o, so) < OBJ_OVERLAP_L(o, so) && \
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

#define IS_ON_STATIC_OBJECT(o) \
  for (Object *so = object_head; so; so = so->next) { \
    if ((o == so) || (so->state & STATIC) == FALSE) { \
      continue;                                       \
    }                                                 \
    if (OBJECTS_COLLIDING(o, so)) {                   \
      if (OBJ_OVERLAP_LEAST_T(o, so)) {               \
        ((o)->state |= ON_STATIC_OBJECT);             \
      }                                               \
      else if (OBJ_OVERLAP_LEAST_B(o, so)) {          \
        (o)->state &= ~ON_STATIC_OBJECT;              \
      }                                               \
      else if (OBJ_OVERLAP_LEAST_L(o, so)) {          \
        (o)->state &= ~ON_STATIC_OBJECT;              \
      }                                               \
      else if (OBJ_OVERLAP_LEAST_R(o, so)) {          \
        (o)->state &= ~ON_STATIC_OBJECT;              \
      }                                               \
    }                                                 \
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

typedef struct Vector {
  double x;
  double y;

  Vector &operator*=(const Vector &other);
  Vector &operator*=(const double scalar);

  void set(const double x, const double y);
  void accel(const double accel_x, const double accel_y, const double delta_t) noexcept;
} Vector;

typedef struct AccelPoint {
  Vector                vel;
  double                time;
  struct AccelPoint *next;
} AccelPoint;

typedef enum {
  OBJECT_MOVING
} ObjectFlag;

typedef struct ObjectMovingData {
  short x_positive;
  short x_negative;
  short y_positive;
  short y_negative;
} ObjectMovingData;

typedef struct Object {
  /* Object data. */
  Vector pos;
  Vector vel;
  Vector friction;
  Vector max_speed;
  Ushort width;
  Ushort height;
  Uint   state;
  bit_flag_t<8> flag;
  ObjectMovingData moving_data;
  struct Object *next;
  /* Object methods. */
  void draw(void);
} Object;

typedef enum {
  PLAYER_POSITIVE,
  PLAYER_NEGATIVE,
} PlayerAccelOpt;

typedef enum {
  PLAYER_JUMPING,
  PLAYER_CAN_FLY,
  PLAYER_ON_OBJECT
} PlayerState;

typedef enum {
  PLAYER_UP,
  PLAYER_DOWN,
  PLAYER_LEFT,
  PLAYER_RIGHT,
  PLAYER_UP_RIGHT,
  PLAYER_UP_LEFT,
  PLAYER_DOWN_RIGHT,
  PLAYER_DOWN_LEFT
} PlayerDirection;

typedef struct Player {
  Vector pos;
  Vector vel;
  Vector accel;
  Vector max_speed;
  int width;
  int height;
  bit_flag_t<8> state;
  bit_flag_t<8> direction;

  void draw(void);
  void check_collisions(void);
  void jump(void);
  void calculate_pos_change(double __attr_const delta_t) noexcept;
  void calculate_accel_change(PlayerAccelOpt __attr_const opt, double __attr_const delta_t) noexcept;
} Player;

typedef enum {
  ENGINE_RUNNING
} EngineState;

class Engine {
 private:
  time_point<high_resolution_clock> _frame_st;
  map<Uint, MVector<function<void(SDL_Event)>>> _event_map;
  SDL_Event _ev;

 public:
  bit_flag_t<8> state;

  void frame_start(void);
  void frame_end(void);
  void poll_events(void);
  template <typename Callback>
  void add_event_callback(Uint event, Callback &&callback) {
    _event_map[event].push_back(std::forward<Callback>(callback));
  }
};