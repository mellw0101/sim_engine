#include "../include/prototypes.h"

Object *object_create(const double pos_x, const double pos_y, const Ushort width, const Ushort height,
                      const double friction_x, const double friction_y, Uint state, const double max_speed) {
  Object *object = (Object *)malloc(sizeof(Object));
  if (!object) {
    fprintf(stderr, "Failed to malloc Object.\n");
    return nullptr;
  }
  object->pos.x       = pos_x;
  object->pos.y       = pos_y;
  object->vel.x       = 0;
  object->vel.y       = 0;
  object->width       = width;
  object->height      = height;
  object->friction.x  = friction_x;
  object->friction.y  = friction_y;
  object->state       = state;
  object->max_speed.x = max_speed;
  object->flag.clear();
  object->next        = nullptr;
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

SDL_Rect object_rect(Object *const *object) {
  SDL_Rect rect = {(int)(*object)->pos.x, (int)(*object)->pos.y, (*object)->width, (*object)->height};
  return rect;
}

/* Check if 'object' is colliding with a 'static_object'. */
void object_collision_with_static_obj(Object *const *object, Object *const *static_object) {
  if (OBJECTS_COLLIDING(*object, *static_object)) {
    if (OBJ_OVERLAP_LEAST_T(*object, *static_object)) {
      (*object)->pos.y = OBJ_T(*static_object) - (*object)->height;
      CONSTRAIN_VEL_IF(((*object)->vel.y > 0.00), (*object)->vel.y)
      ((*object)->state |= ON_STATIC_OBJECT);
      /* Apply friction if 'static_object' has any. */
      if ((*static_object)->friction.y > 0.00) {
        if ((*object)->vel.x > 0.00) {
          ACCEL((*object)->vel.x, -(*static_object)->friction.y, TIME_STEP_S);
          CONSTRAIN_VEL_IF(((*object)->vel.x < 0.00), (*object)->vel.x);
        }
        else if ((*object)->vel.x < 0.00) {
          ACCEL((*object)->vel.x, (*static_object)->friction.y, TIME_STEP_S);
          CONSTRAIN_VEL_IF(((*object)->vel.x > 0.00), (*object)->vel.x)
        }
      }
      return;
    }
    else if (OBJ_OVERLAP_LEAST_B(*object, *static_object)) {
      OBJ_T(*object) = OBJ_B(*static_object);
      CONSTRAIN_VEL_IF(((*object)->vel.y < 0.00), (*object)->vel.y);
    }
    else if (OBJ_OVERLAP_LEAST_L(*object, *static_object)) {
      (*object)->pos.x = OBJ_L(*static_object) - (*object)->width;
      CONSTRAIN_VEL_IF(((*object)->vel.x > 0.00), (*object)->vel.x);
    }
    else if (OBJ_OVERLAP_LEAST_R(*object, *static_object)) {
      OBJ_L(*object) = OBJ_R(*static_object);
      CONSTRAIN_VEL_IF(((*object)->vel.x < 0.00), (*object)->vel.x);
    }
  }
}

void Object::draw(void) {
  SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
  const SDL_Rect r {(int)pos.x, (int)pos.y, width, height};
  SDL_RenderFillRect(ren, &r);
}
