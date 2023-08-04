#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "Lourenkko";
const char* password = "acks9282";

ESP8266WebServer server(80);

const int led = 5;
bool ledStatus = false;

void handleRoot() {
  String textoHTML;

  // Estilos CSS para a página
  textoHTML += "<style>";
  textoHTML += "body { font-family: Arial, sans-serif; text-align: center; }";
  textoHTML += "h1 { color: #007BFF; }";
  textoHTML += "button { padding: 10px 20px; font-size: 16px; border: none; cursor: pointer; margin: 10px; }";
  textoHTML += ".liga { background-color: #00CC00; color: white; }"; // Botão "Liga" verde
  textoHTML += ".desliga { background-color: #FF0000; color: white; }"; // Botão "Desliga" vermelho
  textoHTML += "</style>";

  textoHTML += "<h1>Curso Solu&#xE7;&#xF5;es Integradas com IOT</h1>";
  textoHTML += "<p>Controle o LED via Web Server clicando abaixo</p>";
  textoHTML += "<a href=\"/liga\"><button class=\"liga\">Liga</button></a>";
  textoHTML += "<a href=\"/desliga\"><button class=\"desliga\">Desliga</button></a>";

  server.send(200, "text/html", textoHTML);
}

void handleLiga() {
  digitalWrite(led, HIGH);
  ledStatus = true;
  handleRoot(); // Redireciona de volta para a página raiz após clicar no botão
}

void handleDesliga() {
  digitalWrite(led, LOW);
  ledStatus = false;
  handleRoot(); // Redireciona de volta para a página raiz após clicar no botão
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void){
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW); // Inicia com o LED desligado
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/liga", handleLiga); // Handler para acionar o LED (liga)
  server.on("/desliga", handleDesliga); // Handler para desligar o LED

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}
