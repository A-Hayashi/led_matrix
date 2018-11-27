#include <Wire_slave.h>

//R1
//G1
//B1
//R2
//G2
//B2
//CLK
//D
//OE
//A
//B
//C
//LAT

void setup() {
  Serial.begin(9600);
  Serial.println("KAITEN Start");

  i2c_init();
}

void loop() {
  delay(1000);
}

void i2c_init()
{
  Wire.begin(0x15) ;                 // Ｉ２Ｃの初期化、自アドレスを15とする
  Wire.onRequest(requestEvent);     // マスタからのデータ取得要求のコールバック関数登録
  Wire.onReceive(receiveEvent);     // マスタからのデータ送信対応のコールバック関数登録
}

void receiveEvent(int howMany) {
  while (Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  Serial.println("");         // print the integer
}

// マスターからのリクエストに対するデータ送信
void requestEvent() {
  Serial.println("requestEvent");
}

