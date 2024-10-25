#include "../include/prototypes.h"

SDL_Window   *win          = nullptr;
SDL_Renderer *ren          = nullptr;
SDL_Event     event;

char *window_title     = nullptr;
Uchar config_timestamp = 0;
int   window_width     = 1200;
int   window_height    = 800;

Object *objects_head = nullptr;
Object *objects_tail = nullptr;
Engine *engine       = nullptr;
Player *player       = nullptr;
