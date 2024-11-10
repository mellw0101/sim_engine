#include "../include/prototypes.h"

SDL_Window      *win         = nullptr;
SDL_GLContext    context     = nullptr;

Uint             comp_shader = 0;
Uint             SSBO        = 0;
MVector<PosData> compute_buffer;

char *window_title     = nullptr;
Uchar config_timestamp = 0;
float window_width     = 1200;
float window_height    = 800;

Object *objects_head = nullptr;
Object *objects_tail = nullptr;
Engine *engine       = nullptr;
Player *player       = nullptr;
Cloth  *cloth        = nullptr;

MVector<Projectile> projectile;

ClComputeProgram compute;
cl_mem cl_projectile_buf;
float dt_val = TIME_STEP_S;
cl_mem cl_dt;
Uint max_projectiles = 1000;
