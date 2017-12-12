#include <Arduino.h>
#include <IRremote.h>
#define irPin 9
#define b_B_m 0xF7807F
#define b_B_p 0xF700FF
#define b_OFF 0xF740BF
#define b_ON 0xF7C03F
#define b_R 0xF720DF
#define b_G 0xF7A05F
#define b_B 0xF7609F
#define b_W 0xF7E01F
#define b_c1_r3 0xF710EF
#define b_c2_r3 0xF7906F
#define b_c3_r3 0xF750AF
#define b_c4_r3 0xF7D02F
#define b_c1_r4 0xF730CF
#define b_c2_r4 0xF7B04F
#define b_c3_r4 0xF7708F
#define b_c4_r4 0xF7F00F
#define b_c1_r5 0xF708F7
#define b_c2_r5 0xF78877
#define b_c3_r5 0xF748B7
#define b_c4_r5 0xF7C837
#define b_c1_r6 0xF728D7
#define b_c2_r6 0xF7A857
#define b_c3_r6 0xF76897
#define b_c4_r6 0xF7E817

#define RED 10
#define GREEN 5
#define BLUE 3

void setup();
void loop();

IRrecv irrecv(irPin);
decode_results results;


int PWM[3] = {0, 0, 0};  // red, green, blue
int BRIGHTNESS = 255;
float step_multiplier[3] = {90, 100, 110};
float angle[3] = {0, 0, 0};
float sin_change[3] = {0, 0, 0};

float angle_step = 0.0061419211214;
unsigned long lastSinusTIme = 0;
unsigned long sinusDelay = 5;


bool ON = false;
bool FADE = false;

void setup() {
	Serial.begin(9600);
	pinMode(RED, OUTPUT);
	pinMode(GREEN, OUTPUT);
	pinMode(BLUE, OUTPUT);
	irrecv.enableIRIn();
}

/*
void LED(int vred, int vgreen, int vblue)
{
// analogWrite(, vred);
// analogWrite(GREEN, vgreen);
// analogWrite(BLUE, vblue);
}*/

void LED()
{
	analogWrite(RED, PWM[0]);
	analogWrite(GREEN, PWM[1]);
	analogWrite(BLUE, PWM[2]);
}


void Fade (){
	// int step_multiplier = 1000;
	if (ON == true){
		if ( millis() % sinusDelay == 0)  {
			for (int i=0; i < 3; i++) {
				// round 0-360 angle
				// angle += angle_step*(step_multiplier[i]/250);
				angle[i] += angle_step*(step_multiplier[i]/100);
				if (angle[i] >= 6.2831853072) {
					angle[i] = 0;
				};
				//sin_change[i] = ((sin(angle[i])/2)+0.5);  //  sinus float 0 - 1.000
				sin_change[i] = ((sin(angle[i])/2)+0.5);  //  sinus float 0 - 1.000
				PWM[i] = int(sin_change[i]*BRIGHTNESS);
				// Serial.println(PWM[i]);

			}  // for end
		}
	}

}

void PrintPWM() {
    if ( millis() % 300 == 0) {
        Serial.print("R: ");
        Serial.print(PWM[0]);
		Serial.print("\t");
        Serial.print(step_multiplier[0]);
		Serial.print("\t");
		Serial.print(angle[0]);
		Serial.print("\t");
		Serial.print(sin_change[0]);

        Serial.print("\t G: ");
        Serial.print(PWM[1]);
		Serial.print("\t");
		Serial.print(step_multiplier[1]);
		Serial.print("\t");
		Serial.print(angle[1]);
		Serial.print("\t");
		Serial.print(sin_change[1]);


        Serial.print("\t B: ");
        Serial.print(PWM[2]);
		Serial.print("\t");
		Serial.print(step_multiplier[2]);
		Serial.print("\t");
		Serial.print(angle[2]);
		Serial.print("\t");
		Serial.print(sin_change[2]);


        Serial.print("\n");


        }
    }


void loop() {
	/*
if (irrecv.decode(&results)) {
	Serial.print("0x");
	Serial.println(results.value, HEX);
	delay(250);
	irrecv.resume();
	}
	*/

	///*
	if (irrecv.decode(&results)) {
		switch (results.value) {
			//case 0xF7C03F:
		case b_ON:
			Serial.println("ON");
			//LED(254, 254, 254);
			PWM[0] = 254; PWM[1] = 254; PWM[2] = 254;
			ON = true;
			break;

		case b_OFF:
			Serial.println("OFF");
			//LED(0, 0, 0);
			FADE = false;
			PWM[0] = 0; PWM[1] = 0; PWM[2] = 0;
			ON = false;
			break;

		case b_B_m:
			Serial.println("B-");
			if (BRIGHTNESS > 0){ BRIGHTNESS--;}
			break;

		case b_B_p:
			Serial.println("B+");
			if (BRIGHTNESS < 256){ BRIGHTNESS++;}
			break;


		case b_R:   // red
			if (ON == true) {
				//LED(254, 0, 0);
				FADE = false;
				PWM[0] = 254; PWM[1] = 0; PWM[2] = 0;
			}
			Serial.println("red");
			break;

		case b_G:   // green
			if (ON == true) {
				//LED(0, 254, 0);
				FADE = false;
				PWM[0] = 0; PWM[1] = 254; PWM[2] = 0;
			}
			Serial.println("green");
			break;

		case b_B:   // blue
			if (ON == true) {
				// LED(0, 0, 254);
				FADE = false;
				PWM[0] = 0; PWM[1] = 0; PWM[2] = 254;
			}
			Serial.println("blue");
			break;

		case b_W:   // white
			if (ON == true) {
				// LED(254, 254, 254);
				FADE = false;
				PWM[0] = 254; PWM[1] = 254; PWM[2] = 254;
			}
			Serial.println("white");
			break;

		case b_c4_r5:   // fade
			if (ON == true) {
				// LED(254, 254, 254);
				if (FADE == false) {FADE = true;}
				else {FADE = false;};
			}
			Serial.println("fade");
			break;

		}
		//LED();
		irrecv.resume();
	}
	//*/
	LED();
    // PrintPWM();
	if (FADE == true){Fade();};
}
