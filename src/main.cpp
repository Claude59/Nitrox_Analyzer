#include <Arduino.h>

#include <ADS1115.h>
#include <ClickEncoder.h>
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
#define ENC_PIN_A 10
#define ENC_PIN_B 11
#define ENC_PIN_SW 12
#define ENC_STEPS 4

ClickEncoder encoder(ENC_PIN_A, ENC_PIN_B, ENC_PIN_SW, ENC_STEPS);

int16_t encPosPrevious, encPos;
uint8_t buttonState;

void timerIsr()
{
	encoder.service();
}

// BUZZER

#define BUZZER_PIN	7

// STATE MACHINE
state_t state;
uint32_t millisDisplayPrevious;
uint32_t millisAnalyzePrevious;


// DISPLAY
struct display_data {
	uint8_t mode;
	char mainStr;
	char footStr;
};

void renderDisplay()
{
	u8g2.firstPage();
	do {
		switch(state) {
		case STATE_ANALYZE:
			u8g2.setFont(u8g2_font_6x13_tr);
			u8g2.setCursor(0,10);
			u8g2.print(F("Analyzing"));
//			u8g2.setFont(u8g2_font_inb30_mn);
			u8g2.setFont(u8g2_font_logisoso30_tn);
			u8g2.drawStr(0,44,"10.95");
			u8g2.setFont(u8g2_font_6x13_tr);
			u8g2.drawStr(0,63,"pO2 1.6 / MOD 55m");
			break;
		case STATE_CALIBRATE_MENU:
			u8g2.setFont(u8g2_font_6x13_tr);
			u8g2.setCursor(0,10);
			u8g2.print(F("Calibrate ?"));
			u8g2.setFont(u8g2_font_logisoso30_tn);
			u8g2.drawStr(0,44,"69.95");
			u8g2.setFont(u8g2_font_6x13_tr);
			u8g2.drawStr(0,63,"YES - NO");
			break;
		case STATE_CALIBRATE:
			u8g2.setFont(u8g2_font_6x13_tr);
			u8g2.setCursor(0,10);
			u8g2.print(F("Calibrate"));
			break;
		default: 
			;
		}
	} while ( u8g2.nextPage() );
}


void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
	Serial.begin(19200);
	Wire.begin();

	u8g2.begin();
	u8g2.setFont(u8g2_font_6x13_tr);
	
	ads.begin();
//	Serial.print("ADS config: ");
//	Serial.println(ads.readConfig());
	ads.setGain(GAIN_SIXTEEN); // +/- 256mV FSR = 7µV resolution
	ads.setDataRate(DR_16SPS); // 16 sps
	ads.setMux(MUX_DIFF_0_1);  // sensor is connected between AIN0 (P) and AIN1 (N)
	ads.writeConfig();
//	Serial.print("ADS config: ");
//	Serial.println(ads.readConfig());
	ads.startContinuousConversion();

	readings.begin();

	Timer1.initialize(1000);
	Timer1.attachInterrupt(timerIsr);

	encoder.setAccelerationEnabled(true);

//	Serial.print("Acceleration is ");
//	Serial.println((encoder.getAccelerationEnabled()) ? "enabled" : "disabled");

	encPosPrevious = encPos;

	state = STATE_ANALYZE;
	millisDisplayPrevious = millis();
	millisAnalyzePrevious = millis();
}

void loop()
{
	static bool updateDisplay = true;

	// Handle inputs
	buttonState = encoder.getButton();
	encPos += encoder.getValue();
	if (encPos != encPosPrevious) {
		// Serial.print("Encoder Value: ");
		// Serial.println(encPos);

		if (encPos > encPosPrevious) {
			//engine->navigate(engine->getNext());
		}
		else {
			//engine->navigate(engine->getPrev());
		}
		// updateDisplay = true;

		encPosPrevious = encPos;
	}

	// ADC readings
	if (millis() - millisAnalyzePrevious >= ANALYZE_INTERVAL) {
		readings.addReading(ads.readLastConversion());
		millisAnalyzePrevious = millis();
	}

	// State machine
	switch (state) {
		case STATE_START_SCREEN:
			if (millis() - millisDisplayPrevious >= SPLASH_DELAY) {
				state = STATE_ANALYZE;
				updateDisplay = true;
				millisDisplayPrevious = millis();
			}
			break;
		case STATE_ANALYZE:
			// handle input
			if (millis() - millisDisplayPrevious >= DISPLAY_REFRESH_RATE) {
				updateDisplay = true;
				millisDisplayPrevious = millis();
			}
			break;
		case STATE_CALIBRATE_MENU:
			//
			break;
		case STATE_CALIBRATE:
			// handle input
			// wait
			// render
			break;
	}



	if (buttonState != 0) {
		Serial.print("Button: "); Serial.println(buttonState);
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
				Serial.println(ads.readLastConversion());
				break;

			case ClickEncoder::DoubleClicked: //6
				tone(BUZZER_PIN,4000,500);
				break;
		}
	}

	if (updateDisplay) {
		renderDisplay();
		updateDisplay = false;
	}

}
