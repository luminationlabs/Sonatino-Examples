#include <AudioOutputI2S.h>
#include <WiFi.h>
#include <driver/i2s.h>

#define VERBLIB_IMPLEMENTATION
#include "verblib.h"

// To use this example, you must connect an audio input source (e.g. microphone,
// line-in) and an audio output device to Sonatino using a 3.5mm TRRS cable. A
// headset device with a microphone is a good choice, but you can also use a
// TRRS adapter to connect separate audio devices.

// Be sure to read the Sonatino documentation and set the board's level
// selection DIP switches appropriately for your input and output levels.

#define BUFFER_SAMPLES 50
#define CHANNELS 2
#define USE_REVERB true
#define SAMPLE_RATE 48000

#define DAC_BCLK_PIN   15
#define DAC_LRCLK_PIN  16
#define DAC_DATA_PIN   17
#define ADC_BCLK_PIN   38
#define ADC_LRCLK_PIN  39
#define ADC_DATA_PIN   40


AudioOutputI2S *dac;
int16_t input_buffer[BUFFER_SAMPLES];
float filter_buffer[BUFFER_SAMPLES * CHANNELS];
verblib rv;

void processAudio(void *pvParameters) {
  size_t bytes_read = 0;
  float input_sample;
  int16_t out_sample[2];

  while (1) {
    i2s_read(I2S_NUM_1, input_buffer, BUFFER_SAMPLES * sizeof(int16_t),
             &bytes_read, portMAX_DELAY);

    if (bytes_read > 0) {
      // Read mono samples and copy into the stereo filter_buffer
      for (uint32_t i = 0; i < bytes_read / sizeof(int16_t); i++) {
        input_sample =
            (float)input_buffer[i] / (float)std::numeric_limits<int16_t>::max();

        filter_buffer[i * CHANNELS] = input_sample;
        filter_buffer[i * CHANNELS + 1] = input_sample;
      }

      if (USE_REVERB) {
        verblib_process(&rv, filter_buffer, filter_buffer, bytes_read / 2);
      }

      // Send samples to output
      for (uint32_t i = 0; i < bytes_read / sizeof(int16_t) * CHANNELS;
           i += CHANNELS) {
        out_sample[0] =
            filter_buffer[i] * (float)std::numeric_limits<int16_t>::max();
        out_sample[1] =
            filter_buffer[i + 1] * (float)std::numeric_limits<int16_t>::max();
        dac->ConsumeSample(out_sample);
      }
    }
  }
}

void setup() {
  // "USB CDC On Boot" must be enabled
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);

  verblib_initialize(&rv, SAMPLE_RATE, CHANNELS);

  dac = new AudioOutputI2S(); // Uses I2S_NUM_0
  dac->SetChannels(CHANNELS);
  dac->SetPinout(DAC_BCLK_PIN, DAC_LRCLK_PIN, DAC_DATA_PIN);
  dac->SetRate(SAMPLE_RATE);

  // Output can be very loud, so we're setting the gain less than 1
  dac->SetGain(0.3);
  dac->begin();

  // Configure audio input
  i2s_config_t i2s_config_in = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 1024,
    .use_apll = 1,
  };
  i2s_pin_config_t pin_config_in = {
    .mck_io_num = 0,
    .bck_io_num = ADC_BCLK_PIN,
    .ws_io_num = ADC_LRCLK_PIN,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = ADC_DATA_PIN
  };
  esp_err_t err;
  err = i2s_driver_install(I2S_NUM_1, &i2s_config_in, 0, NULL);
  if (err != ESP_OK) {
    while (1) {
      Serial.println("Failed to configure I2S driver for audio input");
      delay(1000);
    }
  }
  err = i2s_set_pin(I2S_NUM_1, &pin_config_in);
  if (err != ESP_OK) {
    while (1) {
      Serial.println("Failed to set I2S pins for audio input");
      delay(1000);
    }
  }

  // Start audio processing task
  xTaskCreatePinnedToCore(processAudio, "processAudio", 2048, NULL, 1, NULL, 0);

  Serial.println("Ready");
}

void loop() {}
