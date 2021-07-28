#pragma once
#include <cstdint>
#include "CandidateSettings.h"

struct Candidate {
  uint64_t width;
  uint64_t height;
  uint64_t startOffset;
  uint64_t startLine;
  uint64_t endLine;
  uint64_t pixelCount;
  int bytePerPixel;

  float meanCorrelationCoefficient;
  float score;
};