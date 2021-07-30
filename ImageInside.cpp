// ImageInside
// GUI application to check for image data inside of files
//
// based on this ImGui example: https://github.com/ocornut/imgui/tree/master/examples/example_glfw_opengl3

#include "CandidateFinder.h"
#include "Settings.h"
#include "ImFileDialog/ImFileDialog.h"
#include "LoadTexture.h"
static Settings& settings = Settings::getInstance();
static CandidateFinder* candidateFinder = nullptr;

#define IMGUI_IMPL_OPENGL_LOADER_GL3W

#include "imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include <stdio.h>

static Candidate* currentCandidateImage = nullptr;
static int64_t candidateImageOffset = -1;
static int64_t candidateImagePixelCount = -1;
static uint64_t candidateImageWidth, candidateImageHeight;
static int currentCandidateImageNumber = -1;
static int currentCandidateColorOffsetCorrection = 0;
static int currentCandidateOffsetCorrection = 0;
static bool currentCandidateReinitialize = false;
//static D3D12_CPU_DESCRIPTOR_HANDLE texture_srv_cpu_handle;
//static D3D12_GPU_DESCRIPTOR_HANDLE texture_srv_gpu_handle;

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
//  Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
//  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include "backends/GL/gl3w.h"            // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>            // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>          // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
#include <glad/gl.h>            // Initialize with gladLoadGL(...) or gladLoaderLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/Binding.h>  // Initialize with glbinding::Binding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/glbinding.h>// Initialize with glbinding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// Include glfw3.h after our OpenGL definitions
#include "backends/GLFW/glfw3.h"

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void ShowCandidateImage(Candidate& candidate, unsigned char* candidateFinderData) {
  // check if this candidate image has to be (re-)initialized (uploaded to GPU)
  if ((candidate.startOffset != candidateImageOffset) || (candidate.pixelCount != candidateImagePixelCount) || currentCandidateReinitialize) {
    candidateImageOffset = (int64_t)candidate.startOffset;
    candidateImagePixelCount = (int64_t)candidate.pixelCount;
    candidateImageWidth = candidate.width;
    candidateImageHeight = candidate.height;

    // change offset and height if changedOffset flag is set
    auto actualOffset = candidateImageOffset;
    auto actualHeight = candidateImageHeight;
    if (currentCandidateColorOffsetCorrection + currentCandidateOffsetCorrection > 0) {
      actualOffset += currentCandidateColorOffsetCorrection + currentCandidateOffsetCorrection;
      actualHeight--;
    }
    currentCandidateReinitialize = false;

    // We need to pass a D3D12_CPU_DESCRIPTOR_HANDLE in ImTextureID, so make sure it will fit
    //static_assert(sizeof(ImTextureID) >= sizeof(D3D12_CPU_DESCRIPTOR_HANDLE), "D3D12_CPU_DESCRIPTOR_HANDLE is too large to fit in an ImTextureID");

    // We presume here that we have our D3D device pointer in g_pd3dDevice
    //ID3D12Resource* my_texture = NULL;

    // Get CPU/GPU handles for the shader resource view
    // Normally your engine will have some sort of allocator for these - here we assume that there's an SRV descriptor heap in
    // g_pd3dSrvDescHeap with at least two descriptors allocated, and descriptor 1 is unused
    //UINT handle_increment = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    int descriptor_index = 1; // The descriptor table index to use (not normally a hard-coded constant, but in this case we'll assume we have slot 1 reserved for us)
    //texture_srv_cpu_handle = g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart();
    //texture_srv_cpu_handle.ptr += ((size_t)handle_increment * descriptor_index);
    //texture_srv_gpu_handle = g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart();
    //texture_srv_gpu_handle.ptr += ((size_t)handle_increment * descriptor_index);

    bool ret = false;
    switch (candidate.bytePerPixel) {
    case 1:
      //ret = LoadTextureFromImageData8Bpp(candidateFinderData + actualOffset, (int)candidateImageWidth, (int)actualHeight, g_pd3dDevice, texture_srv_cpu_handle, &my_texture);
      break;
    case 2:
      //ret = LoadTextureFromImageData16Bpp(candidateFinderData + actualOffset, (int)candidateImageWidth, (int)actualHeight, g_pd3dDevice, texture_srv_cpu_handle, &my_texture);
      break;
    case 3:
      //ret = LoadTextureFromImageData24Bpp(candidateFinderData + actualOffset, (int)candidateImageWidth, (int)actualHeight, g_pd3dDevice, texture_srv_cpu_handle, &my_texture);
      break;
    case 4:
      //ret = LoadTextureFromImageData(candidateFinderData + actualOffset, (int)candidateImageWidth, (int)actualHeight, g_pd3dDevice, texture_srv_cpu_handle, &my_texture);
      break;
    }
    IM_ASSERT(ret);
  }

  auto actualHeight = candidateImageHeight;
  if (currentCandidateColorOffsetCorrection + currentCandidateOffsetCorrection > 0) {
    actualHeight--;
  }
  // scale image, keep aspect ratio, set longest side to 512 pixels
  float w = 512.0f;
  float h = 512.0f;
  if (candidateImageWidth >= actualHeight) {
    h = 512.0f * actualHeight / candidateImageWidth;
  }
  else {
    w = 512.0f * candidateImageWidth / actualHeight;
  }
  //ImGui::Image((ImTextureID)texture_srv_gpu_handle.ptr, ImVec2(w, h));
}


int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImageInside", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
    bool err = gladLoadGL(glfwGetProcAddress) == 0; // glad2 recommend using the windowing library loader instead of the (optionally) bundled one.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
    bool err = false;
    glbinding::Binding::initialize();
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
    bool err = false;
    glbinding::initialize([](const char* name) { return (glbinding::ProcAddress)glfwGetProcAddress(name); });
#else
    bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        bool newFileWasOpened = false;
        candidateFinder = settings.showSettingsWindow(candidateFinder, newFileWasOpened);
        if (newFileWasOpened) {
          delete currentCandidateImage;
          currentCandidateImage = nullptr;
        }

        if (candidateFinder != nullptr) {
          if (candidateFinder->finderState == FinderState::analyzing) {
            ImGui::Begin("Progress", 0, ImGuiWindowFlags_NoResize);
            ImGui::SetWindowSize(ImVec2(800, 80));
            ImGui::ProgressBar(candidateFinder->analysisProgress);
            if (ImGui::Button("Cancel")) {
              delete candidateFinder;
              candidateFinder = nullptr;
              delete currentCandidateImage;
              currentCandidateImage = nullptr;
            }
            ImGui::End();
          }

          if (candidateFinder != nullptr) { // might be nullptr again because of "Cancel" button
            auto total_candidates = candidateFinder->candidates.size();
            // show candidate finder results (so far)
            if (total_candidates > 0) {
              std::set<Candidate, candidateSort> cloned_candidates;
              cloned_candidates = std::set<Candidate, candidateSort>();
              int i = 0;

              {
                std::lock_guard<std::mutex> candidatesGuard(candidateFinder->candidatesMutex);
                for (const auto& candidate : candidateFinder->candidates) {
                  cloned_candidates.insert(candidate);
                  i++;
                  if (i == 1000) break;
                }
              }

              ImGui::Begin("Image candidates");
              ImGui::Text("%d candidates, showing first %d", total_candidates, cloned_candidates.size());
              ImGui::Separator();
              i = 0;
              for (const auto& candidate : cloned_candidates) {
                ImGui::Text("Candidate #%d", i + 1);
                ImGui::Text("Score: %f %%", candidate.score * candidate.bytePerPixel / candidateFinder->dataLength * 100.0f);
                ImGui::Text("Mean absolute correlation coefficient: %f", candidate.meanCorrelationCoefficient);
                ImGui::Text("Size: %d x %d", candidate.width, candidate.height);
                ImGui::Text("File offset: 0x%08Xh", candidate.startOffset);

                ImGui::PushID(i);
                if (ImGui::Button("Show candidate")) {
                  currentCandidateImage = new Candidate(candidate);
                  currentCandidateImageNumber = i;
                  currentCandidateColorOffsetCorrection = 0;
                  currentCandidateOffsetCorrection = 0;
                  currentCandidateReinitialize = true;
                }
                ImGui::PopID();
                ImGui::Separator();

                i++;
              }
              ImGui::End();
            }
          }
        }

        if (currentCandidateImage != nullptr && candidateFinder != nullptr) {
          auto candidate = *currentCandidateImage;

          ImGui::Begin("Image candidate", 0, ImGuiWindowFlags_NoResize);
          ImGui::SetWindowSize(ImVec2(700, 750));
          ImGui::Text("Candidate #%d", currentCandidateImageNumber + 1);
          ImGui::Text("Score: %f %%", candidate.score * candidate.bytePerPixel / candidateFinder->dataLength * 100.0f);
          ImGui::Text("Mean absolute correlation coefficient: %f", candidate.meanCorrelationCoefficient);
          ImGui::Text("Size: %d x %d pixels", candidate.width, candidate.height);
          ImGui::Text("File offset: 0x%08Xh", candidate.startOffset);

          if (candidate.bytePerPixel > 1) {
            ImGui::Separator();
            ImGui::Text("Color correction offset");
            for (int i = 0; i < candidate.bytePerPixel; i++) {
              char buttonLabel[100];
              sprintf(buttonLabel, "Offset by %d bytes", i);
              if (i > 0) {
                ImGui::SameLine();
              }
              if (ImGui::Button(buttonLabel)) {
                if (i != currentCandidateColorOffsetCorrection) {
                  currentCandidateColorOffsetCorrection = i;
                  currentCandidateReinitialize = true;
                }
              }
            }
          }

          ImGui::Separator();
          ImGui::Text("Line start correction offset");
          static int offsetCorrection = 0;
          if (ImGui::SliderInt("pixels##LineStartCorrection", &offsetCorrection, 0, (int)candidate.width - 1)) {
            currentCandidateOffsetCorrection = offsetCorrection * candidate.bytePerPixel;
            currentCandidateReinitialize = true;
          }
          ImGui::Separator();

          ShowCandidateImage(candidate, candidateFinder->dataToAnalyze);

          ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
