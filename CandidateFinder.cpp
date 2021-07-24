#include "CandidateFinder.h"

void analyzeTask(CandidateFinder& candidateFinder) {
  // TODO
  // only a dummy at the moment

  for (int i = 0; i < 500; i++) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    {
      std::lock_guard<std::mutex> guard(candidateFinder.analysisProgressMutex);
      candidateFinder.analysisProgress = i / 500.0f;
    }

    {
      std::lock_guard<std::mutex> guard(candidateFinder.finderStateMutex);
      if (candidateFinder.finderState == FinderState::cancellationRequested) {
        candidateFinder.finderState = FinderState::cancelled;
        return;
      }
    }    
  }

  {
    std::lock_guard<std::mutex> guard(candidateFinder.analysisProgressMutex);
    candidateFinder.analysisProgress = 1.0f;
  }

  {
    std::lock_guard<std::mutex> guard(candidateFinder.finderStateMutex);
    candidateFinder.finderState = FinderState::ready;
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
  std::thread analyzeThread(analyzeTask, std::ref(*this));
  analyzeThread.detach();
}

CandidateFinder::~CandidateFinder() {
  if (finderState == FinderState::analyzing) {
    finderState = FinderState::cancellationRequested;
    while (finderState == FinderState::cancellationRequested);
  }
}
