#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <ESPAsyncWebServer.h>

#include "AudioStreamer.h"

class WebServer {
 public:
  WebServer(int port);
  void setup();
  void audioEvent(String eventType, String eventData);

 private:
  AsyncWebServer *server;
  AsyncEventSource *events;
};

extern WebServer *webServer;

#endif  // WEB_SERVER_H