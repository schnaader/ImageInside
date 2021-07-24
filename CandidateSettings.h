#pragma once

enum class Bitdepth { bpp8, bpp16, bpp24, bpp32 };

struct CandidateSettings {
  int widthMin = 32, widthMax = 4096;
  bool limitHeight = false;
  int heightMin = 32, heightMax = 4096;

  Bitdepth bitDepth = Bitdepth::bpp8;

  float hysteresisMin = 0.5f, hysteresisMax = 0.9f;
};