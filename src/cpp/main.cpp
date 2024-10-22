#include "../include/prototypes.h"

void physics(void) {
  for (Object *object = object_head; object; object = object->next) {
    object_draw(&object);
    if (object->state & STATIC) {
      continue;
    }
    (object->state &= ~ON_STATIC_OBJECT);
    if (object->state & JUMP) {
      object->vel.y = -5.00;
      PRINT_OBJ_VEL_POS(object);
      object->state &= ~JUMP;
    }
    for (Uchar i = 0; i < TIME_STEP; ++i) {
      /* Apply acceleration due to gravity. */
      ACCEL(object->vel.y, GRAVITY, TIME_STEP_S);
      /* Now calculate new position based on velocity. */
      POS(object->pos.x, M_TO_PIXEL(object->vel.x), TIME_STEP_S);
      POS(object->pos.y, M_TO_PIXEL(object->vel.y), TIME_STEP_S);
      HIT_WALL(object, 0.00, window_width - object->width, -100, window_height - object->height);
      for (Object *static_object = object_head; static_object; static_object = static_object->next) {
        if (object == static_object || (static_object->state & STATIC) == FALSE) {
          continue;
        }
        object_collision_with_static_obj(&object, &static_object);
      }
    }
  }
}

#define PLAYER_X_ACCEL 9
#define PLAYER_Y_ACCEL 9
#define FRICTON        0x03

int main(void) {
  object_create((600 - 10), 2, 20, 20, PLAYER_X_ACCEL, PLAYER_Y_ACCEL, NO_STATE, 5);
  object_create(100, (window_height - 90), window_width - 100, 5, FRICTON, FRICTON, STATIC, 5);
  object_create(0, (window_height - 5), window_width, 5, 0, 0, STATIC, 5);
  init();
  while (running) {
    clear_frame();
    render_text();
    prosses_keys();
    physics();
    poll_for_events();
    update_frame();
  }
  cleanup();
}
