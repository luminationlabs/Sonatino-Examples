#include <Arduino.h>
#include <WiFi.h>

#include "AudioStreamer.h"
#include "WebServer.h"
#include "esp_wifi.h"
#include "secrets.h"

#define DAC_BCLK_PIN 15
#define DAC_LRCLK_PIN 16
#define DAC_DATA_PIN 17
#define RGB_LED_PIN 45
#define SHOW_URL_INTERVAL_MS 10000

AudioStreamer stream(DAC_BCLK_PIN, DAC_LRCLK_PIN, DAC_DATA_PIN);
WebServer server(80);
bool ledOn = false;
unsigned long lastUrlTime = 0;

void printUrlToSerial() {
  Serial.print("Open http://");
  Serial.print(WiFi.localIP());
  Serial.println("/ in your browser.");
}

void setup() {
  Serial.begin(115200);

  pinMode(RGB_LED_PIN, OUTPUT);
  WiFi.mode(WIFI_MODE_STA);

  // Remove or modify the following line if you need more WiFi range
  // (high transmit power can contribute to audio noise, depending on antenna)
  esp_wifi_set_max_tx_power(8);

  // Try to connect to the WiFi network while flashing the LED blue
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    neopixelWrite(RGB_LED_PIN, 0, 0, ledOn ? 25 : 0);
    Serial.println("Connecting to WiFi...");
    ledOn = !ledOn;
    delay(500);
  }

  // We're connected to WiFi. Turn the LED green and print the web interface URL
  neopixelWrite(RGB_LED_PIN, 0, 25, 0);
  Serial.println("\nConnected!");
  printUrlToSerial();

  // Set up global pointers to the web server and audio stream
  webServer = &server;
  audioStreamer = &stream;

  // Set up the audio stream and web server
  stream.setup();
  server.setup();
}

void loop() {
  stream.loop();

  // Periodically print the web interface URL to the serial console
  if (millis() - lastUrlTime > SHOW_URL_INTERVAL_MS) {
    printUrlToSerial();
    lastUrlTime = millis();
  }
}
