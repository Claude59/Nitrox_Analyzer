#ifndef _CONFIG_H_
#define _CONFIG_H_

#define SPLASH_DELAY 2000 // ms
#define DISPLAY_REFRESH_RATE 1000 // ms
#define ANALYZE_INTERVAL 250 // ms
//#define ANALYZE_INTERVAL 1000 // ms
#define SAMPLE_SIZE 20u // nb of values to be averaged
#define CALIBRATION_TIME 6000 // ms - should be > ANALYZE_INTERVAL * SAMPLE_SIZE
#define BATTERY_INTERVAL 5000

#define EEPROM_CALIBRATION_ADDRESS 0

// ENCODER
#define ENC_PIN_A   2
#define ENC_PIN_B   3
#define ENC_PIN_SW  4
#define ENC_STEPS   4

// BUZZER
#define BUZZER_PIN	8

#endif // _CONFIG_H_