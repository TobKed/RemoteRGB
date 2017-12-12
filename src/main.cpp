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

// PINS
#define RED 10
#define GREEN 5
#define BLUE 3

// FUNCTIONS
void setup();
void loop();
void CatchRemoteSignals();
void LED();
void Fade();
void PrintPWM();


IRrecv irrecv(irPin);
decode_results results;


int PWM[3] = {0, 0, 0};  // red, green, blue
int BRIGHTNESS = 255;
float step_multiplier[3] = {90, 100, 110};
float angle[3] = {0, 0, 0};
float sin_change[3] = {0, 0, 0};


/*
#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
#define EULER 2.718281828459045235360287471352		*/

float deg_90 = HALF_PI;
float deg_180 = PI;
float deg_270 = deg_90+deg_180;
float deg_360 = TWO_PI;
// float angle_step = 0.0061419211214; 	// 0.35190615836°
float angle_step = DEG_TO_RAD/10; 		// 0.1°  (0.001745329252 rad)
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

void loop() {
	CatchRemoteSignals();
	LED();
    PrintPWM();
	if (FADE == true) {Fade(); }
}

void CatchRemoteSignals() {
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
					PWM[0] = 254; PWM[1] = 0; PWM[2] = 0; }
				Serial.println("red");
				break;

			case b_G:   // green
				if (ON == true) {
					//LED(0, 254, 0);
					FADE = false;
					PWM[0] = 0; PWM[1] = 254; PWM[2] = 0; }
				Serial.println("green");
				break;

			case b_B:   // blue
				if (ON == true) {
					// LED(0, 0, 254);
					FADE = false;
					PWM[0] = 0; PWM[1] = 0; PWM[2] = 254; }
				Serial.println("blue");
				break;

			case b_W:   // white
				if (ON == true) {
					// LED(254, 254, 254);
					FADE = false;
					PWM[0] = 254; PWM[1] = 254; PWM[2] = 254; }
				Serial.println("white");
				break;

			case b_c4_r5:   // fade
				if (ON == true) {
					// LED(254, 254, 254);
					if (FADE == false) {FADE = true;}
					else {FADE = false;}; }
				Serial.println("fade");
				break;

			} // end of switch (results.value)
			irrecv.resume();
		} // end of if (irrecv.decode(&results))
}  // end of void CatchRemoteSignals()

////////////////
/* FUNCTIONS" */
////////////////


/*
void LED(int vred, int vgreen, int vblue)
{
// analogWrite(, vred);
// analogWrite(GREEN, vgreen);
// analogWrite(BLUE, vblue);
}*/

void LED() {
	analogWrite(RED, PWM[0]);
	analogWrite(GREEN, PWM[1]);
	analogWrite(BLUE, PWM[2]);
} // end of void LED()

void Fade () {
	// int step_multiplier = 1000;
	if (ON == true) {
		if ( millis() % sinusDelay == 0)  {
			for (int i=0; i < 3; i++) {
				// round 0-360 angle
				// angle += angle_step*(step_multiplier[i]/250);
				angle[i] += angle_step*(step_multiplier[i]/100);
				if (angle[i] >= deg_360) {
					angle[i] = 0; }
				//sin_change[i] = ((sin(angle[i])/2)+0.5);  //  sinus float 0 - 1.000
				sin_change[i] = ((sin(angle[i])/2)+0.5);  //  sinus float 0 - 1.000
				PWM[i] = int(sin_change[i]*BRIGHTNESS);
				// Serial.println(PWM[i]);
				}  // end of if ( millis() % sinusDelay == 0)
			} // end of if ( millis() % sinusDelay == 0)
		} // end of if (ON == true)
	} // end of void Fade ()

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
    } // end of void PrintPWM()
