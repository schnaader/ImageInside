#pragma once
#include "CandidateSettings.h"
#include <cstdint>
#include <chrono>
#include <thread>
#include <mutex>

enum class FinderState { analyzing, ready, cancellationRequested, cancelled };

class CandidateFinder {
  private:
    CandidateSettings candidateSettings;
    unsigned char* dataToAnalyze;
    uint64_t dataLength;

  public:
    CandidateFinder(CandidateSettings settings, unsigned char* dataToAnalyze,
                    uint64_t dataLength);
    ~CandidateFinder();
    FinderState finderState;
    std::mutex finderStateMutex;
    float analysisProgress;
    std::mutex analysisProgressMutex;
};