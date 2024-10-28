#include "../include/prototypes.h"

void Renderer::set_color(Uchar r, Uchar g, Uchar b, Uchar a) {
  SDL_SetRenderDrawColor(ren, r, g, b, a);
}

void Renderer::fill_rect(const SDL_Rect *rect) {
  SDL_RenderFillRect(ren, rect);
}

/* clang-format off */
void Renderer::draw_rotated_rect(float cx, float cy, float width, float height, float angle, SDL_FPoint *points) {
  /* Convert angle to radians. */
  long double rad = ((long double)angle * (M_PI / 180.0L));
  long double cos_a = cos(rad);
  long double sin_a = sin(rad);
  /* Calculate half-dimensions. */
  long double half_w = ((long double)width / 2);
  long double half_h = ((long double)height / 2);
  /* Calculate corner positions based on rotation. */
  /* Top-Left */
  points[0] = {(float)(cx + ((-half_w * cos_a) - (-half_h * sin_a))), (float)(cy + ((-half_w * sin_a) + (-half_h * cos_a)))};
  /* Top-Right */    
  points[1] = {(float)(cx + (( half_w * cos_a) - (-half_h * sin_a))), (float)(cy + (( half_w * sin_a) + (-half_h * cos_a)))};
  /* Bottom-Right */ 
  points[2] = {(float)(cx + (( half_w * cos_a) - ( half_h * sin_a))), (float)(cy + (( half_w * sin_a) + ( half_h * cos_a)))};
  /* Bottom-Left */  
  points[3] = {(float)(cx + ((-half_w * cos_a) - ( half_h * sin_a))), (float)(cy + ((-half_w * sin_a) + ( half_h * cos_a)))};
  /* Close the rectangle. */
  points[4] = points[0];
  /* Draw the rectangle by connecting all lines. */
  SDL_RenderDrawLinesF(ren, points, 5);
}