#include <MFRC522.h>
#include <SPI.h>

#define SS_PIN    21
#define RST_PIN   22

#define SIZE_BUFFER     18
#define MAX_SIZE_BLOCK  16

#define pinVerde     12
#define pinVermelho  32

// Objeto para autenticação
MFRC522::MIFARE_Key key;
// Status da operação de autenticação
MFRC522::StatusCode status;

// Inicialização do módulo RFID com pinos definidos
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);
  SPI.begin();

  pinMode(pinVerde, OUTPUT);
  pinMode(pinVermelho, OUTPUT);

  // Inicializa o módulo RFID
  mfrc522.PCD_Init();
  
  Serial.println("Aproxime o seu cartao do leitor...");
  Serial.println();
}

void loop() {
  // Verifica se há um novo cartão presente
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  // Lê a identificação do cartão
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Obtém a escolha do usuário a partir do menu
  int opcao = menu();

  // Realiza a operação de acordo com a escolha
  if (opcao == 0)
    leituraDados();
  else if (opcao == 1)
    gravarDados();
  else {
    Serial.println(F("Opção Incorreta!"));
    return;
  }

  // Encerra a comunicação com o cartão
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

// Realiza a leitura dos dados do cartão/tag
void leituraDados() {
  // Imprime os detalhes do cartão/tag
  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid));

  // Configuração da chave de autenticação
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  // Buffer para armazenar os dados lidos
  byte buffer[SIZE_BUFFER] = {0};
  // Bloco a ser lido
  byte bloco = 1;
  // Tamanho dos dados lidos
  byte tamanho = SIZE_BUFFER;

  // Autentica o cartão/tag para leitura
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, bloco, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    digitalWrite(pinVermelho, HIGH);
    delay(1000);
    digitalWrite(pinVermelho, LOW);
    return;
  }

  // Lê os dados do bloco
  status = mfrc522.MIFARE_Read(bloco, buffer, &tamanho);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    digitalWrite(pinVermelho, HIGH);
    delay(1000);
    digitalWrite(pinVermelho, LOW);
    return;
  } else {
    digitalWrite(pinVerde, HIGH);
    delay(1000);
    digitalWrite(pinVerde, LOW);
  }

  // Imprime os dados lidos
  Serial.print(F("\nDados bloco ["));
  Serial.print(bloco);
  Serial.print(F("]: "));
  for (uint8_t i = 0; i < MAX_SIZE_BLOCK; i++) {
    Serial.write(buffer[i]);
  }
  Serial.println(" ");
}

// Realiza a gravação dos dados no cartão/tag
void gravarDados() {
  // Imprime os detalhes do cartão/tag
  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid));
  // Define um timeout de 30 segundos para entrada de dados
  Serial.setTimeout(30000L);
  Serial.println(F("Insira os dados a serem gravados com o caractere '#' ao final\n[máximo de 16 caracteres]:"));

  // Configuração da chave de autenticação
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  // Buffer para armazenar os dados a serem gravados
  byte buffer[MAX_SIZE_BLOCK] = "";
  // Bloco a ser gravado
  byte bloco;
  // Tamanho dos dados inseridos
  byte tamanhoDados;

  // Lê os dados inseridos até encontrar o caractere '#'
  tamanhoDados = Serial.readBytesUntil('#', (char*)buffer, MAX_SIZE_BLOCK);
  // Preenche o restante do buffer com espaços em branco
  for (byte i = tamanhoDados; i < MAX_SIZE_BLOCK; i++) {
    buffer[i] = ' ';
  }

  // Define o bloco de gravação e converte o buffer em string
  bloco = 1;
  String str = (char*)buffer;
  Serial.println(str);

  // Autentica o cartão/tag para gravação
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
                                    bloco, &key, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    digitalWrite(pinVermelho, HIGH);
    delay(1000);
    digitalWrite(pinVermelho, LOW);
    return;
  }
  
  // Realiza a gravação dos dados no bloco
  status = mfrc522.MIFARE_Write(bloco, buffer, MAX_SIZE_BLOCK);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    digitalWrite(pinVermelho, HIGH);
    delay(1000);
    digitalWrite(pinVermelho, LOW);
    return;
  } else {
    Serial.println(F("MIFARE_Write() success: "));
    digitalWrite(pinVerde, HIGH);
    delay(1000);
    digitalWrite(pinVerde, LOW);
  }
}

// Exibe um menu para escolha da operação
int menu() {
  Serial.println(F("\nEscolha uma opção:"));
  Serial.println(F("0 - Leitura de Dados"));
  Serial.println(F("1 - Gravação de Dados\n"));

  // Aguarda até que o usuário envie dados pela porta serial
  while (!Serial.available()) {}

  // Lê a opção enviada pelo usuário
  int op = (int)Serial.read();
  // Remove os caracteres extras (como o 'enter' ou '\n')
  while (Serial.available()) {
    if (Serial.read() == '\n') break;
    Serial.read();
  }
  // Retorna a opção convertida para um valor numérico
  return (op - 48);
}
