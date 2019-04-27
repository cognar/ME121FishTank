
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);



int Readings[100];
float average = 0;


void setup() {
  Serial.begin(9600);
  pinMode(8, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  // initialize the LCD
  lcd.begin();
  lcd.clear();
  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.print("Reading:   ");
  lcd.print("READ");
  lcd.setCursor(0,1);
  lcd.print("Voltage:   ");
  lcd.print("VOLT");

  /*//Test Relays
  digitalWrite(2, HIGH);
  delay(200);
  digitalWrite(2, LOW);
  digitalWrite(3, HIGH);
  delay(200);
  digitalWrite(3, LOW);
  delay(200);
  digitalWrite(4, HIGH);
  delay(200);
  digitalWrite(4, LOW);
*/
}

void loop() {
  average = 0;
  delay(100);
  digitalWrite(8, HIGH);
  delay(1000);
  for (int i = 0; i < 100; i++){
    Readings[i] = analogRead(0);
    updateScreen(Readings[i]);
    delay(100);    
    
  }
  digitalWrite(8, LOW);
  for(int i = 0; i < 100; i++){
    average += Readings[i];
    
  }
  average = average / 100;
  Serial.println(average);
  Serial.println();
}

void updateScreen(float message){
  lcd.clear();
  lcd.print("Reading:   ");
  lcd.print(message);
  lcd.setCursor(0,1);
  lcd.print("Voltage:   ");
  lcd.print("VOLT");


}
