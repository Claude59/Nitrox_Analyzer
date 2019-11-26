#ifndef _ROLLING_AVERAGE_H_
#define _ROLLING_AVERAGE_H_

class RollingAverage {
public:
    RollingAverage();
    RollingAverage(uint8_t size);
    void init();
    void init(uint16_t value);

public:
    void addReading(uint16_t value);
    uint16_t getAverage();

private:
    uint8_t size;
    uint8_t index;
    uint8_t n_readings;
    uint16_t* readings;
    uint16_t sum;
};

#endif // _ROLLING_AVERAGE_H_