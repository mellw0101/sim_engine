#include "../include/prototypes.h"

void player_create(void) noexcept {
  player = (Player *)malloc(sizeof(Player));
  if (!player) {
    fprintf(stderr, "Failed to malloc player.");
    exit(1);
  }
  player->pos.set(100, 0);
  player->vel.set(0, 0);
  player->accel.set(9, 9);
  player->max_speed.set(20, 20);
  player->width  = 10;
  player->height = 20;
  player->state.clear();
  player->direction.clear();
}

void Player::draw(void) {
  SDL_SetRenderDrawColor(ren, 0, 0, 255, 255);
  const SDL_FRect r = {(float)pos.x, (float)pos.y, (float)width, (float)height};
  SDL_RenderFillRectF(ren, &r);
}

void Player::check_collisions(void) {
  for (Object *object = objects_head; object; object = object->next) {
    /* Check if player is colliding with any objects. */
    if (OBJECTS_COLLIDING(player, object)) {
      /* Top. */
      if (OBJ_OVERLAP_LEAST_T(player, object)) {
        player->pos.y = OBJ_T(object) - player->height;
        CONSTRAIN_VEL_IF((player->vel.y > 0), player->vel.y);
        player->state.set<PLAYER_ON_OBJECT>();
        /* Apply friction to player, if object has any. */
        if (object->friction.y > 0) {
          /* Player is moving right. */
          if (player->vel.x > 0) {
            player->vel.accel(-object->friction.x, 0, TIME_STEP_S);
            CONSTRAIN_VEL_IF((player->vel.x < 0), player->vel.x);
          }
          /* Player is moving left. */
          else if (player->vel.x < 0) {
            player->vel.accel(object->friction.x, 0, TIME_STEP_S);
            CONSTRAIN_VEL_IF((player->vel.x > 0), player->vel.x);
          }
        }
      }
      /* Bottom. */
      else if (OBJ_OVERLAP_LEAST_B(player, object)) {
        player->pos.y = OBJ_B(object);
        CONSTRAIN_VEL_IF((player->vel.y < 0), player->vel.y);
      }
      /* Left. */
      else if (OBJ_OVERLAP_LEAST_L(player, object)) {
        player->pos.x = OBJ_L(object) - player->width;
        CONSTRAIN_VEL_IF((player->vel.x < 0), player->vel.x);
      }
      /* Right. */
      else if (OBJ_OVERLAP_LEAST_R(player, object)) {
        player->pos.x = OBJ_R(object);
        CONSTRAIN_VEL_IF((player->vel.x < 0), player->vel.x);
      }
    }
  }
}

void Player::jump(void) {
  vel.y = -5;
  state.unset<PLAYER_JUMPING>();
}

void Player::calculate_pos_change(const double delta_t) noexcept {
  if (!delta_t) {
    error_on_param_zero();
  }
  pos.x += (M_TO_PIXEL(vel.x) * delta_t);
  pos.y += (M_TO_PIXEL(vel.y) * delta_t);
}
