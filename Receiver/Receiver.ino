//Receiver for the smart RGB lamp controlled via ESP NOW
//Developed by Maxim Bortnikov
//For more information and for the schematics please visit
//https://github.com/Northstrix/Smart-RGB-lamp-controlled-via-ESP-NOW
#include <ESP8266WiFi.h>
#include <espnow.h>
#include<Adafruit_NeoPixel.h>
#define LED_PIN 2 // WS2812 data pin
#define LED_COUNT 60 // Number of LEDs
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800);
int sat = 255;
typedef struct struct_message {
  int hue;
  int brt;
  int currentmode;
} struct_message;

// Create a struct_message called package
struct_message package;

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&package, incomingData, sizeof(package));
  int br = package.brt;
  int hue = package.hue;
  int c = package.currentmode;

  if (c == 0){
  uint32_t stcolor = strip.ColorHSV(hue, sat, br);
  strip.fill(stcolor);
  strip.show();
  }

  if (c == 1){
  strip.setBrightness(br);
  uint32_t color = strip.Color(255, 255, 255);
  strip.fill(color, 0, LED_COUNT);
  strip.show(); 
  }

  if (c == 2){
  strip.setBrightness(255);
  uint32_t color = strip.Color(255, 255, 255);
  strip.fill(color, 0, LED_COUNT);
  strip.show(); 
  }
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  delay(1000);
  strip.begin();
  strip.show();
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  
}
