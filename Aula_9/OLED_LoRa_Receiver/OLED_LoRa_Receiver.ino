#include "heltec.h"  // Inclui a biblioteca Heltec para usar as funcionalidades do dispositivo
#include "images.h"   // Inclui a biblioteca de imagens (provavelmente ícones para o display)

#define BAND    868E6  // Define a frequência de operação do rádio LoRa

String rssi = "RSSI --";    // Variável para guardar a intensidade do sinal
String packSize = "--";     // Variável para guardar o tamanho do pacote
String packet ;             // Variável para guardar o conteúdo do pacote
const int relayPin = 13;    // Pino para controlar o relé
bool irrigationOn = false;  // Variável para guardar o estado da irrigação

void logo(){
  Heltec.display->clear();   // Limpa o display
  Heltec.display->drawXbm(0,5,logo_width,logo_height,logo_bits);  // Exibe um logotipo no display
  Heltec.display->display();  // Atualiza o display
}

void LoRaData(){
  Heltec.display->clear();   // Limpa o display
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);  // Define o alinhamento do texto
  Heltec.display->setFont(ArialMT_Plain_10);         // Define a fonte do texto
  Heltec.display->drawString(0 , 15 , "Received "+ packSize + " bytes");  // Exibe informações sobre o tamanho do pacote
  Heltec.display->drawStringMaxWidth(0 , 26 , 128, packet);  // Exibe o conteúdo do pacote no display
  Heltec.display->drawString(0, 0, rssi);  // Exibe a intensidade do sinal
  Heltec.display->display();  // Atualiza o display
}

void cbk(int packetSize) {
  packet = "";  // Limpa o conteúdo do pacote
  packSize = String(packetSize,DEC);  // Converte o tamanho do pacote para String
  for (int i = 0; i < packetSize; i++) { packet += (char) LoRa.read(); }  // Lê o conteúdo do pacote
  rssi = "RSSI " + String(LoRa.packetRssi(), DEC);  // Lê a intensidade do sinal
  LoRaData();  // Atualiza o display com os dados recebidos
}

void setup() { 
  pinMode(relayPin, OUTPUT);  // Configura o pino do relé como saída

  // Inicialização do dispositivo Heltec WiFi Kit
  Heltec.begin(
    true /*DisplayEnable Enable*/,
    true /*Heltec.Heltec.Heltec.LoRa Disable*/,
    true /*Serial Enable*/,
    true /*PABOOST Enable*/,
    BAND /*long BAND*/
  );

  Heltec.display->init();  // Inicializa o display
  Heltec.display->flipScreenVertically();  // Inverte a orientação do display
  Heltec.display->setFont(ArialMT_Plain_10);  // Define a fonte do texto
  logo();  // Exibe o logotipo no display
  delay(1500);
  Heltec.display->clear();
  
  Heltec.display->drawString(0, 0, "Heltec.LoRa Initial success!");
  Heltec.display->drawString(0, 10, "Wait for incoming data...");
  Heltec.display->display();
  delay(1000);
  
  LoRa.receive();  // Entra no modo de recepção de dados LoRa
}

void loop() {
  int packetSize = LoRa.parsePacket();  // Verifica se há um pacote disponível para leitura
  if (packetSize) { cbk(packetSize); }  // Se houver um pacote, processa-o
  delay(10);  // Pequena pausa
}

void checkIrrigation() {  
  if (packet == "1") {  // Se o conteúdo do pacote for "1"
    if (!irrigationOn) {  // Se a irrigação ainda não estiver ligada
      irrigationOn = true;  // Liga a irrigação
      digitalWrite(relayPin, HIGH);  // Ativa o relé
    }
  } else {
    irrigationOn = false;  // Desliga a irrigação
    digitalWrite(relayPin, LOW);  // Desativa o relé
  }
}
