#include "../include/prototypes.h"

void Renderer::set_color(Uchar r, Uchar g, Uchar b, Uchar a) {
  SDL_SetRenderDrawColor(ren, r, g, b, a);
}

void Renderer::fill_rect(const SDL_Rect *rect) {
  SDL_RenderFillRect(ren, rect);
}

void Renderer::draw_lines(const SDL_FPoint *const &points, int count) {
  SDL_RenderDrawLinesF(ren, points, count);
}

/* clang-format off */
void Renderer::draw_rotated_rect(float cx, float cy, float width, float height, float angle, SDL_FPoint *points) {
  /* Convert angle to radians. */
  float rad = (angle * (M_PI / 180.0L));
  float cos_a = cos(rad);
  float sin_a = sin(rad);
  /* Calculate half-dimensions. */
  float half_w = (width / 2);
  float half_h = (height / 2);
  /* Calculate corner positions based on rotation. */
  points[0] = {(cx + ((-half_w * cos_a) - (-half_h * sin_a))), (cy + ((-half_w * sin_a) + (-half_h * cos_a)))};  /* Top-Left */
  points[1] = {(cx + (( half_w * cos_a) - (-half_h * sin_a))), (cy + (( half_w * sin_a) + (-half_h * cos_a)))};  /* Top-Right */
  points[2] = {(cx + (( half_w * cos_a) - ( half_h * sin_a))), (cy + (( half_w * sin_a) + ( half_h * cos_a)))};  /* Bottom-Right */ 
  points[3] = {(cx + ((-half_w * cos_a) - ( half_h * sin_a))), (cy + ((-half_w * sin_a) + ( half_h * cos_a)))};  /* Bottom-Left */  
  /* Close the rectangle. */
  points[4] = points[0];
  /* Draw the rectangle by connecting all lines. */
  draw_lines(points, 5);
}