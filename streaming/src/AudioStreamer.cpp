#include "AudioStreamer.h"

#include <ArduinoJson.h>

#include "WebServer.h"

AudioStreamer::AudioStreamer(int _bclkPin, int _lrclkPin, int _dataPin) {
  bclkPin = _bclkPin;
  lrclkPin = _lrclkPin;
  dataPin = _dataPin;
}

void AudioStreamer::setup() {
  audio.setBufsize(32000, 655360);
  audio.setPinout(bclkPin, lrclkPin, dataPin, -1);
  audio.setVolume(5);
  audio.setConnectionTimeout(500, 2700);
}

void AudioStreamer::clearTitleInfo() {
  currentTitle = "";
  currentArtist = "";
  currentStation = "";
}

void AudioStreamer::sendStatusUpdate() {
  DynamicJsonDocument doc(1024);
  doc["isPlaying"] = audio.isRunning();
  doc["isPaused"] = isPaused;
  doc["volume"] = audio.getVolume();
  doc["title"] = currentTitle;
  doc["artist"] = currentArtist;
  doc["station"] = currentStation;
  String response;
  serializeJson(doc, response);
  webServer->audioEvent("status", response);
}

void AudioStreamer::loop() {
  audio.loop();

  // Periodically send status updates to the web client
  if (millis() - lastStatusTime > STATUS_INTERVAL_MS) {
    sendStatusUpdate();
    lastStatusTime = millis();
  }
}

void AudioStreamer::open(const char *url) {
  audio.stopSong();
  clearTitleInfo();
  isPaused = false;
  sendStatusUpdate();
  audio.connecttohost(url);
}

void AudioStreamer::stop() {
  audio.stopSong();
  clearTitleInfo();
  isPaused = false;
  sendStatusUpdate();
}

void AudioStreamer::pauseResume() {
  audio.pauseResume();
  isPaused = !isPaused;
  sendStatusUpdate();
}

void AudioStreamer::setVolume(int volume) { audio.setVolume(volume); }

// Audio callbacks
void audio_info(const char *info) { webServer->audioEvent("log", info); }

void audio_id3data(const char *info) {
  if (strncmp(info, "Title: ", 7) == 0 && strlen(info) > 7) {
    audioStreamer->currentTitle = String(info + 7);
    audioStreamer->sendStatusUpdate();
  } else if (strncmp(info, "Artist: ", 8) == 0 && strlen(info) > 8) {
    audioStreamer->currentArtist = String(info + 8);
    audioStreamer->sendStatusUpdate();
  }
}

void audio_showstation(const char *info) {
  audioStreamer->currentStation = String(info);
  audioStreamer->sendStatusUpdate();
}
void audio_showstreamtitle(const char *info) {
  audioStreamer->currentTitle = String(info);
  audioStreamer->sendStatusUpdate();
}

// Global instance
AudioStreamer *audioStreamer = nullptr;
