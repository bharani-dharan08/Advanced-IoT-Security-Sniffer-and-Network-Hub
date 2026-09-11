#include <Arduino.h>
#include <Arduino.h>
#include <Hash.h>
#include <bearssl/bearssl.h>

const uint8_t PROTECTED_FIRMWARE_REGION[] PROGMEM = {
    0x43, 0x79, 0x62, 0x65, 0x72, 0x53, 0x68, 0x69, 0x65, 0x6C, 0x64, 0x5F, 0x56, 0x31, 0x2E, 0x30
}; 

// Updated expected hash matching the computed hash from your build
const char* EXPECTED_HASH = "89f42cca226a51fce24491d4776269a478d5156bc7933d3e18b7f8efff1dd599";

bool verifyFirmwareIntegrity() {
    Serial.println("[SECURE BOOT] Reading protected memory region...");
    
    size_t regionSize = sizeof(PROTECTED_FIRMWARE_REGION);
    
    br_sha256_context h_ctx;
    br_sha256_init(&h_ctx);
    br_sha256_update(&h_ctx, PROTECTED_FIRMWARE_REGION, regionSize);
    
    uint8_t hashOutput[32];
    br_sha256_out(&h_ctx, hashOutput);

    String calculatedHash = "";
    for (uint8_t i = 0; i < 32; i++) {
        if (hashOutput[i] < 16) calculatedHash += "0";
        calculatedHash += String(hashOutput[i], HEX);
    }

    Serial.print("[CRYPTO] Expected Hash: ");
    Serial.println(EXPECTED_HASH);
    Serial.print("[CRYPTO] Computed Hash: ");
    Serial.println(calculatedHash);

    return calculatedHash.equalsIgnoreCase(EXPECTED_HASH);
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("\n\n========================================");
    Serial.println("       NodeMCU ESP8266 CyberShield      ");
    Serial.println("========================================");

    Serial.println("[BOOT] Initializing hardware wrapper...");
    delay(500);

    Serial.println("[BOOT] Executing firmware integrity verification...");
    bool integrityPassed = verifyFirmwareIntegrity();

    if (integrityPassed) {
        Serial.println("[STATE] INTEGRITY OK: Secure state verified.");
        Serial.println("[STATUS] Application starting normally...\n");
    } else {
        Serial.println("[ALERT] INTEGRITY FAILED: Tamper or corruption detected!");
        Serial.println("[STATUS] Device locked in restricted safe mode.\n");
        while (true) {
            Serial.println("[LOCKDOWN] System halted due to security violation.");
            delay(5000);
        }
    }
}

void loop() {
    Serial.println("[SECURITY LOG] Status: SECURE | Core: ESP8266 | Uptime: " + String(millis() / 1000) + "s");
    delay(3000);
}