#include <UTFT.h>

extern uint8_t SmallFont[];
extern uint8_t DotMatrix_XL_Num[];
extern unsigned short  heart[0x640];
extern unsigned short  heart_p[0x640];

// Usage: myGLCD(<model code>, SDA, SCL, CS, RST[, RS]);
UTFT myGLCD(TFT01_22SP,9,8,12,11,10);

void setup()
{
	analogReadResolution(12);
	Serial.begin(57600);
	myGLCD.InitLCD();
	myGLCD.setFont(SmallFont);
	myGLCD.setBackColor(0, 0, 0);
	myGLCD.clrScr();
}

int y;
int pre_y = 0;
int frame = 0;
float accum = 120.0;
float accumL = 120.0;
float AvgMax = 0;
float AvgMin = 1000;
unsigned long LastPulse = 0;
float AvgPulse = 60;

void loop()
	{
	pre_y = y;
	y = analogRead(A0);
	accum = ((float)y)*0.2 + ((float)accum)*0.8;
	accumL = ((float)y)*0.02 + ((float)accumL)*0.98;
	y = (accum-accumL)*2.0+60;
	if (y > 189) y = 188;

	//Serial.println(accumL);
	AvgMin += 0.1;
	AvgMax -= 0.02;
	if(accum > AvgMax) AvgMax = accum*0.15 + AvgMax*0.85;
	if(accum < AvgMin) AvgMin = accum*0.1 + AvgMin*0.9;

	unsigned long DeltaPulse = millis() - LastPulse;

	if ((DeltaPulse > 400) && (accum > AvgMin + (AvgMax-AvgMin)*0.75) && ((AvgMax-AvgMin) > 3.0)) {
		DeltaPulse += 100;//correction (draw time)
		float pulse = 60000.0/(float)DeltaPulse;
		if (pulse > 40 && pulse < 150) {
			AvgPulse = AvgPulse*0.75 + pulse*0.25;
			//Serial.println(AvgPulse);

			myGLCD.setColor(0, 0, 0);
			myGLCD.fillRect(208, 0, 240, 50);
			myGLCD.drawBitmap (112, 0, 32, 50, heart_p);
			myGLCD.setFont(DotMatrix_XL_Num);
			myGLCD.setColor(255, 255, 255);
			myGLCD.print(String((int)AvgPulse), 144, 0);
			delay (10);
			myGLCD.drawBitmap (112, 0, 32, 50, heart);
		}
		LastPulse = millis();
	}

	if (accumL > 50) {
		myGLCD.setColor(0, 0, 0);
		myGLCD.fillRect(144, 0, 240, 50);
		myGLCD.setFont(SmallFont);
		myGLCD.setColor(255, 255, 255);
		myGLCD.print("Please wait, calibration...", CENTER, 52);
		AvgMax = 0;
		AvgMin = 1000;
	}
	else
	{
		frame++;
		if (frame > 319) frame = 0;

		myGLCD.setColor(0, 0, 0);
		myGLCD.drawLine(frame, 51, frame, 239);
		myGLCD.setColor(0, 0, 255);
		myGLCD.drawLine(0, 51, 319, 51);

		myGLCD.setColor(255, 0, 0);
		myGLCD.setBackColor(0, 0, 0);
		myGLCD.drawLine(frame-1, 239-pre_y, frame, 239-y);
		myGLCD.drawLine(frame-1, 239-pre_y+1, frame, 239-y+1);
	}
	delay (10);
}

