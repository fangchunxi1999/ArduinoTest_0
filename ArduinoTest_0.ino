#include <boarddefs.h>
#include <ir_Lego_PF_BitStreamEncoder.h>
#include <IRremote.h>
#include <IRremoteInt.h>

//#include<Arduino.h>

#define segA 8
#define segB A4
#define segC 7
#define segD 13
#define segE A0
#define segF 4
#define segG 11
#define segDP A1

#define dig1 5
#define dig2 6
#define dig3 9
#define dig4 10

#define intp 2

#define RECV 12
IRrecv irrecv(RECV);
decode_results results;

static const uint8_t tonePin = 3;

bool is_intp = true;
byte modeNum = -1;
byte modeMax = 5;
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

unsigned int cycle = 0;
void loop()
{
	segLight = bit10Tobit8(checkA6());
	bool is_onDP = false;
	bool is_mainDis = true;
	int var = 0;

	if (irrecv.decode(&results))
    {
		digitalWrite(13, HIGH);
		unsigned long hex = results.value;
		Serial.print("hex: ");
		Serial.println(hex);
    	irrecv.resume();
		digitalWrite(13, LOW);
	}

	switch (modeNum % modeMax)
	{
		case 0:
			var = (int)((((checkA2() / 1023.0) * 5.0)) * 100);
			is_onDP = true;
			break;
		case 1:
			var = checkA2();
			break;
		case 2:
			is_mainDis = false;
			checkL();
			offLED();
			break;
		case 3:
			var = checkA3();
			break;
		case 4:
			is_mainDis = false;
			IRremo();
			offLED();
			break;

		default:
			var = 0;
			break;
	}

	Serial.print("var: ");
	Serial.println(var);
	//display

	if (is_mainDis)
	{
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
	}
	//display

	cycle++;

	if (cycle > 100)
	{
		cycle = 0;
	}

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
		modeNum = ++modeNum % modeMax;
		//modeNum++;
		cycle = 101;
	}
}

void checkL()
{
	int light = checkA3();
	Serial.print("light: ");
	Serial.println(light);

	if (light > 1000)
	{
		scaleDisplay(0);
	}
	else if (light > 920)
	{
		scaleDisplay(1);
	}
	else if (light > 840)
	{
		scaleDisplay(2);
	}
	else if (light > 760)
	{
		scaleDisplay(3);
	}
	else if (light > 680)
	{
		scaleDisplay(4);
	}
	else if (light > 600)
	{
		scaleDisplay(5);
	}
	else if (light > 520)
	{
		scaleDisplay(6);
	}
	else if (light > 440)
	{
		scaleDisplay(7);
	}
	else
	{
		scaleDisplay(8);
	}
}

void IRremo()
{
	if (irrecv.decode(&results))
    {
		digitalWrite(13, HIGH);
		unsigned long hex = results.value;
		Serial.print("hex: ");
		Serial.println(hex);
    	irrecv.resume();
		digitalWrite(13, LOW);
	}
}

unsigned long readRemote()
{
	unsigned long hex = NULL;
	if (irrecv.decode(&results))
    {
		digitalWrite(13, HIGH);
		hex = results.value;
    	irrecv.resume();
		digitalWrite(13, LOW);
	}
	return hex;
}

int bit10Tobit8(int bit10)
{
	return (bit10 * 256) / 1024;
}

int hexDecode(unsigned long hex)
{
    switch (hex)
    {
        case 0x23EAE8C2: return 0; break;
		case 0x94F37EE4: return 1; break;
	    case 0xF61D79DE: return 2; break;
	    case 0x81772F84: return 3; break;
    	case 0x4D91BBBE: return 4; break;
    	case 0xC7695F20: return 5; break;
    	case 0x8AC8FA2: return 6; break;
    	case 0x95D2E7E4: return 7; break;
	    case 0x1353935E: return 8; break;
    	case 0xCC7E81C8: return 9; break;

        default: return -1; break;
    }
}

int checkA6()
{
	return analogRead(A6);
}

int checkA5()
{
	return analogRead(A5);
}

int checkA3()
{
	return analogRead(A3);
}

int checkA2()
{
	return analogRead(A2);
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

		case 10:
			digitalWrite(segE, HIGH);
			digitalWrite(segF, HIGH);
			delay(5);
            break;

		case 11:
			digitalWrite(segB, HIGH);
			digitalWrite(segC, HIGH);
			digitalWrite(segE, HIGH);
			digitalWrite(segF, HIGH);
			delay(5);
            break;
    }
}

void scaleDisplay(int s)
{
	switch (s)
	{
		case 0:
			offLED();
			delay(5);
			offLED();
			delay(5);
			offLED();
			delay(5);
			offLED();
			delay(5);
			break;

		case 1:
			offLED();
			analogWrite(dig1, segLight);
			segDisplay(10);
			offLED();
			delay(5);
			offLED();
			delay(5);
			offLED();
			delay(5);
			break;

		case 2:
			offLED();
			analogWrite(dig1, segLight);
			segDisplay(11);
			offLED();
			delay(5);
			offLED();
			delay(5);
			offLED();
			delay(5);
			break;

		case 3:
			offLED();
			analogWrite(dig1, segLight);
			segDisplay(11);
			offLED();
			analogWrite(dig2, segLight);
			segDisplay(10);
			offLED();
			delay(5);
			offLED();
			delay(5);
			break;

		case 4:
			offLED();
			analogWrite(dig1, segLight);
			segDisplay(11);
			offLED();
			analogWrite(dig2, segLight);
			segDisplay(11);
			offLED();
			delay(5);
			offLED();
			delay(5);
			break;

		case 5:
			offLED();
			analogWrite(dig1, segLight);
			segDisplay(11);
			offLED();
			analogWrite(dig2, segLight);
			segDisplay(11);
			offLED();
			analogWrite(dig3, segLight);
			segDisplay(10);
			offLED();
			delay(5);
			break;

		case 6:
			offLED();
			analogWrite(dig1, segLight);
			segDisplay(11);
			offLED();
			analogWrite(dig2, segLight);
			segDisplay(11);
			offLED();
			analogWrite(dig3, segLight);
			segDisplay(11);
			offLED();
			delay(5);
			break;

		case 7:
			offLED();
			analogWrite(dig1, segLight);
			segDisplay(11);
			offLED();
			analogWrite(dig2, segLight);
			segDisplay(11);
			offLED();
			analogWrite(dig3, segLight);
			segDisplay(11);
			offLED();
			analogWrite(dig4, segLight);
			segDisplay(10);
			break;

		case 8:
			offLED();
			analogWrite(dig1, segLight);
			segDisplay(11);
			offLED();
			analogWrite(dig2, segLight);
			segDisplay(11);
			offLED();
			analogWrite(dig3, segLight);
			segDisplay(11);
			offLED();
			analogWrite(dig4, segLight);
			segDisplay(11);
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