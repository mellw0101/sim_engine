#include "../include/prototypes.h"

SDL_Window   *win          = nullptr;

char *window_title     = nullptr;
Uchar config_timestamp = 0;
float   window_width     = 1200;
float   window_height    = 800;

Object *objects_head = nullptr;
Object *objects_tail = nullptr;
Engine *engine       = nullptr;
Player *player       = nullptr;
Cloth *cloth = nullptr; 
