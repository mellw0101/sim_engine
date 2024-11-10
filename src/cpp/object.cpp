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
    __m256 o_right  = _mm256_setr_ps(OBJ_R(obj), OBJ_R(obj), OBJ_R(obj), OBJ_R(obj), OBJ_R(obj), OBJ_R(obj), OBJ_R(obj), OBJ_R(obj));
    __m256 o_bottom = _mm256_setr_ps(OBJ_B(obj), OBJ_B(obj), OBJ_B(obj), OBJ_B(obj), OBJ_B(obj), OBJ_B(obj), OBJ_B(obj), OBJ_B(obj));
    __m256 o_left   = _mm256_setr_ps(OBJ_L(obj), OBJ_L(obj), OBJ_L(obj), OBJ_L(obj), OBJ_L(obj), OBJ_L(obj), OBJ_L(obj), OBJ_L(obj));
    __m256 o_top    = _mm256_setr_ps(OBJ_T(obj), OBJ_T(obj), OBJ_T(obj), OBJ_T(obj), OBJ_T(obj), OBJ_T(obj), OBJ_T(obj), OBJ_T(obj));
    for (Uint i = 0; (i + 8) < projectile.size(); i += 8) {
      __m256 p_left    = _mm256_setr_ps(OBJ_L(&projectile[i]),   OBJ_L(&projectile[i+1]), OBJ_L(&projectile[i+2]), OBJ_L(&projectile[i+3]),
                                        OBJ_L(&projectile[i+4]), OBJ_L(&projectile[i+5]), OBJ_L(&projectile[i+6]), OBJ_L(&projectile[i+7]));
      __m256 p_top     = _mm256_setr_ps(OBJ_T(&projectile[i]),   OBJ_T(&projectile[i+1]), OBJ_T(&projectile[i+2]), OBJ_T(&projectile[i+3]),
                                        OBJ_T(&projectile[i+4]), OBJ_T(&projectile[i+5]), OBJ_T(&projectile[i+6]), OBJ_T(&projectile[i+7]));
      __m256 p_right   = _mm256_setr_ps(OBJ_R(&projectile[i]),   OBJ_R(&projectile[i+1]), OBJ_R(&projectile[i+2]), OBJ_R(&projectile[i+3]),
                                        OBJ_R(&projectile[i+4]), OBJ_R(&projectile[i+5]), OBJ_R(&projectile[i+6]), OBJ_R(&projectile[i+7]));
      __m256 p_bottom  = _mm256_setr_ps(OBJ_B(&projectile[i]),   OBJ_B(&projectile[i+1]), OBJ_B(&projectile[i+2]), OBJ_B(&projectile[i+3]),
                                        OBJ_B(&projectile[i+4]), OBJ_B(&projectile[i+5]), OBJ_B(&projectile[i+6]), OBJ_B(&projectile[i+7]));
      // Perform simd cmp.
      __m256 cmp_right_ge_left = _mm256_cmp_ps(o_right, p_left, _CMP_GE_OQ);
      __m256 cmp_bottom_ge_top = _mm256_cmp_ps(o_bottom, p_top, _CMP_GE_OQ);
      __m256 cmp_left_le_right = _mm256_cmp_ps(o_left, p_right, _CMP_LE_OQ);
      __m256 cmp_top_le_bottom = _mm256_cmp_ps(o_top, p_bottom, _CMP_LE_OQ);
      // Combine result.
      __m256 horizontal_overlap = _mm256_and_ps(cmp_right_ge_left, cmp_left_le_right);
      __m256 vertical_overlap   = _mm256_and_ps(cmp_bottom_ge_top, cmp_top_le_bottom);
      // Extract mask.
      int mask = _mm256_movemask_ps(_mm256_and_ps(horizontal_overlap, vertical_overlap));
      for (Uint k = 0; k < 8; ++k) {
        if ((mask & (1 << k)) == (1 << k)) {
          projectile.erase_at(projectile.index_of(&projectile[i + k]));
        }
      }
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
