#include "../include/prototypes.h"

/* Create a point of accelaration at some step of time to be calculated later. */
AccelPoint *create_accel_point(double vel_x, double vel_y, double time, AccelPoint *last_accel_point) {
  AccelPoint *new_point = (AccelPoint *)malloc(sizeof(AccelPoint));
  if (!new_point) {
    fprintf(stderr, "Failed to malloc accel point.\n");
    exit(1);
  }
  new_point->vel.x = vel_x;
  new_point->vel.y = vel_y;
  new_point->time  = time;
  new_point->next  = nullptr;
  if (last_accel_point) {
    last_accel_point->next = new_point;
    last_accel_point       = last_accel_point->next;
  }
  return new_point;
}