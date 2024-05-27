#include "LedControl.h"
#include <LiquidCrystal_I2C.h>

const int dinPin = 11;
const int clockPin = 10;
const int loadPin = 9;
int selectedDifficulty = 1;
//controller
const int xPin = A0;
const int yPin = A1;
const int pinSW = 12;
const int pinBw = 5;
const int pinBo = 6;
const int pinBr = 7;

boolean lastButtonState = false;
boolean lastbws = false;
boolean lastbos = false;
boolean lastbrs = false;
boolean hasStarted = false;
boolean hasSelected = false;
boolean hasEnded = false;
boolean victory = false;
int count = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);
byte matrixBrightness = 2;
byte EmptyMatrix[8][8]{
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}
};

byte CurrentMatrix[8][8]{
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}
};

byte BuiltMatrix[8][8]{
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}
};

byte matrixSize = 8;
byte xPos = 0;
byte yPos = 0;
byte xLastPos = 0;
byte yLastPos = 0;
const int buzzerPin = 13; 
const int minThreshold = 200;
const int maxThreshold = 600;

int xLast;
int yLast;

boolean buttonState = false;
boolean bos = false;
boolean bws = false;
boolean brs = false;

void setup() {
  lcd.init(); 
  lcd.backlight(); 
  lc.shutdown(0, false);
  lc.setIntensity(0, matrixBrightness);
  lc.clearDisplay(0);
  pinMode(pinSW, INPUT_PULLUP);
  pinMode(pinBo, INPUT_PULLUP);
  pinMode(pinBw, INPUT_PULLUP);
  pinMode(pinBr, INPUT_PULLUP);

  randomSeed(analogRead(3));

  pinMode(buzzerPin, OUTPUT);
}


void loop() {
  if(hasStarted == false){
    lcd.clear(); 
    lcd.setCursor(0, 0);
    lcd.print("Press Start"); 
    delay(100);

    boolean currentButtonw = digitalRead(pinBw);
      if (currentButtonw == HIGH) {
        bws = !bws;
      }
    if(bws == true){
      hasStarted = true;
      lcd.clear();
      delay(100);
      bws = false;
    }
}else{
  if(hasSelected == false){
    lcd.clear(); 
    lcd.setCursor(0, 0);
    lcd.print("Select a difficulty:");
    lcd.setCursor(0, 1);
    char myStg[10];
    sprintf(myStg, "%d", selectedDifficulty);
    lcd.print(myStg);
    delay(100);

     boolean currentButtono = digitalRead(pinBo);
      if (currentButtono == HIGH) {
        bos = !bos;
      }
    if(bos == true){
      if(selectedDifficulty < 5){
        selectedDifficulty++;
      }else{
        selectedDifficulty = 1;
      }
      bos = false;
    }
     boolean currentButtonw = digitalRead(pinBw);
      if (currentButtonw == HIGH) {
        bws = !bws;
      }
    if(bws == true){
      hasSelected = true;
      lcd.clear();
      delay(100);

      generateMatrix();
      updateMatrix(CurrentMatrix);
      delay(5000);
    }
  }else{
    if(hasEnded == true){
      updateMatrix(EmptyMatrix);
      if(victory == true){
        lcd.clear(); 
        lcd.setCursor(0, 0);
        lcd.print("Correct Answer!");
        lcd.setCursor(0, 1);
        lcd.print("RESTART GAME");
        delay(100);
      }else{
        lcd.clear(); 
        lcd.setCursor(0, 0);
        lcd.print("Wrong Answer!");
        lcd.setCursor(0, 1);
        lcd.print("RESTART GAME");
        delay(100);
      }
    }else{
    lcd.clear(); 
    lcd.setCursor(0, 0);
    lcd.print("Press red button");
    lcd.setCursor(0, 1);
    lcd.print("to retry");
    delay(100);
     boolean currentButtonr = digitalRead(pinBr);
      if (currentButtonr == HIGH) {
        brs = !brs;
      }
    if(brs == true){
      ClearMatrix();
      brs = false;
    }
  updateMatrix(BuiltMatrix);

  boolean currentButtonState = digitalRead(pinSW);

  if (currentButtonState != lastButtonState) {
    delay (20);
    if (currentButtonState == LOW) {
      buttonState = !buttonState;
    }
  }
  lastButtonState = currentButtonState;
  xLast = xPos;
  yLast = yPos;
  updatePositions();
  delay (100);

  if (buttonState == true) {

    if ((xLast != xPos || yLast != yPos) && buttonState == true) {
      BuiltMatrix[xLast][yLast] = 1;
      count++;
      lc.setLed(1, xLast, yLast, BuiltMatrix[xLast][yLast]);
      buttonState = false;
    }
  }

  BuiltMatrix[xPos][yPos] = 1;
  updateMatrix(BuiltMatrix);
  BuiltMatrix[xPos][yPos] = 0;

  if(count == (selectedDifficulty * 5)){
    victory = checkFinal();
    hasEnded = true;
    if(victory == true){
      playBuzzerWin();
    }else{
      playBuzzerLose();
    }
  }

}
}
}
}

void ClearMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      BuiltMatrix[row][col] = 0;
    }
  }
  count = 0;
}

void generateMatrix() {
  int maxBlocks;//counter for how many blocks to place
  maxBlocks = selectedDifficulty * 5;
 
  //initialize map with zeros in case you played the tutorial before
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      CurrentMatrix[i][j] = 0;
      
    }
  }
 
  // Randomly place blocks
  int blocks = 0;
  while (blocks < maxBlocks) {
    int x = random(0,8);
    int y = random(0,8);
    if(CurrentMatrix[x][y] == 0){
        CurrentMatrix[x][y] = 1;
        blocks++;
    }
  }
}
boolean checkFinal(){
  boolean check = true;
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      if(BuiltMatrix[row][col] != CurrentMatrix[row][col]){
        check = false;
        row = matrixSize;
        col = matrixSize;
      }
    }
  }
  return check;
}
void updateMatrix(byte Matrix[8][8]) {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, Matrix[row][col]);
    }
  }
}

void playBuzzerWin() {
  tone(buzzerPin, 1000, 500); // Play 1000 Hz for 500 milliseconds
  delay(500);
  tone(buzzerPin, 1500, 500); // Play 1500 Hz for 500 milliseconds
  delay(500);
  noTone(buzzerPin);   
}

void playBuzzerLose() {
  tone(buzzerPin, 500, 500);  // Play 500 Hz for 500 milliseconds
  delay(500);
  tone(buzzerPin, 250, 500);  // Play 250 Hz for 500 milliseconds
  delay(500);
  noTone(buzzerPin);
}

void updatePositions() {
  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);
  int hasMoved = false;
  byte newXPos = xPos;
  byte newYPos = yPos;

  if (xValue < minThreshold && newXPos > 0 && hasMoved == false) {
    newXPos--;
    hasMoved = true;
  } else if (xValue > maxThreshold && newXPos < matrixSize - 1 && hasMoved == false) {
        newXPos++;
        hasMoved = true;

  }

  if (yValue < minThreshold && newYPos < matrixSize - 1 && hasMoved == false) {
    newYPos++;
    hasMoved = true;

  } else if (yValue > maxThreshold && newYPos > 0 && hasMoved == false) {
    newYPos--;
    hasMoved = true;

  }

  if (BuiltMatrix[newXPos][newYPos] == 0) {
    xLastPos = xPos;
    yLastPos = yPos;
    xPos = newXPos;
    yPos = newYPos;
  }
}

