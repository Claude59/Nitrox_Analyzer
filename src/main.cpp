#include <Arduino.h>

#include <ADS1115.h>
#include <ClickEncoder.h>
#include <U8g2lib.h>
#include <TimerOne.h>
#include <Wire.h>

#include "scuba.h"
#include "state.h"

// LCD
// U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0); // 128 bytes framebuffer
U8G2_SH1106_128X64_NONAME_2_HW_I2C u8g2(U8G2_R0); // 256 bytes framebuffer

// ADC
ADS1115 ads;

// ENCODER
#define ENC_PIN_A 10
#define ENC_PIN_B 11
#define ENC_PIN_SW 12
#define ENC_STEPS 4

ClickEncoder encoder(ENC_PIN_A, ENC_PIN_B, ENC_PIN_SW, ENC_STEPS);

int16_t oldEncPos, encPos;
uint8_t buttonState;

void timerIsr()
{
	encoder.service();
}

// STATE MACHINE
state_t state;


// DISPLAY
struct display_data {
	uint8_t mode;
	char mainStr;
	char footStr;
};

void display_render()
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
	Serial.print("ADS config: ");
	Serial.println(ads.readConfig());
	ads.setGain(GAIN_EIGHT);
	ads.setDataRate(DR_16SPS);
	ads.setMux(MUX_DIFF_0_1);
	ads.writeConfig();
	Serial.print("ADS config: ");
	Serial.println(ads.readConfig());
	ads.startContinuousConversion();

	Timer1.initialize(1000);
	Timer1.attachInterrupt(timerIsr);

	encoder.setAccelerationEnabled(true);

	Serial.print("Acceleration is ");
	Serial.println((encoder.getAccelerationEnabled()) ? "enabled" : "disabled");

	oldEncPos = -1;
}

void loop()
{
	static bool display_update = true;

	if (display_update) {
		lcd.firstPage();
		do {
			lcd.drawStr(col, row, "Hello World!");
		} while ( lcd.nextPage() );
		display_update = false;
	}

	encPos += encoder.getValue();

	if (encPos != oldEncPos) {
		oldEncPos = encPos;
		Serial.print("Encoder Value: ");
		Serial.println(encPos);
	}

	buttonState = encoder.getButton();

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
				// display_update = true;
				Serial.print("ADC reading:");
				Serial.println(ads.readLastConversion());
				break;

			case ClickEncoder::DoubleClicked: //6
			break;
		}
	}
}