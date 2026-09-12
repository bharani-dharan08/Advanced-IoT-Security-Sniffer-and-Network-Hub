#include <ESP8266WiFi.h>
#include <LittleFS.h>

struct SnifferPacket {
  struct {
    uint16_t frm_ctl;
    uint16_t duration;
    uint8_t addr1[6];
    uint8_t addr2[6];
    uint8_t addr3[6];
    uint16_t seq_ctl;
    uint8_t addr4[6];
  } rx_ctrl;
  uint8_t payload[112];
};

void logToFlash(String filename, String logData) {
  File file = LittleFS.open(filename, "a");
  if (file) {
    file.println(logData);
    file.close();
  }
}

void prom_cb(unsigned char* buf, uint16_t len) {
  if (len < 12) return;
  struct SnifferPacket *packet = (struct SnifferPacket*)buf;
  uint8_t frameType = (packet->payload[0] >> 2) & 0x03;
  uint8_t frameSubtype = (packet->payload[0] >> 4) & 0x0F;

  if (frameType == 0 && frameSubtype == 12) {
    String deauthLog = "[DEAUTH ATTACK] Packet len: " + String(len);
    Serial.println(deauthLog);
    logToFlash("/security_log.txt", deauthLog);
  }
}

void scanForEvilTwins() {
  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; ++i) {
    String ssid = WiFi.SSID(i);
    String bssid = WiFi.BSSIDstr(i);
    for (int j = i + 1; j < n; ++j) {
      if (WiFi.SSID(j) == ssid && WiFi.BSSIDstr(j) != bssid) {
        String twinAlert = "EVIL TWIN SUSPECTED -> SSID: " + ssid;
        Serial.println(twinAlert);
        logToFlash("/security_log.txt", twinAlert);
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  LittleFS.begin();
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  wifi_set_promiscuous_rx_cb(&prom_cb);
  wifi_promiscuous_enable(1);
  wifi_set_channel(1);
  Serial.println("[Node 1] Sniffer Active.");
}

void loop() {
  for (int ch = 1; ch <= 13; ch++) {
    wifi_set_channel(ch);
    delay(400);
  }
  scanForEvilTwins();
}