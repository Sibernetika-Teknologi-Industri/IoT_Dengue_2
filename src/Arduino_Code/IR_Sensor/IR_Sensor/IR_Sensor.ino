void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(A7,INPUT_PULLUP);
  pinMode(A6,INPUT_PULLUP);
  pinMode(A5,INPUT_PULLUP);
  pinMode(A4,INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(!digitalRead(A7)||!digitalRead(A6)||!digitalRead(A5)||!digitalRead(A4))
  {
    Serial.println("detect!");
    }
//  delay(20);
}
