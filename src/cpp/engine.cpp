#include "../include/prototypes.h"

/* Init sdl, the 'win' and the 'engine->ren.ren'. */
void init(void) {
  engine_create();
  player_create();
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "Failed to init sdl.\n");
    exit(1);
  }
  win = SDL_CreateWindow(window_title ? window_title : "placeholder", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                         window_width, window_height,
                         SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_OPENGL);
  if (!win) {
    SDL_Quit();
    fprintf(stderr, "Failed to create win.\n");
    exit(1);
  }
  int w, h;
  SDL_GetWindowSizeInPixels(win, &w, &h);
  window_width = w;
  window_height = h;
  engine->ren.ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
  if (!engine->ren.ren) {
    SDL_DestroyWindow(win);
    SDL_Quit();
    fprintf(stderr, "Failed to create 'engine->ren.ren'.\n");
    exit(1);
  }
  context = openGL_init_SDL_GLContext(win, true);
  if (!context) {
    cleanup();
  }
  // compute.init("src/kernel.cl", "rk4_step");
  // projectile.push_back({});
  // cl_projectile_buf = compute.create_buffer_at_idx(0, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, projectile.data(), projectile.size());
  // cl_dt = compute.create_buffer_at_idx(1, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, &dt_val, 1);
  engine->state.set<ENGINE_RUNNING>();
}

/* Cleanly exit and cleanup resources. */
void cleanup(void) {
  SDL_DestroyRenderer(engine->ren.ren);
  SDL_DestroyWindow(win);
  TTF_Quit();
  SDL_Quit();
  int obj_num = 0;
  Object *current = objects_head;
  while (current) {
    Object *next = current->next;
    free(current);
    current = next;
    ++obj_num;
  }
  printf("Number of objects at cleanup: %d\n", obj_num);
  engine->performance_data.print_report();
  delete engine;
  if (player) {
    free(player);
  }
  exit(0);
}

void clear_frame(void) {
  SDL_SetRenderDrawColor(engine->ren.ren, BLACK);
  SDL_RenderClear(engine->ren.ren);
}

void update_frame(void) {
  SDL_RenderPresent(engine->ren.ren);
}

void prosses_keys(void) {
  const Uchar *state = SDL_GetKeyboardState(nullptr);
  /* Quit when escape is pressed. */
  if (state[SDL_SCANCODE_ESCAPE]) {
    engine->state.unset<ENGINE_RUNNING>();
    return;
  }
  /* First clear player direction.  So that it`s always set correctly. */
  player->direction.clear();
  /* Set the appropriet direction based on currently pressed movement buttons. */
  if (state[SDL_SCANCODE_W]) {
    if (player->state.is_set<PLAYER_CAN_FLY>()) {
      player->data.accel.y -= player->accel.y;
    }
    player->direction.set<PLAYER_UP>();
  }
  if (state[SDL_SCANCODE_S]) {
    player->data.accel.y += player->accel.y;
    player->direction.set<PLAYER_DOWN>();
  }
  if (state[SDL_SCANCODE_A]) {
    player->data.accel.x -= player->accel.x;
    player->direction.set<PLAYER_LEFT>();
  }
  if (state[SDL_SCANCODE_D]) {
    player->data.accel.x += player->accel.x;
    player->direction.set<PLAYER_RIGHT>();
  }
  /* Make sure only one 'true' direction is */
  player->determine_control_direction();
  if (player->state.is_set<PLAYER_ON_OBJECT>() && state[SDL_SCANCODE_SPACE]) {
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
  engine->mouse_data.state.clear();
  engine->enviroment.temp = 20.0f;
  engine->camera.pos = 0.0f;
  engine->add_event_callback(SDL_MOUSEMOTION, [](SDL_Event ev) {
    engine->mouse_data.x = ev.motion.x;
    engine->mouse_data.y = ev.motion.y;
  });
  engine->add_event_callback(SDL_MOUSEBUTTONDOWN, [](SDL_Event ev) {
    if (ev.button.button == SDL_BUTTON_LEFT) {
      engine->mouse_data.state.set<MOUSE_STATE_LEFT_HELD>();
    }
    else if (ev.button.button == SDL_BUTTON_RIGHT) {
      engine->mouse_data.state.set<MOUSE_STATE_RIGHT_HELD>();
    }
  });
  engine->add_event_callback(SDL_MOUSEBUTTONUP, [](SDL_Event ev) {
    if (ev.button.button == SDL_BUTTON_LEFT) {
      engine->mouse_data.state.unset<MOUSE_STATE_LEFT_HELD>();
    }
    else if (ev.button.button == SDL_BUTTON_RIGHT) {
      engine->mouse_data.state.unset<MOUSE_STATE_RIGHT_HELD>();
    }
  }); 
}

void Engine::frame_start(void) {
  ++performance_data.framecount;
  _frame_st = high_resolution_clock::now();
}

/* Calculate the time to sleep based on the duration that has passed sence frame start.  Then sleep. */
void Engine::frame_end(void) {
  time_point<high_resolution_clock> end            = high_resolution_clock::now();
  duration<double, milli>           frame_duration = end - _frame_st;
  if (performance_data.framecount > 1000) {
    performance_data.frametime.data.push_back(frame_duration.count());
  }
  if (frame_duration.count() >= (1000 / FPS)) {
    return;
  }
  duration<double, milli> sleep_time((1000 / FPS) - frame_duration.count());
  if (performance_data.framecount > 100) {
    performance_data.added_frametime.data.push_back(sleep_time.count());
  }
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

void Engine::run(void) {
  while (state.is_set<ENGINE_RUNNING>()) {
    frame_start();
    clear_frame();
    prosses_keys();
    physics();
    poll_events();
    update_frame();
    frame_end();
  }
}
