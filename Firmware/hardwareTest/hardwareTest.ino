#define BTN1 5
#define BTN1_R 6
#define BTN1_G 9
#define BTN1_B 10
#define BTN2 11
#define BTN2_R 12
#define BTN2_G 13
#define BTN2_B 37
#define BTN3 35
#define BTN3_R 36
#define BTN3_G 8
#define BTN3_B 14
#define BUZZER_P 15
#define BUZZER_N 17

#define BLACK 0
#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define MAGENTA 5
#define CYAN 6
#define WHITE 7

int button1State = BLACK, button2State = BLACK, button3State = BLACK;
unsigned long lastTimeButtonWasPressed = 0;

void IRAM_ATTR buttonWasPressed(){
  if(millis() - lastTimeButtonWasPressed > 100){
    lastTimeButtonWasPressed = millis();
    if(!digitalRead(BTN1)){
      if(button1State == 7){
        button1State = 0;
      }
      else{
        button1State++;
      }
    }
    if(!digitalRead(BTN2)){
      if(button2State == 7){
        button2State = 0;
      }
      else{
        button2State++;
      }
    }
    if(!digitalRead(BTN3)){
      if(button3State == 7){
        button3State = 0;
      }
      else{
        button3State++;
      }
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN1_R, OUTPUT);
  pinMode(BTN1_G, OUTPUT);
  pinMode(BTN1_B, OUTPUT);

  pinMode(BTN2, INPUT_PULLUP);
  pinMode(BTN2_R, OUTPUT);
  pinMode(BTN2_G, OUTPUT);
  pinMode(BTN2_B, OUTPUT);

  pinMode(BTN3, INPUT_PULLUP);
  pinMode(BTN3_R, OUTPUT);
  pinMode(BTN3_G, OUTPUT);
  pinMode(BTN3_B, OUTPUT);

  pinMode(BUZZER_P,OUTPUT);
  pinMode(BUZZER_N,OUTPUT);

  attachInterrupt(BTN1, buttonWasPressed, FALLING);
  attachInterrupt(BTN2, buttonWasPressed, FALLING);
  attachInterrupt(BTN3, buttonWasPressed, FALLING);
}

void loop() {
  cycleLEDs();
  //setLED(1, button1State);
  //setLED(2, button2State);
  //setLED(3, button3State);
}

void setLED(int buttonNumber, int color){
  bool redLEDState = color & RED, greenLEDState = color & GREEN, blueLEDState = color & BLUE;

  switch(buttonNumber){
    case 1:
      digitalWrite(BTN1_R, redLEDState); digitalWrite(BTN1_G, greenLEDState); digitalWrite(BTN1_B, blueLEDState);
      break;
    case 2:
      digitalWrite(BTN2_R, redLEDState); digitalWrite(BTN2_G, greenLEDState); digitalWrite(BTN2_B, blueLEDState);
      break;
    case 3:
      digitalWrite(BTN3_R, redLEDState); digitalWrite(BTN3_G, greenLEDState); digitalWrite(BTN3_B, blueLEDState);
      break;
    default:
      digitalWrite(BTN1_R, LOW); digitalWrite(BTN1_G, LOW); digitalWrite(BTN1_B, LOW);
      digitalWrite(BTN2_R, LOW); digitalWrite(BTN2_G, LOW); digitalWrite(BTN2_B, LOW);
      digitalWrite(BTN3_R, LOW); digitalWrite(BTN3_G, LOW); digitalWrite(BTN3_B, LOW);
      break;
  }
}

void cycleLEDs(){
  for (int i = 0; i < 8; i++) {
    setLED(1, i);
    delay(500);
  }
  for (int i = 0; i < 8; i++) {
    setLED(2, i);
    delay(500);
  }
  for (int i = 0; i < 8; i++) {
    setLED(3, i);
    delay(500);
  }
}