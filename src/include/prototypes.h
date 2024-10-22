#include "defenitions.h"

extern char         *window_title;
extern int           window_width;
extern int           window_height;
extern SDL_Window   *window;
extern SDL_Renderer *renderer;
extern SDL_Event     event;
extern bool          running;
extern Object     *object_head;
extern Object     *Objectail;
extern unsigned char config_timestamp;
extern SDL_Texture  *text_texture;

/* 'vector.c' */
Vector vector_multi_copy(const Vector *vector, const double scalar);
Vector vector_multi(Vector *vector, const double scalar);
Vector vector_add(Vector *vector, const Vector *add);
void   vector_set_val(Vector *vector, const double x, const double y);

/* 'object.c' */
Object *object_create(const double pos_x, const double pos_y, const unsigned short width, const unsigned short height,
                        const double accel_x, const double accel_y, unsigned int state, const double max_speed);
SDL_Rect  object_rect(Object *const *object);
void      object_draw(Object *const *object);
void      object_collision_with_static_obj(Object *const *object, Object *const *static_object);

/* 'engine.c' */
void init(void);
void cleanup(void);
void clear_frame(void);
void update_frame(void);
void poll_for_events(void);
void render_text(void);
void prosses_keys(void);

/* 'utils.c' */
AccelPoint *create_accel_point(double vel_x, double vel_y, double time, AccelPoint *last_accel_point);
