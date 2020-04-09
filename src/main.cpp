#include <Arduino.h>

#include <ADS1115.h>
#include <ClickEncoder.h>
#include <EEPROM.h>
#include <RollingAverage.h>
#include <U8g2lib.h>
#include <TimerOne.h>
#include <Wire.h>

#include "config.h"
#include "scuba.h"
#include "state.h"

// LCD
// U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0); // 128 bytes framebuffer
U8G2_SH1106_128X64_NONAME_2_HW_I2C u8g2(U8G2_R0); // 256 bytes framebuffer

// ADC
ADS1115 ads;

// ROLLING AVERAGE
int16_t readingsBuffer[SAMPLE_SIZE];
RollingAverage readings(SAMPLE_SIZE, readingsBuffer);

// ENCODER
#define ENC_PIN_A 2
#define ENC_PIN_B 3
#define ENC_PIN_SW 4
#define ENC_STEPS 4

ClickEncoder encoder(ENC_PIN_A, ENC_PIN_B, ENC_PIN_SW, ENC_STEPS);

int16_t encPosPrev, encPos;
int8_t encDelta;
uint8_t buttonState;

void timerIsr()
{
	encoder.service();
}

// BUZZER

#define BUZZER_PIN	8

// STATE MACHINE
state_t state;
state_dialog_t stateCalibMenu;
uint32_t displayTimer = 0;
uint32_t analyzeTimer = 0;
uint32_t calibrateTimer = 0;
uint32_t batteryTimer = 0;

// OTHER
int16_t batteryVoltage = 0;
int16_t calibrationFactor = 0; // unit is [1e-1 µV / %], value should be ~5000
int16_t oxygenConcentration = 0;

void renderDisplay()
{
	u8g2.firstPage();
	do {
		switch(state) {
		case STATE_START_SCREEN:
			// TODO: Add Graphics ?
			u8g2.setFont(u8g2_font_6x13_tr);
			u8g2.setCursor(0,10);
			u8g2.print(F("Nitrox Analyzer"));
			u8g2.setCursor(0,20);
			u8g2.print(F("Starting..."));
			u8g2.setCursor(0,63);
			u8g2.print(F("Battery: "));
			u8g2.print(batteryVoltage/1000);
			u8g2.print(".");
			u8g2.print((batteryVoltage % 1000) / 10);
			u8g2.print("V");
			break;
		case STATE_ANALYZE:
			u8g2.setFont(u8g2_font_6x13_tr);
			u8g2.setCursor(0,10);
			u8g2.print(F("Analyzing"));
//			u8g2.setFont(u8g2_font_inb30_mn);
			u8g2.setFont(u8g2_font_logisoso30_tn);
			u8g2.drawStr(0,44,"00.00");
			u8g2.setFont(u8g2_font_6x13_tr);
			u8g2.drawStr(0,63,"pO2 1.6 > MOD 55m");
			break;
		case STATE_CALIBRATE_MENU:
			u8g2.setFont(u8g2_font_6x13_tr);
			u8g2.setCursor(0,10);
			u8g2.print(F("Calibrate ?"));
			u8g2.setFont(u8g2_font_logisoso30_tn);
			u8g2.drawStr(0,44,"20.95");
			u8g2.setFont(u8g2_font_6x13_tr);
			if (stateCalibMenu == YES) {
				u8g2.drawBox(0,53,64,10);
			}
			else {
				u8g2.drawBox(63,53,64,10);
			}
			u8g2.setFontMode(1); // transparent background
			u8g2.setDrawColor(2); // XOR
			u8g2.setCursor(24,63);
			u8g2.print(F("YES"));
			u8g2.setCursor(90,63);
			u8g2.print(F("NO"));
			// reset drawing modes
			u8g2.setFontMode(0);
			u8g2.setDrawColor(1);
			break;
		case STATE_CALIBRATE:
			u8g2.setFont(u8g2_font_6x13_tr);
			u8g2.setCursor(30,10);
			u8g2.print(F("Calibration"));
			u8g2.setCursor(30,20);
			u8g2.print(F("in progress"));
			u8g2.setCursor(21,40);
			u8g2.print(F("Please wait..."));
			break;
		default: 
			;
		}
	} while ( u8g2.nextPage() );
}


void setup()
{
#ifdef DEBUG
	Serial.begin(19200);
	Serial.println("*\n* Nitrox Analyser - DEBUG\n*");
#endif
	Wire.begin();

	u8g2.begin();
	u8g2.setFont(u8g2_font_6x13_tr);
	
	ads.begin();
	ads.setGain(GAIN_SIXTEEN); // +/- 256mV FSR = 7.812µV resolution
	ads.setDataRate(DR_16SPS); // 16 sps
	ads.setMux(MUX_DIFF_0_1);  // sensor is connected between AIN0 (P) and AIN1 (N)
	ads.writeConfig();
#ifdef DEBUG
	Serial.print("ADS config: ");
	Serial.println(ads.readConfig());
#endif
	ads.startContinuousConversion();

	readings.begin();

	Timer1.initialize(1000);
	Timer1.attachInterrupt(timerIsr);

	// initialize variables

	encPosPrev = encPos;
	//state = STATE_ANALYZE;
	state = STATE_START_SCREEN;
	stateCalibMenu = NO;
	displayTimer = millis();
	batteryTimer = -BATTERY_INTERVAL; // force initial reading
	// Load last calibration factor
	// EEPROM.get(EEPROM_CALIBRATION_ADDRESS, calibrationFactor);
}

void loop()
{
	static bool updateDisplay = true;

	// Handle inputs
	buttonState = encoder.getButton();
	encPos += encoder.getValue();
	encDelta = encPos - encPosPrev;
	encPosPrev = encPos;
#ifdef DEBUG
	if (buttonState != 0) {
		Serial.print("Button: "); Serial.println(buttonState);
	}
	if (encDelta != 0) {
		Serial.print("Encoder: "); Serial.println(encDelta);
	}
#endif

	// ADC readings
	if (millis() - analyzeTimer >= ANALYZE_INTERVAL) {
		readings.addReading(ads.readLastConversion());
		analyzeTimer = millis();
	}

	// Battery
	if (millis() - batteryTimer >= BATTERY_INTERVAL) {
		batteryVoltage = analogRead(A0);
		// convert ADC reading to mV
		// ref = 3.3V -> mV = adc * 100 / 31
		// + factor 2 from divider
		batteryVoltage *= (10 * 2);
		batteryVoltage /= 31;
		batteryVoltage *= 10;
#ifdef DEBUG
		Serial.print("Battery: "); Serial.println(batteryVoltage);
#endif
		batteryTimer = millis();
	}

	// State machine
	switch (state) {
		case STATE_START_SCREEN:
			if (millis() - displayTimer >= SPLASH_DELAY) {
				state = STATE_ANALYZE;
				updateDisplay = true;
				displayTimer = millis();
			}
			break;
		case STATE_ANALYZE:
			// handle input
			if (buttonState == ClickEncoder::Held) {
				state = STATE_CALIBRATE_MENU;
				stateCalibMenu = YES;
				updateDisplay = true;
				break;
			}
			if (millis() - displayTimer >= DISPLAY_REFRESH_RATE) {
				int32_t sensorMicroVolts = ((int32_t)readings.getAverage() * 7812L) / 1000L;
				oxygenConcentration = (int16_t)((sensorMicroVolts * 1000L) / calibrationFactor);
				// TODO: calculate MOD
				updateDisplay = true;
				displayTimer = millis();
			}
			break;
		case STATE_CALIBRATE_MENU:
			if (encDelta != 0) {
				if (encDelta > 0) {
					stateCalibMenu = NO;
				}
				else if (encDelta < 0) {
					stateCalibMenu = YES;
				}
				updateDisplay = true;
			}
			if (buttonState == ClickEncoder::Clicked) {
				if (stateCalibMenu == YES) {
					state = STATE_CALIBRATE;
					calibrateTimer = millis();
				}
				else {
					state = STATE_ANALYZE;
				}
				updateDisplay = true;
			} 
			break;
		case STATE_CALIBRATE:
			// TODO: handle input
			if (millis() - calibrateTimer >= CALIBRATION_TIME) {
				// TODO: check calibration sample quality (e.g. max deviation)
				// 7812 is ADS resolution in [nV / LSB] at PGA = 16
				// 2095 is calibration oxygen concentration (20.95% in air)
				int32_t sensorMicroVolts = ((int32_t)readings.getAverage() * 7812L) / 1000L;
				calibrationFactor = (int16_t)((sensorMicroVolts * 1000L) / 2095L);
				// EEPROM.put(EEPROM_CALIBRATION_ADDRESS, calibrationResult);
#ifdef DEBUG
				Serial.println("Calibration complete:");
				Serial.print(sensorMicroVolts); Serial.println(" µV");
				Serial.println(calibrationFactor);
#endif
				state = STATE_ANALYZE;
				updateDisplay = true;
			}
			// render
			break;
	}


#ifdef DEBUG
	if (buttonState != 0) {
		switch (buttonState) {
			case ClickEncoder::Open:          //0
			break;

			case ClickEncoder::Closed:        //1
			break;

			case ClickEncoder::Pressed:       //2
			break;

			case ClickEncoder::Held:          //3
			break;

			case ClickEncoder::Released:      //4
			break;

			case ClickEncoder::Clicked:       //5
				// updateDisplay = true;
				Serial.print("ADC reading:");
				//Serial.println(ads.readLastConversion());
				Serial.println(readings.getAverage());
				Serial.print("O2 concentration:"); Serial.println(oxygenConcentration);
				break;

			case ClickEncoder::DoubleClicked: //6
				tone(BUZZER_PIN,4000,500);
				break;
		}
	}
#endif

	if (updateDisplay) {
		renderDisplay();
		updateDisplay = false;
	}

}
