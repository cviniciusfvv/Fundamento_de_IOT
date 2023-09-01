#include <WiFi.h>
#include <IOXhop_FirebaseESP32.h>
#include <ArduinoJson.h>
#include <MFRC522.h>
#include <SPI.h>

#define WIFI_SSID "Redmi 9"
#define WIFI_PASSWORD "460592@La"
#define FIREBASE_HOST "https://senai-iot-8a4ec-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "GnEODOs3FiQLiAEjXxBkXRRHc5krO5NX4uyxoFwx"
#define SS_PIN 21
#define RST_PIN 22

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;

String nome = "";

void setup() {
  Serial.begin(115200);
  Serial.println();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  Serial.print("Conectando ao wifi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  SPI.begin();
  mfrc522.PCD_Init();
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  if (autenticar()) {
    lerNome();
    atualizarFirebase();
  }
  
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

bool autenticar() {
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Autenticação falhou: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return false;
  }

  return true;
}

void lerNome() {
  byte buffer[18];
  byte tamanho = 18;

  status = mfrc522.MIFARE_Read(1, buffer, &tamanho);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Leitura falhou: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  nome = "";
  for (byte i = 0; i < 16; i++) {
    if (buffer[i] != 0x00) {
      nome += (char)buffer[i];
    }
  }

  Serial.print(F("Nome lido: "));
  Serial.println(nome);
}

void atualizarFirebase() {
  Firebase.setString("/SaladeReuniao/Sala1", nome);
  Serial.println(F("Dados atualizados no Firebase."));
  delay(3000);
}
