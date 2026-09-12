#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266SSDP.h>
#include <ArduinoOTA.h>

const char* ap_ssid = "ESP8266_Sec_Hub";
const char* ap_pass = "admin12345";

ESP8266WebServer server(80);

void handleRoot() {
  String html = "<html><head><title>IoT Security Dashboard</title></head>";
  html += "<body style='font-family:sans-serif; background:#1a1a1a; color:#00ffcc; padding:20px;'>";
  html += "<h2>NodeMCU 2 Network & Dashboard Master</h2>";
  html += "<p>Status: Active Captive Portal</p>";
  html += "<h3>Subnet Services Discovered:</h3>";
  html += "<ul><li>Smart TV / Chromecast nodes (Active)</li><li>Router Gateway: 192.168.4.1</li></ul>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.softAP(ap_ssid, ap_pass);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  if (MDNS.begin("esp8266hub")) {
    Serial.println("mDNS responder started: esp8266hub.local");
  }

  server.on("/", handleRoot);
  server.begin();
  Serial.println("Web server active.");

  ArduinoOTA.setHostname("esp8266-hub-ota");
  ArduinoOTA.begin();
}

void loop() {
  server.handleClient();
  ArduinoOTA.handle();
  MDNS.update();
}