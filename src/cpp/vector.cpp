#include "../include/prototypes.h"

Vector vector_multi_copy(const Vector *vector, const float scalar) {
  const Vector new_vector = {vector->x * scalar, vector->y * scalar};
  return new_vector;
}

Vector vector_multi(Vector *vector, const float scalar) {
  vector->x *= scalar;
  vector->y *= scalar;
  return *vector;
}

Vector vector_add(Vector *vector, const Vector *add) {
  vector->x += add->x;
  vector->y += add->y;
  return *vector;
}

void vector_set_val(Vector *vector, const float x, const float y) {
  vector->x = x;
  vector->y = y;
}

/* ----------------------- */
/* <<- Vector method`s ->> */
/* ----------------------- */
void Vector::accel_x(const float x, const float delta_t) noexcept {
  this->x += (x * delta_t);
}

void Vector::accel_y(const float y, const float delta_t) noexcept {
  this->y += (y * delta_t);
}

/* Limit 'x' and 'y' to 'vec' ether positiv or negative. */
void Vector::constrain(bool positive, const Vector &vec) noexcept {
  if (positive) {
    (x > vec.x) ? (x = vec.x) : 0;
    (y > vec.y) ? (y = vec.y) : 0;
  }
  else {
    (x < vec.x) ? (x = vec.x) : 0;
    (y < vec.y) ? (y = vec.y) : 0;
  }
}

void Vector::adjust_angle(float angle) {
  float radians = (angle * (M_PI / 180.0f));
  y = (x * tan(radians));
}
