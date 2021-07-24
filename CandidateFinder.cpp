#include "CandidateFinder.h"
#include "Correlation.h"

void analyzeTask(CandidateFinder& candidateFinder) {
  // estimate progress max (how many pixels will be used to calculate the correlation coefficients)
  uint64_t progress_max = 0;
  auto settings = candidateFinder.candidateSettings;

  for (int width = settings.widthMin; width < settings.widthMax; width++) {
    uint64_t bytes_to_process = candidateFinder.dataLength;
    if (settings.limitHeight) {
      bytes_to_process = std::min(bytes_to_process, (uint64_t)width * settings.heightMax);
    }
    progress_max += width * ((bytes_to_process - width) / width);
  }

  // calculate correlation coefficients for each width
  uint64_t progress = 0;
  unsigned char* dataptr;
  for (uint64_t width = settings.widthMin; width < settings.widthMax; width++) {
    auto correlationCoefficientsForWidth = std::vector<float>();

    uint64_t offset1 = 0;
    uint64_t height = 0;
    uint64_t maxHeight = UINT64_MAX;
    if (settings.limitHeight) {
      maxHeight = settings.heightMax;
    }

    while ((offset1 + 2 * width < candidateFinder.dataLength) && (height < maxHeight)) {
      dataptr = candidateFinder.dataToAnalyze + offset1;
      float correlationCoefficient = std::abs(CorrelationCoefficient(dataptr, dataptr + width, width));
      correlationCoefficientsForWidth.push_back(correlationCoefficient);
      offset1 += width;
      height++;

      progress += width;
      {
        std::lock_guard<std::mutex> guard(candidateFinder.analysisProgressMutex);
        candidateFinder.analysisProgress = progress / (float)progress_max;
      }

      {
        std::lock_guard<std::mutex> guard(candidateFinder.finderStateMutex);
        if (candidateFinder.finderState == FinderState::cancellationRequested) {
          candidateFinder.finderState = FinderState::cancelled;
          return;
        }
      }
    }

    candidateFinder.correlationCoefficientsForLines.push_back(correlationCoefficientsForWidth);
  }

  std::lock_guard<std::mutex> progressGuard(candidateFinder.analysisProgressMutex);
  candidateFinder.analysisProgress = 1.0f;
  std::lock_guard<std::mutex> finderGuard(candidateFinder.finderStateMutex);
  candidateFinder.finderState = FinderState::ready;
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

  free(dataToAnalyze);
}