#include "../include/prototypes.h"

void PlayerWeaponPart::draw(float angle_offset) {
  engine->ren.draw_rotated_rect(rect.x, rect.y, rect.w, rect.h, (player->weapon.angle + angle_offset), points);
}

void PlayerWeapon::init(void) {
  state.clear();
  state.set<PLAYER_WEAPON_ENABLED>();
  state.set<PLAYER_WEAPON_FOLLOWS_MOUSE>();
  type.clear();
  barrel.temp     = engine->enviroment.temp;
  barrel.color[0] = {GRAY_80};
  barrel.color[1] = {GRAY_80};
  barrel.color[2] = {GRAY_80};
  barrel.color[3] = {GRAY_80};
  equip(PLAYER_WEAPON_TYPE_AK_47);
}

void PlayerWeapon::equip(PlayerWeaponType weapon) {
  type.set(weapon);
  if (type.is_set<PLAYER_WEAPON_TYPE_AK_47>()) {
    barrel.length       = 415.0f;
    barrel.mass         = barrel_mass(MM_TO_M(barrel.length), MM_TO_M(16.0f), MM_TO_M(7.62f), STEEL_DENSITY);
    float temp_increace = single_shot_temp_increace(
      smokeless_gunpowder_energy(ROUND_7_62_GRAINS), 0.35f, heat_capacity(barrel.mass, STEEL_HEAT_CAPACITY));
    printf("barrel mass: %f\n", barrel.mass);
    printf("single shot temp increace: %f\n", temp_increace);
  }
}

void PlayerWeapon::draw_body(void) {
  /* If the rifle is equiped the set the body to the body of the rifle. */
  if (type.is_set<PLAYER_WEAPON_TYPE_AK_47>()) {
    body.rect = {
      (float)((player->pos.x + (player->width / 2)) + engine->camera.pos.x),
      (float)((player->pos.y + (player->height / 4)) + +engine->camera.pos.y),
      60,
      14,
    };
    const Color colors[4] = {{GRAY_80}, {GRAY_80}, {GRAY_80}, {GRAY_80}};
    engine->ren.draw_rotated_box(
      body.rect.x, body.rect.y, body.rect.w, body.rect.h, player->weapon.angle, colors, body.points);
  }
}

void PlayerWeapon::draw_barrel(void) {
  if (type.is_set<PLAYER_WEAPON_TYPE_AK_47>()) {
    const MVec2 center = MVec2::center_with_offset(body.points[1], body.points[2], {-10, 0});
    barrel.rect        = {(float)center.x, (float)center.y, 20, 4};
    if (barrel.temp <= 300.0f) {
      barrel.color[0] = {GRAY_80};
      barrel.color[1] = {GRAY_80};
      barrel.color[2] = {GRAY_80};
      barrel.color[3] = {GRAY_80};
    }
    engine->ren.draw_rotated_box(
      barrel.rect.x, barrel.rect.y, barrel.rect.w, barrel.rect.h, angle, barrel.color, barrel.points);
  }
}

void PlayerWeapon::draw_magazine(void) {
  if (type.is_set<PLAYER_WEAPON_TYPE_AK_47>()) {
    constexpr Uint    accuracy = 20;
    static SDL_FPoint lline[accuracy];
    static SDL_FPoint rline[accuracy];
    if (angle > 90 || angle < -90) {
      const MVec2 lpoint1 = MVec2::center_with_offset(body.points[0], body.points[1], {0, 5});
      const MVec2 lpoint2 = MVec2::center_with_offset(body.points[0], body.points[1], {-25, 15});
      const MVec2 rpoint1 = MVec2::center_with_offset(body.points[0], body.points[1], {0, 15});
      const MVec2 rpoint2 = MVec2::center_with_offset(body.points[0], body.points[1], {-20, 25});
      calculate_arc_points(lpoint1, lpoint2, MVec2::center_with_offset(lpoint1, lpoint2, {-4}), lline, accuracy);
      calculate_arc_points(rpoint1, rpoint2, MVec2::center_with_offset(rpoint1, rpoint2, {-4}), rline, accuracy);
    }
    else {
      const MVec2 lpoint1 = MVec2::center_with_offset(body.points[2], body.points[3], {0, -5});
      const MVec2 lpoint2 = MVec2::center_with_offset(body.points[2], body.points[3], {-25, -15});
      const MVec2 rpoint1 = MVec2::center_with_offset(body.points[2], body.points[3], {0, -15});
      const MVec2 rpoint2 = MVec2::center_with_offset(body.points[2], body.points[3], {-20, -25});
      calculate_arc_points(lpoint1, lpoint2, MVec2::center_with_offset(lpoint1, lpoint2, {4}), lline, accuracy);
      calculate_arc_points(rpoint1, rpoint2, MVec2::center_with_offset(rpoint1, rpoint2, {4}), rline, accuracy);
      engine->ren.draw_filled_arc(lline, accuracy, {RED}, {-5});
      engine->ren.draw_filled_arc(rline, accuracy, {RED}, {5});
      const Color colors[3] = {{RED}, {RED}, {RED}};
      engine->ren.draw_triangle(lline[0], lline[accuracy - 1], rline[accuracy - 1], colors);
      engine->ren.draw_triangle(lline[0], lline[accuracy - 1], rline[0], colors);
    }
    SDL_FPoint top[2]    = {lline[0], rline[0]};
    SDL_FPoint bottom[2] = {lline[accuracy - 1], rline[accuracy - 1]};
    engine->ren.draw_lines(top, 2);
    engine->ren.draw_lines(bottom, 2);
    engine->ren.draw_lines(lline, accuracy);
    engine->ren.draw_lines(rline, accuracy);
  }
}

void PlayerWeapon::draw_handle(void) {
  if (type.is_set<PLAYER_WEAPON_TYPE_AK_47>()) {
    if (angle > 90.0f || angle < -90.0f) {
      const MVec2      lpoint1   = MVec2::center_with_offset(body.points[0], body.points[1], {0, -20});
      const MVec2      lpoint2   = MVec2::center_with_offset(body.points[0], body.points[1], {-15, -25});
      const MVec2      rpoint1   = MVec2::center_with_offset(body.points[0], body.points[1], {0, -12});
      const MVec2      rpoint2   = MVec2::center_with_offset(body.points[0], body.points[1], {-15, -17});
      const SDL_FPoint line[4]   = {lpoint1, lpoint2, rpoint2, rpoint1};
      const Color      colors[4] = {{GRAY_80}, {GRAY_80}, {GRAY_80}, {GRAY_80}};
      engine->ren.draw_quadliteral(line, colors);
    }
    else {
      const MVec2      lpoint1   = MVec2::center_with_offset(body.points[2], body.points[3], {0, 20});
      const MVec2      lpoint2   = MVec2::center_with_offset(body.points[2], body.points[3], {-15, 25});
      const MVec2      rpoint1   = MVec2::center_with_offset(body.points[2], body.points[3], {0, 12});
      const MVec2      rpoint2   = MVec2::center_with_offset(body.points[2], body.points[3], {-15, 17});
      const SDL_FPoint line[4]   = {lpoint1, lpoint2, rpoint2, rpoint1};
      const Color      colors[4] = {{GRAY_80}, {GRAY_80}, {GRAY_80}, {GRAY_80}};
      engine->ren.draw_quadliteral(line, colors);
    }
  }
}

void PlayerWeapon::draw(void) {
  if (!state.is_set<PLAYER_WEAPON_ENABLED>()) {
    return;
  }
  if (state.is_set<PLAYER_WEAPON_FOLLOWS_MOUSE>()) {
    engine->ren.set_color(RED);
    angle = anglef(body.rect.x, body.rect.y, engine->mouse_data.x, engine->mouse_data.y);
    draw_body();
    draw_barrel();
    draw_magazine();
    draw_handle();
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
      MVec2   center = MVec2::center_with_offset(MVec2::from(barrel.points[1]) - engine->camera.pos,
                                                 MVec2::from(barrel.points[2]) - engine->camera.pos, {-10, 0});
      Object *bullet = object_create(center, 2, 2, {0, 0}, 0, 0);
      bullet->vel.set_magnitude_and_angle(700.0f, center.angle_to((engine->mouse_data.x - engine->camera.pos.x),
                                                                  (engine->mouse_data.y - engine->camera.pos.y)));
      bullet->flag.set<OBJECT_IS_PROJECTILE>();
      if (type.is_set<PLAYER_WEAPON_TYPE_AK_47>()) {
        barrel.temp += single_shot_temp_increace(
          smokeless_gunpowder_energy(ROUND_7_62_GRAINS), 0.35f, heat_capacity(barrel.mass, STEEL_HEAT_CAPACITY));
      }
    }
  }
}
