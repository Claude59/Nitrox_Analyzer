#include <Arduino.h>

#include <ClickEncoder.h>
#include <U8g2lib.h>
#include <TimerOne.h>
#include <Wire.h>

// LCD
U8G2_SH1106_128X64_NONAME_1_HW_I2C lcd(U8G2_R0);

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

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
	Serial.begin(19200);
	lcd.begin();
	lcd.setFont(u8g2_font_6x13_tr);
	
	Timer1.initialize(1000);
	Timer1.attachInterrupt(timerIsr);

	encoder.setAccelerationEnabled(true);

	Serial.print("Acceleration is ");
	Serial.println((encoder.getAccelerationEnabled()) ? "enabled" : "disabled");

	oldEncPos = -1;
}

void loop()
{
	static int row=12;
	static int col=0;
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
			break;

			case ClickEncoder::DoubleClicked: //6
			break;
		}
	}
}