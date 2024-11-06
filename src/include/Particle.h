#pragma once

#include <Mlib/Flag.h>
#include <Mlib/MVec2.h>
#include <Mlib/Vector.h>
#include <cstdio>

/* clang-format off */

/* Example structure to encapsulate a particle's state */
typedef struct Particle {
  MVec2 pos;
  MVec2 prev_pos;
  bool pinned;
} Particle;

class Cloth {
 private:
  MVector<Particle> particles;
  int width;
  int height;
  int spacing;

  void satisfy_constraints(Particle &p1, Particle &p2) {
    MVec2 delta = (p1.pos - p2.pos);
    float distance = delta.magnitude();
    float diff = ((distance - spacing) / distance);
    MVec2 offset = (delta * (diff * 0.5f));
    if (!p1.pinned) {
      p1.pos = (p1.pos - offset);
    }
    if (!p2.pinned) {
      p2.pos = (p2.pos + offset);
    }
  }

  void apply_constraints(void) {
    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
        int index = (y * width + x);
        if (x < (width - 1)) {
          satisfy_constraints(particles[index], particles[index + 1]);
        }
        if (y < (height - 1)) {
          satisfy_constraints(particles[index], particles[index + width]);
        }
      }
    }
  }

 public:
 
  MVector<Particle> &get_particles(void) {
    return particles;
  }

  int get_width(void) const {
    return width;
  }

  int get_height(void) const {
    return height;
  }
  void update(void) {
    for (auto &particle : particles) {
      if (!particle.pinned) {
        const MVec2 vel = (particle.pos - particle.prev_pos);
        particle.prev_pos = particle.pos;
        particle.pos = (particle.pos + vel + gravity_vector);
      }
    }
    apply_constraints();
  }

  Cloth(int width, int height, int particle_spacing) : width(width), height(height), spacing(particle_spacing) {
    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
        Particle particle = {{((float)x * spacing), (float(y * spacing))}, {((float)x * spacing), (float(y * spacing))}, false};
        if ((y == 0) || ((x == 0) || (x == (width - 1)))) {
          particle.pinned = true;
        }
        particles.push_back(particle);
      }
    }
  }
};
