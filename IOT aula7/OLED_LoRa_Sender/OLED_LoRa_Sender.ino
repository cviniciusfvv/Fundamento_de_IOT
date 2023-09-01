#include "heltec.h"
#include "images.h"

#define BAND    868E6  // Definição da frequência de operação

unsigned int counter = 0;  // Variável para contar algo, mas não está sendo usada aqui
String rssi = "RSSI --";   // Variável para guardar a intensidade do sinal
String packSize = "--";    // Variável para guardar o tamanho do pacote
String packet ;            // Variável para guardar o conteúdo do pacote
//bool irrigationOn = false; // Variável para armazenar o estado da irrigação
String chave = "IGR"
//Chave para processar apenas 
const int buttonPin = 2;  // Pino onde o botão está conectado
int potred = 0; // Variavel para armazenar o valor lido pelo potênciometro
int lastreadpot = 0;  // Último valor lido pelo potênciometro

void logo()
{
  Heltec.display->clear();
  Heltec.display->drawXbm(0,5,logo_width,logo_height,logo_bits);  // Exibe um logotipo no display
  Heltec.display->display();
}

void setup()
{

  Heltec.begin(true, true, true, true, BAND);  // Inicialização do dispositivo Heltec WiFi Kit
  
  Heltec.display->init();
  Heltec.display->flipScreenVertically();  
  Heltec.display->setFont(ArialMT_Plain_10);
  logo();
  delay(1500);
  Heltec.display->clear();
  
  Heltec.display->drawString(0, 0, "Heltec.LoRa Initial success!");
  Heltec.display->display();
  delay(1000);
}

void loop()
{
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);
  
  Heltec.display->drawString(0, 0, "Enviando msg: ");
  Heltec.display->drawString(0, 20, "Valor lido pelo potênciômetro");
  delay(50);
  Heltec.display->drawString(90, 0, String(potred));  // Mostra o valor lido do potênciometro
  

  Heltec.display->display();

  // Envio do pacote
  LoRa.beginPacket();
  LoRa.print(potred,chave);
  LoRa.endPacket();
  
  portred = analogRead(buttonPin);  // Lê o estado atual do botão

  if (poteread != lastreadpot) {  // Verifica se houve mudança no estado do botão
    portred = analogRead(buttonPin);
    delay(50);  // Pequena pausa para evitar o efeito de bouncing (repiqueteio) do botão
  }

  poteread = lastreadpot;  // Atualiza o estado anterior do botão
}
