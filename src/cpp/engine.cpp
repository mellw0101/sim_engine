#include "../include/prototypes.h"

/* Init sdl, the 'win' and the 'ren'. */
void init(void) {
  engine_create();
  player_create();
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "Failed to init sdl.\n");
    exit(1);
  }
  win = SDL_CreateWindow(window_title ? window_title : "placeholder", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                         window_width, window_height,
                         SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_MAXIMIZED);
  if (!win) {
    SDL_Quit();
    fprintf(stderr, "Failed to create win.\n");
    exit(1);
  }
  SDL_GetWindowSizeInPixels(win, &window_width, &window_height);
  ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
  if (!ren) {
    SDL_DestroyWindow(win);
    SDL_Quit();
    fprintf(stderr, "Failed to create ren.\n");
    exit(1);
  }
  engine->state.set<ENGINE_RUNNING>();
}

/* Cleanly exit and cleanup resources. */
void cleanup(void) {
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  TTF_Quit();
  SDL_Quit();
  Object *current = objects_head;
  while (current) {
    Object *next = current->next;
    free(current);
    current = next;
  }
  delete engine;
  if (player) {
    free(player);
  }
  exit(0);
}

void clear_frame(void) {
  SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
  SDL_RenderClear(ren);
}

void update_frame(void) {
  SDL_RenderPresent(ren);
}

void prosses_keys(void) {
  const Uchar *state = SDL_GetKeyboardState(nullptr);
  /* Quit when escape is pressed. */
  if (state[SDL_SCANCODE_ESCAPE]) {
    engine->state.unset<ENGINE_RUNNING>();
    return;
  }
  player->direction.clear();
  if (state[SDL_SCANCODE_W] && player->state.is_set<PLAYER_CAN_FLY>()) {
    player->vel.accel(0, -player->accel.y, FRAMETIME_S);
    player->direction.set<PLAYER_UP>();
  }
  if (state[SDL_SCANCODE_S]) {
    player->vel.accel(0, player->accel.y, FRAMETIME_S);
    player->direction.set<PLAYER_DOWN>();
  }
  if (state[SDL_SCANCODE_A]) {
    player->vel.accel(-player->accel.x, 0, FRAMETIME_S);
    player->direction.set<PLAYER_LEFT>();
  }
  if (state[SDL_SCANCODE_D]) {
    player->vel.accel(player->accel.x, 0, FRAMETIME_S);
    player->direction.set<PLAYER_RIGHT>();
  }
  /* Unset both directions when they cancel eachother. */
  if (player->direction.is_set<PLAYER_UP>() && player->direction.is_set<PLAYER_DOWN>()) {
    player->direction.unset<PLAYER_UP>();
    player->direction.unset<PLAYER_DOWN>();
  }
  if (player->direction.is_set<PLAYER_LEFT>() && player->direction.is_set<PLAYER_RIGHT>()) {
    player->direction.unset<PLAYER_LEFT>();
    player->direction.unset<PLAYER_RIGHT>();
  }
  /* If more than one direction is set and thay dont cancel out set a diaginal direction. */
  if (player->direction.is_set<PLAYER_UP>() && player->direction.is_set<PLAYER_RIGHT>()) {
    player->direction.set<PLAYER_UP_RIGHT>();
  }
  if (player->direction.is_set<PLAYER_UP>() && player->direction.is_set<PLAYER_LEFT>()) {
    player->direction.set<PLAYER_UP_LEFT>();
  }
  if (player->direction.is_set<PLAYER_DOWN>() && player->direction.is_set<PLAYER_RIGHT>()) {
    player->direction.set<PLAYER_DOWN_RIGHT>();
  }
  if (player->direction.is_set<PLAYER_DOWN>() && player->direction.is_set<PLAYER_LEFT>()) {
    player->direction.set<PLAYER_DOWN_LEFT>();
  }
  /* If any diaginal direction is set, unset the two directions that points in that direction. */
  if (player->direction.is_set<PLAYER_UP_RIGHT>()) {
    player->direction.unset<PLAYER_UP>();
    player->direction.unset<PLAYER_RIGHT>();
  }
  if (player->direction.is_set<PLAYER_UP_LEFT>()) {
    player->direction.unset<PLAYER_UP>();
    player->direction.unset<PLAYER_LEFT>();
  }
  if (player->direction.is_set<PLAYER_DOWN_RIGHT>()) {
    player->direction.unset<PLAYER_DOWN>();
    player->direction.unset<PLAYER_RIGHT>();
  }
  if (player->direction.is_set<PLAYER_DOWN_LEFT>()) {
    player->direction.unset<PLAYER_DOWN>();
    player->direction.unset<PLAYER_LEFT>();
  }
  if (state[SDL_SCANCODE_SPACE] && player->state.is_set<PLAYER_ON_OBJECT>()) {
    player->state.set<PLAYER_JUMPING>();
  }
}

void engine_create(void) {
  engine = new (std::nothrow) Engine();
  if (!engine) {
    fprintf(stderr, "Failed to alloc Engine.");
    exit(1);
  }
  engine->state.clear();
}

void Engine::frame_start(void) {
  _frame_st = high_resolution_clock::now();
}

/* Calculate the time to sleep based on the duration that has passed sence frame start.  Then sleep. */
void Engine::frame_end(void) {
  time_point<high_resolution_clock> end            = high_resolution_clock::now();
  duration<double, milli>           frame_duration = end - _frame_st;
  if (frame_duration.count() >= (1000 / FPS)) {
    return;
  }
  duration<double, milli> sleep_time((1000 / FPS) - frame_duration.count());
  std::this_thread::sleep_for(sleep_time);
}

void Engine::poll_events(void) {
  while (SDL_PollEvent(&_ev)) {
    if (_ev.type == SDL_QUIT) {
      engine->state.unset<ENGINE_RUNNING>();
      return;
    }
    if (_event_map[_ev.type].empty()) {
      continue;
    }
    for (const auto &func : _event_map[_ev.type]) {
      func(_ev);
    }
  }
}
