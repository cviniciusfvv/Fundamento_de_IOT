#include "heltec.h"
#include "images.h"

#define BAND    868E6  // Definição da frequência de operação

unsigned int counter = 0;  // Variável para contar algo, mas não está sendo usada aqui
String rssi = "RSSI --";   // Variável para guardar a intensidade do sinal
String packSize = "--";    // Variável para guardar o tamanho do pacote
String packet ;            // Variável para guardar o conteúdo do pacote
bool irrigationOn = false; // Variável para armazenar o estado da irrigação

const int buttonPin = 12;  // Pino onde o botão está conectado
int variable = 0;          // Variável para armazenar um valor (0 ou 1) que será enviado

int buttonState = HIGH;      // Estado inicial do botão
int lastButtonState = HIGH;  // Estado anterior do botão

void logo()
{
  Heltec.display->clear();
  Heltec.display->drawXbm(0,5,logo_width,logo_height,logo_bits);  // Exibe um logotipo no display
  Heltec.display->display();
}

void setup()
{
  pinMode(buttonPin, INPUT_PULLUP);  // Configura o pino do botão como entrada com resistor de pull-up interno

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
  Heltec.display->drawString(90, 0, String(variable));  // Mostra o valor da variável
  
  // Exibe o status da irrigação
  if (irrigationOn) {
    Heltec.display->drawString(0, 20, "Irrigação: Ligada");
  } else {
    Heltec.display->drawString(0, 20, "Irrigação: Desligada");
  }
  
  Heltec.display->display();

  // Envio do pacote
  LoRa.beginPacket();
  LoRa.print(variable);
  LoRa.endPacket();
  
  buttonState = digitalRead(buttonPin);  // Lê o estado atual do botão

  if (buttonState != lastButtonState) {  // Verifica se houve mudança no estado do botão
    if (buttonState == LOW) {  // Verifica se o botão foi pressionado
      variable = 1 - variable;  // Alterna o valor da variável entre 0 e 1
      irrigationOn = !irrigationOn;  // Alterna o estado da irrigação
    }

    delay(50);  // Pequena pausa para evitar o efeito de bouncing (repiqueteio) do botão
  }

  lastButtonState = buttonState;  // Atualiza o estado anterior do botão
}
