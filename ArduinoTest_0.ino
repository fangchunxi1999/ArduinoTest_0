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
byte modeNum = 0;
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

	attachInterrupt(digitalPinToInterrupt(intp), intp_program, LOW);

	checkSystem();
}

void loop()
{
	is_intp = false;

	segLight = bit10Tobit8(checkA7());
	Serial.print("modeNum: ");
	Serial.println(modeNum);
	
}

void intp_program()
{
	if (!is_intp)
	{
		modeNum++;
		setAlldig(HIGH);
		digitalWrite(segG, HIGH);
		offLED();
		is_intp = true;
	}
}

int bit10Tobit8(int bit10)
{
	return (bit10 / 1023) * 255;
}

int checkA7()
{
	return analogRead(A7);
}

int checkA6()
{
	return analogRead(A6);
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