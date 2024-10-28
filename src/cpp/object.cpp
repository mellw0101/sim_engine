#include "../include/prototypes.h"

Object *object_create(const Vector &pos, const Ushort width, const Ushort height, const Vector &friction, Uint state,
                      const float max_speed) {
  Object *object = (Object *)malloc(sizeof(Object));
  if (!object) {
    fprintf(stderr, "Failed to malloc Object.\n");
    return nullptr;
  }
  object->pos         = pos;
  object->vel         = 0;
  object->width       = width;
  object->height      = height;
  object->friction    = friction;
  object->state       = state;
  object->max_speed.x = max_speed;
  object->max_speed.y = max_speed;
  object->flag.clear();
  object->next = nullptr;
  if (!objects_head) {
    objects_head = object;
    objects_tail = object;
  }
  else {
    objects_tail->next = object;
    objects_tail       = objects_tail->next;
  }
  return object;
}

void unlink_object(Object *obj) {
  /* If the object we want to unlink is the head then set the head to the next obj. */
  if (obj == objects_head) {
    objects_head = obj->next;
    /* If there is only one object make sure to set the tail aswell.  In this
     * case that would be nullptr for both as there only was one object. */
    if (obj == objects_tail) {
      objects_tail = objects_head;
    }
  }
  else {
    FOR_EACH_OBJECT(object) {
      if (object->next == obj) {
        object->next = obj->next;
        /* If the object we want to unlink is the tail then set the previus obj as the tail. */
        if (obj == objects_tail) {
          objects_tail = object;
        }
        break;
      }
    }
  }
  free(obj);
}

void Object::draw(void) {
  engine->ren.set_color(RED);
  const SDL_FRect r {(float)pos.x, (float)pos.y, (float)width, (float)height};
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
