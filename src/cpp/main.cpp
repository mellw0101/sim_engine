#include "../include/prototypes.h"

void physics(void) {
  player->state.unset<PLAYER_ON_OBJECT>();
  for (Uint ts = 0; ts < TIME_STEP; ++ts) {
    for (Uint i = 0; i < projectile.size(); ++i) {
      /* Draw projectile. */
      SDL_SetRenderDrawColor(engine->ren.ren, RED);
      SDL_FRect r = {projectile[i].data.pos.x, projectile[i].data.pos.y, projectile[i].width, projectile[i].height};
      engine->ren.fill_rect(&r);
      /* Calculate air resistance. */
      float  air_density          = calculate_air_density(20, PIXEL_TO_M((window_height - projectile[i].data.pos.y)));
      float  cross_sectional_area = calculate_cross_sectional_area_box(projectile[i].width, projectile[i].height);
      MVec2 air_resistance = calculate_air_resistance(projectile[i].data.vel, 0.8f, air_density, cross_sectional_area, 10.0f);
      /* Perform rk4 step. */
      projectile[i].data.accel = air_resistance + gravity_vector;
      // rk4_step(&projectile[i].data.pos, &projectile[i].data.vel, TIME_STEP_S, (air_resistance + gravity_vector));
      /* When a projectile goes outside view-port remove it. */
      if (OBJ_L(&projectile[i]) > window_width || OBJ_T(&projectile[i]) > window_height || OBJ_R(&projectile[i]) < 0) {
        projectile.erase(&projectile[i]);
        continue;
      }
    }
    projectile_rk4_step(TIME_STEP_S);
    // if (!projectile.empty()) {
    //   cl_projectile_buf = compute.realloc_buffer_at_idx(0, cl_projectile_buf, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, projectile.data(), projectile.size());
    //   compute.enqueue_kernel(projectile.size());
    //   compute.read_kernel_buf(cl_projectile_buf, projectile.data(), projectile.size());
    // }
    projectile_collisions();
  }
  for (Uchar i = 0; i < TIME_STEP; ++i) {
    /* Constrain the player inside the window for now. */
    HIT_WALL(player, 0, (window_width - player->width), -100, (window_height - player->height));
    FOR_EACH_OBJECT(object) {
      /* If object is a moving object, meaning it moves over a set path.  Then animate next step. */
      if (object->flag.is_set<OBJECT_MOVING>()) {
        object->move();
      }
      player->check_collision_with(object);
    }
    /* Now we calculate the next pos of player based on the player`s velocity. */
    rk4_step(&player->data.pos, &player->data.vel, TIME_STEP_S, (player->data.accel + gravity_vector));
  }
  if (player->data.pos.x > (window_width / 2)) {
    engine->camera.pos.x = ((window_width / 2) - player->data.pos.x);
  }
  player->data.accel = 0.0f;
  if (player->state.is_set<PLAYER_JUMPING>()) {
    player->jump();
  }
  if (engine->mouse_data.state.is_set<MOUSE_STATE_LEFT_HELD>()) {
    player->weapon.attack(SDL_BUTTON_LEFT);
  }
  /* Then here cool it down otherwise. */
  else if (player->weapon.barrel.temp > engine->enviroment.temp) {
    player->weapon.barrel.temp -= 0.26;
  }
  else if (player->weapon.barrel.temp < engine->enviroment.temp) {
    player->weapon.barrel.temp = engine->enviroment.temp;
  }
  player->draw();
  FOR_EACH_OBJECT(object) {
    object->draw();
  }
}

#define PLAYER_X_ACCEL 0x09
#define PLAYER_Y_ACCEL 0x09
#define FRICTON        0x03

int main(void) {
  init();
  object_create({100, (window_height - 240)}, window_width - 100, 5, {FRICTON, FRICTON}, STATIC, 5);
  object_create({0, (window_height - 5)}, window_width, 5, {}, STATIC, 5);
  Object *moving_obj = object_create({window_width - 300, window_height - 400}, 100, 5, {FRICTON, FRICTON}, 0, 0);
  moving_obj->flag.set<OBJECT_MOVING>();
  moving_obj->moving_data.direction = OBJECT_MOVING_POSITIVE;
  moving_obj->moving_data.speed     = 1;
  moving_obj->moving_data.positive.set(moving_obj->data.pos.x, moving_obj->data.pos.y + 100);
  moving_obj->moving_data.negative = moving_obj->data.pos;
  engine->run();
  cleanup();
}
