#include "../include/prototypes.h"

void PlayerWeaponPart::draw(void) {
  engine->ren.draw_rotated_rect(rect.x, rect.y, rect.w, rect.h, player->weapon.angle, points);
}

void PlayerWeapon::init(void) {
  state.clear();
  state.set<PLAYER_WEAPON_ENABLED>();
  state.set<PLAYER_WEAPON_FOLLOWS_MOUSE>();
  type.clear();
  type.set<PLAYER_WEAPON_RIFLE>();
}

void PlayerWeapon::draw_body(void) {
  if (state.is_set<PLAYER_WEAPON_ENABLED>()) {
    /* If the rifle is equiped the set the body to the body of the rifle. */
    if (type.is_set<PLAYER_WEAPON_RIFLE>()) {
      body.rect = {
        (player->pos.x + (player->width / 2)),
        (player->pos.y + (player->height / 4)),
        60,
        14,
      };
    }
    body.draw();
  }
}

void PlayerWeapon::draw_barrel(void) {
  if (state.is_set<PLAYER_WEAPON_ENABLED>()) {
    if (type.is_set<PLAYER_WEAPON_RIFLE>()) {
      Vector center = calculate_center_of_two_points(
        {body.points[1].x, body.points[1].y}, {body.points[2].x, body.points[2].y}, {-10, 0});
      barrel.rect = {center.x, center.y, 20, 4};
    }
    barrel.draw();
  }
}

void PlayerWeapon::draw_magazine(void) {
  if (!state.is_set<PLAYER_WEAPON_ENABLED>()) {
    return;
  }
  if (type.is_set<PLAYER_WEAPON_RIFLE>()) {
    if (angle > 90 || angle < -90) {
      Vector center = calculate_center_of_two_points(
        {body.points[0].x, body.points[0].y}, {body.points[1].x, body.points[1].y}, {-10, 10});
      magazine.rect = {center.x, center.y, 6, 20};
    }
    else {
      Vector center = calculate_center_of_two_points(
        {body.points[2].x, body.points[2].y}, {body.points[3].x, body.points[3].y}, {-10, -10});
      magazine.rect = {center.x, center.y, 6, 20};
    }
  }
  magazine.draw();
}

void PlayerWeapon::draw(void) {
  if (!state.is_set<PLAYER_WEAPON_ENABLED>()) {
    return;
  }
  if (state.is_set<PLAYER_WEAPON_FOLLOWS_MOUSE>()) {
    engine->ren.set_color(RED);
    angle = calculate_angle_between_points(body.rect.x, body.rect.y, engine->mouse_data.x, engine->mouse_data.y);
    draw_body();
    draw_barrel();
    draw_magazine();
  }
}

void PlayerWeapon::attack(int button) {
  if (!player->weapon.state.is_set<PLAYER_WEAPON_ENABLED>() ||
      ((engine->mouse_data.x > player->pos.x && engine->mouse_data.x < player->pos.x + player->width) &&
       (engine->mouse_data.y > player->pos.y && engine->mouse_data.y < player->pos.y + player->height))) {
    return;
  }
  if (button == SDL_BUTTON_LEFT) {
    if (player->weapon.state.is_set<PLAYER_WEAPON_FOLLOWS_MOUSE>()) {
      Vector center = calculate_center_of_two_points(
        {barrel.points[1].x, barrel.points[1].y}, {barrel.points[2].x, barrel.points[2].y}, {-10, 0});
      Object *bullet = object_create({center.x, center.y}, 2, 2, {0, 0}, 0, 0);
      bullet->vel.set_magnitude_and_angle(
        40, calculate_angle_between_points(center.x, center.y, engine->mouse_data.x, engine->mouse_data.y));
      bullet->flag.set<OBJECT_IS_PROJECTILE>();
    }
  }
}
