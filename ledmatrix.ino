#include <Wire_slave.h>
#include "P3RGB64x32MatrixPanel.h"

//R1  PA8
//G1  PA0
//B1  PA9
//R2  PA10
//G2  PA1
//B2  PA15

//CLK PB14
//LAT PB5
//OE  PB15

//A   PB12
//B   PB3
//C   PB13
//D   PB4

P3RGB64x32MatrixPanel matrix(PA8, PA0, PA9, PA10, PA1, PA15, PB14, PB5, PB15, PB12, PB3, PB13, PB4);

void setup() {
  Serial.begin(9600);
  Serial.println("LED Matrix Start");
  delay(1000);
  matrix.begin();
  i2c_init();

  //  // draw a pixel in solid white
  //  matrix.drawPixel(0, 0, matrix.color444(15, 15, 15));
  //  delay(500);
  //
  //  // fix the screen with green
  //  matrix.fillRect(0, 0, matrix.width(), matrix.height(), matrix.color444(0, 15, 0));
  //  delay(500);
  //
  //  // draw a box in yellow
  //  matrix.drawRect(0, 0, matrix.width(), matrix.height(), matrix.color444(15, 15, 0));
  //  delay(500);
  //
  //  // draw an 'X' in red
  //  matrix.drawLine(0, 0, matrix.width() - 1, matrix.height() - 1, matrix.color444(15, 0, 0));
  //  matrix.drawLine(matrix.width() - 1, 0, 0, matrix.height() - 1, matrix.color444(15, 0, 0));
  //  delay(500);
  //
  //  // draw a blue circle
  //  matrix.drawCircle(10, 10, 10, matrix.color444(0, 0, 15));
  //  delay(500);
  //
  //  // fill a violet circle
  //  matrix.fillCircle(40, 21, 10, matrix.color444(15, 0, 15));
  //  delay(500);
  //
  //  // fill the screen with 'black'
  //  matrix.fillScreen(matrix.color444(0, 0, 0));
  //
  //  // draw some text!
  //  matrix.setTextSize(1);     // size 1 == 8 pixels high
  //  matrix.setTextWrap(false); // Don't wrap at end of line - will do ourselves
  //
  //  matrix.setCursor(8, 0);    // start at top left, with 8 pixel of spacing
  //  uint8_t w = 0;
  //  char *str = "P3indoorSMDDisplay";
  //  for (w = 0; w < 8; w++) {
  //    matrix.setTextColor(Wheel(w));
  //    matrix.print(str[w]);
  //  }
  //  matrix.setCursor(2, 8);    // next line
  //  for (w = 8; w < 18; w++) {
  //    matrix.setTextColor(Wheel(w));
  //    matrix.print(str[w]);
  //  }
  //  matrix.println();
  //  //matrix.setTextColor(matrix.Color333(4,4,4));
  //  //matrix.println("Industries");
  //  matrix.setTextColor(matrix.color444(15, 15, 15));
  //  matrix.println("LED MATRIX!");
  //
  //  // print each letter with a rainbow color
  //  matrix.setTextColor(matrix.color444(15, 0, 0));
  //  matrix.print('3');
  //  matrix.setTextColor(matrix.color444(15, 4, 0));
  //  matrix.print('2');
  //  matrix.setTextColor(matrix.color444(15, 15, 0));
  //  matrix.print('x');
  //  matrix.setTextColor(matrix.color444(8, 15, 0));
  //  matrix.print('6');
  //  matrix.setTextColor(matrix.color444(0, 15, 0));
  //  matrix.print('4');
  //  matrix.setCursor(34, 24);
  //  matrix.setTextColor(matrix.color444(0, 15, 15));
  //  matrix.print("*");
  //  matrix.setTextColor(matrix.color444(0, 8, 15));
  //  matrix.print('R');
  //  matrix.setTextColor(matrix.color444(0, 0, 15));
  //  matrix.print('G');
  //  matrix.setTextColor(matrix.color444(8, 0, 15));
  //  matrix.print("B");
  //  matrix.setTextColor(matrix.color444(15, 0, 8));
  //  matrix.println("*");
}

void loop() {
}

void i2c_init()
{
  Wire.begin(0x20) ;                 // Ｉ２Ｃの初期化、自アドレスを0x20とする
  Wire.onRequest(requestEvent);     // マスタからのデータ取得要求のコールバック関数登録
  Wire.onReceive(receiveEvent);     // マスタからのデータ送信対応のコールバック関数登録
}

enum {
  cmd_print = 0x01,
  cmd_setCursor,
  cmd_fillRect,
  cmd_setTextSize,
  cmd_setTextColor
};

void receiveEvent(int howMany) {
  Serial.println("receiveEvent");
  byte cmd = Wire.read();
  Serial.print("cmd:");
  Serial.println(cmd);

  if (cmd == cmd_print) {
    if (howMany > 1) {
      char buff[100];
      for (int i = 0; i < howMany - 1; i++) {
        buff[i] = Wire.read();
      }
      matrix.print(buff);
      Serial.print(buff);
    }
  }

  if (cmd == cmd_setCursor) {
    if (howMany == 3) {
      byte x = Wire.read();
      byte y = Wire.read();
      matrix.setCursor(x, y);
      Serial.print("setCursor ");
      Serial.print(x);
      Serial.print(", ");
      Serial.println(y);
    }
  }

  if (cmd == cmd_fillRect) {
    if (howMany == 8) {
      byte x = Wire.read();
      byte y = Wire.read();
      byte w = Wire.read();
      byte h = Wire.read();
      byte r = Wire.read();
      byte g = Wire.read();
      byte b = Wire.read();
      matrix.fillRect(x, y, w, h, matrix.color444(r, g, b));
      Serial.print("fillRect ");
      Serial.print(x);
      Serial.print(", ");
      Serial.print(y);
      Serial.print(", ");
      Serial.print(w);
      Serial.print(", ");
      Serial.print(h);
      Serial.print(", ");
      Serial.print(r);
      Serial.print(", ");
      Serial.print(g);
      Serial.print(", ");
      Serial.print(b);
      Serial.println(", ");
    }
  }

  if (cmd == cmd_setTextSize) {
    if (howMany == 2) {
      byte size = Wire.read();
      matrix.setTextSize(size);
      Serial.print("setTextSize ");
      Serial.println(size);
    }
  }

  if (cmd == cmd_setTextColor) {
    if (howMany == 4) {
      byte r = Wire.read();
      byte g = Wire.read();
      byte b = Wire.read();
      matrix.setTextColor(matrix.color444(r, g, b));
      Serial.print("setTextColor ");
      Serial.print(r);
      Serial.print(", ");
      Serial.print(g);
      Serial.print(", ");
      Serial.println(b);
    }
  }
}

// マスターからのリクエストに対するデータ送信
void requestEvent() {
  Serial.println("requestEvent");
}

uint16_t Wheel(byte WheelPos) {
  if (WheelPos < 8) {
    return matrix.color444(15 - WheelPos * 2, WheelPos * 2, 0);
  } else if (WheelPos < 16) {
    WheelPos -= 8;
    return matrix.color444(0, 15 - WheelPos * 2, WheelPos * 2);
  } else {
    WheelPos -= 16;
    return matrix.color444(0, WheelPos * 2, 7 - WheelPos * 2);
  }
}

