#include "../include/prototypes.h"
// clang-format off

void init_compute_shader(void) {
  __glewUseProgram(comp_shader);
  __glewGenBuffers(1, &SSBO);
  __glewUniform1f(__glewGetUniformLocation(comp_shader, "delta_time"), FRAMETIME_S);
  __glewUniform1f(__glewGetUniformLocation(comp_shader, "pix_per_m"), PIXELS_PER_METER);
  __glewUniform2f(__glewGetUniformLocation(comp_shader, "gravity"), 0.0f, GRAVITY);
}

void input_compute_buffer(void) {
  __glewBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
  __glewBufferData(
    GL_SHADER_STORAGE_BUFFER,
    (compute_buffer.size() * sizeof(PosData)),
    compute_buffer.data(),
    GL_DYNAMIC_COPY
  );
  __glewBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBO);
}

void dispatch_compute_shader(void) {
  __glewUseProgram(comp_shader);
  __glewDispatchCompute(compute_buffer.size(), 1, 1);
  __glewMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void retrieve_compute_buffer(void) {
  PosData *compute_data = (PosData *)__glewMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);
  for (Uint i = 0; i < compute_buffer.size(); ++i) {
    compute_buffer[i] = compute_data[i];
  }
  __glewUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
  __glewBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}