#include <WiFi.h>
#include <AudioFileSourcePROGMEM.h>
#include <AudioFileSourceSD.h>
#include <AudioGeneratorMP3a.h>
#include <AudioOutputI2S.h>
#include <SPI.h>
#include <SD.h>


// MP3 example excerpt from:
// "Impact Andante"
// Kevin MacLeod (incompetech.com)
// Licensed under Creative Commons: By Attribution 3.0
// http://creativecommons.org/licenses/by/3.0/
#include "example_mp3.h"

// Or play an MP3 file from the microSD card:
#define USE_MICROSD_CARD false
#define FILE_PATH "/test.mp3"

// Audio files can also be flashed to the ESP32-S3's flash memory (SPIFFS, FatFs, etc.)
// See https://github.com/earlephilhower/ESP8266Audio/tree/master/examples for more examples


#define DAC_BCLK_PIN          15
#define DAC_LRCLK_PIN         16
#define DAC_DATA_PIN          17
#define MICROSD_SPI_SS_PIN    41
#define MICROSD_SPI_SCK_PIN   42
#define MICROSD_SPI_MOSI_PIN  43
#define MICROSD_SPI_MISO_PIN  44
#define RGB_LED_PIN           45


AudioGeneratorMP3a *mp3;
AudioFileSourcePROGMEM *file;
AudioFileSourceSD *sd_file;
AudioOutputI2S *dac;
unsigned long last_led_blink = 0;
int led_color_index = 0;
int colors[][3] = {
    {255, 0, 0},    // Red
    {0, 255, 0},    // Green
    {0, 0, 255},    // Blue
    {255, 255, 0},  // Yellow
    {255, 0, 255}   // Magenta
};


void playMP3() {
  if (USE_MICROSD_CARD) {
    sd_file = new AudioFileSourceSD(FILE_PATH);
    mp3->begin(sd_file, dac);
  } else {
    file = new AudioFileSourcePROGMEM(example_data, sizeof(example_data));
    mp3->begin(file, dac);
  }
  Serial.println("Playing MP3...");
}

void setup() {
  // "USB CDC On Boot" must be enabled
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);

  pinMode(RGB_LED_PIN, OUTPUT);

  if (USE_MICROSD_CARD) {
    // Set up access to microSD card
    SPI.begin(MICROSD_SPI_SCK_PIN, MICROSD_SPI_MISO_PIN, MICROSD_SPI_MOSI_PIN, MICROSD_SPI_SS_PIN);
    if (!SD.begin(MICROSD_SPI_SS_PIN)) {
      while (1) {
        Serial.println("Card mount failed");
        delay(1000);
      }
    }
  }

  dac = new AudioOutputI2S();
  dac->SetPinout(DAC_BCLK_PIN, DAC_LRCLK_PIN, DAC_DATA_PIN);

  // Output can be very loud, so we're setting the gain less than 1
  dac->SetGain(0.1);
  mp3 = new AudioGeneratorMP3a();
  playMP3();

  Serial.println("Ready");
}

void loop() {
  if (mp3->isRunning()) {
    if (!mp3->loop()) {
      mp3->stop();
      Serial.println("Done");
      
      // Play the file again
      playMP3();
    }
  }

  // Cycle through some LED colors
  if (millis() - last_led_blink > 1000) {
    last_led_blink = millis();

    neopixelWrite(
      RGB_LED_PIN,
      colors[led_color_index][0],
      colors[led_color_index][1],
      colors[led_color_index][2]
    );

    led_color_index = (led_color_index + 1) % (sizeof(colors) / sizeof(colors[0]));
  }
}
