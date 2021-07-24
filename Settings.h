#pragma once
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
    int widthMin = 32, widthMax = 4096;
    bool limitHeight;
    int heightMin = 32, heightMax = 4096;

    enum class Bitdepth { bpp8, bpp16, bpp24, bpp32 };
    Bitdepth bitDepth;

    float hysteresisMin, hysteresisMax;

    void showSettingsWindow();
};

