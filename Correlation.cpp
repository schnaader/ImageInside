#include "Correlation.h"
#include <cmath>
#include <cfloat>

// Computes the correlation coefficient of two rows of image data x and y with a length of y bytes.
// The correlation coefficient is defined as the result of dividing the empiric covariance by the empiric variance.

static double CorrelationCoefficient(unsigned char* x, unsigned char* y, int length) {
	// calculate means of x and y
	double xSum = 0.0;
	double ySum = 0.0;
	for (int i = 0; i < length; i++) {
		xSum += x[i];
		ySum += y[i];
	}
	double xMean = xSum / length;
	double yMean = ySum / length;
	
	double covariance = 0, varianceX = 0, varianceY = 0;
	for (int i = 0; i < length; i++) {
		double xDiff = x[i] - xMean;
		double yDiff = y[i] - yMean;
		
		covariance += xDiff * yDiff;
		varianceX += xDiff * xDiff;
		varianceY += yDiff * yDiff;
	}
	
	// prevent division by zero
	if (varianceX * varianceY < DBL_EPSILON) {
		return 0;
	}
	
	return covariance / std::sqrt(varianceX * varianceY);
}