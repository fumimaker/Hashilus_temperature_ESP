#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <ArduinoOSC.h>

const char* ssid = "ASUS_28_2G";
const char* pwd = "hashilus";
const IPAddress ip(192, 168, 100, 90);
const IPAddress gateway(192, 168, 100, 1);
const IPAddress subnet(255, 255, 255, 0);

//const char* host = "192.168.100.7";
const char *host = "192.168.100.7";
const int recv_port = 9999;
const int send_port = 9999;

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
OscWiFi osc;

boolean switch_state = false;
void setup() {
  pinMode(2, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(2, LOW);
  digitalWrite(13, LOW);
  Serial.begin(115200);
  Serial.println("Adafruit MLX90614 test");  
  mlx.begin();
  WiFi.begin(ssid, pwd);
  WiFi.config(ip, gateway, subnet);
  while (WiFi.status() != WL_CONNECTED) { Serial.print("."); delay(500); }
  Serial.print("WiFi connected, IP = "); Serial.println(WiFi.localIP());
  // ArduinoOSC
  osc.begin(recv_port);

  osc.subscribe("/need/reply", [](OscMessage &m) {
    Serial.print("/need/reply: ");
    Serial.print(m.ip());
    Serial.print(" ");
    Serial.print(m.port());
    Serial.print(" ");
    Serial.print(m.size());
    Serial.print(" ");
    Serial.print(m.address());
    Serial.print(" ");
    Serial.print(m.arg<int>(0));
    Serial.println();
    int i = 12;
    float f = 34.56F;
    double d = 78.987;
    String s = "reply Received.";
    bool b = true;
    osc.send(host, send_port, "/send", i, f, d, s, b);
  });
}

void loop() {
  float objectTemp = mlx.readObjectTempC();
  float ambientTemp = mlx.readAmbientTempC();
  Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempC()); 
  Serial.print("*C\tObject = "); Serial.print(mlx.readObjectTempC());
  Serial.print("*C");

  if(objectTemp > 30.0){
    switch_state = false;
    digitalWrite(2, LOW);
    digitalWrite(13, LOW);
    Serial.println(" Turn Off");
  }
  else{
    switch_state = true;
    digitalWrite(2, HIGH);
    digitalWrite(13, HIGH);
    Serial.println(" Turn On");
  }
  osc.send(host, send_port, "/objectTemp", objectTemp);// float
  osc.send(host, send_port, "/switch", switch_state); // boolean
  //osc.send(host, send_port, "/test", 123);

  osc.parse(); // should be called
  delay(1000);
}
