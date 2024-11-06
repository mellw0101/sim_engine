#include "../include/prototypes.h"

void physics(void) {
  player->state.unset<PLAYER_ON_OBJECT>();
  /* Apply acceleration due to gravity. */
  for (Uchar i = 0; i < TIME_STEP; ++i) {
    /* Constrain the player inside the window for now. */
    HIT_WALL(player, 0, (window_width - player->width), -100, (window_height - player->height));
    FOR_EACH_OBJECT(object) {
      /* If object is a moving object, meaning it moves over a set path.  Then animate next step. */
      if (object->flag.is_set<OBJECT_MOVING>()) {
        object->move();
      }
      /* If object is a projectile, calculate its next position based on it`s velocity. */
      if (object->flag.is_set<OBJECT_IS_PROJECTILE>()) {
        float  air_density          = calculate_air_density(20, PIXEL_TO_M((window_height - object->pos.y)));
        float  cross_sectional_area = calculate_cross_sectional_area_box(object->width, object->height);
        MVec2 air_resistance = calculate_air_resistance(object->vel, 0.8f, air_density, cross_sectional_area, 10.0f);
        rk4_step(&object->pos, &object->vel, TIME_STEP_S, (air_resistance + gravity_vector));
        /* When a projectile goes outside view-port unlink it. */
        if (OBJ_L(object) > window_width || OBJ_T(object) > window_height || OBJ_R(object) < 0) {
          unlink_object(object);
          continue;
        }
      }
      /* Otherwise check if player is coliding with object. */
      else {
        player->check_collision_with(object);
      }
    }
    /* Now we calculate the next pos of player based on the player`s velocity. */
    rk4_step(&player->pos, &player->vel, TIME_STEP_S, (player->acceleration + gravity_vector));
  }
  if (player->pos.x > (window_width / 2)) {
    engine->camera.pos.x = ((window_width / 2) - player->pos.x);
  }
  player->acceleration = 0;
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
  moving_obj->moving_data.positive.set(moving_obj->pos.x, moving_obj->pos.y + 100);
  moving_obj->moving_data.negative = moving_obj->pos;
  engine->run();
  cleanup();
}
