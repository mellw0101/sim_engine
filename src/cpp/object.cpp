#include "../include/prototypes.h"

Object *object_create(const MVec2 &pos, const Ushort width, const Ushort height, const MVec2 &friction, Uint state,
                      const float max_speed) {
  Object *obj = (Object *)malloc(sizeof(*obj));
  if (!obj) {
    fprintf(stderr, "Failed to malloc Object.\n");
    return nullptr;
  }
  obj->pos         = pos;
  obj->vel         = 0;
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
    obj->prev    = objects_tail;
    objects_tail = objects_tail->next;
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

void projectile_collision(Object *projectile, Object *object) {
  if (object->flag.is_set<OBJECT_IS_PROJECTILE>()) {
    return;
  }
  if (OBJECTS_COLLIDING(projectile, object)) {
    return;
  }
}


void Object::draw(void) {
  engine->ren.set_color(RED);
  const SDL_FRect r {(pos.x + engine->camera.pos.x), (pos.y + engine->camera.pos.y), (float)width, (float)height};
  SDL_RenderFillRectF(engine->ren.ren, &r);
}

void Object::move(void) noexcept {
  /* Positive. */
  if (moving_data.direction) {
    pos += (moving_data.speed / 100);
    pos.constrain(true, moving_data.positive);
    if (pos == moving_data.positive) {
      moving_data.direction = false;
    }
  }
  /* Negative. */
  else {
    pos -= (moving_data.speed / 100);
    pos.constrain(false, moving_data.negative);
    if (pos == moving_data.negative) {
      moving_data.direction = true;
    }
  }
}

void Object::calculate_pos_change(float delta_t) noexcept {
  pos += {(M_TO_PIXEL(vel.x) * delta_t), (M_TO_PIXEL(vel.y) * delta_t)};
}
