#include "Settings.h"
#include "imgui.h"
#include <algorithm>

Settings& Settings::getInstance() {
  static Settings instance;
  return instance;
}

void Settings::showSettingsWindow() {

  float oldHysteresisMin = hysteresisMin;
  float oldHysteresisMax = hysteresisMax;

  ImGui::Begin("Settings");

  ImGui::Text("Width");
  ImGui::InputInt("min##1", &widthMin);
  ImGui::InputInt("max##1", &widthMax);

  ImGui::Text("Limit height");
  ImGui::SameLine();
  ImGui::Checkbox("##limitHeight", &limitHeight);
  if (limitHeight) {
    ImGui::Text("Height");
    ImGui::InputInt("min##2", &heightMin);
    ImGui::InputInt("max##2", &heightMax);
  }

  ImGui::Text("Bitdepth");
  static int current_bitdepthInt = 0;
  const char* bitDepthItems[] = { "8 bit", "16 bit", "24 bit", "32 bit"};
  ImGui::Combo("bitdepth", &current_bitdepthInt, bitDepthItems, IM_ARRAYSIZE(bitDepthItems));
  bitDepth = static_cast<Bitdepth>(current_bitdepthInt);

  ImGui::Text("Absolute correlation hysteresis");
  ImGui::SliderFloat("min##3", &hysteresisMin, 0.0f, 1.0f);
  ImGui::SliderFloat("max##3", &hysteresisMax, 0.0f, 1.0f);

  ImGui::End();

  // depending on which of the min/max values was changed, we want to adjust the other if needed
  // to satisfy the min <= max condition
  if (oldHysteresisMin != hysteresisMin) {
    hysteresisMax = std::max(hysteresisMin, hysteresisMax);
    hysteresisMin = std::min(hysteresisMin, hysteresisMax);
  }
  else if (oldHysteresisMax != hysteresisMax) {
    hysteresisMin = std::min(hysteresisMin, hysteresisMax);
    hysteresisMax = std::max(hysteresisMin, hysteresisMax);
  }
}