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
    /* Create the obj arrays with a single value each. */
    const __avx o_right (OBJ_R(obj));
    const __avx o_bottom(OBJ_B(obj));
    const __avx o_left  (OBJ_L(obj));
    const __avx o_top   (OBJ_T(obj));
    for (Uint i = 0; i < projectile.size(); i += 8) {
      const __avx p_left(  OBJ_L(&projectile[i]),   OBJ_L(&projectile[i+1]), OBJ_L(&projectile[i+2]), OBJ_L(&projectile[i+3]),
                           OBJ_L(&projectile[i+4]), OBJ_L(&projectile[i+5]), OBJ_L(&projectile[i+6]), OBJ_L(&projectile[i+7]));
      const __avx p_top(   OBJ_T(&projectile[i]),   OBJ_T(&projectile[i+1]), OBJ_T(&projectile[i+2]), OBJ_T(&projectile[i+3]),
                           OBJ_T(&projectile[i+4]), OBJ_T(&projectile[i+5]), OBJ_T(&projectile[i+6]), OBJ_T(&projectile[i+7]));
      const __avx p_right( OBJ_R(&projectile[i]),   OBJ_R(&projectile[i+1]), OBJ_R(&projectile[i+2]), OBJ_R(&projectile[i+3]),
                           OBJ_R(&projectile[i+4]), OBJ_R(&projectile[i+5]), OBJ_R(&projectile[i+6]), OBJ_R(&projectile[i+7]));
      const __avx p_bottom(OBJ_B(&projectile[i]),   OBJ_B(&projectile[i+1]), OBJ_B(&projectile[i+2]), OBJ_B(&projectile[i+3]),
                           OBJ_B(&projectile[i+4]), OBJ_B(&projectile[i+5]), OBJ_B(&projectile[i+6]), OBJ_B(&projectile[i+7]));
      /* Perform simd cmp. */
      const __avx cmp_right_ge_left = (o_right  >= p_left); 
      const __avx cmp_bottom_ge_top = (o_bottom >= p_top); 
      const __avx cmp_left_le_right = (o_left   <= p_right); 
      const __avx cmp_top_le_bottom = (o_top    <= p_bottom);
      /* Combine result. */
      const __m256 horizontal_overlap = _mm256_and_ps(cmp_right_ge_left, cmp_left_le_right);
      const __m256 vertical_overlap   = _mm256_and_ps(cmp_bottom_ge_top, cmp_top_le_bottom);
      /* Extract mask. */
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
  /* Define some constants. */
  const __avx dt  (delta_t);
  const __avx half(0.5f);
  const __avx six (6.0f);
  const __avx two (2.0f);
  const __avx pix (PIXELS_PER_METER);
  for (Uint i = 0; i < projectile.size(); i += 8) {
    __avx posx(  projectile[i].data.pos.x,     projectile[i+1].data.pos.x,   projectile[i+2].data.pos.x,   projectile[i+3].data.pos.x,
                 projectile[i+4].data.pos.x,   projectile[i+5].data.pos.x,   projectile[i+6].data.pos.x,   projectile[i+7].data.pos.x);
    __avx posy(  projectile[i].data.pos.y,     projectile[i+1].data.pos.y,   projectile[i+2].data.pos.y,   projectile[i+3].data.pos.y,
                 projectile[i+4].data.pos.y,   projectile[i+5].data.pos.y,   projectile[i+6].data.pos.y,   projectile[i+7].data.pos.y);
    __avx velx(  projectile[i].data.vel.x,     projectile[i+1].data.vel.x,   projectile[i+2].data.vel.x,   projectile[i+3].data.vel.x,
                 projectile[i+4].data.vel.x,   projectile[i+5].data.vel.x,   projectile[i+6].data.vel.x,   projectile[i+7].data.vel.x);
    __avx vely(  projectile[i].data.vel.y,     projectile[i+1].data.vel.y,   projectile[i+2].data.vel.y,   projectile[i+3].data.vel.y,
                 projectile[i+4].data.vel.y,   projectile[i+5].data.vel.y,   projectile[i+6].data.vel.y,   projectile[i+7].data.vel.y);
    __avx accelx(projectile[i].data.accel.x,   projectile[i+1].data.accel.x, projectile[i+2].data.accel.x, projectile[i+3].data.accel.x,
                 projectile[i+4].data.accel.x, projectile[i+5].data.accel.x, projectile[i+6].data.accel.x, projectile[i+7].data.accel.x);
    __avx accely(projectile[i].data.accel.y,   projectile[i+1].data.accel.y, projectile[i+2].data.accel.y, projectile[i+3].data.accel.y,
                 projectile[i+4].data.accel.y, projectile[i+5].data.accel.y, projectile[i+6].data.accel.y, projectile[i+7].data.accel.y);
    const __avx fx  = (accelx * dt);
    const __avx fy  = (accely * dt);
    const __avx k1x = (velx * dt);
    const __avx k1y = (vely * dt);
    const __avx k2x = ((velx + (half * fx)) * dt);
    const __avx k2y = ((vely + (half * fy)) * dt);
    const __avx k3x = k2x;
    const __avx k3y = k2y;
    const __avx k4x = ((velx + fx) * dt);
    const __avx k4y = ((vely + fy) * dt);
    posx += ((((k1x + ((two * k2x) + (two * k3x))) + k4x) / six) * pix);
    posy += ((((k1y + ((two * k2y) + (two * k3y))) + k4y) / six) * pix);
    velx += (((fx + ((two * fx) + (two * fx))) + fx) / six);
    vely += (((fy + ((two * fy) + (two * fy))) + fy) / six);
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
