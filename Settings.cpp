#include "CandidateFinder.h"
#include "Settings.h"
#include "imgui.h"
#include "ImFileDialog/ImFileDialog.h"
#include <algorithm>

Settings& Settings::getInstance() {
  static Settings instance;
  return instance;
}

CandidateFinder* Settings::showSettingsWindow(CandidateFinder* globalCandidateFinder) {
  CandidateFinder* candidateFinder = globalCandidateFinder;

  float oldHysteresisMin = globalSettings.hysteresisMin;
  float oldHysteresisMax = globalSettings.hysteresisMax;

  ImGui::Begin("Settings");

  ImGui::Text("Width");
  ImGui::InputInt("min##1", &globalSettings.widthMin);
  ImGui::InputInt("max##1", &globalSettings.widthMax);

  ImGui::Text("Limit height");
  ImGui::SameLine();
  ImGui::Checkbox("##limitHeight", &globalSettings.limitHeight);
  if (globalSettings.limitHeight) {
    ImGui::Text("Height");
    ImGui::InputInt("min##2", &globalSettings.heightMin);
    ImGui::InputInt("max##2", &globalSettings.heightMax);
  }

  ImGui::Text("Bitdepth");
  static int current_bitdepthInt = 0;
  const char* bitDepthItems[] = { "8 bit", "16 bit", "24 bit", "32 bit"};
  ImGui::Combo("bitdepth", &current_bitdepthInt, bitDepthItems, IM_ARRAYSIZE(bitDepthItems));
  globalSettings.bitDepth = static_cast<Bitdepth>(current_bitdepthInt);

  ImGui::Text("Absolute correlation hysteresis");
  ImGui::SliderFloat("min##3", &globalSettings.hysteresisMin, 0.0f, 1.0f);
  ImGui::SliderFloat("max##3", &globalSettings.hysteresisMax, 0.0f, 1.0f);

  // a file can be opened to start analysis when no analysis is running at the moment
  if ((candidateFinder == nullptr) || (candidateFinder->finderState == FinderState::ready)) {
    if (ImGui::Button("Open file..."))
      ifd::FileDialog::Instance().Open("FileOpenDialog", "Open file", ",.*");

    if (ifd::FileDialog::Instance().IsDone("FileOpenDialog")) {
      if (ifd::FileDialog::Instance().HasResult()) {
        std::string res = ifd::FileDialog::Instance().GetResult().u8string();

        if (candidateFinder != nullptr) {
          if (candidateFinder->finderState == FinderState::ready) {
            delete candidateFinder;
            candidateFinder = nullptr;
          }
        }

        if (candidateFinder == nullptr) {
          candidateFinder = new CandidateFinder(globalSettings, nullptr, 0);
        }
      }
      ifd::FileDialog::Instance().Close();
    }
  }

  ImGui::End();

  // depending on which of the min/max values was changed, we want to adjust the other if needed
  // to satisfy the min <= max condition
  if (oldHysteresisMin != globalSettings.hysteresisMin) {
    globalSettings.hysteresisMax = std::max(globalSettings.hysteresisMin, globalSettings.hysteresisMax);
    globalSettings.hysteresisMin = std::min(globalSettings.hysteresisMin, globalSettings.hysteresisMax);
  }
  else if (oldHysteresisMax != globalSettings.hysteresisMax) {
    globalSettings.hysteresisMin = std::min(globalSettings.hysteresisMin, globalSettings.hysteresisMax);
    globalSettings.hysteresisMax = std::max(globalSettings.hysteresisMin, globalSettings.hysteresisMax);
  }

  return candidateFinder;
}