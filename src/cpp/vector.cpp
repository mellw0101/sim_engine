#include "../include/prototypes.h"

MVec2 vector_multi_copy(const MVec2 *vector, const float scalar) {
  const MVec2 new_vector = {vector->x * scalar, vector->y * scalar};
  return new_vector;
}

MVec2 vector_multi(MVec2 *vector, const float scalar) {
  vector->x *= scalar;
  vector->y *= scalar;
  return *vector;
}

MVec2 vector_add(MVec2 *vector, const MVec2 *add) {
  vector->x += add->x;
  vector->y += add->y;
  return *vector;
}

void vector_set_val(MVec2 *vector, const float x, const float y) {
  vector->x = x;
  vector->y = y;
}
