#include "CandidateFinder.h"
#include "Correlation.h"

void analyzeTask(CandidateFinder& candidateFinder) {
  int bytePerPixel = 1;
  switch (candidateFinder.candidateSettings.bitDepth) {
    case Bitdepth::bpp16:
      bytePerPixel = 2;
      break;
    case Bitdepth::bpp24:
      bytePerPixel = 3;
      break;
    case Bitdepth::bpp32:
      bytePerPixel = 4;
      break;
  }

  // estimate progress max (how many pixels will be used to calculate the correlation coefficients)
  uint64_t progress_max = 0;
  auto settings = candidateFinder.candidateSettings;

  for (uint64_t width = settings.widthMin; width <= settings.widthMax; width++) {
    uint64_t bytes_to_process = candidateFinder.dataLength;
    if (settings.limitHeight) {
      bytes_to_process = std::min(bytes_to_process, (uint64_t)width * bytePerPixel * settings.heightMax);
    }
    progress_max += width * ((bytes_to_process - width * bytePerPixel) / (width * bytePerPixel));
  }

  // calculate correlation coefficients for each width
  uint64_t progress = 0;
  unsigned char* dataptr;
  for (uint64_t width = settings.widthMin; width <= settings.widthMax; width++) {
    uint64_t widthInBytes = width * bytePerPixel;

    auto correlationCoefficientsForWidth = std::vector<float>();

    uint64_t offset = 0;
    uint64_t height = 0;
    uint64_t maxHeight = UINT64_MAX;
    if (settings.limitHeight) {
      maxHeight = settings.heightMax;
    }

    Candidate candidate;
    bool processingCandidate = false;
    float correlationCoefficientSum;

    while ((offset + 2 * widthInBytes < candidateFinder.dataLength) && (height < maxHeight)) {
      dataptr = candidateFinder.dataToAnalyze + offset;
      float correlationCoefficient = std::abs(CorrelationCoefficient(dataptr, dataptr + widthInBytes, widthInBytes));
      correlationCoefficientsForWidth.push_back(correlationCoefficient);

      if (!processingCandidate) {
        if (correlationCoefficient > settings.hysteresisMax) {
          processingCandidate = true;

          candidate.bytePerPixel = bytePerPixel;
          candidate.width = width;
          candidate.startOffset = offset;
          candidate.startLine = height;
          correlationCoefficientSum = correlationCoefficient;
        }
      }
      else {
        correlationCoefficientSum += correlationCoefficient;

        if (correlationCoefficient < settings.hysteresisMin) {
          processingCandidate = false;
          candidate.endLine = height;
          candidate.height = candidate.endLine - candidate.startLine;
          candidate.pixelCount = candidate.width * candidate.height;
          candidate.meanCorrelationCoefficient = correlationCoefficientSum / (candidate.height + 1);
          candidate.score = candidate.pixelCount * candidate.meanCorrelationCoefficient;

          if ((!settings.limitHeight) || ((candidate.height <= settings.heightMax) && (candidate.height >= settings.heightMin))) {
            std::lock_guard<std::mutex> candidatesGuard(candidateFinder.candidatesMutex);
            candidateFinder.candidates.insert(candidate);
          }
        }
      }

      offset += widthInBytes;
      progress += width;
      height++;

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

    // if a candidate is still in processing, add it now
    if (processingCandidate) {
      processingCandidate = false;
      candidate.endLine = height;
      candidate.height = candidate.endLine - candidate.startLine;
      candidate.pixelCount = candidate.width * candidate.height;
      candidate.meanCorrelationCoefficient = correlationCoefficientSum / (candidate.height + 1);
      candidate.score = candidate.pixelCount * candidate.meanCorrelationCoefficient;

      if ((!settings.limitHeight) || ((candidate.height <= settings.heightMax) && (candidate.height >= settings.heightMin))) {
        std::lock_guard<std::mutex> candidatesGuard(candidateFinder.candidatesMutex);
        candidateFinder.candidates.insert(candidate);
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
    for (;;) {
      {
        std::lock_guard<std::mutex> finderGuard(finderStateMutex);
        if (finderState != FinderState::cancellationRequested) {
          break;
        }
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }

  free(dataToAnalyze);
}