#include <SPI.h>
#include <Wire.h>
#include <Servo.h>
#define MPU 0x68
#define DISTANZA_MOTORINI 7
const int DIM = 270;
int horzPin = 2;
int vertPin = 3;
const int trigPin = 8;
const int echoPin = 9;
long duration;
float Distances[DIM];
float distance;
float x = 0, y = 0, z = 0;
Servo ServoX, ServoY, ServoZ, ServoD;
float AcX, AcY, AcZ;

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT); // pin sensore ultrasuoni
  pinMode(echoPin, INPUT);
  ServoX.attach(4);
  ServoY.attach(3);
  ServoZ.attach(2);
  ServoD.attach(5);
  init_MPU(); // Inizializzazione MPU6050
  FunctionsMPU(); // Acquisisco assi AcX, AcY, AcZ
  stabilizza();
  Serial.print("Size of float: ");
  Serial.print(sizeof(float));
  Serial.print("\n");
  Serial.print("Size of double: ");
  Serial.print(sizeof(double));
}

void loop()
{
  spostamento_analogico(); //Commentare questa riga durante la calibrazione accelerometro,togliere il commento per la calibrazione dell'analogico.
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
void FunctionsMPU() {
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // request a total of 14 registers
  AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
}
void spostamento_analogico()
{
  double horzVal, Calib_horz = 0; //Calibrazione analogico variando lo 0
  double vertVal, Calib_vert = 0;
  vertVal = analogRead(vertPin);//campiono analogico
  vertVal = mapfloat(vertVal, 0, 1023, -1, 1);//Lo mappo rispetto a tre possibili velocità per ogni direzione
  vertVal = vertVal + Calib_vert;  //Salvo il valore in una variabile che contenga la calibrazione
  horzVal = analogRead(horzPin);// Campiono analogico
  horzVal = mapfloat(horzVal, 0, 1023, -1, 1);// lo mappo rispetto a tre possibili velocità
  horzVal = horzVal + Calib_horz;//Aggiorno la variabile letta con la calibrazione
  if (horzVal >= 0.6) {
    x = x + horzVal ;
    ServoX.write(x);
  }
  if (vertVal >= 0.6 ) {
    y = y + vertVal;
    ServoY.write(y);
  }
  if (horzVal <= -0.6) {
    x = x + horzVal ;
    ServoX.write(x);
  }
  if (vertVal <= -0.6 ) {
    y = y + vertVal;
    ServoY.write(y);
  }
  delay(10);

}

//Funzione per il calcolo degli angoli Pitch e Roll
double FunctionsPitchRoll(double A, double B, double C) {
  double DatoA, DatoB, Value;
  DatoA = A;
  DatoB = (B * B) + (C * C);
  DatoB = sqrt(DatoB);

  Value = atan2(DatoA, DatoB);
  Value = Value * 180 / 3.14;

  return Value;
}
void misura() {
  float angolo;
  for (int i = 0; i < 270; i++) {
    angolo = 45 + (i * 0.33);
    ServoD.write(angolo);
    delay(50);
    distance = calculateDistance();
    Distances[i] = distance;

  }
  for (int i = 135 ; i > 90; i--) {
    ServoD.write(i);
    delay(5);
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
float angoloScottex(double alfa, double d, double y){
  if(alfa>90){
  alfa = (PI/180.0) * alfa;
  float beta = (float) ((180.0 / PI)* (asin((d*sin(alfa)) / (sqrt(d*d + y*y - (2 * d * y * cos(alfa)))))));
  return (float)180.0 - beta;
  }
  else{
    alfa = (PI/180.0) * alfa;
  float beta = (float) ((180.0 / PI)* (asin((d*sin(alfa)) / (sqrt(d*d + y*y - (2 * d * y * cos(alfa)))))));
  return (float)180 - beta;
  }
  
}


void stabilizza() {
  float Pitch, Roll;
  float  Calib_x = 96, Calib_y = 90, Calib_z = 90; //Calibrazione aceelerometro.
  Roll = FunctionsPitchRoll(AcX, AcY, AcZ);   //Calcolo angolo Roll
  Pitch = FunctionsPitchRoll(AcY, AcX, AcZ);  //Calcolo angolo Pitch
  x = -Roll + Calib_x;
  y = Pitch + Calib_y;
  ServoX.write(x);
  ServoY.write(y);

  misura();
   float distanza_i=Distances[0];
  for (int i=0;i<270;i++)
  {
  if(Distances[i]<distanza_i)
  {
    distanza_i=Distances[i];
    Calib_z= 45 + i*0.33;
    }
  z = Calib_z;
  }
 
  z = angoloScottex(Calib_z, distanza_i, DISTANZA_MOTORINI);
   
//If you comment on this line, the laser will position itself perpendicular to the wall, otherwise not commenting it will point to the nearest object it has found during calibration.
  ServoZ.write(z);
 }
  







