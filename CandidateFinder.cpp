#include "CandidateFinder.h"

void analyzeTask(float& analysisProgress, std::mutex& analysisProgressMutex,
                 FinderState& finderState, std::mutex& finderStateMutex) {
  // TODO
  // only a dummy at the moment

  for (int i = 0; i < 500; i++) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    {
      std::lock_guard<std::mutex> guard(analysisProgressMutex);
      analysisProgress = i / 500.0f;
    }
  }

  {
    std::lock_guard<std::mutex> guard(analysisProgressMutex);
    analysisProgress = 1.0f;
  }

  {
    std::lock_guard<std::mutex> guard(finderStateMutex);
    finderState = FinderState::ready;
  }
}

CandidateFinder::CandidateFinder(CandidateSettings settings, unsigned char* dataToAnalyze,
                                 uint64_t dataLength) {
  candidateSettings = settings;
  finderState = FinderState::analyzing;
  this->dataToAnalyze = dataToAnalyze;
  this->dataLength = dataLength;
  analysisProgress = 0.0f;

  // start analyze task in background
  std::thread analyzeThread(analyzeTask,
                            std::ref(analysisProgress), std::ref(analysisProgressMutex),
                            std::ref(finderState), std::ref(finderStateMutex));
  analyzeThread.detach();
}
