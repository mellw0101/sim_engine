#include "../include/prototypes.h"

void test(void) {
  Vector base;
  base.set_magnitude_and_angle(31.112698, -133.667786);
  // Vector point {1, 0};
  float angle = base.magnitude();
  printf("angle: %f\n", angle);
}

void physics(void) {
  player->state.unset<PLAYER_ON_OBJECT>();
  for (Uchar i = 0; i < TIME_STEP; ++i) {
    /* Apply acceleration due to gravity. */
    player->vel.accel_y(GRAVITY, TIME_STEP_S);
    /* Now we calculate the next pos of player based on the player`s velocity. */
    player->calculate_pos_change(TIME_STEP_S);
    /* Constrain the player inside the window for now. */
    HIT_WALL(player, 0, window_width - player->width, -100, window_height - player->height);
    FOR_EACH_OBJECT(object) {
      /* If object is a moving object, meaning it moves over a set path.  Then animate next step. */
      if (object->flag.is_set<OBJECT_MOVING>()) {
        object->move();
      }
      /* If object is a projectile, calculate its next position based on it`s velocity. */
      if (object->flag.is_set<OBJECT_IS_PROJECTILE>()) {
        object->vel.accel_y(GRAVITY, TIME_STEP_S);
        float  air_density          = calculate_air_density(20, PIXEL_TO_M((window_height - object->pos.y)));
        float  cross_sectional_area = calculate_cross_sectional_area_box(object->width, object->height);
        Vector air_resistance = calculate_air_resistance(object->vel, 1.28f, air_density, cross_sectional_area, 10.0f);
        object->vel.accel(air_resistance.x, air_resistance.y, TIME_STEP_S);
        object->calculate_pos_change(TIME_STEP_S);
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
  }
  if (player->state.is_set<PLAYER_JUMPING>()) {
    player->jump();
  }
  if (engine->mouse_data.state.is_set<MOUSE_STATE_LEFT_HELD>()) {
    player->weapon.attack(SDL_BUTTON_LEFT);
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
  test();
  init();
  object_create({100, (window_height - 90)}, window_width - 100, 5, {FRICTON, FRICTON}, STATIC, 5);
  object_create({0, (window_height - 5)}, window_width, 5, {}, STATIC, 5);
  Object *moving_obj = object_create({window_width - 300, window_height - 120}, 100, 5, {FRICTON, FRICTON}, 0, 0);
  moving_obj->flag.set<OBJECT_MOVING>();
  moving_obj->moving_data.direction = OBJECT_MOVING_POSITIVE;
  moving_obj->moving_data.speed     = 1;
  moving_obj->moving_data.positive.set(moving_obj->pos.x, moving_obj->pos.y + 100);
  moving_obj->moving_data.negative = moving_obj->pos;
  engine->run();
  cleanup();
  
}
