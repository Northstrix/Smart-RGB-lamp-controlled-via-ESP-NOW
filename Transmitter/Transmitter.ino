//Transmitter for the smart RGB lamp controlled via ESP NOW
//Developed by Maxim Bortnikov
//For more information and for the schematics please visit
//https://github.com/Northstrix/Smart-RGB-lamp-controlled-via-ESP-NOW#include <esp_now.h>
#include <WiFi.h>
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
//int sat; // Saturation
int sat = 255;
int c = 0; // Mode
int dd = 50; // Debounce delay for brightness buttons
int interval = 25; // Interval between the package submission
int intrvl;
// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0x5C, 0xCF, 0x7F, 0xFD, 0x85, 0x1D};

typedef struct struct_message {
  int hue;
  int brt;
  int currentmode;
} struct_message;

// Create a struct_message called package
struct_message package;

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.clearDisplay();
  display.display();
  strip.begin();
  strip.show();
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Error initializing ESP-NOW");
  display.display();  
    return;
  }

  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10,0);
  display.println("Failed to add peer");
  display.display();  
    return;
  }
}
 
void loop() {
  huemsb = analogRead(34);
  huelsb = analogRead(35);
  //sat = analogRead(25);
  display.clearDisplay();
  if (huemsb > 0)
  huemsb = (huemsb+1)*15;
  //if (sat > 0)
  //sat = (sat + 1)/16 - 1;

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
      if (c == 3)
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
  display.println("Brightness:");
  display.setCursor(66,10);
  display.println(br);
  display.display();
  
  uint32_t stcolor = strip.ColorHSV(huemsb + huelsb, sat, br);
  strip.fill(stcolor);
  strip.show();
  }

  if (c == 1){
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

  if (c == 2){
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
  
  if (intrvl == interval){
  package.hue = huemsb + huelsb;
  package.brt = br;
  package.currentmode = c;
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &package, sizeof(package));
  intrvl = 0;
  }
  intrvl++;
}
