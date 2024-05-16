# Web Streaming Example

This example demonstrates receiving Icecast audio streams over WiFi. It relies on [ESP32-audioI2S](https://github.com/schreibfaul1/ESP32-audioI2S) for playing the streams and [ESPAsyncWebServer](https://github.com/esphome/ESPAsyncWebServer) to serve a simple web interface for controlling playback.

## Setup

This example needs to be built with [PlatformIO](https://platformio.org/) instead of Arduino IDE.

After opening the project in PlatformIO, edit `src/secrets.h` to match your WiFi network's SSID and password. Then build and upload the project to Sonatino.

## Usage

Your Sonatino's RGB LED will begin to blink **blue** until it connects to your WiFi network. Once connected, it will be a solid **green** color. Use the serial monitor to see the IP address of Sonatino on your network, and then navigate to the IP address in your web browser.

The web interface allows you to enter an Icecast stream URL or select a random stream for a specific genre of music (using [radio-browser](https://www.radio-browser.info/)). Once you've entered a stream URL, click the "Start" button to start streaming the audio.

## Troubleshooting

You may see the message "`slow stream, dropouts are possible`" in the event log for high bit rate streams. The ESP32-audioI2S project suggests this can be improved with optimized TCP settings, but changing these settings requires compiling Arduino as an ESP-IDF component (i.e., `framework = arduino, espidf` in your `platformio.ini` file) and adjusting settings in menuconfig. This process can be complex and is not covered in this example. For more information, refer to the ESP32-audioI2S [Better_WiFi_throughput example](https://github.com/schreibfaul1/ESP32-audioI2S/tree/master/examples/Better_WiFi_throughput).
