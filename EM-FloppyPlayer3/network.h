#ifndef network_h
#define network_h
#include <WiFi.h>
#include <WebSocketsServer.h>

WebSocketsServer webSocket = WebSocketsServer(9999);

#include "utils.h"
// F0 9F 92 BE
const char *ssid = "\xf0\x9f\x92\xbe\xf0\x9f\x8E\xB9 Floppiano"; // whale emoji
// const char *ssid2 = "1F4BE FLOPPY_PIANO";
const char *password = "";

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{

  switch (type)
  {
  case WStype_DISCONNECTED:
    chn_flag[CHN_SOCKET] = 0;
    // Serial.printf("[%u] Disconnected!\n", num);
    break;
  case WStype_CONNECTED:
  {
    IPAddress ip = webSocket.remoteIP(num);
    // Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

    // send message to client
    webSocket.sendTXT(num, "1");
  }
  break;
  case WStype_TEXT:
    // Serial.printf("[%u] get Text: %s\n", num, payload);

    // send message to client
    // webSocket.sendTXT(num, "message here");

    // send data to all connected clients
    // webSocket.broadcastTXT("message here");
    if (payload[0] == 'a')
    {
      chn_flag[CHN_SOCKET] = 1;
    }
    else if (payload[0] == 'b')
    {
      chn_flag[CHN_SOCKET] = 0;
    }
    else
    {
      chn_key[CHN_SOCKET] = atoi((char *)payload);
    }
    break;
  case WStype_BIN:
    // Serial.printf("[%u] get binary length: %u\n", num, length);
    // hexdump(payload, length);

    // send message to client
    // webSocket.sendBIN(num, payload, length);
    break;
  case WStype_ERROR:
  case WStype_FRAGMENT_TEXT_START:
  case WStype_FRAGMENT_BIN_START:
  case WStype_FRAGMENT:
  case WStype_FRAGMENT_FIN:
    break;
  }
}

void task_ws(void *)
{
  while (true)
  {
    webSocket.loop();
    vTaskDelay(1);
  }
}

void setup_network()
{
  WiFi.softAP(ssid, password);
  Serial.println(WiFi.localIP());
  // Route for root / web page
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  xTaskCreate(task_ws, "task_ws", 4096, NULL, 5, NULL);
}
#endif