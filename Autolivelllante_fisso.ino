#include <SPI.h>
#include <Wire.h>
#include <servo.h>
#define MPU 0x68
const int DIM = 270;
int once=0;
int horzPin = 2;
int vertPin = 3;
const int trigPin = 8;
const int echoPin = 9;
long duration;
float distances[DIM];
float distance;
float x = 0, y = 0, z = 0;
servo servoX, servoY, servoZ, servoD;
float acX, acY, acZ;
double pitch, roll,horz,vert;
float R[50];
float P[50];

void setup() {
  for(int i=0; i<20;i++)
{
  R[i]=0;
  P[i]=0;
}
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT); // pin sensore ultrasuoni
  pinMode(echoPin, INPUT);
  servoX.attach(3);
  servoY.attach(4);
  servoZ.attach(2);
  servoD.attach(5);
  init_MPU(); // Inizializzazione MPU6050
  stabilizza();
  misura();
  Serial.print("roll\tpitch\n");
}

void loop()
{
  stabilizza();
  spostamento_analogico(); 
  servoX.write(roll+80+horz);
  servoY.write(pitch+96.5+vert);
  Serial.print(roll);
  Serial.print("\t");
  Serial.print(pitch);
  Serial.print("\n");
  
}
float mapfloat(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}
void init_MPU() {
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  delay(1000);
}
void functionsMPU() {
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // request a total of 14 registers
  acX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  acY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  acZ = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
}
void spostamento_analogico()
{
  double horzVal;  //Calibrazione analogico variando lo 0
  double vertVal;
  vertVal = analogRead(vertPin);//campiono analogico
  vertVal = mapfloat(vertVal, 0, 1023, -1, 1);//Lo mappo rispetto a tre possibili velocità per ogni direzione
  horzVal = analogRead(horzPin);// Campiono analogico
  horzVal = mapfloat(horzVal, 0, 1023, -1, 1);
  if (horzVal >= 0.6) {
    horz=horz+horzVal;
   // x = x + horz ;
   // servoX.write(x);
   delay(70);
  }
  if (vertVal >= 0.6 ) {
    vert=vert+vertVal;
 //   y = y + vert;
  //  servoY.write(y);
  delay(70);
  }
  if (horzVal <= -0.6) {
    horz=horz+horzVal;
   // x = x + horz ;
   delay(70);
    //servoX.write(x);
  }
  if (vertVal <= -0.6 ) {
    vert=vert+vertVal;
   // y = y + vert;
   // servoY.write(y);
   delay(70);
  }
  
  
}

//Funzione per il calcolo degli angoli pitch e roll
double functionsPitchRoll(double A, double B, double C) {
  double DatoA, DatoB, value;
  DatoA = A;
  DatoB = (B * B) + (C * C);
  DatoB = sqrt(DatoB);

  value = atan2(DatoA, DatoB);
  value = value * 180 / 3.14;

  return value;
}
void misura() {
  float Calib_z = 90;
  float angolo;
  for (int i = 0; i < 270; i++) {
    angolo = 45 + (i * 0.33);
    servoD.write(angolo);
    delay(100);
    distance = calculateDistance();
    distances[i] = distance;

  }
  for (int i = 135 ; i > 90; i--) {
    servoD.write(i);
    delay(5);
  }
  float distanza_i=distances[0];
  for (int i=0;i<270;i++)
  {
  if(distances[i]<distanza_i)
  {
    distanza_i=distances[i];
    Calib_z= 45 + i*0.33;
    }
    z = Calib_z;
    servoZ.write(z);
  }
}

double calculateDistance() {

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  return distance;
}

void stabilizza() {
  functionsMPU();
  roll = functionsPitchRoll(acX, acY, acZ);   //Calcolo angolo roll
  pitch = functionsPitchRoll(acY, acX, acZ);  //Calcolo angolo pitch
 if(once==0){
  x=roll+ 80;
  y=pitch+ 96.5;
  servoX.write(x );
  servoY.write(y);
  ++once;
  }
  if(once>=0)
  {
    for(int i=0;i<19;i ++)
    {
      R[i]=R[i+1];
    }
    R[19]=roll;
    roll=0;
    for (int i=0;i<20;i++)
    {roll+=R[i];
    }
    roll=(float) roll/20;
    for(int i=0;i<19;i ++)
    {
      P[i]=P[i+1];
    }
    P[19]=pitch;
    pitch=0;
    for (int i=0;i<20;i++)
    {pitch+=P[i];
    }
    pitch=(float) pitch/20;
  }
}






