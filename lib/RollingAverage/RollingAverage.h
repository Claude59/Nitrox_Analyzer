#ifndef _ROLLING_AVERAGE_H_
#define _ROLLING_AVERAGE_H_

#include <Arduino.h>

class RollingAverage {
public:
    RollingAverage();
    RollingAverage(uint8_t size);
    void init();

public:
    void addValue(int16_t value);
    int16_t getAverage();

private:
    uint8_t size;
    uint8_t index;
    uint8_t n_values;
    int16_t* values;
    int32_t sum;
};

#endif // _ROLLING_AVERAGE_H_