 #include <Servo.h>
 #include <LiquidCrystal.h>
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       /* display.ino - программа робота-исследователя rrOne
 * 
 * Код написан Дмитрием Рязанцевым
 * 
 * 01.01.2016
 * 
 * ©2016
 */

#include "NewPing.h"
#include "dht.h"

#define TRIGGER_PIN  6
#define ECHO_PIN     7
#define MAX_DISTANCE 200

#define DHTPIN 18

#define DHTTYPE DHT11

#define D1 2
#define M1 3          
#define D2 4          
#define M2 5          

#define LCD_RS      8
#define LCD_ENABLE  9
#define LCD_D4      10
#define LCD_D5      11
#define LCD_D6      12
#define LCD_D7      13

float dist_cm;
float dist_cm1;
float dist_cm2;

const int numReadings = 30;
            
Servo servoMain;
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

bool allowSteering = true;

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

const int buttonPin = 15;
void SetMotor(int dir, int speed);

void setup() {
  Serial.begin(9600);
  dht.begin();
  servoMain.attach(14);
  pinMode(16, INPUT);
  pinMode(1, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);  
  pinMode(A5, INPUT);

  lcd.begin(16,2);
  lcd.print("\xA8\p\xB8\xB3");
  lcd.print("e\xBF");
  lcd.print("!");
  delay(2000);
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float k = 2*1.12;
  float voltage = k*4.5f/1024*analogRead(A5);
  float chargeLevel_procents;
  float chargeLevel;
  float x;
  float y;
  int z;

  //Сглаживание значения заряда батареи

  float A_v = voltage; //Значение
  float A_K = 0.3; //Коэффициент сглаживания
  static float A_y; //Выходное значение
  
  A_y = A_y - A_K * (A_y - A_v);
  
 // x = A_y - 7.4;
  y = A_y / 8.4 * 100;
  
  chargeLevel_procents = y;
  chargeLevel = z;
  if(chargeLevel_procents >= 0 && chargeLevel_procents < 33) {
    z = 1; // 1/3 заряда
  }
  
  else if(chargeLevel_procents >= 33 && chargeLevel_procents < 66) {
    z = 2; // 2/3 заряда
  }

  
  else if(chargeLevel_procents >= 66 && chargeLevel_procents <= 100) {
    z = 3; // Полный заряд
  }
  
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
  } 
  else {
    Serial.print("Humidity: "); 
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(t);
    Serial.print(" *C");
    Serial.print("\t Distance: ");
    Serial.print(sonar.ping()/US_ROUNDTRIP_CM);       
    Serial.print(" cm ");
    Serial.print("\t Voltage: ");
    Serial.print(A_y);
    Serial.print(" V ");
    Serial.print("\t Charge: ");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("\xE0=");
    lcd.print(sonar.ping()/US_ROUNDTRIP_CM);
    lcd.setCursor(5, 0);
    lcd.print(" t\x99=");
    lcd.print((int)t);
    lcd.setCursor(0, 1);
    lcd.print("H=");
    lcd.print((int)h);
    lcd.print("%");

    lcd.setCursor(15, 0);
    if(z == 1) {
      Serial.print("■");
      lcd.print("\x9F");
    }
    else if(z == 2) {
      Serial.print("■■");
      lcd.print("\x9D");
    }
    else if(z == 3) {
      Serial.print("■■■");
      lcd.print("\x9B");
    }
    else {
      Serial.print("ERROR");
    }
    Serial.print("\r\n");  
  }
  dist_cm = sonar.ping()/US_ROUNDTRIP_CM;
  if(dist_cm > 0  && dist_cm <= 60 ){
    SonarScan();
  }
  else if (dist_cm == 0 || dist_cm >60) {
     SetMotor(1, 150);
  }
 
 } 

 void SetMotor(int dir, int speed){
  switch (dir){
    case 0: // Стоп
      digitalWrite(D1, LOW);  
      digitalWrite(D2, LOW);
      analogWrite(M1, 0);       
      analogWrite(M2, 0);
      break;
    case 1: // Прямо
      digitalWrite(D1, LOW);  
      digitalWrite(D2, LOW);
      analogWrite(M1, speed);       
      analogWrite(M2, speed);
      break;
    case 2: // Назад
      digitalWrite(D1, HIGH);  
      digitalWrite(D2, HIGH);
      analogWrite(M1, speed);       
      analogWrite(M2, speed);
      break;
    case 3: // Влево
      digitalWrite(D1, HIGH);  
      digitalWrite(D2, LOW);
      analogWrite(M1, speed);       
      analogWrite(M2, speed);
      break;
    case 4: // Вправо
      digitalWrite(D1, LOW);  
      digitalWrite(D2, HIGH);
      analogWrite(M1, speed);       
      analogWrite(M2, speed);
      break;
  }
 }

 void SonarScan () {
  dist_cm1 = 0;
  dist_cm2 = 0;
  dist_cm = sonar.ping()/US_ROUNDTRIP_CM;
  SetMotor(0, 0);
    delay(1000);
    servoMain.write(0);
    delay(1500);
    dist_cm1 = sonar.ping()/US_ROUNDTRIP_CM;
    servoMain.write(180);
    delay(1000);
    dist_cm2 = sonar.ping()/US_ROUNDTRIP_CM;
    servoMain.write(90);
    delay(1000);

    if(dist_cm1 > dist_cm2){
      SetMotor(4, 100);
      delay(1000);
    }
    else if (dist_cm1 < dist_cm2) {
      SetMotor(3, 100);
      delay(1000);
    }
    else if (dist_cm1 = dist_cm2) {
      SetMotor(3, 100);
      delay(1000);
    }
 }

