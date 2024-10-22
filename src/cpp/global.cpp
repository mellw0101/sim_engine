#include "../include/prototypes.h"

char         *window_title  = NULL;
int           window_width  = 1200;
int           window_height = 800;
SDL_Window   *window        = NULL;
SDL_Renderer *renderer      = NULL;
SDL_Event     event;
bool          running          = FALSE;
Object     *object_head      = NULL;
Object     *Objectail      = NULL;
unsigned char config_timestamp = 0;

SDL_Texture *text_texture = NULL;
