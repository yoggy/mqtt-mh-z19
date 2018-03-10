//
//  mqtt-mh-z19.ino - test program for esp-wroom-02 & mh-z19
//
//  Requirements:
//    ESP-WROOM-02 development board
//      https://www.switch-science.com/catalog/2500/
//    MH-Z19
//      http://www.winsen-sensor.com/products/ndir-co2-sensor/mh-z19.html
//    Arduino Client for MQTT
//      https://github.com/knolleary/pubsubclient/
//    plerup/espsoftwareserial
//      https://github.com/plerup/espsoftwareserial
//    
//  How to use:
//
//    $ git clone https://github.com/yoggy/mqtt-sht31-ccs811.git
//    $ cd mqtt-sht31-ccs811
//    $ cp config.ino.sample config.ino
//    $ vi config.ino
//       - edit wifi_ssid, wifi_password, mqtt_server, mqtt_publish_topic, ... etc
//    $ open mqtt-sht31-ccs811.ino
//
//  License:
//    Copyright (c) 2018 yoggy <yoggy0@gmail.com>
//    Released under the MIT license
//    http://opensource.org/licenses/mit-license.php;
//

/////////////////////////////////////////////////////////////////////////////

void reboot() {
  Serial.println("REBOOT!!!!!");
  delay(1000);

  ESP.reset();

  while (true) {
    Serial.println("REBOOT!!!!!");
    delay(500);
  };
}

unsigned long last_updated_t;

void clear_time() {
  last_updated_t = millis();
}

unsigned long diff_time() {
  return millis() - last_updated_t;
}

/////////////////////////////////////////////////////////////////////////////

#include <SoftwareSerial.h>  // https://github.com/plerup/espsoftwareserial
SoftwareSerial sws = SoftwareSerial(4, 5); // RX, TX
byte buf[9];

void setup_mh_z19() {
  sws.begin(9600);
}

int read_data_from_mh_z19() {
  for (int i = 0; i < 10; ++i) {
    // send command
    sws.write((uint8_t)0xff);
    sws.write((uint8_t)0x01);
    sws.write((uint8_t)0x86);
    sws.write((uint8_t)0x00);
    sws.write((uint8_t)0x00);
    sws.write((uint8_t)0x00);
    sws.write((uint8_t)0x00);
    sws.write((uint8_t)0x00);
    sws.write((uint8_t)0x79);
    sws.flush();

    // read
    memset(buf, 0, 9);
    sws.readBytes(buf, 9);

    // parse
    if (buf[0] == 0xff && buf[1] == 0x86) {
      int co2 = buf[2] * 256 + buf[3];
      return co2;
    }
    delay(100);
  }
  return -1;
}

/////////////////////////////////////////////////////////////////////////////
#include <ESP8266WiFi.h>
#include <PubSubClient.h>   // https://github.com/knolleary/pubsubclient/

// Wi-fi config (from config.ino)
extern char *wifi_ssid;
extern char *wifi_password;
extern char *mqtt_server;
extern int  mqtt_port;

extern char *mqtt_client_id;
extern bool mqtt_use_auth;
extern char *mqtt_username;
extern char *mqtt_password;

extern char *mqtt_publish_topic;

WiFiClient wifi_client;
PubSubClient mqtt_client(mqtt_server, mqtt_port, NULL, wifi_client);

void setup_mqtt()
{
  Serial.println("setup_mqtt() : start");

  WiFi.begin(wifi_ssid, wifi_password);
  WiFi.mode(WIFI_STA);

  int wifi_count = 0;
  while (WiFi.status() != WL_CONNECTED) {
    wifi_count ++;
    delay(300);
    if (wifi_count > 100) reboot();
  }

  Serial.println("setup_wifi() : wifi connected");

  Serial.println("setup_wifi() : mqtt connecting");
  bool rv = false;
  if (mqtt_use_auth == true) {
    rv = mqtt_client.connect(mqtt_client_id, mqtt_username, mqtt_password);
  }
  else {
    rv = mqtt_client.connect(mqtt_client_id);
  }
  if (rv == false) {
    Serial.println("setup_wifi() : mqtt connecting failed...");
    reboot();
  }
  Serial.println("setup_wifi() : mqtt connected");

  Serial.println("setup_wifi() : success");
}

void publish_message(int co2) {
  char msg[128];
  memset(msg, 0, 128);
  snprintf(msg, 128, "{\"co2\":%d}", co2);

  Serial.print("mqtt_publish : ");
  Serial.println(msg);

  mqtt_client.publish(mqtt_publish_topic, msg);
}

/////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);
  Serial.println("setup()");

  setup_mqtt();
  setup_mh_z19();
}

void loop() {
  if (!mqtt_client.connected()) {
    Serial.println("MQTT disconnected...");
    reboot();
  }
  mqtt_client.loop();

  // every 3sec
  if (diff_time() > 3000) {
    int co2 = read_data_from_mh_z19();
    Serial.print("co2 = ");
    Serial.print(co2);
    Serial.print(" ppm");
    Serial.println();

    if (co2 > 0) {
      publish_message(co2);
      clear_time();
    }
    else {
      Serial.println("read failed...");
    }
  }

  if (diff_time() > 7000) {
    reboot();
  }
}


