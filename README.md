# Sonatino Examples

This repository contains sample code for the [Sonatino](https://sonatino.com) audio development board.

Documentation for Sonatino can be found [here](https://sonatino.com/docs).

**Note**: For board revisions prior to 3.x.x, use the examples in the [v2.x.x](v2.x.x/) folder. Your revision number can be found on the bottom side of the board.

---

## Included Example Projects:

| Name                                                  | Description                                                        |
| ----------------------------------------------------- | ------------------------------------------------------------------ |
| [audio-playback-example](audio-playback-example/)     | Demonstrates how to play an MP3 file                               |
| [audio-processing-example](audio-processing-example/) | Demonstrates real-time audio processing with a basic reverb effect |

## Running The Examples

To run an example, open the `.ino` file with Arduino IDE, connect Sonatino using a USB-C cable, and press **Upload**.

Note: You'll need to first install [Arduino-ESP32](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html#installing-using-arduino-ide) and the [ESP8266Audio](https://github.com/earlephilhower/ESP8266Audio) library if you haven't already done so.

For more information, refer to the **Getting Started** section of the Sonatino documentation.
