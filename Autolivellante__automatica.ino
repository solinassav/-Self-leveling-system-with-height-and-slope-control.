#include <SPI.h>
#include <Wire.h>
#include <Servo.h>
#define MPU 0x68  // I2C address of the MPU-6050
int horzPin = 2; 
int vertPin = 3;
double horzVal;      
double vertVal;  
Servo servoX, servoY,servoZ;
double acX,acY,acZ;
double pitch, roll,Z;

 
void setup(){
  Serial.begin(9600);
  servoX.attach(3);
  servoY.attach(4);
  servoZ.attach(2);
  servoZ.write(110);
  init_MPU(); // Inizializzazione MPU6050
  
  functionsMPU(); 
    
  roll = functionspitchroll(acX, acY, acZ);   //Calcolo angolo roll
  pitch = functionspitchroll(acY, acX, acZ);  //Calcolo angolo pitch
}
 
void loop()
{
  double a,b,c;
  
  
  vertVal = analogRead(vertPin);             
  vertVal = map(vertVal, 0, 1023, 0, 179);   
  b=pitch -0.83 + vertVal;
  servoY.write(b);

  if (vertVal > 170)
  { while (vertVal > 95.5) {
     vertVal = analogRead(vertPin);             
  vertVal = map(vertVal, 0, 1023, 0, 179);
  b=pitch +3 + vertVal;
    servoY.write(b);
      horzVal = analogRead(horzPin);
      horzVal = map(horzVal, 0, 1023, 0, 179);
      c=30 +horzVal;
      servoZ.write(c);
     
    }
  }
  else 
  {
    horzVal = analogRead(horzPin);
    horzVal = map(horzVal, 0, 1023, 179, 0);
    a=  5.5 - roll +horzVal ;
    servoX.write(a);
  }
  delay(100);
 
  

 
 
  Serial.print("a: "); Serial.print(a);  Serial.print("\t");Serial.print(horzVal);  Serial.print("\t");Serial.print(roll);
  Serial.print("b: "); Serial.print(b);
  Serial.print("\t");
  Serial.print("c: "); Serial.print(c);
  Serial.print("\n");
 
 
}
 
void init_MPU(){
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  delay(1000);
}
 
//Funzione per il calcolo degli angoli pitch e roll
double functionspitchroll(double A, double B, double C){
  double DatoA, DatoB, Value;
  DatoA = A;
  DatoB = (B*B) + (C*C);
  DatoB = sqrt(DatoB);
  Value = atan2(DatoA, DatoB);
  Value = Value * 180/3.14;
  return Value;
}
 
//Funzione per l'acquisizione degli assi X,Y,Z del MPU6050
void functionsMPU(){
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,6,true);  // request a total of 14 registers
  acX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  acY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  acZ=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
}

