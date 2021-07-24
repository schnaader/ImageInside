#pragma once
#include <cstdint>

struct Candidate {
  uint64_t width;
  uint64_t startLine;
  uint64_t endLine;
  uint64_t height;
  uint64_t pixelCount;

  float meanCorrelationCoefficient;
  float score;
};