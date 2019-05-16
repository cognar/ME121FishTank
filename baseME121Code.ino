
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);


//Pin definitions
int salinityPowerPin = 8;     //energized salinity sensor
int salinitySensorPin = A0;   //Reads Voltage drop across 10k Resistor
int saltSolenoid = 2;         //NaCl Solenoid
int diSolenoid = 4;           //DI solenoid
int tempSolenoid = 3;         //Temp Solenoid
//Values
float reading;
float average = 0;
float standardDev = 4.5/2422.;
float deadTime = 18000;       //Delay after solinity change
float setPoint = 182./2422.;
long timeSinceChange = 0;
float KpSalt = 0.00007216;      //This is maybe about right
float KpDI = 4000;            //This is wrong in everyway
int tankMass = 98;
float overFlow = 0.15;
float G = 0.8;


void setup() {
  //initialized Serial and pinmodes
  Serial.begin(9600);
  pinMode(salinityPowerPin, OUTPUT);
  pinMode(saltSolenoid, OUTPUT);
  pinMode(tempSolenoid, OUTPUT);
  pinMode(diSolenoid, OUTPUT);
  // initialize the LCD
  lcd.begin();
  lcd.clear();
  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.print("Salinity:   ");
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
  reading = measureSalinity();
  Serial.println(reading);
  updateScreen(reading);
  float error = reading - setPoint;
  if((error > 3 * standardDev) || (error < -3 * standardDev))
    if(millis() > timeSinceChange + deadTime){    //Check is deadtime has elapsed
      adjustSalinity(error, getDeltaT(error, reading));
    }
}


void updateScreen(float message){
  lcd.clear();
  lcd.print("Salinity:   ");
  lcd.print(message);
  lcd.setCursor(0,1);
  lcd.print("Voltage:   ");
  lcd.print("VOLT");


}


float rawToSalinity(float rawRead){
  Serial.print("RAW  ");Serial.println(rawRead);
  float salinity;
  if (rawRead < 142){
    salinity = rawRead / 10423.;
    
  }
  else{
    salinity = (rawRead - 400) / 2422.;
    
  }  
  return salinity;
}


//Takes 100 readings of salinity sensor returns the average
float measureSalinity(){
  float reading = 0;
  digitalWrite(salinityPowerPin, HIGH);
  delay(100);
  for (int i = 0; i < 100; i++){
    reading += analogRead(salinitySensorPin);
    delay(10);
  }
  reading = reading/100.;
  digitalWrite(salinityPowerPin, LOW);
  return(rawToSalinity(reading));

}


void adjustSalinity(float error, int deltaT){
    Serial.print("ERROR");Serial.println(deltaT);
    
    //Serial.println(3*standardDev);
    if(error > 3 * standardDev){                //Water is too salty
      digitalWrite(diSolenoid, HIGH);
      delay(deltaT);                        //this factor is all wrong
      digitalWrite(diSolenoid, LOW);
    }
    else if(error < -3 * standardDev){          //Water is not salty enough
      digitalWrite(saltSolenoid, HIGH);
      delay(deltaT);                      //needs correction factor for overflow
      digitalWrite(saltSolenoid, LOW);    
    }
    timeSinceChange = millis();   
}

int getDeltaT(float error, float currentSalinity){
  int deltaT;
  float salinityToAdd;
  float finalSalinity = currentSalinity + G*(setPoint - currentSalinity);
  if(error < 0){
    salinityToAdd = 0.1;
  }
  else if (error > 0){
    salinityToAdd = 0;
  }
  float massToAdd = tankMass*(1/(1-overFlow))*(currentSalinity - finalSalinity)/(currentSalinity - salinityToAdd);
  if(error < 0){
    deltaT = (massToAdd*1000)/(6);
  }
  else if(error > 0){
    deltaT = (massToAdd*1000)/(7);
  }
  return(deltaT);
}
