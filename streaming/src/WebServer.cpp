
#include "WebServer.h"

#include <ArduinoJson.h>
#include <AsyncJson.h>

#include "html.h"

WebServer::WebServer(int port) {
  server = new AsyncWebServer(port);
  events = new AsyncEventSource("/events");
}

void WebServer::setup() {
  // Serve the HTML file
  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", htmlIndex);
  });

  // Handle POST requests
  AsyncCallbackJsonWebHandler *jsonHandler = new AsyncCallbackJsonWebHandler(
      "/action", [&](AsyncWebServerRequest *request, JsonVariant &json) {
        const JsonObject &jsonObj = json.as<JsonObject>();
        String action = jsonObj["action"];
        bool success = true;
        if (action == "open") {
          String urlStr = jsonObj["url"].as<String>();
          const char *url = urlStr.c_str();
          audioStreamer->open(url);
        } else if (action == "stop") {
          audioStreamer->stop();
        } else if (action == "volume") {
          int volume = jsonObj["volume"].as<int>();
          audioStreamer->setVolume(volume);
        } else if (action == "pause") {
          audioStreamer->pauseResume();
        } else {
          success = false;
        }
        request->send(
            200, "application/json",
            "{\"success\":" + String(success ? "true" : "false") + "}");
      });

  server->addHandler(jsonHandler);
  server->addHandler(events);
  server->begin();
}

// Send event information to the web client
void WebServer::audioEvent(String eventType, String eventData) {
  if (events) {
    DynamicJsonDocument doc(1024);
    doc["event"] = eventType;
    doc["data"] = eventData;
    String response;
    serializeJson(doc, response);
    events->send(response.c_str(), "message", millis());
  }
}

// Global instance
WebServer *webServer = nullptr;
