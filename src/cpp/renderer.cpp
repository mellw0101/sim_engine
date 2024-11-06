#include "../include/prototypes.h"

void Renderer::set_color(Uchar r, Uchar g, Uchar b, Uchar a) {
  SDL_SetRenderDrawColor(ren, r, g, b, a);
}

void Renderer::fill_rect(const SDL_Rect *rect) {
  SDL_RenderFillRect(ren, rect);
}

void Renderer::fill_rect(const SDL_FRect *rect) {
  SDL_RenderFillRectF(ren, rect);
}

void Renderer::draw_lines(const SDL_FPoint *const &points, int count) {
  SDL_RenderDrawLinesF(ren, points, count);
}

/* clang-format off */
void Renderer::draw_rotated_rect(float cx, float cy, float width, float height, float angle, SDL_FPoint *points) {
  /* Convert angle to radians. */
  float rad = (angle * (M_PI / 180.0f));
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

void Renderer::draw_rotated_box(float cx, float cy, float width, float height, float angle,
                                const Color *colors, SDL_FPoint *points, SDL_Texture *texture) {
  const float hw = (width / 2);
  const float hh = (height / 2);
  const float rad = radiansf(angle);
  const float cosv = cos(rad);
  const float sinv = sin(rad);
  Color c[4];
  !colors ? (c[TL] = {255, 0,     0, 255}) : (c[TL] = colors[TL]);
  !colors ? (c[TR] = {0,   255,   0, 255}) : (c[TR] = colors[TR]);
  !colors ? (c[BR] = {0,   0,   255, 255}) : (c[BR] = colors[BR]);
  !colors ? (c[BL] = {255, 255,   0, 255}) : (c[BL] = colors[BL]);
  const SDL_Vertex verts[4] = {
    {{(cx + ((-hw * cosv) - (-hh * sinv))), (cy + (-hw * sinv) + (-hh * cosv))}, c[TL], {0, 0}},
    {{(cx + (( hw * cosv) - (-hh * sinv))), (cy + ( hw * sinv) + (-hh * cosv))}, c[TR], {1, 0}},
    {{(cx + (( hw * cosv) - ( hh * sinv))), (cy + ( hw * sinv) + ( hh * cosv))}, c[BR], {1, 1}},
    {{(cx + ((-hw * cosv) - ( hh * sinv))), (cy + (-hw * sinv) + ( hh * cosv))}, c[BL], {0, 1}},
  };
  points ? (points[TL] = verts[TL].position) : (SDL_FPoint)0;
  points ? (points[TR] = verts[TR].position) : (SDL_FPoint)0;
  points ? (points[BR] = verts[BR].position) : (SDL_FPoint)0;
  points ? (points[BL] = verts[BL].position) : (SDL_FPoint)0;
  int indices[6] = {0, 1, 2, 2, 3, 0};
  SDL_RenderGeometry(ren, texture, verts, 4, indices, 6);
}

void Renderer::draw_triangle(SDL_FPoint p1, SDL_FPoint p2, SDL_FPoint p3, const Color *colors, SDL_Texture *texture) {
  SDL_Color c[3];
  !colors ? (c[0] = {RED}  ) : (c[0] = colors[0]);
  !colors ? (c[1] = {GREEN}) : (c[1] = colors[1]);
  !colors ? (c[2] = {BLUE} ) : (c[2] = colors[2]);
  const SDL_Vertex verts[3] = {
    {p1, c[0], {0, 0}},
    {p2, c[1], {1, 0}},
    {p3, c[2], {0.5f, 1}}
  };
  const int indices[3] = {0, 1, 2};
  SDL_RenderGeometry(ren, texture, verts, 3, indices, 3);
}

void Renderer::draw_filled_arc(SDL_FPoint *points, Uint num_points, Color color, const MVec2 &offset, SDL_Texture *texture) {
  const Color c[3] = {color, color, color};
  for (Uint i = 0; i < (num_points - 1); ++i) {
    draw_triangle(points[i], points[(i + 1)], MVec2::center_with_offset(points[i], points[(i + 1)], offset), c);
  }
}

void Renderer::draw_quadliteral(const SDL_FPoint points[4], const Color colors[4], SDL_Texture *texture) {
  const SDL_Vertex verts[4] = {
    { points[0], colors[0], {0, 0} },
    { points[1], colors[1], {1, 0} },
    { points[2], colors[2], {1, 1} },
    { points[3], colors[3], {0, 1} }
  };
  const int indices[6] = { 0, 1, 2, 2, 3, 0 };
  SDL_RenderGeometry(ren, texture, verts, 4, indices, 6);
}