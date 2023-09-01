#include <WiFi.h>
#include <IOXhop_FirebaseESP32.h>
#include <ArduinoJson.h>
#include <MFRC522.h>
#include <SPI.h>

// Configuração da rede Wi-Fi
#define WIFI_SSID "*******"           // Nome da rede Wi-Fi
#define WIFI_PASSWORD "*******"     // Senha da rede Wi-Fi

// Configurações do Firebase
#define FIREBASE_HOST "https://senai-iot-8a4ec-default-rtdb.firebaseio.com/" // URL do projeto Firebase
#define FIREBASE_AUTH "GnEODOs3FiQLiAEjXxBkXRRHc5krO5NX4uyxoFwx"          // Chave de autenticação

// Configurações do módulo RFID
#define SS_PIN 21 // Pino SS do módulo RFID
#define RST_PIN 22 // Pino RST do módulo RFID

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;

String nome = "";

void setup() {
  Serial.begin(115200);
  Serial.println();

  // Conectar à rede Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();

  // Iniciar conexão com o Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  SPI.begin();
  mfrc522.PCD_Init();
}

void loop() {
  // Verificar se há um novo cartão presente
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Ler a identificação do cartão
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Se a autenticação for bem-sucedida, ler o nome e atualizar o Firebase
  if (autenticar()) {
    lerNome();
    atualizarFirebase();
  }
  
  // Encerrar a comunicação com o cartão
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

// Função para autenticar o cartão RFID
bool autenticar() {
  // Configurar a chave de autenticação
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  // Autenticar o cartão usando a chave configurada
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Autenticação falhou: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return false;
  }

  return true;
}

// Função para ler o nome do cartão RFID
void lerNome() {
  byte buffer[18];
  byte tamanho = 18;

  // Ler os dados do cartão
  status = mfrc522.MIFARE_Read(1, buffer, &tamanho);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Leitura falhou: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  nome = "";
  for (byte i = 0; i < 16; i++) {
    // Construir o nome a partir dos dados lidos
    if (buffer[i] != 0x00) {
      nome += (char)buffer[i];
    }
  }

  Serial.print(F("Nome lido: "));
  Serial.println(nome);
}

// Função para atualizar os dados no Firebase
void atualizarFirebase() {
  // Atualizar o Firebase com o nome lido
  Firebase.setString("/SaladeReuniao/Sala1", nome);
  Serial.println(F("Dados atualizados no Firebase."));
  delay(3000);
}
