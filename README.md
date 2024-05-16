# Sonatino Examples

This repository contains sample code for the [Sonatino](https://sonatino.com) audio development board.

Documentation for Sonatino can be found [here](https://sonatino.com/docs).

**Note**: For board revisions prior to 3.x.x, use the examples in the [v2.x.x](v2.x.x/) folder. Your revision number can be found on the bottom side of the board.

---

## Included Example Projects:

| Name                      | Environment | Description                                                        |
| ------------------------- | ----------- | ------------------------------------------------------------------ |
| [playback](playback/)     | Arduino IDE | Demonstrates how to play an MP3 file                               |
| [processing](processing/) | Arduino IDE | Demonstrates real-time audio processing with a basic reverb effect |
| [streaming](streaming/)   | PlatformIO  | Demonstrates receiving an audio stream over WiFi                   |

## Running The Examples

Each example project is designed to run in a specific environment. The following instructions will guide you through running the examples in the respective environments.


- Arduino IDE Projects
  - To run an example, open the `.ino` file with Arduino IDE, connect Sonatino using a USB-C cable, and press **Upload**.
  - Note: You'll need to first install [Arduino-ESP32](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html#installing-using-arduino-ide) and the [ESP8266Audio](https://github.com/earlephilhower/ESP8266Audio) library if you haven't already done so.

- PlatformIO Projects
  - Open the folder containing the example in PlatformIO, and it should install the necessary libraries and platform (espressif32). Connect Sonatino using a USB-C cable, then build and upload the project.

---

For more information, refer to the **Getting Started** section of the Sonatino documentation.
