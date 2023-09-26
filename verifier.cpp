#include <BeeS3.h> //include the bee s3 helper library
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <mbedtls/platform.h>
#include <mbedtls/sha256.h>
#include <mbedtls/sha512.h>
#include <mbedtls/oid.h>
#include <mbedtls/entropy.h>
#include <mbedtls/error.h>
#include <mbedtls/md.h>
#include <mbedtls/bignum.h>
#include <mbedtls/hmac_drbg.h>


// ****TODO**** move to the secrets.h ?
const char* ssid = "YOURSSID";
const char* password = "YOURPASS";

BEES3 bees3;  //pull the BEES3 class from the helper and name it bees3 so we can use it below;
int color = 20; // orange
AsyncWebServer server(80);
// landing page delegate
void onIndexRequest(AsyncWebServerRequest *request) {
  ////Serial.println("[placeholder] HTTP GET request of ");
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