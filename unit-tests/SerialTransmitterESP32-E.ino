//TX1 = 25 , TX2 = 17
//RX1 = 26, RX2 = 16
//ATTENTION --> TX1 et RX1 modifiés dans la lib "HardwareSerial"

void setup() {
  Serial.begin(115200);

  Serial.println("Serial USB ok");

  Serial1.begin(9600);
  Serial1.println("Hello World");
  Serial.println("Serial 1 run");

  Serial2.begin(9600);
  Serial2.println("Hello World");
  Serial.println("Serial 2 run");
}

void loop() {
  if(Serial1.available()){
    char c = Serial1.read();
    Serial.print(c);
  }
  if(Serial2.available()){
    char c = Serial2.read();
    Serial.print(c);
  }
  if(Serial.available()){
    char c = Serial.read();
    Serial1.print(c);
    Serial2.print(c);
  }

}
