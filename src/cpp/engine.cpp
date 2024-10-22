#include "../include/prototypes.h"

/* Init sdl, the 'window' and the 'renderer'. */
void init(void) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "Failed to init sdl.\n");
    exit(1);
  }
  window =
    SDL_CreateWindow(window_title ? window_title : "placeholder", SDL_WINDOWPOS_UNDEFINED,
                     SDL_WINDOWPOS_UNDEFINED, window_width, window_height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (window == NULL) {
    SDL_Quit();
    fprintf(stderr, "Failed to create window.\n");
    exit(1);
  }
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    SDL_DestroyWindow(window);
    SDL_Quit();
    fprintf(stderr, "Failed to create renderer.\n");
    exit(1);
  }
  running = TRUE;
}

/* Cleanly exit and cleanup resources. */
void cleanup(void) {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_DestroyTexture(text_texture);
  TTF_Quit();
  SDL_Quit();
  Object *current = object_head;
  while (current != NULL) {
    Object *next = current->next;
    free(current);
    current = next;
  }
  exit(0);
}

void clear_frame(void) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
}

void update_frame(void) {
  SDL_RenderPresent(renderer);
  SDL_Delay((1.0 / FPS) * 1000);
}

void poll_for_events(void) {
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      running = FALSE;
    }
  }
}

void render_text(void) {
  int width  = 0;
  int height = 0;
  SDL_QueryTexture(text_texture, NULL, NULL, &width, &height);
  SDL_Rect quad = {((window_width - width) / 2), ((window_height - height) / 2), width, height};
  SDL_RenderCopy(renderer, text_texture, NULL, &quad);
}

void prosses_keys(void) {
  const unsigned char *state = SDL_GetKeyboardState(NULL);
  if (state[SDL_SCANCODE_ESCAPE]) {
    running = FALSE;
  }
  for (Object *object = object_head; object; object = object->next) {
    if (object->state & STATIC) {
      continue;
    }
    if (state[SDL_SCANCODE_W] && (object->state & FLYING_ENABLED)) {
      ACCEL(object->vel.y, -object->accel.y, FRAMETIME_S);
    }
    if (state[SDL_SCANCODE_S]) {
      ACCEL(object->vel.y, object->accel.y, FRAMETIME_S);
    }
    if (state[SDL_SCANCODE_D]) {
      ACCEL(object->vel.x, object->accel.x, FRAMETIME_S);
    }
    if (state[SDL_SCANCODE_A]) {
      ACCEL(object->vel.x, -object->accel.x, FRAMETIME_S);
    }
    if (state[SDL_SCANCODE_SPACE] && (object->state & ON_STATIC_OBJECT)) {
      object->state |= JUMP;
    }
    if (state[SDL_SCANCODE_8] && (event.common.timestamp != config_timestamp)) {
      config_timestamp = event.common.timestamp;
      object->state |= INCREACE_ACCELERATION;
    }
    if (state[SDL_SCANCODE_9]) {
      object->state |= DECREACE_ACCELERATION;
    }
  }
}
