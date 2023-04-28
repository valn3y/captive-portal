#include <FS.h>
#include <ArduinoJson.h>
#include "config.h"
#include "functions.h"
#include "filesystem.h"
#include "jsonfiles.h"
#include "webfiles.h"
#include "servingWebPages.h"
#include "blinkPin.h"

unsigned long previousMillis = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);

  // BLINK FOR SOME MILLISECONDS

  start_spi();
  logging("----- BOOTING ----- ");
  logging("setup :: File system started ")

  logging("setup :: Starting Load settings... ");
  load_settings()
  create_system()
  logging("setup :: Load settings done!!! ");

  logging("setup :: Starting Copy web files... ");
  copy_web_files(true);
  logging("setup :: Copy web files done!!! ");

  logging("Setup :: Starting config web services... ");
  start_ap();
  logging("Setup :: Config web services done!!! ");

  // BLINK 3 TIMES OR MORE
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
  httpServer.handleClient();
  led_msg(LED);

  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis >= 2000) {
    previousMillis = currentMillis;
    client_status();
  }
}
