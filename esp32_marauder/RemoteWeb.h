#ifndef RemoteWeb_h
#define RemoteWeb_h

#include "configs.h"
#include <Arduino.h>
#include <WiFi.h>
#include "ESPAsyncWebServer.h"

// Simple remote control panel: creates a SoftAP ("ESP32HACKER") and serves a
// web page from which a phone browser can run any Marauder CLI command and view
// scan results. Only one of {RemoteWeb, Evil Portal} should be active at a time.
class RemoteWeb {
  public:
    bool active = false;
    void toggle();
    void start(bool show_info = true);
    void stop();
    void main();   // call from loop(): runs any command queued by the web server

    // Command queued from the async web handler, executed in the main loop.
    // (Running CLI commands directly in the AsyncWebServer task overflows its
    // small stack and reboots the ESP32, so we only queue here and run later.)
    volatile bool pending = false;
    char pending_cmd[160] = {0};

  private:
    AsyncWebServer* server = nullptr;
    void setupRoutes();
};

extern RemoteWeb remote_web_obj;

#endif
