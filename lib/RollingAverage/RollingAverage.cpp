#include <RollingAverage.h>

RollingAverage::RollingAverage(uint8_t s) {
    size = s;
    readings = (uint16_t*) malloc(size * sizeof(uint16_t));
    if (readings == NULL) size = 0;
}

void RollingAverage::init() {
    sum = 0;
    for (uint8_t i; i < size; i++) {
        readings[i] = 0;
    }
    index = 0;
    n_readings = 0;
}

void addReading(uint16_t value) {
    if (n_readings < size) {
        n_readings++;
        sum += value;
    }
    else {
        sum = sum - readings[index] + value;
    }

    readings[index] = value;
    index = (index == size - 1) ? 0 : index + 1;
}

uint16_t getAverage() {
    return (sum + n_readings / 2) / n_readings;
}