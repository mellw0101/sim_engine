#include "../include/prototypes.h"

double PerformanceData::PerformanceMetric::mean(void) {
  return (std::accumulate(data.begin(), data.end(), 0.0) / data.size());
}

double PerformanceData::PerformanceMetric::min(void) {
  const double min = *std::min_element(data.begin(), data.end());
  return min;
}

double PerformanceData::PerformanceMetric::max(void) {
  const double max = *std::max_element(data.begin(), data.end());
  return max;
}

void PerformanceData::print_report(void) noexcept {
  printf("Performance report\n"
         "  Frame count: %lu\n"
         "\n"
         "  Frametime:\n"
         "    Mean: %lf ms\n"
         "     Min: %lf ms\n"
         "     Max: %lf ms\n"
         "\n"
         "  Added frametime:\n"
         "    Mean: %lf ms\n"
         "     Min: %lf ms\n"
         "     Max: %lf ms\n",
         framecount, frametime.mean(), frametime.min(), frametime.max(), added_frametime.mean(), added_frametime.min(),
         added_frametime.max());
}
