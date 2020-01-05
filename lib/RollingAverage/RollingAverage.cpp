#include <RollingAverage.h>

RollingAverage::RollingAverage(uint8_t s) {
	size = s;
	values = (int16_t*) malloc(size * sizeof(int16_t));
	if (values == NULL) size = 0;
}

void RollingAverage::init() {
	sum = 0;
	for (uint8_t i; i < size; i++) {
		values[i] = 0;
	}
	index = 0;
	n_values = 0;
}

void RollingAverage::addValue(int16_t value) {
	if (n_values < size) {
		n_values++;
		sum += value;
	}
	else {
		sum = sum - values[index] + value;
	}

	values[index] = value;
	index = (index == size - 1) ? 0 : index + 1;
}

int16_t RollingAverage::getAverage() {
//	return (int16_t)((sum + n_values / 2) / n_values);	// using (+ n_values / 2) reduce truncation errors by rounding
														// only useful if n_values is large and values are relatively small
	return (int16_t)(sum / n_values);	
}