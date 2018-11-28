#include "P3RGB64x32MatrixPanel.h"

P3RGB64x32MatrixPanel* P3RGB64x32MatrixPanel ::singleton;
HardwareTimer P3RGB64x32MatrixPanel::timer(2);

void P3RGB64x32MatrixPanel::onTimer() {
  singleton->draw();
  //  singleton->test();
}

void P3RGB64x32MatrixPanel::begin() {
  singleton = this;
  disableDebugPorts();
  pinMode(pinR1, OUTPUT);
  pinMode(pinG1, OUTPUT);
  pinMode(pinB1, OUTPUT);
  pinMode(pinR2, OUTPUT);
  pinMode(pinG2, OUTPUT);
  pinMode(pinB2, OUTPUT);

  pinMode(pinLAT, OUTPUT);
  pinMode(pinCLK, OUTPUT);
  pinMode(pinOE,  OUTPUT);

  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(pinC, OUTPUT);
  pinMode(pinD, OUTPUT);

  digitalWrite(pinLAT, LOW);
  digitalWrite(pinCLK, LOW);
  digitalWrite(pinOE, HIGH);

  // Pause the timer while we're configuring it
  timer.pause();

  // Set up period
  timer.setPeriod(30); // in microseconds
  //  timer.setPeriod(10 * 1000); // in microseconds

  // Set up an interrupt on channel 1
  timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  timer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  timer.attachCompare1Interrupt(onTimer);

  // Refresh the timer's count, prescale, and overflow
  timer.refresh();

  // Start the timer counting
  timer.resume();
}

void P3RGB64x32MatrixPanel::stop() {
  timer.pause();
}

uint16_t P3RGB64x32MatrixPanel::colorHSV(long hue, uint8_t sat, uint8_t val) {
  uint8_t  r, g, b, lo;
  uint16_t s1, v1;

  // Hue ( 0 - 1535 )
  hue %= 1536;
  if (hue < 0) hue += 1536;
  lo = hue & 255;          // Low byte  = primary/secondary color mix
  switch (hue >> 8) {      // High byte = sextant of colorwheel
    case 0 : r = 255     ; g =  lo     ; b =   0     ; break; // R to Y
    case 1 : r = 255 - lo; g = 255     ; b =   0     ; break; // Y to G
    case 2 : r =   0     ; g = 255     ; b =  lo     ; break; // G to C
    case 3 : r =   0     ; g = 255 - lo; b = 255     ; break; // C to B
    case 4 : r =  lo     ; g =   0     ; b = 255     ; break; // B to M
    default: r = 255     ; g =   0     ; b = 255 - lo; break; // M to R
  }

  s1 = sat + 1;
  r  = 255 - (((255 - r) * s1) >> 8);
  g  = 255 - (((255 - g) * s1) >> 8);
  b  = 255 - (((255 - b) * s1) >> 8);

  v1 = val + 1;
  r = (r * v1) >> 11;
  g = (g * v1) >> 11;
  b = (b * v1) >> 11;

  return color555(r, g, b);
}

void P3RGB64x32MatrixPanel::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if (x < 0 || x >= 64 || y < 0 || y >= 32) return;
  int16_t idx = x + y * 64;
  drawBuffer()[idx] = color;
}

void P3RGB64x32MatrixPanel::test() {
  static byte x = 0;

  if (x == 0) {
    digitalWrite(pinR1, LOW);
    digitalWrite(pinG1, LOW);
    digitalWrite(pinB1, LOW);
    digitalWrite(pinR2, LOW);
    digitalWrite(pinG2, LOW);
    digitalWrite(pinB2, LOW);
    digitalWrite(pinLAT, LOW);
    digitalWrite(pinCLK, LOW);
    digitalWrite(pinOE,  LOW);
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, LOW);
    digitalWrite(pinC, LOW);
    digitalWrite(pinD, LOW);
  }

  if (x == 1)
    digitalWrite(pinR1, HIGH);  //10x
  if (x == 2)
    digitalWrite(pinG1, HIGH);  //20x
  if (x == 3)
    digitalWrite(pinB1, HIGH);  //30x
  if (x == 4)
    digitalWrite(pinR2, HIGH);  //40x
  if (x == 5)
    digitalWrite(pinG2, HIGH);  //50x
  if (x == 6)
    digitalWrite(pinB2, HIGH);  //60x
  if (x == 7)
    digitalWrite(pinLAT, HIGH);  //70x
  if (x == 8)
    digitalWrite(pinCLK, HIGH);  //80x
  if (x == 9)
    digitalWrite(pinOE,  HIGH);  //90x
  if (x == 10)
    digitalWrite(pinA, HIGH);  //100x
  if (x == 11)
    digitalWrite(pinB, HIGH);  //110x
  if (x == 12)
    digitalWrite(pinC, HIGH);  //120x
  if (x == 13)
    digitalWrite(pinD, HIGH);  //130x

  x++;
  if (x > 13) x = 0;
}

void P3RGB64x32MatrixPanel::draw() {
  static byte cnt = 30;
  static byte y = 15;
  static uint32_t out = 0;
  y = (y + 1) % 16;

  if (y == 0)
    cnt = (cnt + 1) % 31; // 31 must be skipped

  byte cmp = (cnt >> 4) | ((cnt >> 2) & 0x2) | (cnt & 0x4) | ((cnt << 2) & 0x8) | ((cnt << 4) & 0x10);

  for (int x = 0; x < 64; x++) {
    bool r1, b1, g1, r2, g2, b2;
    uint16_t c = matrixbuff[x + y * 64];
    r1 = (c & 0x1f) > cmp;
    g1 = ((c >>  5) & 0x1f) > cmp;
    b1 = ((c >> 10) & 0x1f) > cmp;
    c = matrixbuff[x + (y + 16) * 64];
    r2 = (c & 0x1f) > cmp;
    g2 = ((c >>  5) & 0x1f) > cmp;
    b2 = ((c >> 10) & 0x1f) > cmp;

    out = GPIOA->regs->ODR & ~(((uint32_t)1 << 8) | ((uint32_t)1 << 0) | ((uint32_t)1 << 9) | ((uint32_t)1 << 10) | ((uint32_t)1 << 1) | ((uint32_t)1 << 15));
    out |= ((uint32_t)r1 << 8) | ((uint32_t)g1 << 0) | ((uint32_t)b1 << 9) | ((uint32_t)r2 << 10) | ((uint32_t)g2 << 1) | ((uint32_t)b2 << 15);
    GPIOA->regs->ODR = out;

    GPIOB->regs->BSRR = (uint32_t)1 << 14;  //CLK HIGH
//    delayMicroseconds(1);
    GPIOB->regs->BRR = (uint32_t)1 << 14;   //CLK LOW
  }

  GPIOB->regs->BSRR = ((uint32_t)1 << 15) | ((uint32_t)1 << 5);

  bool a = (y & 0x01) ? HIGH : LOW;
  bool b = (y & 0x02) ? HIGH : LOW;
  bool c = (y & 0x04) ? HIGH : LOW;
  bool d = (y & 0x08) ? HIGH : LOW;
  out = GPIOB->regs->ODR & ~(((uint32_t)1 << 12) | ((uint32_t)1 << 3) | ((uint32_t)1 << 13) | ((uint32_t)1 << 4));
  out |= ((uint32_t)a << 12) | ((uint32_t)b << 3) | ((uint32_t)c << 13) | ((uint32_t)d << 4);
  GPIOB->regs->ODR = out;

  GPIOB->regs->BRR = ((uint32_t)1 << 15) | ((uint32_t)1 << 5);
}

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

