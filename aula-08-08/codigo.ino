
#include <DHT.h>             // Aqui, estamos dizendo ao esp para incluir a biblioteca do DHT.
#include <ESP8266WiFi.h>     // Estamos trazendo outra ferramenta para nos ajudar a se conectar à internet.

String apiKey = "V3T6NLNK15JTEM3P";  // Essa é uma senha secreta que nos permite enviar as informações para o site.

const char *ssid =  "Redmi 9";   // O nome da rede Wi-Fi que queremos conectar.
const char *pass =  "460592@La"; // A senha da rede Wi-Fi.

const char* server = "api.thingspeak.com"; // O endereço do site onde queremos enviar as informações.

#define DHTPIN 4  // O lugar onde o sensor está conectado no Esp.

DHT dht(DHTPIN, DHT11); // Estamos dizendo ao computador que queremos usar o sensor que está conectado no pino D2.

WiFiClient client; // Essa é outra ferramenta que usamos para conversar com a internet.

void setup() {
       Serial.begin(115200);  // Preparamos o computador para conversar conosco.
       delay(10);             // Esperamos um pouquinho para nos prepararmos.

       dht.begin();           // Ligamos o sensor para ele começar a medir.

       Serial.println("Connecting to "); // Mostramos na tela que estamos tentando nos conectar à rede Wi-Fi.
       Serial.println(ssid);             // Mostramos o nome da rede.

       WiFi.begin(ssid, pass);  // Começamos a nos conectar à rede Wi-Fi.

      while (WiFi.status() != WL_CONNECTED)  // Enquanto não estivermos conectados...
     {
            delay(500);         // Esperamos um pouco.
            Serial.print(".");  // Mostramos um ponto na tela.
     }
      Serial.println("");      // Pulamos uma linha na tela.
      Serial.println("WiFi connected"); // Mostramos que conseguimos nos conectar à rede Wi-Fi.

}

void loop() {
      float h = dht.readHumidity();  // Pedimos ao sensor para nos dizer qual é a umidade.
      float t = dht.readTemperature(); // Pedimos ao sensor para nos dizer qual é a temperatura.

      if (isnan(h) || isnan(t))  // Se não conseguirmos medir a umidade ou temperatura...
      {
          Serial.println("Failed to read from DHT sensor!");  // Mostramos que algo deu errado.
          return;  // E paramos por aqui.
      }

      if (client.connect(server,80))  // Se conseguirmos nos conectar ao site onde queremos enviar as informações...
      {  
          String postStr = apiKey;  // Pegamos a nossa senha secreta.
          postStr +="&field1=";     // E adicionamos na mensagem.
          postStr += String(t);     // Adicionamos a temperatura na mensagem.
          postStr +="&field2=";     // E dizemos que agora vem a umidade.
          postStr += String(h);     // Adicionamos a umidade na mensagem.
          postStr += "\r\n\r\n";    // Damos espaço para terminar a mensagem.

          client.print("POST /update HTTP/1.1\n");   // Começamos a escrever a mensagem.
          client.print("Host: api.thingspeak.com\n"); // Dizemos para onde estamos enviando.
          client.print("Connection: close\n");        // Dizemos que vamos fechar a conexão quando terminarmos.
          client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n"); // Damos a senha secreta para o site entender.
          client.print("Content-Type: application/x-www-form-urlencoded\n"); // Dizemos que tipo de mensagem é.
          client.print("Content-Length: ");  // Dizemos o tamanho da mensagem.
          client.print(postStr.length());    // Mostramos o tamanho.
          client.print("\n\n");              // Damos espaço para começar a mensagem.
          client.print(postStr);             // Finalmente, enviamos a mensagem.

          Serial.print("Temperature: ");  // Mostramos na tela a temperatura.
          Serial.print(t);                // Mostramos a temperatura.
          Serial.print(" degrees Celcius, Humidity: "); // Mostramos que agora vem a umidade.
          Serial.print(h);                // Mostramos a umidade.
          Serial.println("%. Send to Thingspeak.");  // Dizemos que enviamos as informações para o site.
      }
      client.stop();  // Terminamos de conversar com o site.

      Serial.println("Waiting...");  // Mostramos na tela que estamos esperando um pouco.

      delay(1000); // Esperamos um segundo antes de fazer tudo de novo.
}