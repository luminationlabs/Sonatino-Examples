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


#define BUFFER_SAMPLES  50
#define CHANNELS        2
#define USE_REVERB      true
#define SAMPLE_RATE     48000

#define MCLK_PIN        0
#define DAC_BCLK_PIN    15
#define DAC_LRCLK_PIN   16
#define DAC_DATA_PIN    17
#define ADC_BCLK_PIN    38
#define ADC_LRCLK_PIN   39
#define ADC_DATA_PIN    40

float output_gain = 0.3;  // Set output gain

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
        out_sample[0] = filter_buffer[i] *
                        (float)std::numeric_limits<int16_t>::max() *
                        output_gain;
        out_sample[1] = filter_buffer[i + 1] *
                        (float)std::numeric_limits<int16_t>::max() *
                        output_gain;

        size_t bytes_written;
        i2s_write(I2S_NUM_0, out_sample, sizeof(out_sample), &bytes_written,
                  portMAX_DELAY);
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);

  verblib_initialize(&rv, SAMPLE_RATE, CHANNELS);

  // Configure audio output (DAC)
  i2s_config_t i2s_config_out = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
      .sample_rate = SAMPLE_RATE,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
      .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
      .communication_format = I2S_COMM_FORMAT_I2S_MSB,
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
      .dma_buf_count = 8,
      .dma_buf_len = 128,
      .use_apll = 1,
      .tx_desc_auto_clear = true,
  };
  i2s_pin_config_t pin_config_out = {.mck_io_num = MCLK_PIN,
                                     .bck_io_num = DAC_BCLK_PIN,
                                     .ws_io_num = DAC_LRCLK_PIN,
                                     .data_out_num = DAC_DATA_PIN,
                                     .data_in_num = I2S_PIN_NO_CHANGE};
  esp_err_t err;
  err = i2s_driver_install(I2S_NUM_0, &i2s_config_out, 0, NULL);
  if (err != ESP_OK) {
    while (1) {
      Serial.println("Failed to configure I2S driver for audio output");
      delay(1000);
    }
  }
  err = i2s_set_pin(I2S_NUM_0, &pin_config_out);
  if (err != ESP_OK) {
    while (1) {
      Serial.println("Failed to set I2S pins for audio output");
      delay(1000);
    }
  }
  i2s_zero_dma_buffer(I2S_NUM_0);

  // Configure audio input (ADC)
  i2s_config_t i2s_config_in = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
      .sample_rate = SAMPLE_RATE,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format = I2S_COMM_FORMAT_I2S_MSB,
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
      .dma_buf_count = 8,
      .dma_buf_len = 1024,
      .use_apll = 1,
  };
  i2s_pin_config_t pin_config_in = {.mck_io_num = MCLK_PIN,
                                    .bck_io_num = ADC_BCLK_PIN,
                                    .ws_io_num = ADC_LRCLK_PIN,
                                    .data_out_num = I2S_PIN_NO_CHANGE,
                                    .data_in_num = ADC_DATA_PIN};
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
  xTaskCreatePinnedToCore(processAudio, "processAudio", 4096, NULL, 1, NULL, 0);

  Serial.println("Ready");
}

void loop() {}
