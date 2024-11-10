#include "calc.h"
#include "defenitions.h"

extern SDL_Window   *win;
extern SDL_GLContext context;

extern Uint             comp_shader;
extern Uint             SSBO;
extern MVector<PosData> compute_buffer;

extern char   *window_title;
extern Uchar   config_timestamp;
extern float   window_width;
extern float   window_height;
extern Object *objects_head;
extern Object *objects_tail;
extern Engine *engine;
extern Player *player;
extern Cloth  *cloth;

extern MVector<Projectile> projectile;

/* main.cpp */
void physics(void);

/* 'vector.c' */
MVec2 vector_multi_copy(const MVec2 *vector, const double scalar);
MVec2 vector_multi(MVec2 *vector, const double scalar);
MVec2 vector_add(MVec2 *vector, const MVec2 *add);
void  vector_set_val(MVec2 *vector, const double x, const double y);

/* 'object.c' */
Object *object_create(const MVec2 &pos, const Ushort width, const Ushort height, const MVec2 &friction, Uint state,
                      const float max_speed);
void    unlink_object(Object *obj);
void projectile_collisions(void);

/* 'engine.c' */
void init(void);
void cleanup(void);
void clear_frame(void);
void update_frame(void);
void poll_for_events(void);
void prosses_keys(void);
void engine_create(void);

/* 'utils.c' */
void init_compute_shader(void);
void input_compute_buffer(void);
void dispatch_compute_shader(void);
void retrieve_compute_buffer(void);
void add_projectiles_to_compute_buffer(void);
void add_compute_data_to_projectiles(void);
void do_compute(void);

/* player.cpp */
void player_create(void) noexcept;

/* errors */
void __attribute__((__error__("Parameter cannot be 0"))) error_on_param_zero(void);
