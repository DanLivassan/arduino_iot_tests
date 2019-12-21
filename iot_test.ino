#include "SoftwareSerial.h"

SoftwareSerial ESP_Serial(10, 11); // RX, TX

String rede = "SANTANA2g";
String pass = "Santana0905";
String resposta = "";
String server = "68.183.130.180";
String endpoint = "/iot/";
void setup(){
  Serial.begin(9600);
  ESP_Serial.begin(9600);
  
  initialize_wifi_module(1000);
  if (connect_to_wifi(rede, pass, 10000)){
    send_get_request(server, endpoint, "");  
  }
  


  
}

void loop(){
   
}


bool initialize_wifi_module(int timeout){
  Serial.println("Resetando o modulo ESP8266");
  sendCommand("AT+RST");
  Serial.println(readResponse(timeout, false));
  delay(1000);
}

bool connect_to_wifi(String ssid, String pass, int timeout){
  clean_buffer(6000);
  Serial.println("Conectando a internet...");
  delay(1000);
  sendCommand("AT+CWJAP=\""+ssid+"\",\""+pass+"\"");
  delay(5000);
  Serial.println(readResponse(10000, false));
  if (resposta.indexOf("OK") == -1) { //procura na resposta se houve OK
    Serial.println("Nao foi possivel conectar a rede WiFi: "+ssid);
    Serial.println("Verifique se o nome da rede e senha foram preenchidos corretamente no codigo e tente novamente.");
    return false;
  } else {
    Serial.println("Sucesso! Conectado a rede WiFi.");
    return true;
  }
}


void send_get_request(String url, String endpoint, String params){
  sendCommand("AT+CIPSTART=\"TCP\",\""+url+"\",8000");
  Serial.println(readResponse(5000, true));
  String request = format_get_request(endpoint);
  int len = request.length();
  String cmd = "";
  cmd+="AT+CIPSEND="+String(len);
  sendCommand(cmd);
  Serial.println(readResponse(5000, true));
  sendCommand(request);
  Serial.println(readResponse(5000, true));
}

String format_get_request(String endpoint){
  return "GET "+endpoint+" HTTP/1.1\r\n\r\n";  
}

void sendCommand(String cmd) {
  ESP_Serial.println(cmd);
}

void clean_buffer(int timeout){
  unsigned long timeIn = millis();
  while (timeIn + timeout > millis()){
    if(ESP_Serial.available()){
      Serial.print(char(ESP_Serial.read()));  
    }
  }
}

String readResponse(unsigned int timeout, bool wait) {
  unsigned long timeIn = millis();
  resposta = "";
  while (timeIn + timeout > millis()) {
    if (ESP_Serial.available()) {
      char c = ESP_Serial.read();
      resposta += c;
    }
    else{
      if (!wait){
        return resposta;
      }
    }
  }
  return resposta;
}

