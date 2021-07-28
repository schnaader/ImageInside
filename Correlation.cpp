#include "Correlation.h"
#include <cmath>
#include <cfloat>

// Computes the correlation coefficient of two rows of image data "x" and "y" with a length of "length" bytes.
// The correlation coefficient is defined as the result of dividing the empiric covariance by the empiric variance.

float CorrelationCoefficient(unsigned char* x, unsigned char* y, int length) {
	// calculate means of x and y
	float xSum = 0.0f;
	float ySum = 0.0f;
	for (int i = 0; i < length; i++) {
		xSum += x[i];
		ySum += y[i];
	}
	float xMean = xSum / length;
	float yMean = ySum / length;
	
	float covariance = 0, varianceX = 0, varianceY = 0;
	for (int i = 0; i < length; i++) {
		float xDiff = x[i] - xMean;
		float yDiff = y[i] - yMean;
		
		covariance += xDiff * yDiff;
		varianceX += xDiff * xDiff;
		varianceY += yDiff * yDiff;
	}
	
	// prevent division by zero
	if (varianceX * varianceY < FLT_EPSILON) {
		return 0;
	}
	
	return covariance / std::sqrt(varianceX * varianceY);
}