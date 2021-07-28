#include "CandidateFinder.h"
#include "Settings.h"
#include "imgui.h"
#include "ImFileDialog/ImFileDialog.h"
#include <algorithm>

Settings& Settings::getInstance() {
  static Settings instance;
  return instance;
}

CandidateFinder* Settings::showSettingsWindow(CandidateFinder* globalCandidateFinder, bool& newFileWasOpened) {
  CandidateFinder* candidateFinder = globalCandidateFinder;

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
  bool hysteresisMinChanged = false, hysteresisMaxChanged = false;
  hysteresisMinChanged = ImGui::SliderFloat("min##3", &globalSettings.hysteresisMin, 0.0f, 1.0f);
  hysteresisMaxChanged = ImGui::SliderFloat("max##3", &globalSettings.hysteresisMax, 0.0f, 1.0f);

  // a file can be opened to start analysis, an already running analysis
  // will be cancelled
  if (ImGui::Button("Open file..."))
    ifd::FileDialog::Instance().Open("FileOpenDialog", "Open file", ",.*");

  if (ifd::FileDialog::Instance().IsDone("FileOpenDialog")) {
    if (ifd::FileDialog::Instance().HasResult()) {
      auto selectedFile = ifd::FileDialog::Instance().GetResult();

      uint64_t filesize = std::filesystem::file_size(selectedFile);
      unsigned char* data = (unsigned char*)malloc(filesize);
      if (data != nullptr) {
        FILE* f = fopen(selectedFile.u8string().c_str(), "rb");
        unsigned char* dataptr = data;
        size_t bytes_read;
        do {
          bytes_read = fread(dataptr, 1, 1024, f);
          dataptr += bytes_read;
        } while (bytes_read > 0);
        fclose(f);

        if (candidateFinder != nullptr) {
          delete candidateFinder;
          candidateFinder = nullptr;
        }

        candidateFinder = new CandidateFinder(globalSettings, data, filesize);
        newFileWasOpened = true;
      }
    }
    ifd::FileDialog::Instance().Close();
  }

  ImGui::End();

  // depending on which of the min/max values was changed, we want to adjust the other if needed
  // to satisfy the min <= max condition
  if (hysteresisMinChanged) {
    globalSettings.hysteresisMax = std::max(globalSettings.hysteresisMin, globalSettings.hysteresisMax);
    globalSettings.hysteresisMin = std::min(globalSettings.hysteresisMin, globalSettings.hysteresisMax);
  }
  else if (hysteresisMaxChanged) {
    globalSettings.hysteresisMin = std::min(globalSettings.hysteresisMin, globalSettings.hysteresisMax);
    globalSettings.hysteresisMax = std::max(globalSettings.hysteresisMin, globalSettings.hysteresisMax);
  }

  return candidateFinder;
}