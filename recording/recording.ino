#include <SD.h>
#include <SPI.h>
#include <WiFi.h>
#include <driver/i2s.h>

// To use this example, you must connect an audio input source (e.g. microphone,
// line-in) and insert a microSD card (formatted with a FAT32 filesystem) into
// the microSD card slot on Sonatino.

// Be sure to read the Sonatino documentation and set the board's input level
// selection DIP switch appropriately for your input source.

#define BUFFER_SAMPLES        1024
#define SAMPLE_RATE           48000
#define CHANNELS              1
#define RECORD_TIME_SECONDS   10
#define FILENAME_PREFIX       "/REC_"
#define FILENAME_EXTENSION    ".wav"
#define TEMP_FILENAME         "/TEMP.wav"

#define MCLK_PIN              0
#define ADC_BCLK_PIN          38
#define ADC_LRCLK_PIN         39
#define ADC_DATA_PIN          40
#define MICROSD_SPI_SS_PIN    41
#define MICROSD_SPI_SCK_PIN   42
#define MICROSD_SPI_MOSI_PIN  43
#define MICROSD_SPI_MISO_PIN  44
#define RGB_LED_PIN           45

int16_t input_buffer[BUFFER_SAMPLES];
File audioFile;
volatile uint32_t totalBytesWritten = 0;
volatile uint32_t recordingStartTime = 0;
volatile uint16_t fileIndex = 1;
volatile bool ledState = false;

void writeWavHeader(File &file, uint32_t sampleRate, uint16_t bitsPerSample,
                    uint16_t channels, uint32_t totalDataLen) {
  uint32_t byteRate = sampleRate * channels * bitsPerSample / 8;
  uint16_t blockAlign = channels * bitsPerSample / 8;

  file.seek(0);
  file.write((const uint8_t *)"RIFF", 4);
  file.write((uint8_t *)&totalDataLen, 4);
  file.write((const uint8_t *)"WAVE", 4);
  file.write((const uint8_t *)"fmt ", 4);
  uint32_t subChunk1Size = 16;
  uint16_t audioFormat = 1;
  file.write((uint8_t *)&subChunk1Size, 4);
  file.write((uint8_t *)&audioFormat, 2);
  file.write((uint8_t *)&channels, 2);
  file.write((uint8_t *)&sampleRate, 4);
  file.write((uint8_t *)&byteRate, 4);
  file.write((uint8_t *)&blockAlign, 2);
  file.write((uint8_t *)&bitsPerSample, 2);
  file.write((const uint8_t *)"data", 4);
  uint32_t subChunk2Size = totalDataLen - 36;
  file.write((uint8_t *)&subChunk2Size, 4);
}

String getNextFilename() {
  char filename[20];

  while (true) {
    snprintf(filename, sizeof(filename),
             FILENAME_PREFIX "%04d" FILENAME_EXTENSION, fileIndex);
    if (!SD.exists(filename)) {
      return String(filename);
    }
    fileIndex++;
  }
}

void closeWavFile() {
  // Update WAV header with the correct file size
  uint32_t fileSize = audioFile.size();
  audioFile.seek(0);
  writeWavHeader(audioFile, SAMPLE_RATE, 16, CHANNELS, fileSize - 8);
  audioFile.close();

  // Rename the temp file to the next sequential name
  String newFilename = getNextFilename();
  SD.rename(TEMP_FILENAME, newFilename.c_str());

  // Flash the LED blue quickly to indicate the file was saved
  for (int i = 0; i < 6; i++) {
    neopixelWrite(RGB_LED_PIN, 0, 0, 255);  // Blue
    delay(100);
    neopixelWrite(RGB_LED_PIN, 0, 0, 0);  // Off
    delay(100);
  }

  Serial.print("Saved recording: ");
  Serial.println(newFilename);
}

void startNewRecording() {
  // Start a new recording by opening TEMP_FILENAME and writing the WAV header
  audioFile = SD.open(TEMP_FILENAME, FILE_WRITE);
  if (!audioFile) {
    Serial.println("Failed to create temporary wav file");
    while (1);
  }
  writeWavHeader(audioFile, SAMPLE_RATE, 16, CHANNELS, 0);
  totalBytesWritten = 0;
  recordingStartTime = millis();
}

void processAudio(void *pvParameters) {
  uint32_t lastLedToggleTime = millis();
  size_t bytes_read = 0;

  while (1) {
    // Toggle LED
    if (millis() - lastLedToggleTime >= 1000) {
      ledState = !ledState;
      if (ledState) {
        neopixelWrite(RGB_LED_PIN, 255, 0, 0);  // Red
      } else {
        neopixelWrite(RGB_LED_PIN, 0, 0, 0);  // Off
      }
      lastLedToggleTime = millis();
    }

    // Read audio data
    esp_err_t read_err = i2s_read(
        I2S_NUM_1, input_buffer, BUFFER_SAMPLES * sizeof(int16_t), &bytes_read,
        100 / portTICK_RATE_MS);  // Use a non-blocking read with a timeout
    if (read_err == ESP_OK && bytes_read > 0) {
      audioFile.write((uint8_t *)input_buffer, bytes_read);
      totalBytesWritten += bytes_read;
    }

    // Check if RECORD_TIME_SECONDS have passed
    if (millis() - recordingStartTime >= RECORD_TIME_SECONDS * 1000) {
      closeWavFile();
      startNewRecording();
    }

    vTaskDelay(1);
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);

  // Set up access to microSD card
  SPI.begin(MICROSD_SPI_SCK_PIN, MICROSD_SPI_MISO_PIN, MICROSD_SPI_MOSI_PIN,
            MICROSD_SPI_SS_PIN);
  if (!SD.begin(MICROSD_SPI_SS_PIN)) {
    while (1) {
      Serial.println("Card mount failed");
      delay(1000);
    }
  }

  // Set up RGB LED
  pinMode(RGB_LED_PIN, OUTPUT);

  // Configure audio input
  i2s_config_t i2s_config_in = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
      .sample_rate = SAMPLE_RATE,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format = I2S_COMM_FORMAT_STAND_I2S,
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
      .dma_buf_count = 8,
      .dma_buf_len = BUFFER_SAMPLES,
      .use_apll = 1,
  };
  i2s_pin_config_t pin_config_in = {.mck_io_num = MCLK_PIN,
                                    .bck_io_num = ADC_BCLK_PIN,
                                    .ws_io_num = ADC_LRCLK_PIN,
                                    .data_out_num = I2S_PIN_NO_CHANGE,
                                    .data_in_num = ADC_DATA_PIN};
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

  // Clear I2S buffer (prevents pop at beginning of first recording)
  size_t bytes_read = 0;
  for (int i = 0; i < 50; i++) {
    i2s_read(I2S_NUM_1, input_buffer, BUFFER_SAMPLES * sizeof(int16_t),
             &bytes_read, 100 / portTICK_RATE_MS);
  }

  // Start recording
  startNewRecording();

  // Start audio processing task
  xTaskCreatePinnedToCore(processAudio, "processAudio", 4096, NULL, 1, NULL, 1);

  Serial.println("Recording...");
}

void loop() {}
