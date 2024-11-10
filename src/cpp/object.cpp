#include "../include/prototypes.h"

Object *object_create(const MVec2 &pos, const Ushort width, const Ushort height, const MVec2 &friction, Uint state,
                      const float max_speed) {
  Object *obj = (Object *)malloc(sizeof(*obj));
  if (!obj) {
    fprintf(stderr, "Failed to malloc Object.\n");
    return nullptr;
  }
  obj->data.pos    = pos;
  obj->data.vel    = 0;
  obj->width       = width;
  obj->height      = height;
  obj->friction    = friction;
  obj->state       = state;
  obj->max_speed.x = max_speed;
  obj->max_speed.y = max_speed;
  obj->flag.clear();
  obj->next = nullptr;
  obj->prev = nullptr;
  if (!objects_head) {
    objects_head = obj;
    objects_tail = obj;
  }
  else {
    objects_tail->next = obj;
    obj->prev          = objects_tail;
    objects_tail       = objects_tail->next;
  }
  return obj;
}

void unlink_object(Object *obj) {
  /* If the object we want to unlink is the head then set the head to the next obj. */
  if (obj == objects_head) {
    objects_head = obj->next;
  }
  if (obj == objects_tail) {
    objects_tail = obj->prev;
  }
  /* Unlink the object. */
  if (obj->prev) {
    obj->prev->next = obj->next;
  }
  if (obj->next) {
    obj->next->prev = obj->prev;
  }
  free(obj);
}

/* clang-format off */
void projectile_collisions(void) {
  FOR_EACH_OBJECT(obj) {
    // Create the obj arrays with a single value each.
    const __m256 o_right  = _mm256_set1_ps(OBJ_R(obj));
    const __m256 o_bottom = _mm256_set1_ps(OBJ_B(obj));
    const __m256 o_left   = _mm256_set1_ps(OBJ_L(obj));
    const __m256 o_top    = _mm256_set1_ps(OBJ_T(obj));
    for (Uint i = 0; i < projectile.size(); i += 8) {
      const __m256 p_left    = _mm256_setr_ps(
        OBJ_L(&projectile[i]),         OBJ_L(&projectile[(i+1 % 8)]), OBJ_L(&projectile[(i+2 % 8)]), OBJ_L(&projectile[(i+3 % 8)]),
        OBJ_L(&projectile[(i+4 % 8)]), OBJ_L(&projectile[(i+5 % 8)]), OBJ_L(&projectile[(i+6 % 8)]), OBJ_L(&projectile[(i+7 % 8)])
      );
      const __m256 p_top     = _mm256_setr_ps(
        OBJ_T(&projectile[i]),         OBJ_T(&projectile[(i+1 % 8)]), OBJ_T(&projectile[(i+2 % 8)]), OBJ_T(&projectile[(i+3 % 8)]),
        OBJ_T(&projectile[(i+4 % 8)]), OBJ_T(&projectile[(i+5 % 8)]), OBJ_T(&projectile[(i+6 % 8)]), OBJ_T(&projectile[(i+7 % 8)])
      );
      const __m256 p_right   = _mm256_setr_ps(
        OBJ_R(&projectile[i]),         OBJ_R(&projectile[(i+1 % 8)]), OBJ_R(&projectile[(i+2 % 8)]), OBJ_R(&projectile[(i+3 % 8)]),
        OBJ_R(&projectile[(i+4 % 8)]), OBJ_R(&projectile[(i+5 % 8)]), OBJ_R(&projectile[(i+6 % 8)]), OBJ_R(&projectile[(i+7 % 8)])
      );
      const __m256 p_bottom  = _mm256_setr_ps(
        OBJ_B(&projectile[i]),         OBJ_B(&projectile[(i+1 % 8)]), OBJ_B(&projectile[(i+2 % 8)]), OBJ_B(&projectile[(i+3 % 8)]),
        OBJ_B(&projectile[(i+4 % 8)]), OBJ_B(&projectile[(i+5 % 8)]), OBJ_B(&projectile[(i+6 % 8)]), OBJ_B(&projectile[(i+7 % 8)])
      );
      // Perform simd cmp.
      const __m256 cmp_right_ge_left = _mm256_cmp_ps(o_right, p_left, _CMP_GE_OQ);
      const __m256 cmp_bottom_ge_top = _mm256_cmp_ps(o_bottom, p_top, _CMP_GE_OQ);
      const __m256 cmp_left_le_right = _mm256_cmp_ps(o_left, p_right, _CMP_LE_OQ);
      const __m256 cmp_top_le_bottom = _mm256_cmp_ps(o_top, p_bottom, _CMP_LE_OQ);
      // Combine result.
      const __m256 horizontal_overlap = _mm256_and_ps(cmp_right_ge_left, cmp_left_le_right);
      const __m256 vertical_overlap   = _mm256_and_ps(cmp_bottom_ge_top, cmp_top_le_bottom);
      // Extract mask.
      const int mask = _mm256_movemask_ps(_mm256_and_ps(horizontal_overlap, vertical_overlap));
      for (Uint k = 0; k < 8; ++k) {
        if ((mask & (1 << k)) == (1 << k)) {
          projectile.erase(&projectile[i + k]);
        }
      }
    }
  }
}

void projectile_rk4_step(float delta_t) {
  const __m256 dt   = _mm256_set1_ps(delta_t);
  const __m256 half = _mm256_set1_ps(0.5f);
  const __m256 six  = _mm256_set1_ps(6.0f);
  const __m256 two  = _mm256_set1_ps(2.0f);
  const __m256 pix  = _mm256_set1_ps(PIXELS_PER_METER);
  for (Uint i = 0; i < projectile.size(); i += 8) {
    __m256 posx = _mm256_setr_ps(
      projectile[i].data.pos.x,         projectile[i+(1 % 8)].data.pos.x, projectile[i+(2 % 8)].data.pos.x, projectile[i+(3 % 8)].data.pos.x,
      projectile[i+(4 % 8)].data.pos.x, projectile[i+(5 % 8)].data.pos.x, projectile[i+(6 % 8)].data.pos.x, projectile[i+(7 % 8)].data.pos.x
    );
    __m256 posy = _mm256_setr_ps(
      projectile[i].data.pos.y,         projectile[i+(1 % 8)].data.pos.y, projectile[i+(2 % 8)].data.pos.y, projectile[i+(3 % 8)].data.pos.y,
      projectile[i+(4 % 8)].data.pos.y, projectile[i+(5 % 8)].data.pos.y, projectile[i+(6 % 8)].data.pos.y, projectile[i+(7 % 8)].data.pos.y
    );
    __m256 velx = _mm256_setr_ps(
      projectile[i].data.vel.x,         projectile[i+(1 % 8)].data.vel.x, projectile[i+(2 % 8)].data.vel.x, projectile[i+(3 % 8)].data.vel.x,
      projectile[i+(4 % 8)].data.vel.x, projectile[i+(5 % 8)].data.vel.x, projectile[i+(6 % 8)].data.vel.x, projectile[i+(7 % 8)].data.vel.x
    );
    __m256 vely = _mm256_setr_ps(
      projectile[i].data.vel.y,         projectile[i+(1 % 8)].data.vel.y, projectile[i+(2 % 8)].data.vel.y, projectile[i+(3 % 8)].data.vel.y,
      projectile[i+(4 % 8)].data.vel.y, projectile[i+(5 % 8)].data.vel.y, projectile[i+(6 % 8)].data.vel.y, projectile[i+(7 % 8)].data.vel.y
    );
    __m256 accelx = _mm256_setr_ps(
      projectile[i].data.accel.x,         projectile[i+(1 % 8)].data.accel.x, projectile[i+(2 % 8)].data.accel.x, projectile[i+(3 % 8)].data.accel.x,
      projectile[i+(4 % 8)].data.accel.x, projectile[i+(5 % 8)].data.accel.x, projectile[i+(6 % 8)].data.accel.x, projectile[i+(7 % 8)].data.accel.x
    );
    __m256 accely = _mm256_setr_ps(
      projectile[i].data.accel.y,         projectile[i+(1 % 8)].data.accel.y, projectile[i+(2 % 8)].data.accel.y, projectile[i+(3 % 8)].data.accel.y,
      projectile[i+(4 % 8)].data.accel.y, projectile[i+(5 % 8)].data.accel.y, projectile[i+(6 % 8)].data.accel.y, projectile[i+(7 % 8)].data.accel.y
    );
    const __m256 fx = _mm256_mul_ps(accelx, dt);
    const __m256 fy = _mm256_mul_ps(accely, dt);
    const __m256 k1x = _mm256_mul_ps(velx, dt);
    const __m256 k1y = _mm256_mul_ps(vely, dt);
    const __m256 k2x = _mm256_mul_ps(_mm256_add_ps(velx, _mm256_mul_ps(half, fx)), dt);
    const __m256 k2y = _mm256_mul_ps(_mm256_add_ps(vely, _mm256_mul_ps(half, fy)), dt);
    const __m256 k3x = k2x;
    const __m256 k3y = k2y;
    const __m256 k4x = _mm256_mul_ps(_mm256_add_ps(velx, fx), dt);
    const __m256 k4y = _mm256_mul_ps(_mm256_add_ps(vely, fy), dt);
    posx = _mm256_add_ps(posx, _mm256_mul_ps(_mm256_div_ps(_mm256_add_ps(_mm256_add_ps(k1x, _mm256_add_ps(_mm256_mul_ps(two, k2x), _mm256_mul_ps(two, k3x))), k4x), six), pix));
    posy = _mm256_add_ps(posy, _mm256_mul_ps(_mm256_div_ps(_mm256_add_ps(_mm256_add_ps(k1y, _mm256_add_ps(_mm256_mul_ps(two, k2y), _mm256_mul_ps(two, k3y))), k4y), six), pix));
    velx = _mm256_add_ps(velx, _mm256_div_ps(_mm256_add_ps(_mm256_add_ps(fx, _mm256_add_ps(_mm256_mul_ps(two, fx), _mm256_mul_ps(two, fx))), fx), six));
    vely = _mm256_add_ps(vely, _mm256_div_ps(_mm256_add_ps(_mm256_add_ps(fy, _mm256_add_ps(_mm256_mul_ps(two, fy), _mm256_mul_ps(two, fy))), fy), six));
    for (Uint k = 0; k < 8 && i + k < projectile.size(); ++k) {
      projectile[i + k].data.pos.x   = posx[k];
      projectile[i + k].data.pos.y   = posy[k];
      projectile[i + k].data.vel.x   = velx[k];
      projectile[i + k].data.vel.y   = vely[k];
      projectile[i + k].data.accel.x = accelx[k];
      projectile[i + k].data.accel.y = accely[k];
    }
  }
}

void Object::draw(void) {
  engine->ren.set_color(RED);
  const SDL_FRect r {
    (data.pos.x + engine->camera.pos.x), (data.pos.y + engine->camera.pos.y), (float)width, (float)height};
  SDL_RenderFillRectF(engine->ren.ren, &r);
}

void Object::move(void) noexcept {
  /* Positive. */
  if (moving_data.direction) {
    data.pos += (moving_data.speed / 100);
    data.pos.constrain(true, moving_data.positive);
    if (data.pos == moving_data.positive) {
      moving_data.direction = false;
    }
  }
  /* Negative. */
  else {
    data.pos -= (moving_data.speed / 100);
    data.pos.constrain(false, moving_data.negative);
    if (data.pos == moving_data.negative) {
      moving_data.direction = true;
    }
  }
}

void Object::calculate_pos_change(float delta_t) noexcept {
  data.pos += {(M_TO_PIXEL(data.vel.x) * delta_t), (M_TO_PIXEL(data.vel.y) * delta_t)};
}
