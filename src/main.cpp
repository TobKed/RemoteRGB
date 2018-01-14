
#include <Arduino.h>
#include <IRremote.h>

#define b_B_m 0xF7807F
#define b_B_p 0xF700FF
#define b_OFF 0xF740BF
#define b_ON 0xF7C03F

#define b_R_1 0xF720DF
#define b_R_2 0xF710EF
#define b_R_3 0xF730CF
#define b_R_4 0xF708F7
#define b_R_5 0xF728D7

#define b_G_1 0xF7A05F
#define b_G_2 0xF7906F
#define b_G_3 0xF7B04F
#define b_G_4 0xF78877
#define b_G_5 0xF7A857

#define b_B_1 0xF7609F
#define b_B_2 0xF750AF
#define b_B_3 0xF7708F
#define b_B_4 0xF748B7
#define b_B_5 0xF76897

#define b_W_1 0xF7E01F
#define b_W_2 0xF7D02F
#define b_W_3 0xF7F00F
#define b_W_4 0xF7C837
#define b_W_5 0xF7E817

/* REMOTE BUTTONS LAYOUT AND CODES
b_Bp	0xF700FF	b_Bm	0xF7807F	b_OFF 	0xF740BF	b_ON	0xF7C03F
b_R_1	0xF720DF	b_G_1	0xF7A05F	b_B_1	0xF7609F	b_W_1	0xF7E01F
b_R_2	0xF710EF	b_G_2	0xF7906F	b_B_2	0xF750AF	b_W_2	0xF7D02F
b_R_3	0xF730CF	b_G_3	0xF7B04F	b_B_3	0xF7708F	b_W_3	0xF7F00F
b_R_4	0xF708F7	b_G_4	0xF78877	b_B_4	0xF748B7	b_W_4	0xF7C837
b_R_5	0xF728D7	b_G_5	0xF7A857	b_B_5	0xF76897	b_W_5	0xF7E817
*/

// PINSC
#define RED 10
#define GREEN 5
#define BLUE 3
#define irPin 9

// FUNCTIONS
void setup();
void loop();
void CatchRemoteSignals();
void LED();
void SetPWM(int colors[3]);
void Fade();
void SmoothChange();
void PrintPWM();

IRrecv irrecv(irPin);
decode_results results;

float deg_90 = HALF_PI;
float deg_180 = PI;
float deg_270 = deg_90 + deg_180;
float deg_360 = TWO_PI;
float angle_step = DEG_TO_RAD / 10;  // 0.1°  (0.001745329252 rad)
unsigned long lastSinusTIme = 0;
unsigned long sinusDelay = 5;

int PWM[3] = {0, 0, 0};  // red, green, blue
int brightness = 255;
float fade_step_multiplier = 300;
float angle[3] = {deg_270, deg_270, deg_270};
float sin_change[3] = {0, 0, 0};
int target_color[3] = {0, 0, 0};
int start_color[3] = {0, 0, 0};
int range_color[3] = {0, 0, 0};
float color_step_multiplier[3] = {300, 300, 300};

// BOOLS
bool ON = false;
bool FADE = false;
bool COLOR_CHANGE = true;
bool COLOR_UP[3] = {true, true, true};

// COLORS
int OFF[3] = {0, 0, 0};
int WHITE[3] = {255, 255, 255};

// row column rgb
int color_array[5][3][3] = {{{255, 0, 0}, {0, 255, 0}, {0, 0, 255}},
							{{255, 51, 0}, {0, 254, 100}, {50, 1, 253}},
							{{254, 103, 0}, {0, 255, 202}, {101, 0, 254}},
							{{254, 153, 0}, {0, 204, 253}, {151, 0, 253}},
							{{255, 204, 0}, {0, 153, 253}, {204, 0, 255}}};

int R_1[3] =  	{color_array[0][0][0], color_array[0][0][1], color_array[0][0][2]}; //{255, 0, 0};
int R_2[3] = 	{color_array[1][0][0], color_array[1][0][1], color_array[1][0][2]}; //{255, 51, 0};
int R_3[3] = 	{color_array[2][0][0], color_array[2][0][1], color_array[2][0][2]}; //{254, 103, 0};
int R_4[3] = 	{color_array[3][0][0], color_array[3][0][1], color_array[3][0][2]}; //{254, 153, 0};
int R_5[3] = 	{color_array[4][0][0], color_array[4][0][1], color_array[4][0][2]}; //{255, 204, 0};

int G_1[3] = 	{color_array[0][1][0], color_array[0][1][1], color_array[0][1][2]}; //{0, 255, 0};
int G_2[3] = 	{color_array[1][1][0], color_array[1][1][1], color_array[1][1][2]}; //{0, 254, 100};
int G_3[3] = 	{color_array[2][1][0], color_array[2][1][1], color_array[2][1][2]}; //{0, 255, 202};
int G_4[3] = 	{color_array[3][1][0], color_array[3][1][1], color_array[3][1][2]}; //{0, 204, 253};
int G_5[3] = 	{color_array[4][1][0], color_array[4][1][1], color_array[4][1][2]}; //{0, 153, 253};

int B_1[3] = 	{color_array[0][2][0], color_array[0][2][1], color_array[0][2][2]}; //{0, 0, 255};
int B_2[3] = 	{color_array[1][2][0], color_array[1][2][1], color_array[1][2][2]}; //{50, 1, 253};
int B_3[3] = 	{color_array[2][2][0], color_array[2][2][1], color_array[2][2][2]}; //{101, 0, 254};
int B_4[3] = 	{color_array[3][2][0], color_array[3][2][1], color_array[3][2][2]}; //{151, 0, 253};
int B_5[3] = 	{color_array[4][2][0], color_array[4][2][1], color_array[4][2][2]}; //{204, 0, 255};

// random color
long rand_row;
long rand_column;
int random_color[3] = {color_array[0][0][0], color_array[0][0][1], color_array[0][0][2]};


void setup() {
  Serial.begin(9600);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  irrecv.enableIRIn();
  randomSeed(analogRead(0));
}

void loop() {
  CatchRemoteSignals();
  SmoothChange();
  LED();
  PrintPWM();
  if (FADE == true) {
    Fade();
  }
}


////////////////
/* FUNCTIONS" */
////////////////

void CatchRemoteSignals() {
  if (irrecv.decode(&results)) {
    switch (results.value) {
      case b_ON:
        ON = true;
        FADE = false;
        SetPWM(WHITE);
        break;

      case b_OFF:
        FADE = false;
        SetPWM(OFF);
        break;

      case b_B_m:
        brightness -= 10;
        if (brightness <= 0) {
          brightness = 0;
        }
        break;

      case b_B_p:
        brightness += 10;
        if (brightness >= 255) {
          brightness = 255;
        }
        break;

      case b_W_1:  // white
        SetPWM(WHITE);
        FADE = false;
        break;

	  case b_W_2:  // fade speed up
	    fade_step_multiplier += 50;
		if (fade_step_multiplier >= 2000) {
	      fade_step_multiplier = 2000;
		}
		break;

	  case b_W_3:  // fade speed down
		fade_step_multiplier -= 50;
		if (fade_step_multiplier <= 100) {
		  fade_step_multiplier = 100;
		}
		break;

      case b_W_4:  // fade
        if (ON == true) {
          if (FADE == false) {
            FADE = true;
          } else {
            FADE = false;
          }
        }
        break;

      case b_R_1:
        SetPWM(R_1);
        FADE = false;
        break;
      case b_R_2:
        SetPWM(R_2);
        FADE = false;
        break;
      case b_R_3:
        SetPWM(R_3);
        FADE = false;
        break;
      case b_R_4:
        SetPWM(R_4);
        FADE = false;
        break;
      case b_R_5:
        SetPWM(R_5);
        FADE = false;
        break;

      case b_G_1:
        SetPWM(G_1);
        FADE = false;
        break;
      case b_G_2:
        SetPWM(G_2);
        FADE = false;
        break;
      case b_G_3:
        SetPWM(G_3);
        FADE = false;
        break;
      case b_G_4:
        SetPWM(G_4);
        FADE = false;
        break;
      case b_G_5:
        SetPWM(G_5);
        FADE = false;
        break;

      case b_B_1:
        SetPWM(B_1);
        FADE = false;
        break;
      case b_B_2:
        SetPWM(B_2);
        FADE = false;
        break;
      case b_B_3:
        SetPWM(B_3);
        FADE = false;
        break;
      case b_B_4:
        SetPWM(B_4);
        FADE = false;
        break;
      case b_B_5:
        SetPWM(B_5);
        FADE = false;
        break;
    }  // end of switch (results.value)

    irrecv.resume();
  }  // end of if (irrecv.decode(&results))
}  // end of void CatchRemoteSignals()

void LED() {
  float brightness_coefficient;
  brightness_coefficient = brightness / 255.0;
  analogWrite(RED, int(PWM[0] * brightness_coefficient));
  analogWrite(GREEN, int(PWM[1] * brightness_coefficient));
  analogWrite(BLUE, int(PWM[2] * brightness_coefficient));
}  // end of void LED()

void SetPWM(int color[3]) {
  if (target_color[0] != color[0] || target_color[1] != color[1] ||
      target_color[2] != color[2]) {
    for (int i = 0; i < 3; i++) {
      target_color[i] = color[i];
      start_color[i] = PWM[i];
      range_color[i] = target_color[i] - start_color[i];
      if (range_color[i] < 0) {
        angle[i] = deg_90;
      } else {
        angle[i] = deg_270;
      }
    }
  }
}  // void SetPWM(int color[3])

void SmoothChange() {
  if (ON == true) {
    if (millis() % sinusDelay == 0) {
      for (int i = 0; i < 3; i++) {
        if (target_color[i] != PWM[i]) {
		  if (FADE == true) {
			  angle[i] += angle_step * (fade_step_multiplier / 100);
		  }
		  else {
			  angle[i] += angle_step * (color_step_multiplier[i] / 100);
		  }
          if (angle[i] >= deg_360) {
            angle[i] = 0;
          }
          sin_change[i] =
              ((sin(angle[i]) / 2) + 0.5);  //  sinus float 0 - 1.000
          int stepper = 0;
          if (range_color[i] < 255) {
            stepper = min(start_color[i], target_color[i]);
          }
          PWM[i] = int(sin_change[i] * abs(range_color[i]) + stepper);
        }  // end if
        else {
          angle[i] = deg_90;
        }
      }  // end of for (int i=0; i < 3; i++)
      if (PWM[0] == 0 && PWM[1] == 0 && PWM[2] == 0 && target_color[0] == 0 &&
          target_color[1] == 0 && target_color[0] == 0) {
        ON = false;
      } else {
        ON = true;
      }
    }  // end of if ( millis() % sinusDelay == 0)
  }    // end of if (ON == true)

  if (target_color[0] != PWM[0] || target_color[1] != PWM[1] ||
      target_color[2] != PWM[2]) {
    COLOR_CHANGE = true;
  } else {
    COLOR_CHANGE = false;
  }
}  // end of void Fade ()

void Fade() {
  if (FADE == true && COLOR_CHANGE == 0) {
    rand_row = random(5);
    rand_column = random(3);
    int random_color[3] = {color_array[rand_row][rand_column][0],
                           color_array[rand_row][rand_column][1],
                           color_array[rand_row][rand_column][2]};
    SetPWM(random_color);
  }
}

void PrintPWM() {
  if (millis() % 200 == 0) {
    Serial.print(PWM[0]);
    Serial.print(" ");
    Serial.print(PWM[1]);
    Serial.print(" ");
    Serial.print(PWM[2]);
    Serial.print("\n");
  }
}  // end of void PrintPWM()
