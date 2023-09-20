#include <BeeS3.h> //include the bee s3 helper library
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// ****TODO**** move to the secrets.h ?
const char* ssid = "YOURSSID";
const char* password = "YOURPASS";

BEES3 bees3;  //pull the BEES3 class from the helper and name it bees3 so we can use it below;
int color = 20; // orange
AsyncWebServer server(80);
// landing page delegate
void onIndexRequest(AsyncWebServerRequest *request) {
  Serial.println("[placeholder] HTTP GET request of ");
  request->send(200, "text/plain", "Hello World, its Arduino!");
}
// 404 delegate
void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not Found");
}

void setup() {
  bees3.begin();
  bees3.setPixelBrightness(255 / 2);
  Serial.begin(115200);
/*
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("WiFi fault\n");
    color = 100; // unhappy led
    return;
  }*/

  WiFi.softAP("ESP32-AP", "thisisarduino");
  Serial.println("AP running");

  server.on("/", HTTP_GET, onIndexRequest);
  server.onNotFound(notFound);
  server.begin();
}

void loop() {

  bees3.setPixelColor(BEES3::colorWheel(color));
  ////color++;
  ////bees3.setPixelColor(orange);  

  Serial.print("My IP address: ");
  ////Serial.println(WiFi.localIP());
  Serial.println(WiFi.softAPIP());
  delay(5000);
}