#include "../include/prototypes.h"

void physics(void) {
  player->draw();
  player->state.unset<PLAYER_ON_OBJECT>();
  for (Uchar i = 0; i < TIME_STEP; ++i) {
    /* Apply acceleration due to gravity. */
    player->vel.accel(0, GRAVITY, TIME_STEP_S);
    /* Now we calculate the next pos of player based on the player`s velocity. */
    player->calculate_pos_change(TIME_STEP_S);
    /* Constrain the player inside the window for now. */
    HIT_WALL(player, 0, window_width - player->width, -100, window_height - player->height);
    player->check_collisions();
  }
  if (player->state.is_set<PLAYER_JUMPING>()) {
    player->jump();
  }
  for (Object *object = objects_head; object; object = object->next) {
    object->draw();
    if (object->flag.is_set<OBJECT_MOVING>()) {

    }
  }
}

#define PLAYER_X_ACCEL 0x09
#define PLAYER_Y_ACCEL 0x09
#define FRICTON        0x03

int main(void) {
  init();
  object_create(100, (window_height - 90), window_width - 100, 5, FRICTON, FRICTON, STATIC, 5);
  object_create(0, (window_height - 5), window_width, 5, 0, 0, STATIC, 5);
  while (engine->state.is_set<ENGINE_RUNNING>()) {
    engine->frame_start();
    clear_frame();
    prosses_keys();
    physics();
    engine->poll_events();
    update_frame();
    engine->frame_end();
  }
  cleanup();
}
