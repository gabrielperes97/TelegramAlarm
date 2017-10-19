#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <list>

const byte ARMAR = 0x40;
const byte DESARMAR = 0x50;
const byte VERIFICAR = 0x60;
const byte DISPARAR = 0x70;

const byte ARMADO = 0x61;
const byte DESARMADO = 0x62;
const byte DISPARANDO = 0x63;


char ssid[] = "Meu WIFI";
char password[] = "aquela Senha";

std::list<String> inscritos;

#define BOTtoken "token do bot telegram"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int Bot_mtbs = 500; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done

int alarmStatus=0;//0 Desativado, 1 ativado, 2 disparando

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void sendToSubscribers(String msg)
{
  for (std::list<String>::iterator id=inscritos.begin(); id != inscritos.end(); ++id)
  {
    bot.sendMessage(*id, msg);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      for (int i=0; i<numNewMessages; i++) {
        if (bot.messages[i].text == "/armar")
        {
          Serial.write(ARMAR);
        }
        else if (bot.messages[i].text == "/desarmar")
        {
          Serial.write(DESARMAR);
        }
        else if (bot.messages[i].text == "/verificar")
        {
          Serial.write(VERIFICAR);    
        }
        else if (bot.messages[i].text == "/disparar")
        {
          Serial.write(DISPARAR);
        }
        else if (bot.messages[i].text == "/registrar")
        {         
          inscritos.push_back(bot.messages[i].chat_id);
          bot.sendMessage(bot.messages[i].chat_id, "Você foi inscrita");
        }
        else
        {
          bot.sendMessage(bot.messages[i].chat_id, "Eu sou meio burra, só entendo comandos básicos. Insira /armar /desarmar /verificar /disparar /registrar ou /desregistrar");
        }       
      }
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    Bot_lasttime = millis();
  }
  if (Serial.available())
  {
    byte b = Serial.read();
    switch(b)
    {
      case ARMADO:
        sendToSubscribers("O alarme está armado");
        break;
      case DESARMADO:
        sendToSubscribers("O alarme está desarmado");
        break;
      case DISPARANDO:
        sendToSubscribers("O alarme está disparando");
        break;
    }
  }

}
