#pragma once
#include "CandidateSettings.h"

class Settings
{
  // singleton class
  private:
    Settings() {}
  public:
    static Settings& getInstance();
    Settings(Settings const&) = delete;
    void operator=(Settings const&) = delete;

  // data and GUI
  public:
    CandidateSettings globalSettings;

    CandidateFinder* showSettingsWindow(CandidateFinder* globalCandidateFinder, bool& newFileWasOpened);
};