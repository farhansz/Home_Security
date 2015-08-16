#include "Keypad.h"
#include <LiquidCrystal.h>
#include <Servo.h>
const String password="5555";
volatile boolean isMoving=false;
//Configuration
int LCD_RS=30, LCD_EN=32, D4=22, D5=24, D6=26, D7=28; //-----------------LCD CONFIG-------------------------------------------------------------------------------------------------------
int r0=23, r1=25, r2=27, r3=29, c0=31, c1=33, c2=35; //------------------KEYBOARD CONFIG--------------------------------------------------------------------------------------------------
LiquidCrystal lcd(LCD_RS, LCD_EN, D4, D5, D6, D7);
int tempSensorPin = A7;
int buzzPin = 36;
int redLed = 39;
int greenLed = 37;
int motionLed = 38;
int motionPin = 34;
int gasPin = A6;
int proxiPin=A0;
Servo servo;

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {r0, r1, r2, r3}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {c0, c1, c2}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

//Global variables //-------------------------------GLOBAL VARIABLES---------------------------------------------------------------------------------------------------
String txt = "";
String txt2 = "";
int pinLength;
int sensorValue = 0;
int gasValue=0;

void setup() { //----------------------------SETUP------------------------------------------------------------------------------------------------------------------------------------
  Serial.begin(9600);
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(motionLed, OUTPUT);
  pinMode(buzzPin, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  servo.attach(9);
 
  lcd.begin(16, 2);
  lcd.print("WELCOME");
  delay(1000);
  lcd.clear();
  digitalWrite(redLed, HIGH);
  pinLength = 0;
//  attachInterrupt(0, keyDetect2, CHANGE);
  attachInterrupt(1, motionDetect, CHANGE);
  //attachInterrupt(5, proxiCheck, CHANGE);

  cli();

  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 65535;//15624 = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei();
}

void proxiCheck() {
  if(analogRead(proxiPin)<=800) {
    lcd.setCursor(15,1);
    lcd.print(" ");
    lcd.setCursor(15,1);
    lcd.print("1");
  } else {
    lcd.setCursor(15,1);
    lcd.print(" ");
    lcd.setCursor(15,1);
    lcd.print("0");
  }
}

void keyDetect() {
  char key = keypad.getKey();
  if (key != NO_KEY) {
    lcd.setCursor(0,1);
    lcd.print(key);
    beep();
  }
}

void motionDetect() {
  if (digitalRead(motionPin) == HIGH) {
    isMoving=true;
  } else {
    isMoving=false;
  }
}

//void motionDetect() {
//  lcd.home();
//  lcd.print("       ");
//  if (digitalRead(motionPin) == HIGH) {
//    lcd.setCursor(0,0);
//    lcd.print("MOVING");
//    digitalWrite(motionLed, HIGH);
//  } else {
//    lcd.setCursor(0,0);
//    lcd.print("STATIC");
//    digitalWrite(motionLed, LOW);
//  }
//}

void beep() {
  digitalWrite(buzzPin, HIGH);
  delay(200);
  digitalWrite(buzzPin, LOW);
}
void doubleBeep() {
  beep();
  delay(200);
  beep();
}


boolean chkStr(String str) {
  if (str == password) {
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("              ");
    lcd.setCursor(0, 1);
    lcd.print("PIN: OK!");
    //beep();
    servo.write(180);               
    delay(15);
    return true;
  } else {
    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, LOW);
    //beep();
    return false;
  }
}

void keyDetect2() {
  if (pinLength < 6) {
    char key = keypad.getKey();
    if (key != NO_KEY) {
      if (key == '#') {
        txt = "";
        pinLength = 0;
        lcd.setCursor(0, 1);
        lcd.print("              ");
        lcd.setCursor(0, 1);
        lcd.print("PIN: ");
        beep();
        chkStr(txt);
      } else if (key == '*') {
        if (txt.length() > 0) {
          txt = txt.substring(0, txt.length() - 1);
          int len=txt.length();
          txt2=""; //-------------------------------------------------------------------------
          for(int i=0;i<len; i++) {
            txt2 += "*";
          }
          lcd.setCursor(0, 1);
          lcd.print("               ");
          lcd.setCursor(0, 1);
          lcd.print("PIN: " + txt2);
          pinLength--;
          beep();
          chkStr(txt);
        } else {
          doubleBeep();
        }
      } else {
        txt += key;
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        int len=txt.length();
        txt2=""; //-------------------------------------------------------------------------
        for(int i=0;i<len; i++) {
          txt2 += "*";
        }
        lcd.print("PIN: " + txt2);
        beep();
        pinLength++;
        chkStr(txt);
      }
    }
  } else {
    lcd.setCursor(0, 1);
    lcd.print("               ");
    lcd.setCursor(0, 1);
    lcd.print("PIN: NO WAY!");
    txt = "";
    pinLength = 0;
    servo.write(90);              // tell servo to go to position in variable 'pos' 
    delay(15);
  }
}


ISR(TIMER1_COMPA_vect) {
  otherStuffs();
//  tempMotion();
//  keyDetect();
//  delay(2000);
}

void loop() { //----------------------------LOOP------------------------------------------------------------------------------------------------------------------------------------
  keyDetect2();
}


void otherStuffs() {
  //servo.write(0);
  sensorValue = analogRead(tempSensorPin);
  delay(10);
  sensorValue = analogRead(tempSensorPin);
  sensorValue = sensorValue * 0.48828125;
//  float mv = (sensorValue / 1024.0) * 5000;
//  int cel = (int) mv / 10;
  int cel = sensorValue;
  
  delay(10);
  //gasValue = analogRead(gasPin);
  //delay(10);
  gasValue = analogRead(gasPin);
  ///gasValue =gasValue +690;

   delay(10);
//  Serial.println(gasValue);
 //if (cel <= 50 && gasValue <= 750)
  //{
  if(isMoving) {
    lcd.setCursor(0,0);
    lcd.print(" ");
    lcd.setCursor(0,0);
    lcd.print("M");
    //beep();
    digitalWrite(motionLed, HIGH);
    delay(1000);
    lcd.setCursor(2,0);
    lcd.print("        ");
    lcd.setCursor(2,0);
    lcd.print("T=");
    lcd.print(cel);
    lcd.print("'C");
    lcd.setCursor(10,0);
    lcd.print("      ");
    lcd.setCursor(10,0);
    lcd.print("G=");
    lcd.print(gasValue);
  } else {
    lcd.setCursor(0,0);
    lcd.print(" ");
    lcd.setCursor(0,0);
    lcd.print("S");
    digitalWrite(motionLed, LOW);
    delay(1000);
    lcd.setCursor(2,0);
    lcd.print("       ");
    lcd.setCursor(2,0);
    lcd.print("T=");
    lcd.print(cel);
    lcd.print("'C");
    lcd.setCursor(10,0);
    lcd.print("       ");
    lcd.setCursor(10,0);
    lcd.print("G=");
    lcd.print(gasValue);
  }
 // }
  //alarm
//  else
//  {
//    lcd.print("!!!WARNING!!!");
//    doubleBeep();
//    doubleBeep();
//    //doubleBeep();
//    //delay(5000);
//  }
  
  delay(1000);

}

//http://www.instructables.com/answers/multiple-processes-in-arduino/
//http://playground.arduino.cc/Code/TimedAction
//https://www.arduino.cc/en/Reference/AttachInterrupt
//http://rasas.me/projects/arduino/keypad-activated-interrupt
//http://forum.arduino.cc/index.php?topic=45671.0
//http://playground.arduino.cc/Main/PinChangeIntExample
//http://gammon.com.au/interrupts
//http://forum.arduino.cc/index.php?topic=8323.0
//http://playground.arduino.cc/Main/PinChangeInterrupt
//http://www.bristolwatch.com/arduino/arduino_irq.htm
//https://www.arduino.cc/en/Tutorial/LiquidCrystal
//https://www.arduino.cc/en/Reference/AttachInterrupt
//http://www.geertlangereis.nl/Electronics/Pin_Change_Interrupts/PinChange_en.html
