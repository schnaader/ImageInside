#pragma once
#include <Candidate.h>
#include "CandidateSettings.h"
#include <cstdint>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
#include <set>
#include <algorithm>

enum class FinderState { analyzing, ready, cancellationRequested, cancelled };

struct candidateSort final {
  bool operator()(Candidate a, Candidate b) const { return a.score > b.score; }
};

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
    std::set<Candidate, candidateSort> candidates;
    std::mutex candidatesMutex;
};