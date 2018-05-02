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
IRsend irsend;
decode_results results;

static const uint8_t tonePin = 3;
uint8_t tempoToPlay  = 1;

byte modeNum = 0;
byte modeMax = 8;
bool disModeNum = true;

#define debug2Serial true

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
	irrecv.enableIRIn();
}

unsigned int cycle = 0;
int tmp = 0;
int var = 0;
bool is_playTone = false;


void loop()
{
	if (disModeNum)
	{
		offLED();
		delay(100);
		digitalWrite(dig3, HIGH);
		segDisplay(modeNum);
		delay(1000);
		disModeNum = false;
		offLED();
	}
	long ct = millis();
	bool is_onDP = false;
	bool is_mainDis = true;
	bool is_remote = false;
	bool is_fast = false;
	switch (modeNum % modeMax)
	{
		case 0:
			tmp = (int)((((checkA2() / 1023.0) * 5.0)) * 100);
			is_onDP = true;
			break;
		case 1:
			tmp = checkA2();
			break;
		case 2:
			is_mainDis = false;
			checkS(checkA3());
			offLED();
			break;
		case 3:
			tmp = checkA3();
			break;
		case 4:
			is_fast = true;
			offLED();
			digitalWrite(dig3, HIGH);
			if (irrecv.decode(&results))
    		{
				uint8_t melodyToPlay = (uint8_t)hexDecode(results.value);
				is_playTone = true;
				playMelody(melodyToPlay, tempoToPlay);
            	noTone(tonePin);
				digitalWrite(13, LOW);
    			irrecv.resume();
			}
			digitalWrite(dig3, LOW);
			break;
		case 5:
			is_mainDis = false;
			checkS(checkA6());
			offLED();
			break;
		case 6:
			tmp = (int)((((checkA6() / 1023.0) * 5.0)) * 100);
			is_onDP = true;
			break;
		case 7:
			tmp = checkA6();
			break;

		default:
			tmp = 0;
			break;
	}

	if (!is_fast)
	{
		if (millis() % 25 == 0)
		{
			var = tmp;
		}
		int dis = var;
		#if (debug2Serial)
		Serial.print("var: ");
		Serial.println(var);
		#endif
		//display

		if (is_mainDis)
		{
			byte t1 = dis / 1000;
			dis -= t1 *1000;
			offLED();
			if (t1 != 0)
			{
				digitalWrite(dig1, HIGH);
				segDisplay(t1);
			}

			byte t2 = dis / 100;
			dis -= t2 *100;
			offLED();
			digitalWrite(dig2, HIGH);
			if (is_onDP)
			{
				digitalWrite(segDP, HIGH);
			}
			segDisplay(t2);

			byte t3 = dis / 10;
			dis -= t3 *10;
			offLED();
			digitalWrite(dig3, HIGH);
			segDisplay(t3);

			byte t4 = dis;
			offLED();
			digitalWrite(dig4, HIGH);
			segDisplay(t4);
		}
		//display

		cycle++;

		if (cycle > 100)
		{
			cycle = 0;
		}

		#if (debug2Serial)
		Serial.print("modeNum: ");
		Serial.println(modeNum);
		Serial.print("cycle: ");
		Serial.println(millis() - ct);
		#endif
	}
}

void intp_program()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 20)
  {
    if (digitalRead(intp) == HIGH)
	  {
		  modeNum = ++modeNum % modeMax;
		  noTone(tonePin);
		  is_playTone = false;
		  tmp = 0;
		  var = 0;
		  cycle = 101;
		  disModeNum = true;
	  }
  }
  last_interrupt_time = interrupt_time;
}

void checkS(int light)
{
	#if (debug2Serial)
	Serial.print("light: ");
	Serial.println(light);
	#endif

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
			digitalWrite(dig1, HIGH);
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
			digitalWrite(dig1, HIGH);
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
			digitalWrite(dig1, HIGH);
			segDisplay(11);
			offLED();
			digitalWrite(dig2, HIGH);
			segDisplay(10);
			offLED();
			delay(5);
			offLED();
			delay(5);
			break;

		case 4:
			offLED();
			digitalWrite(dig1, HIGH);
			segDisplay(11);
			offLED();
			digitalWrite(dig2, HIGH);
			segDisplay(11);
			offLED();
			delay(5);
			offLED();
			delay(5);
			break;

		case 5:
			offLED();
			digitalWrite(dig1, HIGH);
			segDisplay(11);
			offLED();
			digitalWrite(dig2, HIGH);
			segDisplay(11);
			offLED();
			digitalWrite(dig3, HIGH);
			segDisplay(10);
			offLED();
			delay(5);
			break;

		case 6:
			offLED();
			digitalWrite(dig1, HIGH);
			segDisplay(11);
			offLED();
			digitalWrite(dig2, HIGH);
			segDisplay(11);
			offLED();
			digitalWrite(dig3, HIGH);
			segDisplay(11);
			offLED();
			delay(5);
			break;

		case 7:
			offLED();
			digitalWrite(dig1, HIGH);
			segDisplay(11);
			offLED();
			digitalWrite(dig2, HIGH);
			segDisplay(11);
			offLED();
			digitalWrite(dig3, HIGH);
			segDisplay(11);
			offLED();
			digitalWrite(dig4, HIGH);
			segDisplay(10);
			break;

		case 8:
			offLED();
			digitalWrite(dig1, HIGH);
			segDisplay(11);
			offLED();
			digitalWrite(dig2, HIGH);
			segDisplay(11);
			offLED();
			digitalWrite(dig3, HIGH);
			segDisplay(11);
			offLED();
			digitalWrite(dig4, HIGH);
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

#include <avr/pgmspace.h>
//********** define Note Codes start **********//


// Octave 0 Note Codes
#define       NOTE_C_0(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00000000)
#define      NOTE_CS_0(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00000001)
#define       NOTE_D_0(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00000010)
#define      NOTE_DS_0(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00000011)
#define       NOTE_E_0(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00000100)
#define       NOTE_F_0(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00000101)
#define      NOTE_FS_0(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00000110)
#define       NOTE_G_0(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00000111)
#define      NOTE_GS_0(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00001000)
#define       NOTE_A_0(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00001001)
#define      NOTE_AS_0(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00001010)
#define       NOTE_B_0(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00001011)

// Octave 1 Note Codes
#define       NOTE_C_1(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00010000)
#define      NOTE_CS_1(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00010001)
#define       NOTE_D_1(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00010010)
#define      NOTE_DS_1(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00010011)
#define       NOTE_E_1(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00010100)
#define       NOTE_F_1(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00010101)
#define      NOTE_FS_1(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00010110)
#define       NOTE_G_1(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00010111)
#define      NOTE_GS_1(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00011000)
#define       NOTE_A_1(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00011001)
#define      NOTE_AS_1(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00011010)
#define       NOTE_B_1(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00011011)

// Octave 2 Note Codes
#define       NOTE_C_2(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00100000)
#define      NOTE_CS_2(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00100001)
#define       NOTE_D_2(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00100010)
#define      NOTE_DS_2(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00100011)
#define       NOTE_E_2(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00100100)
#define       NOTE_F_2(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00100101)
#define      NOTE_FS_2(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00100110)
#define       NOTE_G_2(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00100111)
#define      NOTE_GS_2(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00101000)
#define       NOTE_A_2(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00101001)
#define      NOTE_AS_2(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00101010)
#define       NOTE_B_2(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00101011)

// Octave 3 Note Codes
#define       NOTE_C_3(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00110000)
#define      NOTE_CS_3(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00110001)
#define       NOTE_D_3(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00110010)
#define      NOTE_DS_3(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00110011)
#define       NOTE_E_3(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00110100)
#define       NOTE_F_3(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00110101)
#define      NOTE_FS_3(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00110110)
#define       NOTE_G_3(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00110111)
#define      NOTE_GS_3(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00111000)
#define       NOTE_A_3(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00111001)
#define      NOTE_AS_3(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00111010)
#define       NOTE_B_3(DURATION) ( (((uint16_t)DURATION)<<8) | 0b00111011)

// Octave 4 Note Codes
#define       NOTE_C_4(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01000000)
#define      NOTE_CS_4(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01000001)
#define       NOTE_D_4(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01000010)
#define      NOTE_DS_4(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01000011)
#define       NOTE_E_4(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01000100)
#define       NOTE_F_4(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01000101)
#define      NOTE_FS_4(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01000110)
#define       NOTE_G_4(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01000111)
#define      NOTE_GS_4(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01001000)
#define       NOTE_A_4(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01001001)
#define      NOTE_AS_4(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01001010)
#define       NOTE_B_4(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01001011)

// Octave 5 Note Codes
#define       NOTE_C_5(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01010000)
#define      NOTE_CS_5(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01010001)
#define       NOTE_D_5(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01010010)
#define      NOTE_DS_5(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01010011)
#define       NOTE_E_5(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01010100)
#define       NOTE_F_5(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01010101)
#define      NOTE_FS_5(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01010110)
#define       NOTE_G_5(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01010111)
#define      NOTE_GS_5(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01011000)
#define       NOTE_A_5(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01011001)
#define      NOTE_AS_5(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01011010)
#define       NOTE_B_5(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01011011)

// Octave 6 Note Codes
#define       NOTE_C_6(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01100000)
#define      NOTE_CS_6(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01100001)
#define       NOTE_D_6(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01100010)
#define      NOTE_DS_6(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01100011)
#define       NOTE_E_6(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01100100)
#define       NOTE_F_6(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01100101)
#define      NOTE_FS_6(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01100110)
#define       NOTE_G_6(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01100111)
#define      NOTE_GS_6(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01101000)
#define       NOTE_A_6(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01101001)
#define      NOTE_AS_6(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01101010)
#define       NOTE_B_6(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01101011)

// Octave 7 Note Codes
#define       NOTE_C_7(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01110000)
#define      NOTE_CS_7(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01110001)
#define       NOTE_D_7(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01110010)
#define      NOTE_DS_7(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01110011)
#define       NOTE_E_7(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01110100)
#define       NOTE_F_7(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01110101)
#define      NOTE_FS_7(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01110110)
#define       NOTE_G_7(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01110111)
#define      NOTE_GS_7(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01111000)
#define       NOTE_A_7(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01111001)
#define      NOTE_AS_7(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01111010)
#define       NOTE_B_7(DURATION) ( (((uint16_t)DURATION)<<8) | 0b01111011)

// Octave 8 Note Codes
#define       NOTE_C_8(DURATION) ( (((uint16_t)DURATION)<<8) | 0b10000000)
#define      NOTE_CS_8(DURATION) ( (((uint16_t)DURATION)<<8) | 0b10000001)
#define       NOTE_D_8(DURATION) ( (((uint16_t)DURATION)<<8) | 0b10000010)
#define      NOTE_DS_8(DURATION) ( (((uint16_t)DURATION)<<8) | 0b10000011)
#define       NOTE_E_8(DURATION) ( (((uint16_t)DURATION)<<8) | 0b10000100)
#define       NOTE_F_8(DURATION) ( (((uint16_t)DURATION)<<8) | 0b10000101)
#define      NOTE_FS_8(DURATION) ( (((uint16_t)DURATION)<<8) | 0b10000110)
#define       NOTE_G_8(DURATION) ( (((uint16_t)DURATION)<<8) | 0b10000111)
#define      NOTE_GS_8(DURATION) ( (((uint16_t)DURATION)<<8) | 0b10001000)
#define       NOTE_A_8(DURATION) ( (((uint16_t)DURATION)<<8) | 0b10001001)
#define      NOTE_AS_8(DURATION) ( (((uint16_t)DURATION)<<8) | 0b10001010)
#define       NOTE_B_8(DURATION) ( (((uint16_t)DURATION)<<8) | 0b10001011)

#define    NOTE_SILENT(DURATION) ((((uint16_t)DURATION)<<8) | 0b00001111)


//********** define Note Codes end **********//

//**********  **********//

//Track 0
static const uint16_t Melody0[] PROGMEM = {
           NOTE_CS_7( 113 ),  NOTE_SILENT(   7 ),    NOTE_DS_7( 113 ),     NOTE_E_7( 255 ), 
            NOTE_E_7(  86 ),  NOTE_SILENT(  19 ),     NOTE_E_7( 255 ),     NOTE_E_7(  86 ), 
         NOTE_SILENT(  19 ),    NOTE_DS_7( 255 ),    NOTE_DS_7( 200 ),  NOTE_SILENT(  25 ), 
            NOTE_B_6( 255 ),     NOTE_B_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_GS_6( 255 ), 
           NOTE_GS_6( 255 ),    NOTE_GS_6( 255 ),    NOTE_GS_6( 255 ),    NOTE_GS_6( 255 ), 
           NOTE_GS_6( 255 ),    NOTE_GS_6(  65 ),  NOTE_SILENT( 255 ),  NOTE_SILENT(  70 ), 
           NOTE_GS_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_A_6( 113 ),  NOTE_SILENT(   7 ), 
            NOTE_B_6( 255 ),     NOTE_B_6( 200 ),  NOTE_SILENT(  25 ),     NOTE_B_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_A_6( 255 ),     NOTE_A_6( 200 ),  NOTE_SILENT(  25 ), 
           NOTE_GS_6( 255 ),    NOTE_GS_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_GS_6( 255 ), 
           NOTE_GS_6( 255 ),    NOTE_GS_6( 255 ),    NOTE_GS_6( 255 ),    NOTE_GS_6( 255 ), 
           NOTE_GS_6( 255 ),    NOTE_GS_6(  65 ),  NOTE_SILENT(  85 ),    NOTE_CS_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_B_5( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_E_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_FS_6( 255 ), 
           NOTE_FS_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 200 ), 
         NOTE_SILENT(  25 ),     NOTE_E_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_B_5( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_CS_6( 255 ),    NOTE_CS_6(  86 ),  NOTE_SILENT(  19 ), 
           NOTE_FS_6( 255 ),    NOTE_FS_6(  86 ),  NOTE_SILENT(  19 ),    NOTE_GS_6( 255 ), 
           NOTE_GS_6( 255 ),    NOTE_GS_6( 255 ),    NOTE_GS_6( 146 ),  NOTE_SILENT(  49 ), 
           NOTE_CS_6( 113 ),  NOTE_SILENT(   7 ),    NOTE_DS_6( 113 ),  NOTE_SILENT(   7 ), 
            NOTE_E_6( 255 ),     NOTE_E_6( 255 ),     NOTE_E_6( 255 ),     NOTE_E_6( 255 ), 
            NOTE_E_6( 255 ),     NOTE_E_6(  92 ),  NOTE_SILENT(  73 ),     NOTE_B_5(  63 ), 
            NOTE_E_6(  64 ),    NOTE_GS_6( 255 ),    NOTE_GS_6(  96 ),  NOTE_SILENT(   2 ), 
           NOTE_FS_6( 255 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 146 ), 
         NOTE_SILENT(  49 ),     NOTE_B_6(  63 ),    NOTE_DS_7(  64 ),    NOTE_FS_7(  65 ), 
            NOTE_B_7( 255 ),     NOTE_B_7( 255 ),     NOTE_B_7( 255 ),     NOTE_B_7(   1 ), 
         NOTE_SILENT(   2 ),     NOTE_B_5( 227 ),  NOTE_SILENT(  13 ),    NOTE_FS_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_E_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_FS_6( 255 ),    NOTE_FS_6(  86 ),  NOTE_SILENT(  19 ), 
            NOTE_E_6( 255 ),     NOTE_E_6(  86 ),  NOTE_SILENT(  19 ),    NOTE_FS_6( 240 ), 
           NOTE_CS_6( 215 ),  NOTE_SILENT(  25 ),     NOTE_E_6( 255 ),     NOTE_E_6( 200 ), 
         NOTE_SILENT(  25 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 200 ),  NOTE_SILENT(  25 ), 
           NOTE_GS_6( 255 ),    NOTE_GS_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_FS_6( 240 ), 
           NOTE_DS_6( 215 ),  NOTE_SILENT(  25 ),     NOTE_E_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_E_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_FS_6( 255 ),    NOTE_FS_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_GS_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_FS_6( 240 ),    NOTE_CS_6( 215 ),  NOTE_SILENT(  25 ), 
            NOTE_E_6( 255 ),     NOTE_E_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_CS_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_CS_6( 255 ),    NOTE_CS_6( 255 ),    NOTE_CS_6( 255 ), 
           NOTE_CS_6( 146 ),  NOTE_SILENT(  49 ),     NOTE_B_5( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ), 
            NOTE_E_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_FS_6( 255 ),    NOTE_FS_6(  86 ), 
         NOTE_SILENT(  19 ),     NOTE_E_6( 255 ),     NOTE_E_6(  86 ),  NOTE_SILENT(  19 ), 
           NOTE_FS_6( 240 ),    NOTE_CS_6( 215 ),  NOTE_SILENT(  25 ),     NOTE_E_6( 255 ), 
            NOTE_E_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 200 ), 
         NOTE_SILENT(  25 ),    NOTE_GS_6( 255 ),    NOTE_GS_6( 200 ),  NOTE_SILENT(  25 ), 
           NOTE_CS_6( 240 ),    NOTE_GS_5( 255 ),    NOTE_GS_5( 255 ),    NOTE_GS_5( 161 ), 
         NOTE_SILENT(  49 ),     NOTE_B_5( 227 ),  NOTE_SILENT(  13 ),     NOTE_B_5( 255 ), 
            NOTE_B_5( 255 ),     NOTE_B_5( 255 ),     NOTE_B_5( 146 ),  NOTE_SILENT(  49 ), 
           NOTE_FS_5( 255 ),    NOTE_FS_5( 255 ),    NOTE_FS_5( 173 ),  NOTE_SILENT(  37 ), 
           NOTE_GS_5( 255 ),    NOTE_GS_5( 255 ),    NOTE_GS_5( 255 ),    NOTE_GS_5( 255 ), 
           NOTE_GS_5( 119 ),  NOTE_SILENT(  61 ),     NOTE_B_5( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ), 
            NOTE_E_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_FS_6( 255 ),    NOTE_FS_6(  86 ), 
         NOTE_SILENT(  19 ),     NOTE_E_6( 255 ),     NOTE_E_6(  86 ),  NOTE_SILENT(  19 ), 
           NOTE_FS_6( 240 ),    NOTE_CS_6( 215 ),  NOTE_SILENT(  25 ),     NOTE_E_6( 255 ), 
            NOTE_E_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 200 ), 
         NOTE_SILENT(  25 ),    NOTE_GS_6( 255 ),    NOTE_GS_6( 200 ),  NOTE_SILENT(  25 ), 
           NOTE_FS_6( 240 ),    NOTE_DS_6( 215 ),  NOTE_SILENT(  25 ),     NOTE_E_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_E_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_FS_6( 255 ),    NOTE_FS_6(  86 ),  NOTE_SILENT(  19 ), 
           NOTE_GS_6( 255 ),    NOTE_GS_6(  86 ),  NOTE_SILENT(  19 ),    NOTE_CS_6( 240 ), 
           NOTE_GS_5( 255 ),    NOTE_GS_5( 188 ),  NOTE_SILENT(  37 ),     NOTE_B_5( 255 ), 
            NOTE_B_5( 200 ),  NOTE_SILENT(  25 ),    NOTE_CS_6( 227 ),  NOTE_SILENT(  13 ), 
            NOTE_E_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_GS_6( 255 ),    NOTE_GS_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_CS_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_CS_6( 255 ),    NOTE_CS_6( 200 ),  NOTE_SILENT(  25 ), 
            NOTE_E_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_E_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_GS_6( 255 ),    NOTE_GS_6( 200 ), 
         NOTE_SILENT(  25 ),    NOTE_CS_6( 255 ),    NOTE_CS_6( 200 ),  NOTE_SILENT(  25 ), 
           NOTE_CS_6( 255 ),    NOTE_CS_6( 255 ),    NOTE_CS_6( 255 ),    NOTE_CS_6( 146 ), 
         NOTE_SILENT(  49 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 173 ), 
         NOTE_SILENT(  37 ),     NOTE_E_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_FS_6( 255 ), 
           NOTE_FS_6( 200 ),  NOTE_SILENT(  25 ),     NOTE_B_5( 255 ),     NOTE_B_5( 200 ), 
         NOTE_SILENT(  25 ),    NOTE_DS_6( 255 ),    NOTE_DS_6( 255 ),    NOTE_DS_6( 173 ), 
         NOTE_SILENT(  37 ),    NOTE_CS_6( 255 ),    NOTE_CS_6( 255 ),    NOTE_CS_6( 255 ), 
           NOTE_CS_6( 255 ),    NOTE_CS_6( 255 ),    NOTE_CS_6(  92 ),  NOTE_SILENT(  73 ), 
            NOTE_B_4(  63 ),     NOTE_E_5(  64 ),    NOTE_GS_5( 255 ),    NOTE_GS_5( 255 ), 
           NOTE_GS_5( 255 ),    NOTE_GS_5(  66 ),  NOTE_SILENT(   2 ),    NOTE_CS_6( 255 ), 
           NOTE_CS_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_CS_6( 255 ),    NOTE_CS_6( 200 ), 
         NOTE_SILENT(  25 ),     NOTE_E_6( 255 ),     NOTE_E_6( 200 ),  NOTE_SILENT(  25 ), 
           NOTE_CS_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_E_6( 255 ),     NOTE_E_6( 200 ), 
         NOTE_SILENT(  25 ),    NOTE_GS_6( 255 ),    NOTE_GS_6( 200 ),  NOTE_SILENT(  25 ), 
           NOTE_FS_6( 255 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 173 ),  NOTE_SILENT(  37 ), 
            NOTE_E_6( 255 ),     NOTE_E_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_FS_6( 255 ), 
           NOTE_FS_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_GS_6( 255 ),    NOTE_GS_6( 200 ), 
         NOTE_SILENT(  25 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 200 ),  NOTE_SILENT(  25 ), 
            NOTE_B_5( 227 ),  NOTE_SILENT(  13 ),     NOTE_B_5( 255 ),     NOTE_B_5( 200 ), 
         NOTE_SILENT(  25 ),    NOTE_CS_6( 255 ),    NOTE_CS_6( 255 ),    NOTE_CS_6( 173 ), 
         NOTE_SILENT(  37 ),     NOTE_E_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_E_5( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_GS_5( 227 ),  NOTE_SILENT(  13 ),     NOTE_E_6( 255 ), 
            NOTE_E_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_DS_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_FS_6( 255 ),    NOTE_FS_6( 200 ),  NOTE_SILENT(  25 ),     NOTE_E_6( 255 ), 
            NOTE_E_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_DS_6( 255 ),    NOTE_DS_6( 200 ), 
         NOTE_SILENT(  25 ),     NOTE_B_5( 255 ),     NOTE_B_5( 200 ),  NOTE_SILENT(  25 ), 
           NOTE_GS_5( 255 ),    NOTE_GS_5( 200 ),  NOTE_SILENT(  25 ),    NOTE_GS_5( 255 ), 
           NOTE_GS_5( 255 ),    NOTE_GS_5( 255 ),    NOTE_GS_5( 146 ),  NOTE_SILENT(  49 ), 
           NOTE_CS_6( 255 ),    NOTE_CS_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_CS_6( 255 ), 
           NOTE_CS_6( 200 ),  NOTE_SILENT(  25 ),     NOTE_E_6( 255 ),     NOTE_E_6( 200 ), 
         NOTE_SILENT(  25 ),    NOTE_CS_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_E_6( 255 ), 
            NOTE_E_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_GS_6( 255 ),    NOTE_GS_6( 200 ), 
         NOTE_SILENT(  25 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 173 ), 
         NOTE_SILENT(  37 ),     NOTE_B_5( 255 ),     NOTE_B_5( 200 ),  NOTE_SILENT(  25 ), 
           NOTE_CS_6( 255 ),    NOTE_CS_6( 200 ),  NOTE_SILENT(  25 ),     NOTE_B_6( 255 ), 
            NOTE_B_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_GS_6( 255 ),    NOTE_GS_6( 200 ), 
         NOTE_SILENT(  25 ),    NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_E_6( 255 ), 
            NOTE_E_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 200 ), 
         NOTE_SILENT(  25 ),    NOTE_FS_6( 240 ),     NOTE_D_6( 215 ),  NOTE_SILENT(  25 ), 
            NOTE_E_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ), 
            NOTE_E_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 255 ), 
           NOTE_FS_6( 173 ),  NOTE_SILENT(  37 ),    NOTE_FS_6( 240 ),    NOTE_CS_6( 215 ), 
         NOTE_SILENT(  25 ),     NOTE_E_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_FS_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_E_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_GS_6( 255 ), 
           NOTE_GS_6( 200 ),  NOTE_SILENT(  25 ),     NOTE_B_5( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_CS_6( 255 ),    NOTE_CS_6( 255 ),    NOTE_CS_6( 255 ),    NOTE_CS_6( 255 ), 
           NOTE_CS_6( 119 ),  NOTE_SILENT(  61 ),     NOTE_F_5( 255 ),     NOTE_F_5( 255 ), 
            NOTE_F_5( 255 ),     NOTE_F_5( 146 ),  NOTE_SILENT( 255 ),  NOTE_SILENT( 255 ), 
         NOTE_SILENT( 255 ),  NOTE_SILENT( 255 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 200 ), 
           NOTE_CS_7( 255 ),    NOTE_CS_7(  86 ),  NOTE_SILENT(  19 ),    NOTE_FS_6( 255 ), 
           NOTE_FS_6(  86 ),  NOTE_SILENT(  19 ),    NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_FS_6( 255 ),    NOTE_FS_6(  86 ),  NOTE_SILENT(  19 ),     NOTE_E_6( 255 ), 
            NOTE_E_6(  86 ),  NOTE_SILENT(  19 ),     NOTE_E_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_FS_6( 255 ),    NOTE_FS_6(  86 ),  NOTE_SILENT(  19 ),     NOTE_A_6( 255 ), 
            NOTE_A_6(  86 ),  NOTE_SILENT(  19 ),     NOTE_B_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_CS_7( 255 ),    NOTE_CS_7( 200 ),  NOTE_SILENT(  25 ),     NOTE_A_5( 255 ), 
            NOTE_A_5( 200 ),  NOTE_SILENT(  25 ),    NOTE_FS_6( 255 ),    NOTE_FS_6(  86 ), 
         NOTE_SILENT(  19 ),     NOTE_A_6( 255 ),     NOTE_A_6(  86 ),  NOTE_SILENT(  19 ), 
            NOTE_B_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_7( 255 ),    NOTE_CS_7(  86 ), 
         NOTE_SILENT(  19 ),     NOTE_B_6( 255 ),     NOTE_B_6(  86 ),  NOTE_SILENT(  19 ), 
            NOTE_A_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_B_6( 255 ),     NOTE_B_6(  86 ), 
         NOTE_SILENT(  19 ),     NOTE_A_6( 255 ),     NOTE_A_6(  86 ),  NOTE_SILENT(  19 ), 
            NOTE_E_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 200 ), 
         NOTE_SILENT(  25 ),     NOTE_E_5( 255 ),     NOTE_E_5( 200 ),  NOTE_SILENT( 255 ), 
         NOTE_SILENT( 250 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 200 ),  NOTE_SILENT(  25 ), 
            NOTE_E_6( 255 ),     NOTE_E_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_FS_6( 255 ), 
           NOTE_FS_6( 200 ),  NOTE_SILENT(  25 ),     NOTE_A_6( 255 ),     NOTE_A_6( 200 ), 
         NOTE_SILENT(  25 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 200 ),  NOTE_SILENT(  25 ), 
           NOTE_FS_6( 255 ),    NOTE_FS_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_CS_6( 255 ), 
           NOTE_CS_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_DS_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_DS_5( 227 ),  NOTE_SILENT(  13 ),    NOTE_GS_5( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_GS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_GS_6( 255 ),    NOTE_GS_6( 255 ), 
           NOTE_GS_6( 255 ),    NOTE_GS_6( 146 ),  NOTE_SILENT(  49 ),     NOTE_F_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_F_5( 227 ),  NOTE_SILENT(  13 ),     NOTE_A_5( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_CS_7( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_7( 255 ), 
           NOTE_CS_7( 255 ),    NOTE_CS_7( 255 ),    NOTE_CS_7( 146 ),  NOTE_SILENT(  49 ), 
           NOTE_CS_7( 255 ),    NOTE_CS_7(  86 ),  NOTE_SILENT(  19 ),    NOTE_FS_6( 255 ), 
           NOTE_FS_6(  86 ),  NOTE_SILENT(  19 ),    NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_FS_6( 255 ),    NOTE_FS_6(  86 ),  NOTE_SILENT(  19 ),     NOTE_E_6( 255 ), 
            NOTE_E_6(  86 ),  NOTE_SILENT(  19 ),     NOTE_E_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_FS_6( 255 ),    NOTE_FS_6(  86 ),  NOTE_SILENT(  19 ),     NOTE_A_6( 255 ), 
            NOTE_A_6(  86 ),  NOTE_SILENT(  19 ),     NOTE_B_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_CS_7( 255 ),    NOTE_CS_7( 200 ),  NOTE_SILENT(  25 ),     NOTE_A_5( 255 ), 
            NOTE_A_5( 200 ),  NOTE_SILENT(  25 ),    NOTE_FS_6( 255 ),    NOTE_FS_6(  86 ), 
         NOTE_SILENT(  19 ),     NOTE_A_6( 255 ),     NOTE_A_6(  86 ),  NOTE_SILENT(  19 ), 
            NOTE_B_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_7( 255 ),    NOTE_CS_7(  86 ), 
         NOTE_SILENT(  19 ),     NOTE_E_7( 255 ),     NOTE_E_7(  86 ),  NOTE_SILENT(  19 ), 
            NOTE_B_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_7( 255 ),    NOTE_CS_7(  86 ), 
         NOTE_SILENT(  19 ),     NOTE_B_6( 255 ),     NOTE_B_6(  86 ),  NOTE_SILENT(  19 ), 
            NOTE_A_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 200 ), 
         NOTE_SILENT(  25 ),     NOTE_E_6( 255 ),     NOTE_E_6( 200 ),  NOTE_SILENT(  25 ), 
           NOTE_FS_6( 255 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 146 ), 
         NOTE_SILENT(  49 ),    NOTE_CS_7(  63 ),    NOTE_FS_7(  64 ),     NOTE_A_7( 255 ), 
            NOTE_A_7( 255 ),     NOTE_A_7( 255 ),     NOTE_A_7(  66 ),  NOTE_SILENT(   2 ), 
           NOTE_GS_6( 255 ),    NOTE_GS_6(  86 ),  NOTE_SILENT(  19 ),    NOTE_GS_5( 255 ), 
           NOTE_GS_5(  86 ),  NOTE_SILENT(  19 ),    NOTE_CS_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_GS_6( 255 ),    NOTE_GS_6( 200 ),  NOTE_SILENT(  25 ),     NOTE_A_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_A_6( 255 ),     NOTE_A_6( 255 ),     NOTE_A_6(  59 ), 
         NOTE_SILENT(  31 ),    NOTE_FS_5( 255 ),    NOTE_FS_5(  86 ),  NOTE_SILENT(  19 ), 
            NOTE_A_5( 227 ),  NOTE_SILENT(  13 ),     NOTE_D_6( 255 ),     NOTE_D_6( 200 ), 
         NOTE_SILENT(  25 ),     NOTE_D_5( 255 ),     NOTE_D_5( 200 ),  NOTE_SILENT(  25 ), 
            NOTE_B_6( 255 ),     NOTE_B_6( 255 ),     NOTE_B_6( 255 ),     NOTE_B_6( 255 ), 
            NOTE_B_6( 255 ),     NOTE_B_6(  92 ),  NOTE_SILENT(  73 ),    NOTE_CS_7( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_CS_7( 255 ),    NOTE_CS_7( 255 ),    NOTE_CS_7( 255 ), 
           NOTE_CS_7( 255 ),    NOTE_CS_7( 255 ),    NOTE_CS_7( 255 ),    NOTE_CS_7(  65 ), 
         NOTE_SILENT( 255 ),  NOTE_SILENT( 255 ),  NOTE_SILENT( 255 ),  NOTE_SILENT( 255 ), 
           NOTE_FS_6( 255 ),    NOTE_FS_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_CS_7( 255 ), 
           NOTE_CS_7( 200 ),  NOTE_SILENT(  25 ),     NOTE_B_6( 255 ),     NOTE_B_6( 200 ), 
         NOTE_SILENT(  25 ),    NOTE_CS_7( 255 ),    NOTE_CS_7(  86 ),  NOTE_SILENT(  19 ), 
           NOTE_FS_6( 255 ),    NOTE_FS_6(  86 ),  NOTE_SILENT(  19 ),    NOTE_FS_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_FS_6( 255 ),    NOTE_FS_6(  86 ),  NOTE_SILENT(  19 ), 
            NOTE_E_6( 255 ),     NOTE_E_6(  86 ),  NOTE_SILENT(  19 ),     NOTE_E_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_FS_6( 255 ),    NOTE_FS_6(  86 ),  NOTE_SILENT(  19 ), 
            NOTE_A_6( 255 ),     NOTE_A_6(  86 ),  NOTE_SILENT(  19 ),     NOTE_B_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_CS_7( 255 ),    NOTE_CS_7( 200 ),  NOTE_SILENT(  25 ), 
            NOTE_A_5( 255 ),     NOTE_A_5( 200 ),  NOTE_SILENT(  25 ),    NOTE_FS_6( 255 ), 
           NOTE_FS_6(  86 ),  NOTE_SILENT(  19 ),     NOTE_A_6( 255 ),     NOTE_A_6(  86 ), 
         NOTE_SILENT(  19 ),     NOTE_B_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_7( 255 ), 
           NOTE_CS_7(  86 ),  NOTE_SILENT(  19 ),     NOTE_B_6( 255 ),     NOTE_B_6(  86 ), 
         NOTE_SILENT(  19 ),     NOTE_A_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_B_6( 255 ), 
            NOTE_B_6(  86 ),  NOTE_SILENT(  19 ),     NOTE_A_6( 255 ),     NOTE_A_6(  86 ), 
         NOTE_SILENT(  19 ),     NOTE_E_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_FS_6( 255 ), 
           NOTE_FS_6( 200 ),  NOTE_SILENT(  25 ),     NOTE_E_5( 255 ),     NOTE_E_5( 200 ), 
         NOTE_SILENT( 255 ),  NOTE_SILENT( 250 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 200 ), 
         NOTE_SILENT(  25 ),     NOTE_E_6( 255 ),     NOTE_E_6( 200 ),  NOTE_SILENT(  25 ), 
           NOTE_FS_6( 255 ),    NOTE_FS_6( 200 ),  NOTE_SILENT(  25 ),     NOTE_A_6( 255 ), 
            NOTE_A_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 200 ), 
         NOTE_SILENT(  25 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 200 ),  NOTE_SILENT(  25 ), 
           NOTE_CS_6( 255 ),    NOTE_CS_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_DS_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_DS_5( 227 ),  NOTE_SILENT(  13 ),    NOTE_GS_5( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_GS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_GS_6( 255 ), 
           NOTE_GS_6( 255 ),    NOTE_GS_6( 255 ),    NOTE_GS_6( 146 ),  NOTE_SILENT(  49 ), 
            NOTE_D_7( 227 ),  NOTE_SILENT(  13 ),     NOTE_F_5( 227 ),  NOTE_SILENT(  13 ), 
            NOTE_A_5( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_7( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_CS_7( 255 ),    NOTE_CS_7( 255 ),    NOTE_CS_7( 255 ),    NOTE_CS_7( 146 ), 
         NOTE_SILENT(  49 ),    NOTE_CS_7( 255 ),    NOTE_CS_7(  86 ),  NOTE_SILENT(  19 ), 
           NOTE_FS_6( 255 ),    NOTE_FS_6(  86 ),  NOTE_SILENT(  19 ),    NOTE_FS_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_FS_6( 255 ),    NOTE_FS_6(  86 ),  NOTE_SILENT(  19 ), 
            NOTE_E_6( 255 ),     NOTE_E_6(  86 ),  NOTE_SILENT(  19 ),     NOTE_E_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_FS_6( 255 ),    NOTE_FS_6(  86 ),  NOTE_SILENT(  19 ), 
            NOTE_A_6( 255 ),     NOTE_A_6(  86 ),  NOTE_SILENT(  19 ),     NOTE_B_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_CS_7( 255 ),    NOTE_CS_7( 200 ),  NOTE_SILENT(  25 ), 
            NOTE_A_5( 255 ),     NOTE_A_5( 200 ),  NOTE_SILENT(  25 ),    NOTE_FS_6( 255 ), 
           NOTE_FS_6(  86 ),  NOTE_SILENT(  19 ),     NOTE_A_6( 255 ),     NOTE_A_6(  86 ), 
         NOTE_SILENT(  19 ),     NOTE_B_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_7( 255 ), 
           NOTE_CS_7(  86 ),  NOTE_SILENT(  19 ),     NOTE_E_7( 255 ),     NOTE_E_7(  86 ), 
         NOTE_SILENT(  19 ),     NOTE_B_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_7( 255 ), 
           NOTE_CS_7(  86 ),  NOTE_SILENT(  19 ),     NOTE_B_6( 255 ),     NOTE_B_6(  86 ), 
         NOTE_SILENT(  19 ),    NOTE_CS_7( 227 ),  NOTE_SILENT(  13 ),    NOTE_FS_6( 255 ), 
           NOTE_FS_6( 200 ),  NOTE_SILENT(  25 ),     NOTE_E_6( 255 ),     NOTE_E_6( 200 ), 
         NOTE_SILENT(  25 ),    NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_FS_5( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_B_5( 113 ),  NOTE_SILENT(   7 ),     NOTE_D_6( 113 ), 
         NOTE_SILENT(   7 ),    NOTE_FS_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_B_6( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_D_7( 113 ),  NOTE_SILENT(   7 ),    NOTE_CS_7(  63 ), 
           NOTE_FS_7(  64 ),     NOTE_A_7( 255 ),     NOTE_A_7( 255 ),     NOTE_A_7( 255 ), 
            NOTE_A_7(  66 ),  NOTE_SILENT(   2 ),    NOTE_GS_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_GS_5( 113 ),  NOTE_SILENT(   7 ),    NOTE_CS_6( 113 ),  NOTE_SILENT(   7 ), 
            NOTE_E_6( 113 ),  NOTE_SILENT(   7 ),    NOTE_GS_6( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_CS_7( 113 ),  NOTE_SILENT(   7 ),     NOTE_E_7( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_GS_7( 255 ),    NOTE_GS_7( 200 ),  NOTE_SILENT(  25 ),     NOTE_A_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_A_6( 255 ),     NOTE_A_6( 255 ),     NOTE_A_6(  59 ), 
         NOTE_SILENT(  31 ),    NOTE_FS_5( 255 ),    NOTE_FS_5(  86 ),  NOTE_SILENT(  19 ), 
            NOTE_A_5( 227 ),  NOTE_SILENT(  13 ),     NOTE_D_6( 255 ),     NOTE_D_6( 255 ), 
            NOTE_D_6( 173 ),  NOTE_SILENT(  37 ),     NOTE_E_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_GS_6( 255 ),    NOTE_GS_6( 255 ),    NOTE_GS_6( 255 ),    NOTE_GS_6( 255 ), 
           NOTE_GS_6( 255 ),    NOTE_GS_6(  92 ),  NOTE_SILENT(  73 ),    NOTE_FS_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 255 ), 
           NOTE_FS_6( 255 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 255 ),    NOTE_FS_6(  65 ), 
         NOTE_SILENT( 255 ),  NOTE_SILENT( 255 ),  NOTE_SILENT(  55 ),     NOTE_B_6( 255 ), 
            NOTE_B_6( 255 ),     NOTE_B_6( 255 ),     NOTE_B_6( 255 ),     NOTE_B_6( 255 ), 
            NOTE_B_6(  92 ),  NOTE_SILENT(  73 ),    NOTE_CS_7( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_CS_7( 255 ),    NOTE_CS_7( 255 ),    NOTE_CS_7( 255 ),    NOTE_CS_7( 255 ), 
           NOTE_CS_7( 119 ),  NOTE_SILENT(  61 ),     NOTE_A_5( 255 ),     NOTE_A_5(  86 ), 
         NOTE_SILENT(  19 ),    NOTE_GS_5( 255 ),    NOTE_GS_5(  86 ),  NOTE_SILENT(  19 ), 
            NOTE_E_5( 227 ),  NOTE_SILENT(  13 ),    NOTE_FS_5( 255 ),    NOTE_FS_5( 255 ), 
           NOTE_FS_5( 255 ),    NOTE_FS_5( 255 ),    NOTE_FS_5( 255 ),    NOTE_FS_5(  92 ), 
};
static const uint16_t Melody0_Length = sizeof(Melody0) / sizeof(uint16_t);

//Track 1
static const uint16_t Melody1[] PROGMEM = {
           NOTE_AS_5( 227 ),  NOTE_SILENT(  13 ),    NOTE_DS_6( 227 ),  NOTE_SILENT(  13 ), 
            NOTE_F_6( 255 ),     NOTE_F_6( 255 ),     NOTE_F_6( 255 ),     NOTE_F_6( 146 ), 
         NOTE_SILENT(  49 ),    NOTE_DS_6( 255 ),    NOTE_DS_6( 200 ),  NOTE_SILENT(  25 ), 
            NOTE_G_6( 255 ),     NOTE_G_6( 255 ),     NOTE_G_6( 255 ),     NOTE_G_6( 146 ), 
         NOTE_SILENT(  49 ),    NOTE_DS_6( 255 ),    NOTE_DS_6( 200 ),  NOTE_SILENT(  25 ), 
           NOTE_GS_6( 255 ),    NOTE_GS_6( 200 ),  NOTE_SILENT(  25 ),     NOTE_G_6( 255 ), 
            NOTE_G_6( 200 ),  NOTE_SILENT(  25 ),     NOTE_F_6( 255 ),     NOTE_F_6( 200 ), 
         NOTE_SILENT(  25 ),     NOTE_G_6( 255 ),     NOTE_G_6( 255 ),     NOTE_G_6( 255 ), 
            NOTE_G_6( 146 ),  NOTE_SILENT(  49 ),    NOTE_DS_6( 255 ),    NOTE_DS_6( 200 ), 
         NOTE_SILENT(  25 ),    NOTE_AS_6( 255 ),    NOTE_AS_6( 255 ),    NOTE_AS_6( 255 ), 
           NOTE_AS_6( 146 ),  NOTE_SILENT(  49 ),     NOTE_G_6( 255 ),     NOTE_G_6( 200 ), 
         NOTE_SILENT(  25 ),     NOTE_C_7( 255 ),     NOTE_C_7( 255 ),     NOTE_C_7( 255 ), 
            NOTE_C_7( 146 ),  NOTE_SILENT(  49 ),     NOTE_G_6( 255 ),     NOTE_G_6( 200 ), 
         NOTE_SILENT(  25 ),    NOTE_AS_6( 255 ),    NOTE_AS_6( 200 ),  NOTE_SILENT(  25 ), 
           NOTE_GS_6( 255 ),    NOTE_GS_6( 200 ),  NOTE_SILENT(  25 ),     NOTE_G_6( 255 ), 
            NOTE_G_6( 200 ),  NOTE_SILENT(  25 ),     NOTE_G_6( 255 ),     NOTE_G_6( 255 ), 
            NOTE_G_6( 255 ),     NOTE_G_6( 255 ),     NOTE_G_6( 255 ),     NOTE_G_6( 255 ), 
            NOTE_G_6( 255 ),     NOTE_G_6( 255 ),     NOTE_G_6( 255 ),     NOTE_G_6( 255 ), 
            NOTE_G_6( 185 ),  NOTE_SILENT( 145 ),     NOTE_F_6( 255 ),     NOTE_F_6( 255 ), 
            NOTE_F_6( 255 ),     NOTE_F_6( 255 ),     NOTE_F_6( 255 ),     NOTE_F_6( 255 ), 
            NOTE_F_6( 255 ),     NOTE_F_6( 255 ),     NOTE_F_6( 239 ),  NOTE_SILENT( 255 ), 
         NOTE_SILENT( 255 ),  NOTE_SILENT(  91 ),    NOTE_AS_5( 255 ),    NOTE_AS_5( 255 ), 
           NOTE_AS_5( 255 ),    NOTE_AS_5( 255 ),    NOTE_AS_5( 255 ),    NOTE_AS_5(  92 ), 
         NOTE_SILENT(  73 ),    NOTE_AS_5( 255 ),    NOTE_AS_5( 255 ),    NOTE_AS_5( 255 ), 
           NOTE_AS_5( 255 ),    NOTE_AS_5( 255 ),    NOTE_AS_5(  92 ),  NOTE_SILENT(  73 ), 
           NOTE_AS_5( 255 ),    NOTE_AS_5( 255 ),    NOTE_AS_5( 255 ),    NOTE_AS_5( 255 ), 
           NOTE_AS_5( 255 ),    NOTE_AS_5(  92 ),  NOTE_SILENT(  73 ),    NOTE_AS_5( 255 ), 
           NOTE_AS_5( 200 ),  NOTE_SILENT(  25 ),    NOTE_AS_6( 255 ),    NOTE_AS_6( 152 ), 
         NOTE_SILENT(  73 ),    NOTE_GS_6( 255 ),    NOTE_GS_6( 200 ),  NOTE_SILENT(  25 ), 
            NOTE_C_6( 255 ),     NOTE_C_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_DS_6( 255 ), 
           NOTE_DS_6( 255 ),    NOTE_DS_6( 255 ),    NOTE_DS_6( 255 ),    NOTE_DS_6( 255 ), 
           NOTE_DS_6(  92 ),  NOTE_SILENT(  73 ),     NOTE_D_6( 255 ),     NOTE_D_6( 255 ), 
            NOTE_D_6( 255 ),     NOTE_D_6( 255 ),     NOTE_D_6( 255 ),     NOTE_D_6(  92 ), 
           NOTE_DS_7( 255 ),    NOTE_DS_7( 255 ),    NOTE_DS_7( 255 ),    NOTE_DS_7( 255 ), 
           NOTE_DS_7( 255 ),    NOTE_DS_7( 255 ),    NOTE_DS_7( 255 ),    NOTE_DS_7( 255 ), 
           NOTE_DS_7( 255 ),    NOTE_DS_7( 255 ),    NOTE_DS_7( 185 ),  NOTE_SILENT( 145 ), 
           NOTE_AS_6( 255 ),    NOTE_AS_6( 255 ),    NOTE_AS_6(  52 ),  NOTE_SILENT(  13 ), 
           NOTE_DS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_DS_7( 255 ),    NOTE_DS_7( 255 ), 
           NOTE_DS_7( 255 ),    NOTE_DS_7( 146 ),  NOTE_SILENT( 255 ),  NOTE_SILENT( 255 ), 
         NOTE_SILENT( 255 ),  NOTE_SILENT( 255 ),    NOTE_AS_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_CS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_7( 255 ),    NOTE_CS_7( 255 ), 
           NOTE_CS_7( 255 ),    NOTE_CS_7( 146 ),  NOTE_SILENT( 255 ),  NOTE_SILENT( 255 ), 
         NOTE_SILENT( 255 ),  NOTE_SILENT( 255 ),     NOTE_C_7( 255 ),     NOTE_C_7( 200 ), 
         NOTE_SILENT(  25 ),    NOTE_GS_6( 255 ),    NOTE_GS_6( 255 ),    NOTE_GS_6( 255 ), 
           NOTE_GS_6( 146 ),  NOTE_SILENT( 255 ),  NOTE_SILENT( 255 ),  NOTE_SILENT( 255 ), 
         NOTE_SILENT( 255 ),    NOTE_DS_6( 255 ),    NOTE_DS_6( 255 ),    NOTE_DS_6( 255 ), 
           NOTE_DS_6( 255 ),    NOTE_DS_6( 255 ),    NOTE_DS_6(  92 ),  NOTE_SILENT(  73 ), 
           NOTE_AS_5( 255 ),    NOTE_AS_5( 200 ),  NOTE_SILENT(  25 ),     NOTE_F_6( 255 ), 
            NOTE_F_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_DS_6( 255 ),    NOTE_DS_6( 200 ), 
         NOTE_SILENT(  25 ),     NOTE_G_6( 255 ),     NOTE_G_6( 255 ),     NOTE_G_6( 255 ), 
            NOTE_G_6( 146 ),  NOTE_SILENT(  49 ),     NOTE_G_6( 227 ),  NOTE_SILENT(  13 ), 
            NOTE_G_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_GS_6( 255 ),    NOTE_GS_6( 200 ), 
         NOTE_SILENT(  25 ),     NOTE_G_6( 255 ),     NOTE_G_6( 200 ),  NOTE_SILENT(  25 ), 
            NOTE_F_6( 255 ),     NOTE_F_6( 200 ),  NOTE_SILENT(  25 ),     NOTE_G_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_C_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_DS_6( 255 ), 
           NOTE_DS_6( 255 ),    NOTE_DS_6( 173 ),  NOTE_SILENT(  37 ),     NOTE_G_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_GS_6( 255 ),    NOTE_GS_6( 200 ),  NOTE_SILENT(  25 ), 
            NOTE_C_6( 255 ),     NOTE_C_6( 200 ),  NOTE_SILENT(  25 ),     NOTE_D_6( 255 ), 
            NOTE_D_6( 200 ),  NOTE_SILENT( 255 ),  NOTE_SILENT( 250 ),     NOTE_G_6( 255 ), 
            NOTE_G_6( 255 ),     NOTE_G_6( 173 ),  NOTE_SILENT(  37 ),     NOTE_G_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_C_7( 255 ),     NOTE_C_7( 200 ),  NOTE_SILENT(  25 ), 
           NOTE_AS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_AS_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_DS_6( 255 ),    NOTE_DS_6( 200 ),  NOTE_SILENT(  25 ),     NOTE_G_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_C_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_DS_6( 255 ), 
           NOTE_DS_6( 255 ),    NOTE_DS_6( 173 ),  NOTE_SILENT(  37 ),     NOTE_G_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_GS_6( 255 ),    NOTE_GS_6( 200 ),  NOTE_SILENT(  25 ), 
            NOTE_C_6( 255 ),     NOTE_C_6( 200 ),  NOTE_SILENT(  25 ),     NOTE_D_6( 255 ), 
            NOTE_D_6( 200 ),  NOTE_SILENT( 255 ),  NOTE_SILENT( 250 ),    NOTE_AS_6( 255 ), 
           NOTE_AS_6( 255 ),    NOTE_AS_6( 255 ),    NOTE_AS_6( 255 ),    NOTE_AS_6( 255 ), 
           NOTE_AS_6(  92 ),  NOTE_SILENT(  73 ),     NOTE_D_7( 255 ),     NOTE_D_7( 200 ), 
         NOTE_SILENT(  25 ),    NOTE_AS_6( 255 ),    NOTE_AS_6( 200 ),  NOTE_SILENT(  25 ), 
           NOTE_AS_6( 255 ),    NOTE_AS_6( 255 ),    NOTE_AS_6( 255 ),    NOTE_AS_6( 146 ), 
         NOTE_SILENT( 255 ),  NOTE_SILENT(  34 ),    NOTE_AS_6( 227 ),  NOTE_SILENT(  13 ), 
            NOTE_G_6( 255 ),     NOTE_G_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_GS_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_GS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_AS_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_AS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_AS_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_DS_6( 255 ),    NOTE_DS_6( 200 ),  NOTE_SILENT(  25 ), 
           NOTE_AS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_AS_6( 227 ),  NOTE_SILENT(  13 ), 
            NOTE_C_7( 227 ),  NOTE_SILENT(  13 ),    NOTE_AS_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_DS_6( 255 ),    NOTE_DS_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_AS_5( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_F_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_DS_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_F_6( 255 ),     NOTE_F_6( 200 ),  NOTE_SILENT(  25 ), 
            NOTE_G_6( 255 ),     NOTE_G_6( 255 ),     NOTE_G_6( 255 ),     NOTE_G_6( 146 ), 
         NOTE_SILENT(  49 ),     NOTE_G_6( 255 ),     NOTE_G_6( 200 ),  NOTE_SILENT(  25 ), 
           NOTE_GS_6( 255 ),    NOTE_GS_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_AS_6( 255 ), 
           NOTE_AS_6( 200 ),  NOTE_SILENT( 255 ),  NOTE_SILENT( 250 ),    NOTE_DS_7( 255 ), 
           NOTE_DS_7( 255 ),    NOTE_DS_7( 255 ),    NOTE_DS_7( 255 ),    NOTE_DS_7( 255 ), 
           NOTE_DS_7(  92 ),  NOTE_SILENT(  73 ),     NOTE_F_7( 255 ),     NOTE_F_7( 200 ), 
         NOTE_SILENT(  25 ),     NOTE_D_7( 255 ),     NOTE_D_7( 200 ),  NOTE_SILENT(  25 ), 
           NOTE_AS_6( 255 ),    NOTE_AS_6( 255 ),    NOTE_AS_6( 255 ),    NOTE_AS_6( 146 ), 
         NOTE_SILENT(  49 ),     NOTE_F_7( 227 ),  NOTE_SILENT(  13 ),     NOTE_F_7( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_F_7( 255 ),     NOTE_F_7( 200 ),  NOTE_SILENT(  25 ), 
           NOTE_DS_7( 255 ),    NOTE_DS_7( 200 ),  NOTE_SILENT( 255 ),  NOTE_SILENT(  10 ), 
            NOTE_G_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_GS_6( 255 ),    NOTE_GS_6( 255 ), 
           NOTE_GS_6( 173 ),  NOTE_SILENT(  37 ),    NOTE_GS_6( 255 ),    NOTE_GS_6( 255 ), 
           NOTE_GS_6( 173 ),  NOTE_SILENT(  37 ),    NOTE_DS_6( 255 ),    NOTE_DS_6( 255 ), 
           NOTE_DS_6( 255 ),    NOTE_DS_6( 146 ),  NOTE_SILENT( 255 ),  NOTE_SILENT(  34 ), 
           NOTE_DS_6( 255 ),    NOTE_DS_6( 255 ),    NOTE_DS_6( 255 ),    NOTE_DS_6( 255 ), 
           NOTE_DS_6( 118 ),  NOTE_SILENT(  49 ),    NOTE_DS_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_DS_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_D_7( 255 ),     NOTE_D_7( 255 ), 
            NOTE_D_7( 255 ),     NOTE_D_7( 255 ),     NOTE_D_7( 255 ),     NOTE_D_7( 255 ), 
            NOTE_D_7( 255 ),     NOTE_D_7( 255 ),     NOTE_D_7( 255 ),     NOTE_D_7( 255 ), 
            NOTE_D_7( 185 ),  NOTE_SILENT( 255 ),  NOTE_SILENT( 255 ),  NOTE_SILENT( 255 ), 
         NOTE_SILENT( 255 ),    NOTE_AS_5( 227 ),  NOTE_SILENT(  13 ),    NOTE_DS_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_F_6( 255 ),     NOTE_F_6( 255 ),     NOTE_F_6( 255 ), 
            NOTE_F_6( 146 ),  NOTE_SILENT(  49 ),    NOTE_DS_6( 255 ),    NOTE_DS_6( 200 ), 
         NOTE_SILENT(  25 ),     NOTE_G_6( 255 ),     NOTE_G_6( 255 ),     NOTE_G_6( 255 ), 
            NOTE_G_6( 146 ),  NOTE_SILENT( 255 ),  NOTE_SILENT( 255 ),  NOTE_SILENT(  19 ), 
           NOTE_GS_6( 255 ),    NOTE_GS_6( 200 ),  NOTE_SILENT(  25 ),     NOTE_G_6( 255 ), 
            NOTE_G_6( 200 ),  NOTE_SILENT(  25 ),     NOTE_F_6( 255 ),     NOTE_F_6( 200 ), 
            NOTE_G_6( 255 ),     NOTE_G_6( 255 ),     NOTE_G_6( 255 ),     NOTE_G_6( 255 ), 
            NOTE_G_6( 255 ),     NOTE_G_6(  92 ),  NOTE_SILENT(  73 ),     NOTE_G_6( 255 ), 
            NOTE_G_6( 255 ),     NOTE_G_6( 255 ),     NOTE_G_6( 146 ),  NOTE_SILENT(  49 ), 
            NOTE_G_6( 255 ),     NOTE_G_6( 200 ),  NOTE_SILENT(  25 ),     NOTE_C_7( 255 ), 
            NOTE_C_7( 255 ),     NOTE_C_7( 255 ),     NOTE_C_7( 146 ),  NOTE_SILENT(  49 ), 
            NOTE_G_6( 255 ),     NOTE_G_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_AS_6( 255 ), 
           NOTE_AS_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_GS_6( 255 ),    NOTE_GS_6( 200 ), 
         NOTE_SILENT(  25 ),     NOTE_G_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_G_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_G_6( 255 ),     NOTE_G_6( 255 ),     NOTE_G_6( 255 ), 
            NOTE_G_6( 255 ),     NOTE_G_6( 255 ),     NOTE_G_6( 255 ),     NOTE_G_6( 255 ), 
            NOTE_G_6( 255 ),     NOTE_G_6( 239 ),  NOTE_SILENT( 121 ),     NOTE_G_6( 255 ), 
            NOTE_G_6( 200 ),  NOTE_SILENT(  25 ),     NOTE_F_6( 255 ),     NOTE_F_6( 255 ), 
            NOTE_F_6( 255 ),     NOTE_F_6( 255 ),     NOTE_F_6( 255 ),     NOTE_F_6( 255 ), 
            NOTE_F_6( 255 ),     NOTE_F_6( 255 ),     NOTE_F_6( 255 ),     NOTE_F_6( 255 ), 
            NOTE_F_6( 185 ),   NOTE_SILENT( 255 ),  NOTE_SILENT( 255 ),  NOTE_SILENT( 255 ), 
         NOTE_SILENT( 255 ),    NOTE_DS_6( 255 ),    NOTE_DS_6( 200 ),  NOTE_SILENT(  25 ), 
           NOTE_DS_6( 255 ),    NOTE_DS_6( 255 ),    NOTE_DS_6( 255 ),    NOTE_DS_6( 255 ), 
           NOTE_DS_6( 255 ),    NOTE_DS_6( 255 ),    NOTE_DS_6( 255 ),    NOTE_DS_6(  38 ),  
};
static const uint16_t Melody1_Length = sizeof(Melody1) / sizeof(uint16_t);

//Track 2
static const uint16_t Melody2[] PROGMEM = {
           NOTE_AS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_7( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_AS_6( 113 ),    NOTE_GS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_FS_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_DS_6( 113 ),  NOTE_SILENT(   7 ),    NOTE_FS_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_GS_6( 255 ),    NOTE_GS_6(  86 ),  NOTE_SILENT( 255 ), 
         NOTE_SILENT(   4 ),    NOTE_AS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_7( 113 ), 
         NOTE_SILENT(   7 ),    NOTE_AS_6( 113 ),  NOTE_SILENT(   7 ),    NOTE_GS_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_6( 113 ), 
         NOTE_SILENT(   7 ),    NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_FS_6( 255 ), 
           NOTE_FS_6(  86 ),  NOTE_SILENT( 255 ),  NOTE_SILENT(   4 ),    NOTE_AS_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_CS_7( 113 ),  NOTE_SILENT(   7 ),    NOTE_AS_6( 113 ), 
         NOTE_SILENT(   7 ),    NOTE_GS_6( 255 ),    NOTE_GS_6( 200 ),  NOTE_SILENT(  25 ), 
           NOTE_AS_6( 113 ),  NOTE_SILENT(   7 ),    NOTE_CS_7( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_CS_7( 255 ),    NOTE_CS_7(  86 ),  NOTE_SILENT(  19 ),    NOTE_DS_7( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_CS_7( 113 ),  NOTE_SILENT(   7 ),     NOTE_B_6( 113 ), 
         NOTE_SILENT(   7 ),    NOTE_AS_6( 255 ),    NOTE_AS_6( 200 ),  NOTE_SILENT( 255 ), 
         NOTE_SILENT(  10 ),     NOTE_F_5( 255 ),     NOTE_F_5(  86 ),  NOTE_SILENT(  19 ), 
           NOTE_FS_5( 255 ),    NOTE_FS_5(  86 ),  NOTE_SILENT( 255 ),  NOTE_SILENT(   4 ), 
           NOTE_AS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_7( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_AS_6( 113 ),  NOTE_SILENT(   7 ),    NOTE_GS_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_DS_6( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_GS_6( 255 ),    NOTE_GS_6(  86 ), 
         NOTE_SILENT( 255 ),  NOTE_SILENT(   4 ),    NOTE_AS_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_CS_7( 113 ),  NOTE_SILENT(   7 ),    NOTE_AS_6( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_GS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_CS_6( 113 ),  NOTE_SILENT(   7 ),    NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_FS_6( 255 ),    NOTE_FS_6(  86 ),  NOTE_SILENT( 255 ),  NOTE_SILENT(   4 ), 
           NOTE_FS_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_F_6( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_DS_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_F_6( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_DS_6( 255 ),    NOTE_DS_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_GS_6( 255 ), 
           NOTE_GS_6( 200 ),  NOTE_SILENT(  25 ),     NOTE_F_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_DS_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_F_6( 227 ),  NOTE_SILENT(  13 ), 
            NOTE_F_6( 255 ),     NOTE_F_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_FS_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_B_4( 255 ),     NOTE_B_4(  86 ),  NOTE_SILENT(  19 ), 
           NOTE_AS_4( 255 ),    NOTE_AS_4(  86 ),  NOTE_SILENT( 255 ),  NOTE_SILENT( 255 ), 
         NOTE_SILENT( 255 ),  NOTE_SILENT( 214 ),    NOTE_CS_7( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_AS_6( 113 ),  NOTE_SILENT(   7 ),    NOTE_GS_6( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_FS_6( 113 ),  NOTE_SILENT(   7 ),    NOTE_GS_6( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_GS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_8( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_AS_7( 113 ),  NOTE_SILENT(   7 ),    NOTE_GS_7( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_FS_7( 113 ),  NOTE_SILENT(   7 ),    NOTE_GS_7( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_GS_7( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_7( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_AS_6( 113 ),  NOTE_SILENT(   7 ),    NOTE_GS_6( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_FS_6( 113 ),  NOTE_SILENT(   7 ),    NOTE_GS_6( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_GS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_8( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_AS_7( 113 ),  NOTE_SILENT(   7 ),    NOTE_GS_7( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_FS_7( 113 ),  NOTE_SILENT(   7 ),    NOTE_GS_7( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_GS_7( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_7( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_AS_6( 113 ),  NOTE_SILENT(   7 ),    NOTE_GS_6( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_FS_6( 113 ),  NOTE_SILENT(   7 ),    NOTE_GS_6( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_GS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_8( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_AS_7( 113 ),  NOTE_SILENT(   7 ),    NOTE_GS_7( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_FS_7( 113 ),  NOTE_SILENT(   7 ),    NOTE_GS_7( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_GS_7( 227 ),  NOTE_SILENT(  13 ),    NOTE_DS_7( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_CS_7( 113 ),  NOTE_SILENT(   7 ),     NOTE_B_6( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_AS_6( 113 ),  NOTE_SILENT(   7 ),    NOTE_CS_7( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_CS_7( 227 ),  NOTE_SILENT(  13 ),    NOTE_DS_8( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_CS_8( 113 ),  NOTE_SILENT(   7 ),     NOTE_B_7( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_AS_7( 113 ),  NOTE_SILENT(   7 ),    NOTE_CS_8( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_CS_8( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_7( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_AS_6( 113 ),  NOTE_SILENT(   7 ),    NOTE_GS_6( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_FS_6( 113 ),  NOTE_SILENT(   7 ),    NOTE_GS_6( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_GS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_8( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_AS_7( 113 ),  NOTE_SILENT(   7 ),    NOTE_GS_7( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_FS_7( 113 ),  NOTE_SILENT(   7 ),    NOTE_GS_7( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_GS_7( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_7( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_AS_6( 113 ),  NOTE_SILENT(   7 ),    NOTE_GS_6( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_FS_6( 113 ),  NOTE_SILENT(   7 ),    NOTE_GS_6( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_GS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_8( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_AS_7( 113 ),  NOTE_SILENT(   7 ),    NOTE_GS_7( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_FS_7( 113 ),  NOTE_SILENT(   7 ),    NOTE_GS_7( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_GS_7( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_7( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_AS_6( 113 ),  NOTE_SILENT(   7 ),    NOTE_GS_6( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_FS_6( 113 ),  NOTE_SILENT(   7 ),    NOTE_GS_6( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_GS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_FS_6( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_GS_6( 113 ),  NOTE_SILENT(   7 ),    NOTE_AS_6( 227 ),  NOTE_SILENT(  13 ), 
            NOTE_B_6( 215 ),    NOTE_GS_6( 255 ),    NOTE_GS_6( 175 ),  NOTE_SILENT(  13 ), 
           NOTE_AS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_7( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_GS_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_AS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_7( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_GS_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_AS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_7( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_DS_7( 227 ),  NOTE_SILENT(  13 ),    NOTE_GS_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_AS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_DS_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_GS_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_AS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_7( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_GS_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_AS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_7( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_GS_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_AS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_7( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_DS_7( 227 ),  NOTE_SILENT(  13 ),    NOTE_GS_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_AS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_AS_5( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_CS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_DS_6( 255 ),    NOTE_DS_6( 200 ), 
         NOTE_SILENT(  25 ),    NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_GS_6( 255 ), 
           NOTE_GS_6( 200 ),  NOTE_SILENT(  25 ),     NOTE_F_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_CS_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_B_5( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_AS_5( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_6( 227 ),  NOTE_SILENT(  13 ), 
            NOTE_F_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 200 ), 
         NOTE_SILENT( 255 ),  NOTE_SILENT(  10 ),    NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ), 
            NOTE_F_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_DS_6( 255 ),    NOTE_DS_6( 200 ), 
         NOTE_SILENT(  25 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 200 ),  NOTE_SILENT(  25 ), 
            NOTE_F_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_6( 255 ),    NOTE_CS_6( 188 ), 
         NOTE_SILENT(  37 ),    NOTE_CS_6( 255 ),    NOTE_CS_6( 255 ),    NOTE_CS_6( 255 ), 
           NOTE_CS_6( 146 ),  NOTE_SILENT( 255 ),  NOTE_SILENT( 255 ),  NOTE_SILENT( 255 ), 
         NOTE_SILENT(   4 ),    NOTE_AS_5( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_DS_6( 255 ),    NOTE_DS_6( 200 ),  NOTE_SILENT(  25 ), 
           NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_GS_6( 255 ),    NOTE_GS_6( 200 ), 
         NOTE_SILENT(  25 ),     NOTE_F_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_B_5( 227 ),  NOTE_SILENT(  13 ),    NOTE_AS_5( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_CS_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_F_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 200 ),  NOTE_SILENT( 255 ), 
         NOTE_SILENT(  10 ),    NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_F_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_DS_6( 255 ),    NOTE_DS_6( 200 ),  NOTE_SILENT(  25 ), 
           NOTE_FS_6( 255 ),    NOTE_FS_6( 200 ),  NOTE_SILENT(  25 ),     NOTE_F_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_CS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_6( 203 ), 
         NOTE_SILENT(  37 ),    NOTE_DS_6( 255 ),    NOTE_DS_6( 255 ),    NOTE_DS_6( 255 ), 
           NOTE_DS_6( 146 ),  NOTE_SILENT( 255 ),  NOTE_SILENT( 255 ),  NOTE_SILENT( 255 ), 
         NOTE_SILENT(   4 ),    NOTE_DS_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_F_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 173 ), 
         NOTE_SILENT(  37 ),     NOTE_F_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_DS_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_F_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_FS_6( 255 ), 
           NOTE_FS_6( 255 ),    NOTE_FS_6( 173 ),  NOTE_SILENT(  37 ),     NOTE_F_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_DS_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_F_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_F_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_CS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_DS_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_CS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_FS_5( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_FS_5( 227 ),  NOTE_SILENT(  13 ),    NOTE_DS_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_CS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_FS_5( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_GS_5( 227 ),  NOTE_SILENT(  13 ),    NOTE_GS_5( 255 ), 
           NOTE_GS_5( 200 ),  NOTE_SILENT(  25 ),    NOTE_AS_5( 255 ),    NOTE_AS_5( 200 ), 
         NOTE_SILENT( 255 ),  NOTE_SILENT(  10 ),    NOTE_AS_6(  42 ),    NOTE_DS_7(  42 ), 
           NOTE_FS_7(  42 ),    NOTE_AS_7( 113 ),  NOTE_SILENT( 241 ),    NOTE_DS_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_F_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_FS_6( 255 ), 
           NOTE_FS_6( 255 ),    NOTE_FS_6( 173 ),  NOTE_SILENT(  37 ),     NOTE_F_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_DS_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_F_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 173 ), 
         NOTE_SILENT(  37 ),     NOTE_F_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_DS_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_F_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_FS_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_GS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_AS_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_B_6( 215 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 199 ), 
           NOTE_AS_6( 227 ),    NOTE_GS_6( 227 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 255 ), 
           NOTE_FS_6( 172 ),    NOTE_CS_6( 255 ),    NOTE_CS_6( 200 ),  NOTE_SILENT(  25 ), 
           NOTE_CS_5(  56 ),  NOTE_SILENT(   4 ),    NOTE_FS_5(  56 ),  NOTE_SILENT(   4 ), 
           NOTE_GS_5(  56 ),  NOTE_SILENT(   4 ),    NOTE_CS_6(  56 ),  NOTE_SILENT(   4 ), 
           NOTE_FS_6(  44 ),  NOTE_SILENT(   4 ),    NOTE_GS_6(  44 ),  NOTE_SILENT(   4 ), 
           NOTE_CS_7(  44 ),  NOTE_SILENT(   4 ),    NOTE_FS_7(  44 ),  NOTE_SILENT(   4 ), 
           NOTE_GS_7(  44 ),  NOTE_SILENT(   4 ),    NOTE_CS_8( 227 ),    NOTE_AS_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_CS_7( 113 ),  NOTE_SILENT(   7 ),    NOTE_AS_6( 113 ), 
           NOTE_GS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_DS_6( 113 ),  NOTE_SILENT(   7 ),    NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_GS_6( 255 ),    NOTE_GS_6(  86 ),  NOTE_SILENT( 255 ),  NOTE_SILENT(   4 ), 
           NOTE_AS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_7( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_AS_6( 113 ),  NOTE_SILENT(   7 ),    NOTE_GS_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_6( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_FS_6( 255 ),    NOTE_FS_6(  86 ), 
         NOTE_SILENT( 255 ),  NOTE_SILENT(   4 ),    NOTE_AS_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_CS_7( 113 ),  NOTE_SILENT(   7 ),    NOTE_AS_6( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_GS_6( 255 ),    NOTE_GS_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_AS_6( 113 ), 
         NOTE_SILENT(   7 ),    NOTE_CS_7( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_7( 255 ), 
           NOTE_CS_7(  86 ),  NOTE_SILENT(  19 ),    NOTE_DS_7( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_CS_7( 113 ),  NOTE_SILENT(   7 ),     NOTE_B_6( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_AS_6( 255 ),    NOTE_AS_6( 200 ),  NOTE_SILENT( 255 ),  NOTE_SILENT( 255 ), 
         NOTE_SILENT( 235 ),    NOTE_FS_5( 255 ),    NOTE_FS_5( 200 ),  NOTE_SILENT(  25 ), 
           NOTE_AS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_CS_7( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_AS_6( 113 ),  NOTE_SILENT(   7 ),    NOTE_GS_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_DS_6( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_GS_6( 255 ),    NOTE_GS_6(  86 ), 
         NOTE_SILENT( 255 ),  NOTE_SILENT(   4 ),    NOTE_AS_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_CS_7( 113 ),  NOTE_SILENT(   7 ),    NOTE_AS_6( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_GS_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_CS_6( 113 ),  NOTE_SILENT(   7 ),    NOTE_FS_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_FS_6( 255 ),    NOTE_FS_6(  86 ),  NOTE_SILENT( 255 ),  NOTE_SILENT(   4 ), 
           NOTE_FS_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_F_6( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_DS_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_F_6( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_DS_6( 255 ),    NOTE_DS_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_GS_6( 255 ), 
           NOTE_GS_6( 200 ),  NOTE_SILENT(  25 ),     NOTE_F_6( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_DS_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_F_6( 227 ),  NOTE_SILENT(  13 ), 
            NOTE_F_6( 255 ),     NOTE_F_6( 200 ),  NOTE_SILENT(  25 ),    NOTE_FS_6( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_FS_5(  56 ),  NOTE_SILENT(   4 ),    NOTE_AS_5(  56 ), 
         NOTE_SILENT(   4 ),    NOTE_CS_6(  56 ),  NOTE_SILENT(   4 ),    NOTE_FS_6(  56 ), 
         NOTE_SILENT(   4 ),    NOTE_AS_6(  44 ),  NOTE_SILENT(   4 ),    NOTE_CS_7(  44 ), 
         NOTE_SILENT(   4 ),    NOTE_FS_7(  44 ),  NOTE_SILENT(   4 ),    NOTE_AS_7(  44 ), 
         NOTE_SILENT(   4 ),    NOTE_CS_8(  44 ),  NOTE_SILENT(   4 ),    NOTE_FS_8( 227 ), 
           NOTE_FS_7( 113 ),  NOTE_SILENT(   7 ),     NOTE_F_7( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_DS_7( 113 ),  NOTE_SILENT(   7 ),     NOTE_F_7( 113 ),    NOTE_DS_7( 255 ), 
           NOTE_DS_7( 200 ),    NOTE_AS_7( 255 ),    NOTE_AS_7( 200 ),    NOTE_GS_7( 227 ), 
           NOTE_CS_7( 227 ),     NOTE_F_7( 255 ),     NOTE_F_7( 187 ),    NOTE_FS_7( 255 ), 
           NOTE_FS_7( 164 ), 
};
static const uint16_t Melody2_Length = sizeof(Melody2) / sizeof(uint16_t);

//Track 3
static const uint16_t Melody3[] PROGMEM = {
            NOTE_F_6( 170 ),  NOTE_SILENT(  10 ),     NOTE_G_6( 170 ),  NOTE_SILENT(  10 ), 
            NOTE_A_6( 255 ),     NOTE_A_6(  85 ),  NOTE_SILENT(  13 ),     NOTE_G_6( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_F_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_C_7( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_G_6( 255 ),     NOTE_G_6( 255 ),     NOTE_G_6(  59 ), 
         NOTE_SILENT(  31 ),     NOTE_E_6( 170 ),  NOTE_SILENT(  10 ),     NOTE_F_6( 170 ), 
         NOTE_SILENT(  10 ),     NOTE_G_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_G_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_F_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_F_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_C_7( 227 ),  NOTE_SILENT(  13 ),     NOTE_F_6( 255 ), 
            NOTE_F_6( 255 ),     NOTE_F_6(  59 ),  NOTE_SILENT(  31 ),     NOTE_F_6( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_E_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_C_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_D_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_AS_6( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_A_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_AS_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_C_7( 227 ),  NOTE_SILENT(  13 ),     NOTE_F_6( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_F_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_E_7( 170 ), 
         NOTE_SILENT(  10 ),     NOTE_F_7( 170 ),  NOTE_SILENT(  10 ),     NOTE_G_7( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_G_7( 255 ),     NOTE_G_7(  86 ),  NOTE_SILENT(  19 ), 
            NOTE_F_7( 113 ),  NOTE_SILENT(   7 ),     NOTE_F_7( 255 ),     NOTE_F_7( 255 ), 
            NOTE_F_7( 255 ),     NOTE_F_7( 146 ),  NOTE_SILENT(  49 ),    NOTE_AS_4(  56 ), 
         NOTE_SILENT(   4 ),     NOTE_C_5(  56 ),  NOTE_SILENT(   4 ),     NOTE_D_5(  56 ), 
         NOTE_SILENT(   4 ),     NOTE_E_5(  56 ),  NOTE_SILENT(   4 ),     NOTE_F_5(  56 ), 
         NOTE_SILENT(   4 ),     NOTE_G_5(  56 ),  NOTE_SILENT(   4 ),     NOTE_A_5(  56 ), 
         NOTE_SILENT(   4 ),    NOTE_AS_5(  56 ),  NOTE_SILENT(   4 ),     NOTE_C_6( 255 ), 
            NOTE_C_6( 200 ),  NOTE_SILENT( 145 ),     NOTE_C_6( 119 ),  NOTE_SILENT( 121 ), 
            NOTE_F_6( 255 ),     NOTE_F_6( 255 ),     NOTE_F_6(  59 ),  NOTE_SILENT(  31 ), 
           NOTE_AS_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_A_6( 113 ),  NOTE_SILENT(   7 ), 
            NOTE_A_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_F_6( 113 ),  NOTE_SILENT(   7 ), 
            NOTE_G_6( 255 ),     NOTE_G_6( 200 ),  NOTE_SILENT(  25 ),     NOTE_A_5(  56 ), 
         NOTE_SILENT(   4 ),    NOTE_AS_5(  56 ),  NOTE_SILENT(   4 ),     NOTE_C_6(  56 ), 
         NOTE_SILENT(   4 ),     NOTE_D_6(  56 ),  NOTE_SILENT(   4 ),     NOTE_E_6(  56 ), 
         NOTE_SILENT(   4 ),     NOTE_F_6(  56 ),  NOTE_SILENT(   4 ),     NOTE_G_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_C_7( 113 ),  NOTE_SILENT(   7 ),     NOTE_F_7( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_G_7(  89 ),  NOTE_SILENT(  31 ),     NOTE_C_7(  45 ), 
            NOTE_C_8(  74 ),     NOTE_F_5( 113 ),  NOTE_SILENT(   7 ),     NOTE_E_5( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_C_5(  96 ),  NOTE_SILENT(  24 ),     NOTE_C_6( 255 ), 
            NOTE_C_6(  86 ),  NOTE_SILENT(  19 ),     NOTE_C_6( 113 ),  NOTE_SILENT(   7 ), 
            NOTE_C_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_F_6( 227 ),  NOTE_SILENT(  13 ), 
            NOTE_G_6( 255 ),     NOTE_G_6(  86 ),  NOTE_SILENT(  19 ),     NOTE_F_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_F_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_G_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_A_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_AS_6( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_A_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_C_6( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_C_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_A_7(  45 ), 
           NOTE_AS_7(  69 ),  NOTE_SILENT(   5 ),     NOTE_A_7( 113 ),  NOTE_SILENT(   7 ), 
            NOTE_F_7( 113 ),  NOTE_SILENT(   7 ),     NOTE_C_7( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_AS_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_A_6( 113 ),  NOTE_SILENT(   7 ), 
            NOTE_C_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_C_6( 107 ),  NOTE_SILENT(  13 ), 
            NOTE_A_6( 255 ),     NOTE_A_6(  86 ),  NOTE_SILENT(  19 ),     NOTE_G_6( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_G_6( 255 ),     NOTE_G_6( 200 ),  NOTE_SILENT(  25 ), 
           NOTE_AS_6( 255 ),    NOTE_AS_6(  86 ),  NOTE_SILENT(  19 ),     NOTE_A_6( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_F_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_F_6( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_F_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_F_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_D_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_D_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_F_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_A_6( 255 ), 
            NOTE_A_6(  86 ),  NOTE_SILENT(  19 ),    NOTE_AS_6( 227 ),  NOTE_SILENT(  13 ), 
            NOTE_A_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_G_6( 113 ),  NOTE_SILENT(   7 ), 
            NOTE_F_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_F_6( 255 ),     NOTE_F_6( 200 ), 
         NOTE_SILENT(  25 ),     NOTE_F_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_E_6( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_F_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_G_6( 255 ), 
            NOTE_G_6(  86 ),  NOTE_SILENT(  19 ),     NOTE_G_6( 227 ),  NOTE_SILENT(  13 ), 
            NOTE_C_7( 170 ),  NOTE_SILENT(  10 ),     NOTE_A_6( 170 ),  NOTE_SILENT(  10 ), 
            NOTE_G_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_F_6( 255 ),     NOTE_F_6( 200 ), 
         NOTE_SILENT( 145 ),     NOTE_F_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_A_6( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_G_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_G_5( 113 ), 
         NOTE_SILENT(   7 ),    NOTE_CS_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_E_6( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_G_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_E_6( 113 ), 
         NOTE_SILENT(   7 ),    NOTE_CS_6( 113 ),  NOTE_SILENT(   7 ),    NOTE_AS_6( 107 ), 
         NOTE_SILENT(  13 ),     NOTE_A_6( 255 ),     NOTE_A_6(  86 ),  NOTE_SILENT(  19 ), 
            NOTE_G_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_G_6( 255 ),     NOTE_G_6( 200 ), 
         NOTE_SILENT(  25 ),    NOTE_AS_6( 255 ),    NOTE_AS_6(  86 ),  NOTE_SILENT(  19 ), 
            NOTE_A_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_F_6( 227 ),  NOTE_SILENT(  13 ), 
            NOTE_F_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_F_6( 113 ),  NOTE_SILENT(   7 ), 
            NOTE_F_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_D_6( 113 ),  NOTE_SILENT(   7 ), 
            NOTE_D_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_F_6( 227 ),  NOTE_SILENT(  13 ), 
            NOTE_A_6( 255 ),     NOTE_A_6(  86 ),  NOTE_SILENT(  19 ),    NOTE_AS_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_A_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_G_6( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_F_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_F_6( 179 ), 
         NOTE_SILENT( 181 ),     NOTE_F_6( 255 ),     NOTE_F_6(  86 ),  NOTE_SILENT(  19 ), 
            NOTE_F_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_E_6( 227 ),  NOTE_SILENT(  13 ), 
            NOTE_F_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_G_6( 255 ),     NOTE_G_6(  86 ), 
         NOTE_SILENT(  19 ),     NOTE_C_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_C_6( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_C_7( 179 ),  NOTE_SILENT( 181 ),     NOTE_G_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_G_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_G_6( 113 ), 
         NOTE_SILENT(   7 ),    NOTE_FS_6( 173 ),  NOTE_SILENT(  67 ),     NOTE_G_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_A_6( 255 ),     NOTE_A_6( 255 ),     NOTE_A_6(  59 ), 
         NOTE_SILENT(  31 ),    NOTE_AS_6( 170 ),  NOTE_SILENT(  10 ),     NOTE_A_6( 170 ), 
         NOTE_SILENT(  10 ),     NOTE_F_6( 113 ),  NOTE_SILENT(   7 ),    NOTE_AS_6( 170 ), 
         NOTE_SILENT(  10 ),     NOTE_A_6( 170 ),  NOTE_SILENT(  10 ),     NOTE_F_6( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_A_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_F_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_F_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_A_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_A_6( 240 ),     NOTE_C_5(  56 ),  NOTE_SILENT(   4 ), 
            NOTE_F_5(  56 ),  NOTE_SILENT(   4 ),     NOTE_G_5(  56 ),  NOTE_SILENT(   4 ), 
            NOTE_C_6(  56 ),  NOTE_SILENT(   4 ),     NOTE_F_6(  56 ),  NOTE_SILENT(   4 ), 
            NOTE_G_6(  56 ),  NOTE_SILENT(   4 ),     NOTE_C_7(  56 ),  NOTE_SILENT(   4 ), 
            NOTE_F_7(  56 ),  NOTE_SILENT(   4 ),     NOTE_G_7(  56 ),  NOTE_SILENT(   4 ), 
            NOTE_C_8(  56 ),  NOTE_SILENT(   4 ),     NOTE_G_7(  56 ),  NOTE_SILENT(   4 ), 
            NOTE_F_7(  56 ),  NOTE_SILENT(   4 ),     NOTE_C_7(  56 ),  NOTE_SILENT(   4 ), 
            NOTE_G_6(  56 ),  NOTE_SILENT(   4 ),     NOTE_F_6(  56 ),  NOTE_SILENT(   4 ), 
            NOTE_C_6(  56 ),  NOTE_SILENT(   4 ),     NOTE_G_6(  56 ),  NOTE_SILENT(   4 ), 
            NOTE_F_6(  56 ),  NOTE_SILENT(   4 ),     NOTE_C_6(  47 ),  NOTE_SILENT(  13 ), 
            NOTE_G_5(  56 ),  NOTE_SILENT(   4 ),     NOTE_F_5( 120 ),     NOTE_G_7( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_F_7( 113 ),  NOTE_SILENT(   7 ),     NOTE_G_7(  95 ), 
         NOTE_SILENT( 255 ),  NOTE_SILENT(  10 ),     NOTE_C_6(  56 ),  NOTE_SILENT(   4 ), 
            NOTE_E_6(  56 ),  NOTE_SILENT(   4 ),     NOTE_G_6(  56 ),  NOTE_SILENT(   4 ), 
            NOTE_E_6(  56 ),  NOTE_SILENT(   4 ),     NOTE_G_6(  56 ),  NOTE_SILENT(   4 ), 
            NOTE_C_7(  56 ),  NOTE_SILENT(   4 ),     NOTE_E_7(  56 ),  NOTE_SILENT(   4 ), 
            NOTE_C_7(  56 ),  NOTE_SILENT(   4 ),     NOTE_E_7(  56 ),  NOTE_SILENT(   4 ), 
            NOTE_G_7(  56 ),  NOTE_SILENT(   4 ),     NOTE_A_7( 227 ),  NOTE_SILENT(  13 ), 
            NOTE_A_7( 113 ),  NOTE_SILENT(   7 ),     NOTE_A_7( 113 ),  NOTE_SILENT( 127 ), 
            NOTE_F_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_G_6( 227 ),  NOTE_SILENT(  13 ), 
            NOTE_A_6( 255 ),     NOTE_A_6( 200 ),  NOTE_SILENT(  25 ),     NOTE_F_6( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_F_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_C_7( 227 ), 
         NOTE_SILENT(  13 ),    NOTE_AS_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_A_6( 255 ), 
            NOTE_A_6(  86 ),  NOTE_SILENT(  19 ),     NOTE_F_6( 113 ),  NOTE_SILENT(   7 ), 
            NOTE_F_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_G_6( 227 ),  NOTE_SILENT(  13 ), 
            NOTE_F_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_G_6( 255 ),     NOTE_G_6(  86 ), 
         NOTE_SILENT(  19 ),     NOTE_C_7( 113 ),  NOTE_SILENT(   7 ),     NOTE_C_7( 255 ), 
            NOTE_C_7( 255 ),     NOTE_C_7(  59 ),  NOTE_SILENT(  31 ),     NOTE_E_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_D_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_C_6( 255 ), 
            NOTE_C_6(  86 ),  NOTE_SILENT(  19 ),     NOTE_C_6( 113 ),  NOTE_SILENT(   7 ), 
            NOTE_C_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_C_6( 113 ),  NOTE_SILENT(   7 ), 
           NOTE_AS_6( 255 ),    NOTE_AS_6(  86 ),  NOTE_SILENT(  19 ),     NOTE_A_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_G_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_E_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_G_6( 255 ),     NOTE_G_6(  86 ),  NOTE_SILENT(  19 ), 
            NOTE_F_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_F_6( 227 ),  NOTE_SILENT(  13 ), 
            NOTE_E_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_E_6( 227 ),  NOTE_SILENT(  13 ), 
            NOTE_F_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_F_6( 255 ),     NOTE_F_6(  86 ), 
         NOTE_SILENT(  19 ),     NOTE_C_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_D_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_D_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_D_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_F_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_E_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_F_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_G_6( 255 ), 
            NOTE_G_6(  86 ),  NOTE_SILENT(  19 ),     NOTE_C_6( 113 ),  NOTE_SILENT(   7 ), 
            NOTE_C_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_C_7( 227 ),  NOTE_SILENT(  13 ), 
           NOTE_AS_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_A_6( 227 ),  NOTE_SILENT(  13 ), 
            NOTE_G_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_FS_6( 173 ),  NOTE_SILENT(  67 ), 
            NOTE_G_6(  59 ),  NOTE_SILENT( 181 ),    NOTE_FS_6( 255 ),    NOTE_FS_6( 255 ), 
           NOTE_FS_6(  59 ),  NOTE_SILENT(  31 ),     NOTE_D_6( 170 ),  NOTE_SILENT(  10 ), 
            NOTE_E_6( 170 ),  NOTE_SILENT(  10 ),     NOTE_F_6( 113 ),  NOTE_SILENT(   7 ), 
            NOTE_F_6( 239 ),  NOTE_SILENT( 241 ),     NOTE_E_6( 170 ),  NOTE_SILENT(  10 ), 
            NOTE_F_6( 170 ),  NOTE_SILENT(  10 ),     NOTE_G_6( 113 ),  NOTE_SILENT(   7 ), 
            NOTE_G_6( 119 ),  NOTE_SILENT( 121 ),     NOTE_F_6( 113 ),  NOTE_SILENT(   7 ), 
            NOTE_A_6( 255 ),     NOTE_A_6( 255 ),     NOTE_A_6(  59 ),  NOTE_SILENT( 151 ), 
           NOTE_AS_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_A_6( 227 ),  NOTE_SILENT(  13 ), 
            NOTE_F_6( 255 ),     NOTE_F_6( 200 ),  NOTE_SILENT(  25 ),     NOTE_F_6( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_E_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_C_6( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_D_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_D_6( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_F_5( 113 ),  NOTE_SILENT(   7 ),     NOTE_E_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_E_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_G_5( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_F_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_F_6( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_A_5( 113 ),  NOTE_SILENT(   7 ),     NOTE_G_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_A_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_F_6( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_F_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_C_7( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_D_7( 113 ),  NOTE_SILENT(   7 ),     NOTE_C_7( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_D_7( 227 ),  NOTE_SILENT(  13 ),     NOTE_C_7( 255 ), 
            NOTE_C_7( 255 ),     NOTE_C_7(  59 ),  NOTE_SILENT(  31 ),     NOTE_F_6( 170 ), 
         NOTE_SILENT(  10 ),     NOTE_G_6( 170 ),  NOTE_SILENT(  10 ),     NOTE_A_6( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_A_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_G_6( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_F_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_C_7( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_G_6( 255 ),     NOTE_G_6( 255 ),     NOTE_G_6(  59 ), 
         NOTE_SILENT(  31 ),     NOTE_E_6( 170 ),  NOTE_SILENT(  10 ),     NOTE_F_6( 170 ), 
         NOTE_SILENT(  10 ),     NOTE_G_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_G_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_F_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_F_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_C_7( 227 ),  NOTE_SILENT(  13 ),     NOTE_F_6( 255 ), 
            NOTE_F_6( 255 ),     NOTE_F_6(  59 ),  NOTE_SILENT(  31 ),     NOTE_F_6( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_E_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_C_6( 227 ), 
         NOTE_SILENT(  13 ),     NOTE_D_6( 227 ),  NOTE_SILENT(  13 ),    NOTE_AS_6( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_A_6( 173 ),  NOTE_SILENT(  67 ),    NOTE_AS_6( 119 ), 
         NOTE_SILENT( 121 ),     NOTE_C_7( 227 ),  NOTE_SILENT(  13 ),     NOTE_F_6( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_F_6( 227 ),  NOTE_SILENT(  13 ),     NOTE_E_6( 170 ), 
         NOTE_SILENT(  10 ),     NOTE_F_6( 170 ),  NOTE_SILENT(  10 ),     NOTE_G_6( 113 ), 
         NOTE_SILENT(   7 ),     NOTE_G_6( 255 ),     NOTE_G_6(  86 ),  NOTE_SILENT(  19 ), 
            NOTE_F_6( 113 ),  NOTE_SILENT(   7 ),     NOTE_F_6( 255 ),     NOTE_F_6( 255 ), 
            NOTE_F_6( 255 ),     NOTE_F_6( 255 ),     NOTE_F_6( 255 ),     NOTE_F_6(  92 ), 
};
static const uint16_t Melody3_Length = sizeof(Melody3) / sizeof(uint16_t);

void playMelody_Data(const uint16_t MelodyData[], const uint16_t MelodyLength, const uint8_t tempo)
{
	// 8th Octave Frequencies C8 to B8, lower octaves are calculated from this
	static const uint16_t Freq8[] PROGMEM = { 4186 , 4435 , 4699  , 4978 , 5274 , 5588 , 5920 , 6272 , 6645 , 7040 , 7459 , 7902 };

	for(uint16_t  x = 0; x < MelodyLength; x++)
	{
		if (is_playTone == false)
		{
			break;
		}
    	uint16_t data = pgm_read_word((uint16_t *)&MelodyData[x]);
    	if((data & 0xF) == 0xF) 
    	{
			digitalWrite(13, LOW);
     		noTone(tonePin);
    	}
    	else
    	{
			digitalWrite(13, HIGH);
    		uint16_t Freq = pgm_read_word(&Freq8[data&0xF]) / ( 1 << (8-(data>>4 & 0xF)) );
    		tone(tonePin, Freq);    
    	}   
    	int16_t Duration = data>>8;
    	while(Duration--) delay(tempo);
  	}
}

static void playMelody(uint8_t melodyNumber, uint8_t tempo)
{
	switch(melodyNumber)
	{
		case 0: { playMelody_Data(Melody0, Melody0_Length, tempo); return; }
		case 1: { playMelody_Data(Melody1, Melody1_Length, tempo); return; }
		case 2: { playMelody_Data(Melody2, Melody2_Length, tempo); return; }
		case 3: { playMelody_Data(Melody3, Melody3_Length, tempo); return; }
    	//case 4: { playMelody_Data(Melody4, Melody4_Length, tempo); return; }
    	//case 5: { playMelody_Data(Melody5, Melody5_Length, tempo); return; }
    	//case 6: { playMelody_Data(Melody6, Melody6_Length, tempo); return; }
    	//case 7: { playMelody_Data(Melody7, Melody7_Length, tempo); return; }
    	//case 8: { playMelody_Data(Melody8, Melody8_Length, tempo); return; }
    	//case 9: { playMelody_Data(Melody9, Melody9_Length, tempo); return; }

	}
}