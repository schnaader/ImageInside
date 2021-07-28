#pragma once
#include <cmath>
#include <cfloat>
#include <cstdint>

// Computes the correlation coefficient of two rows of image data x and y with a length of y bytes.
float CorrelationCoefficient(unsigned char* x, unsigned char* y, uint64_t length);