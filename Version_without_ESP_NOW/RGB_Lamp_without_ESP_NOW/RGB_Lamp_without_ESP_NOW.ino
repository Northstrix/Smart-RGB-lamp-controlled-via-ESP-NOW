//RGB_Lamp_without_ESP_NOW
//Developed by Maxim Bortnikov
//For more information and for the schematics please visit
//https://github.com/Northstrix/Smart-RGB-lamp-controlled-via-ESP-NOW
#include<Adafruit_NeoPixel.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define LED_PIN 13 // WS2812 data pin
#define LED_COUNT 4 // Number of LEDs
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800);
#define OLED_RESET     16 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
int huemsb; // Rough adjustment of hue
int huelsb; // Precise adjustment of hue
int br = 127; // Default brightness (0 - 0%; 127 - 50%; 255-100%)
int sat; // Saturation
int c = 0; // Mode
int dd = 50; // Debounce delay for brightness buttons

void rainbow(int sat, int br){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Rainbow mode");
  display.setCursor(0,10);
  display.println("Saturation:");
  display.setCursor(66,10);
  display.println(sat);
  display.setCursor(0,20);
  display.println("Brightness:");
  display.setCursor(66,20);
  display.println(br);
  display.display();  
  for(int i = 0; i< 65536; i++){
  uint32_t stcolor = strip.ColorHSV(i, sat, br);
    if ((analogRead(33)<100) && (analogRead(32)<100)){
      delay(300);
    if ((analogRead(33)<100) && (analogRead(32)<100)){
      c++;
      return;
    }
  }
  strip.fill(stcolor);
  strip.show();
  }
}

void slow_rainbow(int sat, int br){
  display.clearDisplay();
  delay(1);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Slow rainbow mode");
  display.setCursor(0,10);
  display.println("Saturation:");
  display.setCursor(66,10);
  display.println(sat);
  display.setCursor(0,20);
  display.println("Brightness:");
  display.setCursor(66,20);
  display.println(br);
  display.display();  
  for(int i = 0; i< 65536; i++){
  uint32_t stcolor = strip.ColorHSV(i, sat, br);
  delay(54);
    if ((analogRead(33)<100) && (analogRead(32)<100)){
      delay(300);
    if ((analogRead(33)<100) && (analogRead(32)<100)){
      c++;
      return;
    }
  }
  strip.fill(stcolor);
  strip.show();
  }
}

void setup() {
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.clearDisplay();
  display.display();
  delay(1000);
  strip.begin();
  strip.show();
}

void loop() {
  huemsb = analogRead(34);
  huelsb = analogRead(35);
  sat = analogRead(25);
  display.clearDisplay();
  if (huemsb > 0)
  huemsb = (huemsb+1)*15;
  if (sat > 0)
  sat = (sat + 1)/16 - 1;

  if ((analogRead(32)<100) && (analogRead(33)>100)){
    delay(dd);
    if ((analogRead(32)<100) && (analogRead(33)>100))
    br --;
  }
  if ((analogRead(33)<100) && (analogRead(32)>100)){
    delay(dd);
    if ((analogRead(33)<100) && (analogRead(32)>100))
    br ++;
  }

  if ((analogRead(33)<100) && (analogRead(32)<100)){
      delay(500);
    if ((analogRead(33)<100) && (analogRead(32)<100)){
      c++;
      if (c == 5)
      c = 0;
    }
  }

  if (br > 255)
  br = 255;

  if (br < 0)
  br = 0;

  if (c == 0){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Hue:");
  display.setCursor(66,0);
  display.println(huemsb + huelsb);
  display.setCursor(0,10);
  display.println("Saturation:");
  display.setCursor(66,10);
  display.println(sat);
  display.setCursor(0,20);
  display.println("Brightness:");
  display.setCursor(66,20);
  display.println(br);
  display.display();  
  
  uint32_t stcolor = strip.ColorHSV(huemsb + huelsb, sat, br);
  strip.fill(stcolor);
  strip.show();
  }

  if (c == 1){
  rainbow(sat,br);
  }

  if (c == 2){
  slow_rainbow(sat,br);
  }

  if (c == 3){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Lamp mode");
  display.setCursor(0,10);
  display.println("Brightness:");
  display.setCursor(66,10);
  display.println(br);
  display.display();  
  strip.setBrightness(br);
  uint32_t color = strip.Color(255, 255, 255);
  strip.fill(color, 0, LED_COUNT);
  strip.show(); 
  }

  if (c == 4){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Shining moon mode");
  display.setCursor(0,10);
  display.println("Attention! This mode");
  display.setCursor(0,20);
  display.println("can damage your strip");
  display.display();
  strip.setBrightness(255);
  uint32_t color = strip.Color(255, 255, 255);
  strip.fill(color, 0, LED_COUNT);
  strip.show(); 
  }
  
}
