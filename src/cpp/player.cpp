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
  player->width  = 80;
  player->height = 200;
  player->state.clear();
  player->direction.clear();
  player->weapon.init();
}

void Player::draw(void) {
  const SDL_Rect r {(int)pos.x, (int)pos.y, (int)width, (int)height};
  engine->ren.set_color(BLUE);
  engine->ren.fill_rect(&r);
  weapon.draw();
}

void Player::check_collision_with(const Object *const o) noexcept {
  /* Check if player is colliding with the o. */
  if (OBJECTS_COLLIDING(player, o)) {
    /* Top. */
    if (OBJ_OVERLAP_LEAST_T(player, o)) {
      player->pos.y = OBJ_T(o) - player->height;
      CONSTRAIN_VEL_IF((player->vel.y > 0), player->vel.y);
      player->state.set<PLAYER_ON_OBJECT>();
      /* Apply friction to player, if o has any. */
      if (o->friction.y > 0) {
        /* Player is moving right. */
        if (player->vel.x > 0) {
          player->vel.accel(-o->friction.x, 0, TIME_STEP_S);
          CONSTRAIN_VEL_IF((player->vel.x < 0), player->vel.x);
        }
        /* Player is moving left. */
        else if (player->vel.x < 0) {
          player->vel.accel(o->friction.x, 0, TIME_STEP_S);
          CONSTRAIN_VEL_IF((player->vel.x > 0), player->vel.x);
        }
      }
    }
    /* Bottom. */
    else if (OBJ_OVERLAP_LEAST_B(player, o)) {
      player->pos.y = OBJ_B(o);
      CONSTRAIN_VEL_IF((player->vel.y < 0), player->vel.y);
    }
    /* Left. */
    else if (OBJ_OVERLAP_LEAST_L(player, o)) {
      player->pos.x = OBJ_L(o) - player->width;
      CONSTRAIN_VEL_IF((player->vel.x > 0), player->vel.x);
    }
    /* Right. */
    else if (OBJ_OVERLAP_LEAST_R(player, o)) {
      player->pos.x = OBJ_R(o);
      CONSTRAIN_VEL_IF((player->vel.x < 0), player->vel.x);
    }
    /* If the object has a set action for collision then run the action. */
    if (o->flag.is_set<OBJECT_HAS_COLLISION_ACTION>() && o->collision_action) {
      o->collision_action();
    }
  }
}

void Player::check_collisions(void) {
  FOR_EACH_OBJECT(object) {
    check_collision_with(object);
  }
}

void Player::jump(void) noexcept {
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

void Player::determine_control_direction(void) noexcept {
  /* Unset both directions when they cancel eachother. */
  if (direction.is_set<PLAYER_UP>() && direction.is_set<PLAYER_DOWN>()) {
    direction.unset<PLAYER_UP>();
    direction.unset<PLAYER_DOWN>();
  }
  if (direction.is_set<PLAYER_LEFT>() && direction.is_set<PLAYER_RIGHT>()) {
    direction.unset<PLAYER_LEFT>();
    direction.unset<PLAYER_RIGHT>();
  }
  /* If more than one direction is set and they dont cancel out set a diaginal direction.
   * We also unset the two directions that make up that direction. */
  if (direction.is_set<PLAYER_UP>() && direction.is_set<PLAYER_RIGHT>()) {
    direction.set<PLAYER_UP_RIGHT>();
    direction.unset<PLAYER_UP>();
    direction.unset<PLAYER_RIGHT>();
  }
  else if (direction.is_set<PLAYER_UP>() && direction.is_set<PLAYER_LEFT>()) {
    direction.set<PLAYER_UP_LEFT>();
    direction.unset<PLAYER_UP>();
    direction.unset<PLAYER_LEFT>();
  }
  else if (direction.is_set<PLAYER_DOWN>() && direction.is_set<PLAYER_RIGHT>()) {
    direction.set<PLAYER_DOWN_RIGHT>();
    direction.unset<PLAYER_DOWN>();
    direction.unset<PLAYER_RIGHT>();
  }
  else if (direction.is_set<PLAYER_DOWN>() && direction.is_set<PLAYER_LEFT>()) {
    direction.set<PLAYER_DOWN_LEFT>();
    direction.unset<PLAYER_DOWN>();
    direction.unset<PLAYER_LEFT>();
  }
}
