#pragma once
#include "CandidateSettings.h"
#include <cstdint>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>

enum class FinderState { analyzing, ready, cancellationRequested, cancelled };

class CandidateFinder {
  public:
    CandidateFinder(CandidateSettings settings, unsigned char* dataToAnalyze,
                    uint64_t dataLength);
    ~CandidateFinder();
    FinderState finderState;
    std::mutex finderStateMutex;
    float analysisProgress;
    std::mutex analysisProgressMutex;

    CandidateSettings candidateSettings;
    unsigned char* dataToAnalyze;
    uint64_t dataLength;

    std::vector<std::vector<float>> correlationCoefficientsForLines;
};