#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <BigFont01_I2C.h>
#include <GyverEncoder.h>

#define SW 7
#define DT 5
#define CLK 6
#define liquid 10
#define portal 9

Encoder enc(CLK, DT, SW);

LiquidCrystal_I2C lcd(0x27, 16, 2);
BigFont01_I2C     big(&lcd);

char letters[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
char letter;
int numbers[100];
int count = 52;
int letterPointer = 0;
int numberPointer = 0;
int number;
int liquidTime = 1;

void setup()
{
  lcd.init();
  lcd.backlight();
  big.begin();
  lcd.setCursor(0,0);
  lcd.print ("   Dimensions");
  lcd.setCursor(0,1);
  lcd.print (" are loading...");
  delay(500);
  lcd.clear();
  attachPCINT(CLK);
  attachPCINT(DT);
  for (int i = 0; i < 100; i++) {
    numbers[i] = random(0, 999);
  }
  letter = letters[count/26];
  number = numbers[count%100];

  Serial.begin(9100);
  pinMode(liquid, OUTPUT);
  pinMode(portal, OUTPUT);
}

void loop()
{
  blink(liquid,liquidTime);
  enc.tick();
  if (enc.isRight()) direction(true);
  if (enc.isLeft()) direction(false);
  if (enc.isClick()) select();

  // update display
  display();
}

void direction(bool isRight){
  if (isRight) {
    count++;
  } else {
    count--;
  }
  letterPointer = count >= 0? count/26 : 25 - abs(count/26);
  numberPointer = count >= 0? count%100 : 100 - abs(count%100);
  // update dimension code
  number = *(numbers+numberPointer);
  letter = *(letters+letterPointer);
  lcd.clear();
}

void display(){
  big.writechar(0,0, letter);
  big.writeint(0,7,number,3,false);
  // dots spacer's
  lcd.setCursor(5,0);
  lcd.print (char(161));
  lcd.setCursor(5,1);
  lcd.print (char(223));
}

void blink(int lamp, int time){
  int i;
  for(i=0; i<256; i+=50){
    analogWrite(lamp, i);
    delay(time);
  }
  
  for(i=255; i>0; i-=50){
    analogWrite(lamp, i);
    delay(time);
  }
}

void select(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print ("Dimension");
  lcd.setCursor(11,0);
  lcd.print (letter);
  lcd.print (":");
  lcd.print (number);
  lcd.setCursor(2,1);
  lcd.print ("is selected!");
  for(int a = 0; a<20; a++) {
    digitalWrite(portal, 1);
    blink(liquid,6);
  }
  digitalWrite(portal, 0);
  lcd.clear();
}

// PCINT for ATmega328 (UNO, Nano, Pro Mini)
uint8_t attachPCINT(uint8_t pin) {
  if (pin < 8) { // D0-D7 // PCINT2
    PCICR |= (1 << PCIE2);
    PCMSK2 |= (1 << pin);
    return 2;
  }
  else if (pin > 13) { //A0-A5  // PCINT1
    PCICR |= (1 << PCIE1);
    PCMSK1 |= (1 << pin - 14);
    return 1;
  }
  else { // D8-D13  // PCINT0
    PCICR |= (1 << PCIE0);
    PCMSK0 |= (1 << pin - 8);
    return 0;
  }
}

// PCINT vectors for tics
// pins 0-7: PCINT2
// pins 8-13: PCINT0
// pins A0-A5: PCINT1
ISR(PCINT0_vect) {
  //enc.tick();
}
ISR(PCINT1_vect) {
  //enc.tick();
}
ISR(PCINT2_vect) {
  enc.tick();
}

