#include "WiFi.h"
#include "HTTPClient.h"
#include "string.h"

class DateTime{
  private:
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
  public:
    DateTime() : year(2000), month(1), day(1), hour(0), minute(0), second(0){}
    DateTime(String payload){
      this->force(payload);
    }
  
    void force(String payload){
        this->year = payload[30] - 48;
        this->year = this->year * 10 + (payload[31] - 48);
        this->year = this->year * 10 + (payload[32] - 48);
        this->year = this->year * 10 + (payload[33] - 48);

        this->month = payload[35] - 48;
        this->month = this->month * 10 + (payload[36] - 48);

        this->day = payload[38] - 48;
        this->day = this->day * 10 + (payload[39] - 48);

        this->hour = payload[41] - 48;
        this->hour = this->hour * 10 + (payload[42] - 48);

        this->minute = payload[44] - 48;
        this->minute = this->minute * 10 + (payload[45] - 48);

        this->second = 0;
    }

    void go(){
      this->second++;

      if(this->second == 60){
        this->second = 0;
        this->minute++;
      }

      if(this->minute == 60){
        this->minute = 0;
        this->hour++;  
      }

      if(this->hour == 24){
        this->hour = 0;
        this->day++;  
      }

      switch(this->day){
        case 28:
          if(this->year % 4 != 0 && this->month == 2){
            this->day = 1;
            this->month++;  
          }
          break;
        case 29:
          if(this->year % 4 == 0 && this->month == 2){
            this->day = 1;
            this->month++;  
          }
          break;
        case 30:
          switch(this->month){
            case 4:
            case 6:
            case 9:
            case 11:
              this->day = 1;
              this->month++;
              break;
          }
          break;
         case 31:
         switch(this->month){
            case 1:
            case 3:
            case 5:
            case 7:
            case 8:
            case 10:
            case 12:
              this->day = 1;
              this->month++;
              break;
          }
          break;  
      }

      if(this->month == 12){
        this->month = 1;
        this->year++;  
      }
    }

    void show(){
      if(this->day < 10){
        Serial.print(0);  
      }
      Serial.print(this->day);
      Serial.print(".");
      
      if(this->month < 10){
        Serial.print(0);  
      }
      Serial.print(this->month);
      Serial.print(".");

      Serial.print(this->year);
      Serial.print(" ");

      if(this->hour < 10){
        Serial.print(0);  
      }
      Serial.print(this->hour);
      Serial.print(":");

      if(this->minute < 10){
        Serial.print(0);  
      }
      Serial.print(this->minute);
      Serial.print(":");

      if(this->second < 10){
        Serial.print(0);  
      }
      Serial.print(this->second);
      Serial.println();
    }
};

void connectToWifi(bool debug = false){
  while(Serial.available() == 0) {}
  Serial.println("SSID registered from serial.");
  String ssid = Serial.readString();
  char ssid_buf[50];
  ssid.toCharArray(ssid_buf, 50);
  ssid_buf[strlen(ssid_buf) - 1] = '\0';
  Serial.flush();

  while(Serial.available() == 0) {}
  Serial.println("Password registered from serial.");
  String password = Serial.readString();
  char password_buf[50];
  password.toCharArray(password_buf, 50);
  password_buf[strlen(password_buf) - 1] = '\0';
  Serial.flush();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid_buf, password_buf);

  unsigned long timeout = 20000;
  unsigned long start = millis();

  if(debug){
    Serial.println(ssid_buf);
    Serial.println(password_buf);
  }
  while(WiFi.status() != WL_CONNECTED && (millis() - start) < timeout){
    Serial.print(".");
    delay(1000);
  }

  Serial.println();
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("Connection failed!");  
  }else{
    Serial.println("Connected!");  
  }
}

String getPayloadByHTTPRequest(){
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient client;
    client.begin("http://worldclockapi.com/api/json/eest/now");
    int httpCode = client.GET();
    
    if(httpCode > 0){
      return client.getString();
    }else{
      Serial.println("HTTP request failed.");  
    }
  }else{
    Serial.println("Cannot make HTTP request as board is not connected.");  
  }

  return "";
}

DateTime dt;

void setup() {
  Serial.begin(115200);
  connectToWifi();
  
  String payload = getPayloadByHTTPRequest();
  if(payload != ""){
    dt.force(payload);
  }
}

void loop() {
  dt.show();
  delay(1000);
  dt.go();
}
