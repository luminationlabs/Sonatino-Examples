#ifndef AUDIO_STREAMER_H
#define AUDIO_STREAMER_H

#include "Audio.h"

#define STATUS_INTERVAL_MS 5000

class AudioStreamer {
 public:
  AudioStreamer(int bclkPin, int lrclkPin, int dataPin);
  AudioStreamer(){};
  void setup();
  void loop();
  void open(const char *url);
  void stop();
  void pauseResume();
  void setVolume(int volume);
  void sendStatusUpdate();

  String currentTitle = "";
  String currentArtist = "";
  String currentStation = "";

 private:
  void clearTitleInfo();
  int bclkPin;
  int lrclkPin;
  int dataPin;
  Audio audio;
  bool isPaused = false;
  unsigned long lastStatusTime = 0;
};

extern AudioStreamer *audioStreamer;

#endif  // AUDIO_STREAMER_H