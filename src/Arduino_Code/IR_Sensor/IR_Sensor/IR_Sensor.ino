#include <FIR.h>
int factor = 5;

FIR<float, 16> fir;
FIR<float, 16> fir2;
FIR<float, 16> fir3;
FIR<float, 16> fir4;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  float coef[16] = { 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1.};
  fir.setFilterCoeffs(coef);
  float coef2[16] = { 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1.};
  fir2.setFilterCoeffs(coef2);
  float coef3[16] = { 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1.};
  fir3.setFilterCoeffs(coef3);
  float coef4[16] = { 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1.};
  fir4.setFilterCoeffs(coef4);
  Serial.print("Gain set: ");
  Serial.println(fir.getGain());
//  pinMode(A7,INPUT_PULLUP);
//  pinMode(A6,INPUT_PULLUP);
//  pinMode(A5,INPUT_PULLUP);
//  pinMode(A4,INPUT_PULLUP);
  Serial.println("Started!");
}

void loop() {
  // put your main code here, to run repeatedly:
//  if(analogRead(A7)<100 || analogRead(A6)<100 || analogRead(A5)<100 || analogRead(A4)<100)
//  {
//    Serial.println("Detect!");
//    }
//  Serial.print(analogRead(A7)*factor);
  Serial.print(fir.processReading(analogRead(A7)));
  Serial.print("\t");
//  Serial.print(analogRead(A6)*factor*(100/60));
  Serial.print(fir2.processReading(analogRead(A6)));
  Serial.print("\t");
//  Serial.print(analogRead(A5)*factor*(100/60));
  Serial.print(fir3.processReading(analogRead(A5))); 
  Serial.print("\t");
//  Serial.println(analogRead(A4)*factor*(100/60));
  Serial.println(fir4.processReading(analogRead(A4)));
//  Serial.print("\t");
//  if(!digitalRead(A7)||!digitalRead(A6)||!digitalRead(A5)||!digitalRead(A4))
//  {
//    Serial.println("detect!");
//    }
//  if(!digitalRead(A7)){Serial.println("1");}
//  if(!digitalRead(A6)){Serial.println("2");}
//  if(!digitalRead(A5)){Serial.println("3");}
//  if(!digitalRead(A4)){Serial.println("4");}
  
//  delay(20);
}
