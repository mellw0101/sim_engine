#include "../include/prototypes.h"

void PlayerWeaponPart::draw(float angle_offset) {
  engine->ren.draw_rotated_rect(rect.x, rect.y, rect.w, rect.h, (player->weapon.angle + angle_offset), points);
}

void PlayerWeapon::init(void) {
  state.clear();
  state.set<PLAYER_WEAPON_ENABLED>();
  state.set<PLAYER_WEAPON_FOLLOWS_MOUSE>();
  type.clear();
  type.set<PLAYER_WEAPON_RIFLE>();
}

void PlayerWeapon::draw_body(void) {
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

void PlayerWeapon::draw_barrel(void) {
  if (type.is_set<PLAYER_WEAPON_RIFLE>()) {
    Vector center = calculate_center_of_two_points_offset(
      {body.points[1].x, body.points[1].y}, {body.points[2].x, body.points[2].y}, {-10, 0});
    barrel.rect = {center.x, center.y, 20, 4};
  }
  barrel.draw();
}

void PlayerWeapon::draw_magazine(void) {
  if (type.is_set<PLAYER_WEAPON_RIFLE>()) {
    constexpr Uint accuracy = 20;
    SDL_FPoint     lline[accuracy];
    if (angle > 90 || angle < -90) {
      const Vector   lpoint1 = Vector::center_with_offset(body.points[0], body.points[1], {0, 5});
      const Vector   lpoint2 = Vector::center_with_offset(body.points[0], body.points[1], {-25, 15});
      calculate_arc_points(lpoint1, lpoint2, Vector::center_with_offset(lpoint1, lpoint2, {-4}), lline, accuracy);
      engine->ren.draw_lines(lline, accuracy);
      SDL_FPoint     rline[accuracy];
      const Vector   rpoint1 = Vector::center_with_offset(body.points[0], body.points[1], {0, 15});
      const Vector   rpoint2 = Vector::center_with_offset(body.points[0], body.points[1], {-20, 25});
      calculate_arc_points(rpoint1, rpoint2, Vector::center_with_offset(rpoint1, rpoint2, {-4}), rline, accuracy);
      engine->ren.draw_lines(rline, accuracy);
      SDL_FPoint bottom[2] = {lline[accuracy - 1], rline[accuracy - 1]};
      engine->ren.draw_lines(bottom, 2);
    }
    else {
      const Vector   lpoint1 = Vector::center_with_offset(body.points[2], body.points[3], {0, -5});
      const Vector   lpoint2 = Vector::center_with_offset(body.points[2], body.points[3], {-25, -15});
      calculate_arc_points(lpoint1, lpoint2, Vector::center_with_offset(lpoint1, lpoint2, {4}), lline, accuracy);
      engine->ren.draw_lines(lline, accuracy);
      SDL_FPoint     rline[accuracy];
      const Vector   rpoint1 = Vector::center_with_offset(body.points[2], body.points[3], {0, -15});
      const Vector   rpoint2 = Vector::center_with_offset(body.points[2], body.points[3], {-20, -25});
      calculate_arc_points(rpoint1, rpoint2, Vector::center_with_offset(rpoint1, rpoint2, {4}), rline, accuracy);
      engine->ren.draw_lines(rline, accuracy);
      SDL_FPoint bottom[2] = {lline[accuracy - 1], rline[accuracy - 1]};
      engine->ren.draw_lines(bottom, 2);
    }
  }
}

void PlayerWeapon::draw(void) {
  if (!state.is_set<PLAYER_WEAPON_ENABLED>()) {
    return;
  }
  if (state.is_set<PLAYER_WEAPON_FOLLOWS_MOUSE>()) {
    engine->ren.set_color(RED);
    angle = calculate_angle_between_points(body.rect.x, body.rect.y, engine->mouse_data.x, engine->mouse_data.y);
    draw_body();
    constexpr Uint accuracy = 20;
    SDL_FPoint     line[accuracy];
    const Vector   cen_1 = Vector::center(body.points[1], body.points[2]);
    const Vector   cen_2 = Vector::center(body.points[0], body.points[1]);
    calculate_arc_points(cen_1, cen_2, calculate_center_of_two_points_offset(cen_1, cen_2, {4}), line, accuracy);
    engine->ren.draw_lines(line, accuracy);
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
      Vector center = calculate_center_of_two_points_offset(
        {barrel.points[1].x, barrel.points[1].y}, {barrel.points[2].x, barrel.points[2].y}, {-10, 0});
      Object *bullet = object_create({center.x, center.y}, 2, 2, {0, 0}, 0, 0);
      bullet->vel.set_magnitude_and_angle(
        700, calculate_angle_between_points(center.x, center.y, engine->mouse_data.x, engine->mouse_data.y));
      bullet->flag.set<OBJECT_IS_PROJECTILE>();
    }
  }
}
