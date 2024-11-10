__constant float GRAVITY = 9.806f;
__constant float PIX_PER_M = 100.0f;
#define M_TO_PIX(m) ((m) * PIX_PER_M)

// Vector type.
typedef struct {
  float x;
  float y;
} vec2;

// Vector add.  Return`s a copy. 
inline vec2 vec2_add(const vec2 v1, const vec2 v2) {
  vec2 ret;
  ret.x = (v1.x + v2.x);
  ret.y = (v1.y + v2.y);
  return ret;
}

// Vector add with assign.  Adds to 'vec' in place.
inline void vec2_add_assign(__global vec2 *vec, const vec2 other) {
  vec->x += other.x;
  vec->y += other.y;
}

// Convert 'vec2' to 'float2'.
inline float2 vec2_to_float2(const vec2 vec) {
  return (float2)(vec.x, vec.y);
}

// Convert 'float2' to 'vec2'.
inline vec2 float2_to_vec2(const float2 f2) {
  vec2 ret = {f2.x, f2.y};
  return ret;
}

// Position data.
typedef struct {
  vec2 pos;
  vec2 vel;
  vec2 accel;
} PosData;

// Projectile data.
typedef struct {
  PosData data;
  float width;
  float height;
} Projectile;

__kernel void rk4_step(__global Projectile *p, __global float *dt) {
  // Get current id;
  const uint i = get_global_id(0);
  // Convert projectile vec2 data to float2 data. 
  float2 p_pos   = vec2_to_float2(p[i].data.pos);
  float2 p_vel   = vec2_to_float2(p[i].data.vel);
  float2 p_accel = vec2_to_float2(p[i].data.accel);
  // Calculate k4.
  const float2 f = {(p_accel.x * (*dt)), ((p_accel.y + GRAVITY) * (*dt))};
  const float2 k1 = (p_vel * (*dt));
  const float2 k2 = ((p_vel + 0.5f * f) * (*dt));
  const float2 k3 = k2;
  const float2 k4 = ((p_vel + f) * (*dt));
  p_pos += M_TO_PIX((k1 + 2.0f * k2 + 2.0f * k3 + k4) / 6.0f);
  p_vel += ((f + 2.0f * f + 2.0f * f + f) / 6.0f);  
  p[i].data.pos = float2_to_vec2(p_pos);
  p[i].data.vel = float2_to_vec2(p_vel);
}