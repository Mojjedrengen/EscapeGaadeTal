#include <M5StickCPlus.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <FastLED.h>

#define Neopixel_PIN 21
#define NUM_LEDS     13

CRGB leds[NUM_LEDS];

//#define KEY_C_PIN 0
const char* SSID = "Next-Guest";
const char* WIFI_PASS = "";

static const char* fingerprint PROGMEM = "DF 12 7C 16 41 3D 8C 87 0D 8A 40 DE FD 08 2E 7F 8D 3D 08 3B";

const char* MQTT_SERVER = "mqtt.nextservices.dk";
const uint16_t MQTT_PORT = 8883; 
const char* MQTT_CLIENT_ID = "FAMS";

WiFiClientSecure wifiClient;
PubSubClient pubSubClient(wifiClient);

void setup_wifi() {
  Serial.printf("Connecting to %s", SSID);
  WiFi.begin(SSID, WIFI_PASS);

  // Wait until WiFi is connected.
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.printf("\nSuccessfully connected to %s!\n", SSID);
  wifiClient.setInsecure();
}

void setup_mqtt() {
  pubSubClient.setServer(MQTT_SERVER, MQTT_PORT);
  pubSubClient.setCallback(mqtt_callback);
}

void reconnect_mqtt() {
  while (!pubSubClient.connected()) {
    Serial.printf("Attempting to reconnect to MQTT Broker: %s\n", MQTT_SERVER);
    if (pubSubClient.connect(MQTT_CLIENT_ID)) {
      Serial.printf("Connected!\n");
      pubSubClient.publish("DDU4/FAMS", "Connected!");
      pubSubClient.subscribe("DDU4/FAMS/state");
    } else {
      Serial.printf("failed to connect, rc=%d\n", pubSubClient.state());
      delay(5000);
    }
  }
}


int code = 2206; //Koden der skal intastes

int input[3] = {0, 1, 2};
int inputindex = 0;

bool start = false;

// screen size is 135x240

/* After M5StickC is started or reset
  the program in the setUp () function will be run, and this part will only be run once.
  After M5StickCPlus is started or reset, the program in the setup() function will be executed, and this part will only be executed once. */
void setup(){
  // Initialize the M5StickCPlus object. Initialize the M5StickCPlus object
  M5.begin();

  // LCD display. LCd display
  //M5.Lcd.print("Hello World");

  M5.Lcd.setRotation(1); //Rotate the screen 90 degrees clockwise (1*90)

  M5.Lcd.setTextSize(5);

  pinMode(26, INPUT_PULLDOWN);
  pinMode(25, INPUT_PULLDOWN);
  pinMode(36, INPUT_PULLDOWN);
  pinMode(0, INPUT_PULLUP);
  //Port 0 skal side i Ground. De to andre skal side i 5v->.
  setup_wifi();
  setup_mqtt();

  //M5.Lcd.fillScreen(BLUE);
  M5.update();
  //display();

  // Neopixel initialization
    FastLED.addLeds<WS2811, Neopixel_PIN, GRB>(leds, NUM_LEDS)
        .setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(10);
}

/* After the program in setup() runs, it runs the program in loop()
The loop() function is an infinite loop in which the program runs repeatedly
After the program in the setup() function is executed, the program in the loop() function will be executed
The loop() function is an endless loop, in which the program will continue to run repeatedly */
void loop() {
  M5.update();
  if (start == true){
    game_loop();
  }
  //game_loop();

  if (!pubSubClient.connected()) {
    Serial.println("Connection lost to MQTT Broker!");
    reconnect_mqtt();
  }
  pubSubClient.loop();

  delay(150);
}

void game_loop(){
  display();
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.print(inputindex);
  if (digitalRead(36)) {
    changeNumberUp(inputindex);
  } else if (digitalRead(0) == LOW){
    changeNumberDown(inputindex);
  }
  if (digitalRead(26)) {
    if (inputindex < 2){
      inputindex++;
    } else {
      inputindex = 0;
      checkCode();
    }
  }
}

void display(){
  M5.Lcd.drawChar(input[0]+65, 23, 25, 2);
  M5.Lcd.drawChar(input[1]+65, 100, 25, 2);
  M5.Lcd.drawChar(input[2]+65, 175, 25, 2);

  M5.Lcd.drawLine(10, 95, 70, 95, BLUE);
  M5.Lcd.drawLine(80, 95, 150, 95, GREEN);
  M5.Lcd.drawLine(160, 95, 230, 95, RED);
}

void changeNumberUp(int i){
  if (input[i] < 25) {
    input[i]++;
  } else {
    input[i] = 0;
  }
}
void changeNumberDown(int i){
  if (input[i] > 0) {
    input[i]--;
  } else {
    input[i] = 25;
  }
}

void checkCode(){
  int one = input[2];
  int ten = input[1] * 10;
  int hundred = input[0] * 100;
  int playercode = one + ten + hundred;
  
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(200, 0);
  M5.Lcd.print(playercode);

  if (playercode == code){
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::White;
    }
    FastLED.show();  // must be executed for neopixel becoming effective
    M5.Lcd.fillScreen(GREEN);
  }

  M5.Lcd.setTextSize(5);
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    Serial.printf("Message arrived [%s]:\n", topic);
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.printf("topic: %s \n", topic);
    if (strcmp((char *) payload, "start") == 0) {
      start = true;
      Serial.println("Game starts now");
    }
    else if (strcmp((char *) payload, "stop") == 0) {
      start = false;
      Serial.println("Game stops now");
      M5.Lcd.fillScreen(BLACK);
    }
    M5.Lcd.println();
}