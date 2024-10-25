#include "../include/prototypes.h"

Vector vector_multi_copy(const Vector *vector, const double scalar) {
  const Vector new_vector = {vector->x * scalar, vector->y * scalar};
  return new_vector;
}

Vector vector_multi(Vector *vector, const double scalar) {
  vector->x *= scalar;
  vector->y *= scalar;
  return *vector;
}

Vector vector_add(Vector *vector, const Vector *add) {
  vector->x += add->x;
  vector->y += add->y;
  return *vector;
}

void vector_set_val(Vector *vector, const double x, const double y) {
  vector->x = x;
  vector->y = y;
}

Vector &Vector::operator*=(const Vector &other) {
  this->x *= other.x;
  this->y *= other.y;
  return *this;
}

Vector &Vector::operator*=(const double scalar) {
  this->x *= scalar;
  this->y *= scalar;
  return *this;
}

void Vector::set(const double x, const double y) {
  this->x = x;
  this->y = y;
}

void Vector::accel(const double accel_x, const double accel_y, const double delta_t) noexcept {
  x += (accel_x * delta_t);
  y += (accel_y * delta_t);
}