#include <BeeS3.h> //include the bee s3 helper library
#include <WiFi.h>
#include <AsyncTCP.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <mbedtls/bignum.h>
#include <mbedtls/rsa.h>
#include <mbedtls/md.h>

// ****TODO**** move to the secrets.h ?
const char* ssid = "YOURSSID";
const char* password = "YOURPASS";
const size_t CAPACITY = JSON_OBJECT_SIZE(4);

BEES3 bees3;  //pull the BEES3 class from the helper and name it bees3 so we can use it below;
int color = 20; // orange
AsyncWebServer server(80);
// landing page delegate
void onIndexRequest(AsyncWebServerRequest *request) {
  char payload[24] = "Hello SHA 256!";
  byte shaResult[32];
  mbedtls_md_context_t ctx;
  mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;
  const size_t payloadLength = strlen(payload);
  mbedtls_md_init(&ctx);
  mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
  mbedtls_md_starts(&ctx);
  mbedtls_md_update(&ctx, (const unsigned char *) payload, payloadLength);
  mbedtls_md_finish(&ctx, shaResult);
  mbedtls_md_free(&ctx);

  Serial.print("Hash: ");
  for(int i=0; i < sizeof(shaResult); i++) {
    char str[3];
    sprintf(str, "%02x", (int)shaResult[i]);
    Serial.print(str);
  }
  Serial.println();
  request->send(200, "text/plain", "..... did we just mbedTLS successfully?");
}
// 404 delegate
void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not Found");
}
// verify delegate
AsyncCallbackJsonWebHandler* pkcs1Verify = new AsyncCallbackJsonWebHandler("/verify",
  [](AsyncWebServerRequest *request, JsonVariant &jsv) {
  JsonObject jso = jsv.as<JsonObject>();
  ////StaticJsonDocument<CAPACITY> doc;
  ////deserializeJson(doc, jso);

  const char* pong = jso["sig"];

  AsyncResponseStream *response = request->beginResponseStream("application/json");
  //(baby step, echo json field)
  ////serializeJsonPretty(doc, *response);
  response->print("{ \"sig\":");
  response->printf(" \"%s\" ", pong);
  response->print("}");
  Serial.print("Verify: ");
  Serial.println(pong);  //assuming this is hex
  request->send(response);
});

void setup() {
  bees3.begin();
  bees3.setPixelBrightness(255 / 2);
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("WiFi fault\n");
    color = -100; // unhappy condition
    return;
  }
  server.on("/", HTTP_GET, onIndexRequest);
  server.addHandler(pkcs1Verify);
  server.onNotFound(notFound);
  server.begin();
}

void loop() {
  if (color < 0) {
    bees3.setPixelColor(BEES3::colorWheel(100));
    return;
  }

  color++;
  bees3.setPixelColor(BEES3::colorWheel(color));

  Serial.print("My IP address: ");
  Serial.println(WiFi.localIP());
  delay(5000);
}