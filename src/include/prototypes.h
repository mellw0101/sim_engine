#include "defenitions.h"

extern SDL_Window   *win;
extern SDL_Renderer *ren;
extern SDL_Event     event;
extern char         *window_title;
extern Uchar         config_timestamp;
extern int           window_width;
extern int           window_height;
extern Object       *objects_head;
extern Object       *objects_tail;
extern Engine       *engine;
extern Player       *player;

/* 'vector.c' */
Vector vector_multi_copy(const Vector *vector, const double scalar);
Vector vector_multi(Vector *vector, const double scalar);
Vector vector_add(Vector *vector, const Vector *add);
void   vector_set_val(Vector *vector, const double x, const double y);

/* 'object.c' */
Object  *object_create(const double pos_x, const double pos_y, const Ushort width, const Ushort height,
                       const double accel_x, const double accel_y, Uint state, const double max_speed);
SDL_Rect object_rect(Object *const *object);
void     object_collision_with_static_obj(Object *const *object, Object *const *static_object);

/* 'engine.c' */
void init(void);
void cleanup(void);
void clear_frame(void);
void update_frame(void);
void poll_for_events(void);
void render_text(void);
void prosses_keys(void);
void engine_create(void);

/* 'utils.c' */
AccelPoint *create_accel_point(double vel_x, double vel_y, double time, AccelPoint *last_accel_point);

/* player.cpp */
void player_create(void) noexcept;

/* errors */
void __attribute__((__error__("Parameter cannot be 0"))) error_on_param_zero(void);