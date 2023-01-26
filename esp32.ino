#include <PubSubClient.h>
#include <WiFi.h>
#include <esp_wifi.h>

uint8_t newMACAddress[] = {0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf};
const char *ssid = "K2FeO4";
const char *password = "lxz123ac";
const char *mqtt_server = "bemfa.com"; //onenet 的 IP地址
 
#define mqtt_devid "**********"  //设备ID
#define mqtt_pubid "  "     //产品ID
#define mqtt_password " " //鉴权信息
const char*  mqtt_topic = "ESP32HomeRFLight2";
WiFiClient espClient;           //创建一个WIFI连接客户端
PubSubClient client(espClient); // 创建一个PubSub客户端, 传入创建的WIFI客户端
 
char msg_buf[200];                                //发送信息缓冲区

void setupWifi()
{
  WiFi.mode(WIFI_STA);
esp_wifi_set_mac(WIFI_IF_STA, newMACAddress);
Serial.println(WiFi.macAddress());

Serial.print("Connecting to ");
Serial.println(ssid);
WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) 
{
  delay(500);
  Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected.");
Serial.println("IP address: ");
Serial.println(WiFi.localIP()); 
}
 
//收到主题下发的回调, 注意这个回调要实现三个形参 1:topic 主题, 2: payload: 传递过来的信息 3: length: 长度
void callback(char *topic, byte *payload, unsigned int length)
{
  String topic_zj="";
  String data_zj="";
  for (size_t i = 0; i < strlen(topic); i++)
  {
     topic_zj+=(char)topic[i];
  }
  
  for (size_t i = 0; i < length; i++)
  {
     data_zj+=(char)payload[i];
  }
  if(!topic_zj.compareTo(mqtt_topic))
  { 
    
    Serial.write(0xac);
    if(!data_zj.compareTo("H_ON"))
    {
      Serial.write(0x00);
      Serial.write(0x00);
    }
    else if(!data_zj.compareTo("H_OFF"))
    {
      Serial.write(0x00);
      Serial.write(0xFF);
    }
    else if(!data_zj.compareTo("N_ON"))
    {
      Serial.write(0x11);
      Serial.write(0x00);
    }
    else if(!data_zj.compareTo("N_OFF"))
    {
      Serial.write(0x11);
      Serial.write(0xFF);
    }
    else if(!data_zj.compareTo("S_ON"))
    {
      Serial.write(0x22);
      Serial.write(0x00);
    }
    else if(!data_zj.compareTo("S_OFF"))
    {
      Serial.write(0x22);
      Serial.write(0xFF);
    }
    else if(!data_zj.compareTo("W_ON"))
    {
      Serial.write(0x33);
      Serial.write(0x00);
    }
    else if(!data_zj.compareTo("W_OFF"))
    {
      Serial.write(0x33);
      Serial.write(0xFF);
    }
    else if(!data_zj.compareTo("ALL_ON"))
    {
      Serial.write(0xFF);
      Serial.write(0x00);
    }
    else if(!data_zj.compareTo("ALL_OFF"))
    {
      Serial.write(0xFF);
      Serial.write(0xFF);
    }
  }
}
 
//向主题发送模拟的温湿度数据
void sendTempAndHumi()
{
  if (client.connected())
  {

    Serial.print("public message:");
    client.publish("$dp", (uint8_t *)msg_buf, 3); //发送数据到主题$dp
  }
}
 
//重连函数, 如果客户端断线,可以通过此函数重连
void clientReconnect()
{
  while (!client.connected()) //再重连客户端
  {
    delay(3000);
      client.setServer(mqtt_server, 9501);                   //设置客户端连接的服务器
  client.connect(mqtt_devid, mqtt_pubid, mqtt_password); //客户端连接到指定的产品的指定设备.同时输入鉴权信息
  client.subscribe(mqtt_topic);
  client.setCallback(callback);                          //设置好客户端收到信息是的回调
    Serial.println("reconnect MQTT...");
    if (client.connect(mqtt_devid, mqtt_pubid, mqtt_password))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.println("failed");
      Serial.println(client.state());
      Serial.println("try again in 2 sec");
    }
  }
}

void setup() {
  // put your setup code here, to run once:
//   rtc_wdt_protect_off();
//   rtc_wdt_enable();
//   rtc_wdt_feed();
//   rtc_wdt_set_time(RTC_WDT_STAGE0, 8000);
Serial.begin(115200);
setupWifi();                                           //调用函数连接WIFI
delay(2000);
clientReconnect();
}

void loop() {
  // put your main code here, to run repeatedly:


if (!WiFi.isConnected()) //先看WIFI是否还在连接
  {
    setupWifi();
  }
  if (!client.connected()) //如果客户端没连接ONENET, 重新连接
  {
    clientReconnect();
  }
  client.loop(); //客户端循环检测
}
