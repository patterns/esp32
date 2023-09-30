#include <BeeS3.h> //include the bee s3 helper library
#include <WiFi.h>
#include <AsyncTCP.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <mbedtls/bignum.h>
#include <mbedtls/rsa.h>
#include <mbedtls/md.h>
#include <Base64.h>

// ****TODO**** move to the secrets.h ?
const char* ssid = "YOURSSID";
const char* password = "YOURPASS";
const size_t CAPACITY = JSON_OBJECT_SIZE(4);

BEES3 bees3;  //pull the BEES3 class from the helper and name it bees3 so we can use it below;
int color = 20; // orange
AsyncWebServer server(80);
// landing page delegate (see DFRobot article)
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
  request->send(200, "text/plain", "..... we just mbedTLS successfully!");
}
// 404 delegate
void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not Found");
}
// verify delegate
AsyncCallbackJsonWebHandler* pkcs1Verify = new AsyncCallbackJsonWebHandler("/verify",
  [](AsyncWebServerRequest *request, JsonVariant &jsv) {
  JsonObject jso = jsv.as<JsonObject>();

  // access field from json
  const char* mod = jso["modulus"];
  const char* exp = jso["exponent"];
  const char* sig = jso["signature"];
  const char* hashed = jso["hashed"];
  // debug info while dev-ing
  Serial.print("Verify: ");
  Serial.println(sig);
  int verify_fault = 0;

  mbedtls_rsa_context rsa_context;
  mbedtls_mpi mpi_n;
  mbedtls_mpi mpi_e;
  // pre mbedTLS 3.0 (see github.com/espressif/mbedtls/docs/3.0-migration-guide.md)
  mbedtls_rsa_init(&rsa_context, MBEDTLS_RSA_PKCS_V15, 0);
  mbedtls_mpi_init(&mpi_n);
  mbedtls_mpi_init(&mpi_e);
  // read hexadecimal form of N and E
  int read_n = mbedtls_mpi_read_string(&mpi_n, 16, mod);
  int read_e = mbedtls_mpi_read_string(&mpi_e, 16, exp);
  if (read_n != 0 || read_e != 0) verify_fault = 16;

  mbedtls_rsa_import(&rsa_context, &mpi_n, NULL, NULL, NULL, &mpi_e);
  int ready = mbedtls_rsa_complete(&rsa_context);
  if (ready != 0) verify_fault = 32;

  // the sig header is base64 and must be decoded
  int ilen = strlen(sig);
  int olen = Base64.decodedLength((char*)sig, ilen);
  char deco[olen + 1];

  int count = Base64.decode(deco, (char*)sig, ilen);
  //Serial.print("decoded sig header:\t");
  //Serial.println(deco);
  int status = 0;
  if (count != olen) verify_fault = 64;
  else {
    // the mbedTLS verify
    status = mbedtls_rsa_pkcs1_verify(&rsa_context, NULL, NULL, MBEDTLS_RSA_PUBLIC,
                        MBEDTLS_MD_SHA256, 32,
                        (const unsigned char*)hashed, (const unsigned char*)deco);
    if (status != 0) verify_fault = 128;
  }


  mbedtls_mpi_free(&mpi_e);
  mbedtls_mpi_free(&mpi_n);
  mbedtls_rsa_free(&rsa_context);
  switch (verify_fault) {
    case 16: Serial.println("Verify fault, modulus or exponent is not hex.");break;
    case 32: Serial.println("Verify fault, N/E import.");break;
    case 64: Serial.println("Verify fault, base64 decode.");break;
    case 128: Serial.println("Verify fault, signature mismatch.");break;
  }
  // pretty print for consumer
  AsyncResponseStream *response = request->beginResponseStream("application/json");
  response->print("{ \"hashed\":");
  response->printf(" \"%s\" ", hashed);
  response->print(", \"status\":");
  response->printf(" \"%s\" ", (status == 0)? "pass": "mismatch");

  response->print("}");

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