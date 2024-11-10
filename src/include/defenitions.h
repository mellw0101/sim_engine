#pragma once

#include <Mlib/Array.h>
#include <Mlib/Flag.h>
#include <Mlib/MVec2.h>
#include <Mlib/Pair.h>
#include <Mlib/openGL/context.h>
#include <Mlib/openGL/shader.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <algorithm>
#include <chrono>
#include <functional>
#include <map>
#include <numeric>
#include <stdbool.h>
#include <stdio.h>
#include <thread>
#include <immintrin.h> // AVX intrinsics

#include "Color.h"
#include "Particle.h"

using std::function;
using std::map;
using std::milli;
using std::chrono::duration;
using std::chrono::high_resolution_clock;
using std::chrono::time_point;

inline namespace Defines {
#define FALSE 0
#define true  1

#define Uchar  unsigned char
#define Ushort unsigned short
#define Uint   unsigned int
#define Ulong  unsigned long

#define __attr_const __attribute((const))

#define BLACK 0, 0, 0, 255
#define WHITE 255, 255, 255, 255
#define RED   255, 0, 0, 255
#define GREEN 0, 255, 0, 255
#define BLUE  0, 0, 255, 255

#define GRAY_80              80, 80, 80, 255
#define GRAY_80_WRED(amount) (Uchar)(80 + amount), 80, 80, 255

#define FPS               (240.00)
#define FRAMETIME_S       (1.00 / FPS)
#define TIME_STEP         (40)
#define TIME_STEP_S       (FRAMETIME_S / TIME_STEP)
#define PIXELS_PER_METER  (100.00f)
#define PIXEL_TO_M(pixel) (pixel / PIXELS_PER_METER)
#define M_TO_PIXEL(m)     ((m) * PIXELS_PER_METER)

#define SEA_LEVEL_PRESSURE     (101325.0f)  /* Pa */
#define GAS_CONSTANT_AIR       (287.05f)    /* J/(kg·K) */
#define TEMPERATURE_LAPSE_RATE (0.0065f)    /* K/m */
#define SEA_LEVEL_TEMPERATURE  (288.15f)    /* K (15°C) */
#define MOLAR_MASS_AIR         (0.0289644f) /* kg/mol */
#define C_TO_KELVIN(c)         (c + 273.15f)

#define STEEL_DENSITY       7850.0f
#define STEEL_HEAT_CAPACITY 500.0f

#define ROUND_7_62_GRAINS 25.0f

#define FLOAT_MAX std::numeric_limits<float>::max()
#define FLOAT_MIN std::numeric_limits<float>::lowest()

  /* clang-format off */

  /* Some def`s for calculating things. */
  #define ACCEL(v, a, delta_t) \
    (v = v + (a * delta_t))

  #define POS(p, v, delta_t) \
    (p = p + (v * delta_t))

  #define MAX(val, max) \
    (val = (val > max) ? max : val)

  #define MIN(val, min) \
    ((val < min) ? (val = min) : 0)

  #define CLAMP(val, min, max) \
    ((val < min) ? (val = min) : (val > max) ? (val = max) : 0)

  #define RADIANT_F(angle) \
    (angle * (M_PI / 180.0f))

  #define RADIANT_LD(angle) \
    (angle * (M_PI / 180.0L))

  /* Def`s to help with objects. */

  /* If condition is 'true' then set value to 0.00. */
  #define CONSTRAIN_VEL_IF(cond, val) \
    (cond ? (val = 0.00) : 0)

  /* This macro is used to confine an object within a reqtangle. */
  #define HIT_WALL(o, min_x, max_x, min_y, max_y) \
    CLAMP((o)->data.pos.x, min_x, max_x);                                           \
    ((o)->data.pos.x == min_x) ? ((o)->data.vel.x < 0.00) ? ((o)->data.vel.x = 0.00) : 0 : 0; \
    ((o)->data.pos.x == max_x) ? ((o)->data.vel.x > 0.00) ? ((o)->data.vel.x = 0.00) : 0 : 0; \
    CLAMP((o)->data.pos.y, min_y, max_y);                                           \
    ((o)->data.pos.y == min_y) ? ((o)->data.vel.y < 0.00) ? ((o)->data.vel.y = 0.00) : 0 : 0; \
    ((o)->data.pos.y == max_y) ? ((o)->data.vel.y > 0.00) ? ((o)->data.vel.y = 0.00) : 0 : 0

  #define PRINT_VEL_POS(vx, vy, px, py) \
    printf("Vel:(%lf m/s, %lf m/s) Pos:(%lf x, %lf y).\n", vx, vy, px, py)

  #define PRINT_OBJ_VEL_POS(o) \
    PRINT_VEL_POS((o)->vel.x, (o)->vel.y, (o)->pos.x, (o)->pos.y)

  #define PRINT_ACCEL_POINT(p) \
    printf("Point:(%lf pix/s, %lf pix/s, time: %lf).\n", p->vel.x, p->vel.y, p->time)

  #define OBJ_L(o) ((o)->data.pos.x)
  #define OBJ_R(o) ((o)->data.pos.x + (o)->width)
  #define OBJ_T(o) ((o)->data.pos.y)
  #define OBJ_B(o) ((o)->data.pos.y + (o)->height)

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

  /* Itherate thrue all objects. */
  #define FOR_EACH_OBJECT(name) \
    for (Object *name = objects_head; name; name = name->next)

  #define OBJECT_MOVING_POSITIVE true
  #define OBJECT_MOVING_NEGATIVE false

  #define NO_STATE              0
  #define STATIC                (1 << 0)
  #define ON_STATIC_OBJECT      (1 << 1)
  #define INCREACE_MAX_SPEED    (1 << 2)
  #define DECREACE_MAX_SPEED    (1 << 3)
  #define INCREACE_ACCELERATION (1 << 4)
  #define DECREACE_ACCELERATION (1 << 5)
  #define FLYING_ENABLED        (1 << 6)
  #define JUMP                  (1 << 7)
}

typedef struct {
  MVec2 pos;
  MVec2 vel;
  MVec2 accel;
} PosData;

typedef struct Camera {
  MVec2 pos;
} Camera;

/* -------------- */
/* <<- Object ->> */
/* -------------- */
typedef enum {
  OBJECT_MOVING,
  OBJECT_HAS_COLLISION_ACTION,
  OBJECT_HAS_GRAVITY,
  OBJECT_IS_PROJECTILE
} ObjectFlag;

typedef struct ObjectMovingData {
  double speed;
  MVec2 positive;
  MVec2 negative;
  bool direction;
} ObjectMovingData;

typedef struct Object {
  /* ------------------- */
  /* <<- Object data ->> */
  /* ------------------- */
  PosData data;
  MVec2 friction;
  MVec2 max_speed;
  Ushort width;
  Ushort height;
  Uint   state;
  bit_flag_t<8> flag;
  ObjectMovingData moving_data;
  function<void(void)> collision_action;
  Object *next;
  Object *prev;

  /* ----------------------- */
  /* <<- Object method`s ->> */
  /* ----------------------- */
  void draw(void);
  void move(void) noexcept;
  void calculate_pos_change(float delta_t) noexcept;

  /* -------------------------------- */
  /* <<- Object template method`s ->> */
  /* -------------------------------- */
  template <typename Callback>
  __inline__ void __attribute((__always_inline__))
  set_collision_action(Callback &&callback) {
    collision_action = std::forward<Callback>(callback);
    flag.set<OBJECT_HAS_COLLISION_ACTION>();
  }
} Object;

/* Projectile */
typedef struct {
  PosData data;
  float width;
  float height;
} Projectile;

/* -------------------- */
/* <<- PlayerWeapon ->> */
/* -------------------- */
typedef enum {
  PLAYER_WEAPON_FOLLOWS_MOUSE,
  PLAYER_WEAPON_FOLLOWS_MOVEMENT,
  PLAYER_WEAPON_ENABLED
} PlayerWeaponState;

typedef enum {
  PLAYER_WEAPON_TYPE_AK_47
} PlayerWeaponType;

typedef struct PlayerWeaponPart {
  SDL_FPoint points[5];
  SDL_FRect rect;

  void draw(float angle_offset = 0.0f);
} PlayerWeaponPart;

typedef struct PlayerWeapon {
  bit_flag_t<8> state;
  bit_flag_t<8> type;
  float angle;

  PlayerWeaponPart body;

  struct {
    SDL_FRect rect;
    SDL_FPoint points[4];
    Color color[4];
    float temp;
    float length;
    float mass;
  } barrel;
  
  void init(void);
  void equip(PlayerWeaponType weapon);
  void draw_body(void);
  void draw_barrel(void);
  void draw_magazine(void);
  void draw_handle(void);
  void draw(void);
  void attack(int button);
} PlayerWeapon;

/* -------------- */
/* <<- Player ->> */
/* -------------- */
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
  /* ------------------- */
  /* <<- Player data ->> */
  /* ------------------- */
  PosData data;
  MVec2 accel;
  MVec2 max_speed;
  float width;
  float height;
  bit_flag_t<8> state;
  bit_flag_t<8> direction;
  PlayerWeapon weapon;

  /* ----------------------- */
  /* <<- Player method`s ->> */
  /* ----------------------- */
  void draw(void);
  void check_collision_with(const Object *const object) noexcept;
  void check_collisions(void);
  void jump(void) noexcept;
  void calculate_pos_change(float delta_t) noexcept;
  void determine_control_direction(void) noexcept;
} Player;

/* ----------------------- */
/* <-< PerformanceData >-> */
/* ----------------------- */
typedef struct PerformanceData {
  typedef struct PerformanceMetric {
    MVector<double> data;

    double mean(void);
    double min(void);
    double max(void);
  } PerformanceMetric;

  /* ---------------------------- */
  /* <-< PerformanceData data >-> */
  /* ---------------------------- */
  Ulong framecount;
  PerformanceMetric frametime;
  PerformanceMetric added_frametime;

  /* -------------------------------- */
  /* <-< PerformanceData method`s >-> */
  /* -------------------------------- */
  void print_report(void) noexcept;
} PerformanceData;

/* ---------------- */
/* <-< Renderer >-> */
/* ---------------- */
typedef struct Renderer {
  /* Types. */
  typedef enum {TL, TR, BR, BL} RendererCorner;

  /* --------------------- */
  /* <-< Renderer data >-> */
  /* --------------------- */
  SDL_Renderer *ren;

  /* ------------------------- */
  /* <-< Renderer method`s >-> */
  /* ------------------------- */
  void set_color(Uchar r, Uchar g, Uchar b, Uchar a);
  void fill_rect(const SDL_Rect *rect);
  void fill_rect(const SDL_FRect *rect);
  void draw_lines(const SDL_FPoint *const &points, int count);
  void draw_rotated_rect(float cx, float cy, float width, float height, float angle, SDL_FPoint *points);
  void draw_rotated_box(float cx, float cy, float width, float height, float angle,
                        const Color *colors = nullptr, SDL_FPoint *points = nullptr, SDL_Texture *texture = nullptr);
  void draw_triangle(SDL_FPoint p1, SDL_FPoint p2, SDL_FPoint p3, const Color *colors = nullptr, SDL_Texture *texture = nullptr);
  void draw_filled_arc(SDL_FPoint *points, Uint num_points, Color color, const MVec2 &offset, SDL_Texture *texture = nullptr);
  void draw_quadliteral(const SDL_FPoint points[4], const Color colors[4], SDL_Texture *texture = nullptr);
} Renderer;

/* ----------------- */
/* <<- MouseData ->> */
/* ----------------- */
typedef enum {
  MOUSE_STATE_LEFT_HELD,
  MOUSE_STATE_RIGHT_HELD
} MouseState;

typedef struct MouseData {
  float x;
  float y;
  bit_flag_t<8> state;
} MouseData;

/* -------------- */
/* <<- Engine ->> */
/* -------------- */
typedef enum {
  ENGINE_RUNNING
} EngineState;

typedef enum {
  ENGINE_RENDER_COLOR_BLACK,
  ENGINE_RENDER_COLOR_WHITE,
} EngineRenderColor;

class Engine {
 private:
  time_point<high_resolution_clock> _frame_st;
  map<Uint, MVector<function<void(SDL_Event)>>> _event_map;
  SDL_Event _ev;

 public:
  struct {
    float temp;
  } enviroment;

  bit_flag_t<8> state;
  PerformanceData performance_data;
  Renderer ren;
  MouseData mouse_data;
  Camera camera;

  void frame_start(void);
  void frame_end(void);
  void poll_events(void);
  void prosses_key_states(void);
  void run(void);

  template <typename Callback>
  void add_event_callback(Uint event, Callback &&callback) {
    _event_map[event].push_back(std::forward<Callback>(callback));
  }
};