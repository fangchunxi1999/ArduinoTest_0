#include<Arduino.h>

#define segA 7
#define segB 12
#define segC 5
#define segD 13
#define segE A0
#define segF 4
#define segG 8
#define segDP A1

#define dig1 6
#define dig2 9
#define dig3 10
#define dig4 11

#define intp 2

bool is_intp = true;
byte modeNum = -1;
byte modeMax = 3;
int segLight = 255;

void setup()
{
	Serial.begin(115200);

	pinMode(segA, OUTPUT);
	pinMode(segB, OUTPUT);
	pinMode(segC, OUTPUT);
	pinMode(segD, OUTPUT);
	pinMode(segE, OUTPUT);
	pinMode(segF, OUTPUT);
	pinMode(segG, OUTPUT);
	pinMode(segDP, OUTPUT);

	pinMode(dig1, OUTPUT);
	pinMode(dig2, OUTPUT);
	pinMode(dig3, OUTPUT);
	pinMode(dig4, OUTPUT);

	pinMode(intp, INPUT_PULLUP);

	attachInterrupt(digitalPinToInterrupt(intp), intp_program, CHANGE);

	checkSystem();
}

void loop()
{
	segLight = bit10Tobit8(checkA6());
	bool is_onDP = false;
	float var = 0;

	/*
	if (is_intp == true)
	{
		offLED();
		delay(100);
		analogWrite(dig3, segLight);
		segDisplay(modeNum % modeMax);
		is_intp = false;
		delay(395);
		offLED();
	}*/


	switch (modeNum % modeMax)
	{
		case 0:
			var = 0.0f;
			is_onDP = false;
			break;
		case 1:
			var = 1.1f;
			is_onDP = true;
			break;
		case 2:
			var = 2.2f;
			is_onDP = true;
			break;

		default:
			var = 5.0f;
			is_onDP = false;
			break;
	}

	//display

	var *= 100.0f;

	int t1 = var / 1000;
	var = var - t1 *1000;
	offLED();
	analogWrite(dig1, segLight);
	segDisplay(t1);

	int t2 = var / 100;
	var = var - t2 *100;
	offLED();
	analogWrite(dig2, segLight);
	if (is_onDP)
	{
		digitalWrite(segDP, HIGH);
	}
	segDisplay(t2);

	int t3 = var / 10;
	var = var - t3 *10;
	offLED();
	analogWrite(dig3, segLight);
	segDisplay(t3);

	int t4 = var;
	offLED();
	analogWrite(dig4, segLight);
	segDisplay(t4);
	//display

	Serial.print("modeNum: ");
	Serial.println(modeNum);
	Serial.print("segLight: ");
	Serial.println(segLight);
	is_intp = false;
}

void intp_program()
{
	if (digitalRead(intp) == HIGH)
	{
		modeNum++;
	}
	is_intp = true;
}

int bit10Tobit8(int bit10)
{
	return (bit10 * 256) / 1024;
}

int checkA7()
{
	return analogRead(A7);
}

int checkA6()
{
	return analogRead(A6);
}

void segDisplay(int c)
{
    switch (c)
    {
        case 0:
            digitalWrite(segA, HIGH);
			digitalWrite(segB, HIGH);
			digitalWrite(segC, HIGH);
			digitalWrite(segD, HIGH);
			digitalWrite(segE, HIGH);
			digitalWrite(segF, HIGH);
			delay(5);
            break;
        
        case 1:
            digitalWrite(segB, HIGH);
			digitalWrite(segC, HIGH);
			delay(5);
            break;

        case 2:
            digitalWrite(segA, HIGH);
			digitalWrite(segB, HIGH);
			digitalWrite(segD, HIGH);
			digitalWrite(segE, HIGH);
			digitalWrite(segG, HIGH);
			delay(5);
            break;

        case 3:
            digitalWrite(segA, HIGH);
			digitalWrite(segB, HIGH);
			digitalWrite(segC, HIGH);
			digitalWrite(segD, HIGH);
			digitalWrite(segG, HIGH);
			delay(5);
            break;

        case 4:
            digitalWrite(segB, HIGH);
			digitalWrite(segC, HIGH);
			digitalWrite(segF, HIGH);
			digitalWrite(segG, HIGH);
			delay(5);
            break;

        case 5:
            digitalWrite(segA, HIGH);
			digitalWrite(segC, HIGH);
			digitalWrite(segD, HIGH);
			digitalWrite(segF, HIGH);
			digitalWrite(segG, HIGH);
			delay(5);
            break;

        case 6:
            digitalWrite(segA, HIGH);
			digitalWrite(segC, HIGH);
			digitalWrite(segD, HIGH);
			digitalWrite(segE, HIGH);
			digitalWrite(segF, HIGH);
			digitalWrite(segG, HIGH);
			delay(5);
            break;

        case 7:
            digitalWrite(segA, HIGH);
			digitalWrite(segB, HIGH);
			digitalWrite(segC, HIGH);
			delay(5);
            break;

        case 8:
            digitalWrite(segA, HIGH);
			digitalWrite(segB, HIGH);
			digitalWrite(segC, HIGH);
			digitalWrite(segD, HIGH);
			digitalWrite(segE, HIGH);
			digitalWrite(segF, HIGH);
			digitalWrite(segG, HIGH);
			delay(5);
            break;

        case 9:
            digitalWrite(segA, HIGH);
			digitalWrite(segB, HIGH);
			digitalWrite(segC, HIGH);
			digitalWrite(segD, HIGH);
			digitalWrite(segF, HIGH);
			digitalWrite(segG, HIGH);
			delay(5);
            break;

    }
} 

void offLED()
{
	digitalWrite(dig1, LOW);
	digitalWrite(dig2, LOW);
	digitalWrite(dig3, LOW);
	digitalWrite(dig4, LOW);

	digitalWrite(segA, LOW);
	digitalWrite(segB, LOW);
	digitalWrite(segC, LOW);
	digitalWrite(segD, LOW);
	digitalWrite(segE, LOW);
	digitalWrite(segF, LOW);
	digitalWrite(segG, LOW);
	digitalWrite(segDP, LOW);
}

void setAlldig(bool s)
{
	digitalWrite(dig1, s);
	digitalWrite(dig2, s);
	digitalWrite(dig3, s);
	digitalWrite(dig4, s);
}


void checkSystem()
{
	int digiSet[] = {dig1, dig2, dig3, dig4};
	int segSet[] = {segA, segB, segC, segD, segE, segF, segG, segDP};
	offLED();
	for (int i = 0; i <= (sizeof(digiSet) / sizeof(int)); i++)
	{
		digitalWrite(digiSet[i], HIGH);
		for (int j = 0; j <= (sizeof(segSet) / sizeof(int)); j++)
		{
			digitalWrite(segSet[j], HIGH);
			delay(50);
			digitalWrite(segSet[j], LOW);;
		}
		digitalWrite(digiSet[i], LOW);
		delay(50);
	}
}