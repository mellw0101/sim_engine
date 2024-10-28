#include "defenitions.h"
#include "calc.h"

extern SDL_Window   *win;
extern char         *window_title;
extern Uchar         config_timestamp;
extern float           window_width;
extern float           window_height;
extern Object       *objects_head;
extern Object       *objects_tail;
extern Engine       *engine;
extern Player       *player;

/* main.cpp */
void physics(void);

/* 'vector.c' */
Vector vector_multi_copy(const Vector *vector, const double scalar);
Vector vector_multi(Vector *vector, const double scalar);
Vector vector_add(Vector *vector, const Vector *add);
void   vector_set_val(Vector *vector, const double x, const double y);

/* 'object.c' */
Object *object_create(const Vector &pos, const Ushort width, const Ushort height, const Vector &friction, Uint state,
                      const float max_speed);
void unlink_object(Object *obj);

/* 'engine.c' */
void init(void);
void cleanup(void);
void clear_frame(void);
void update_frame(void);
void poll_for_events(void);
void prosses_keys(void);
void engine_create(void);

/* 'utils.c' */

/* player.cpp */
void player_create(void) noexcept;

/* errors */
void __attribute__((__error__("Parameter cannot be 0"))) error_on_param_zero(void);