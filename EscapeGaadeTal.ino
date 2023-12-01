#include <M5StickCPlus.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <FastLED.h>

#define Neopixel_PIN 32
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


#define code 2206 //Koden der skal intastes

#define highlightColor 0xa8b545
#define letterColor WHITE
#define backgroundColor BLACK
#define winColor 0xa8b545

const int letterPos[3] = {23, 100, 175};
const int underline1[3] = {10, 80, 160};
const int underline2[3] = {70, 150, 230};

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
    FastLED.setBrightness(64);

    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Black;
    }
    FastLED.show();  // must be executed for neopixel becoming effectiv
    M5.Lcd.fillScreen(backgroundColor);
}

/* After the program in setup() runs, it runs the program in loop()
The loop() function is an infinite loop in which the program runs repeatedly
After the program in the setup() function is executed, the program in the loop() function will be executed
The loop() function is an endless loop, in which the program will continue to run repeatedly */
void loop() {
  M5.update();
  pubSubClient.loop();

  if (!pubSubClient.connected()) {
    Serial.println("Connection lost to MQTT Broker!");
    reconnect_mqtt();
  } 

  if (start == true){
    game_loop();
  }
  //game_loop();

  delay(250);
}

void game_loop(){
  //display();
  if (digitalRead(36)) {
    changeNumberUp(inputindex);
    display();
  } else if (digitalRead(0) == LOW){
    changeNumberDown(inputindex);
    display();
  }
  if (digitalRead(26)) {
    if (inputindex < 2){
      inputindex++;
      display();
    } else {
      inputindex = 0;
      display();
      checkCode();
    }
  }
}

void display(){
  M5.Lcd.fillScreen(backgroundColor);
  int inputleft;
  int inputright;

  switch (inputindex) {
    case 0:
      inputleft = 2;
      inputright = 1;
      break;
    case 1:
      inputleft = 0;
      inputright = 2;
      break;
    case 2:
      inputleft = 1;
      inputright = 0;
      break;
    default:
      Serial.println("error: index array out of bounds!");
      break;
  }
  M5.Lcd.setTextColor(highlightColor);
  M5.Lcd.drawChar(input[inputindex]+65, letterPos[inputindex], 25, 2);

  M5.Lcd.setTextColor(letterColor);
  M5.Lcd.drawChar(input[inputleft]+65, letterPos[inputleft], 25, 2);
  M5.Lcd.drawChar(input[inputright]+65, letterPos[inputright], 25, 2);

  M5.Lcd.drawLine(underline1[inputindex], 95, underline2[inputindex], 95, highlightColor);
  M5.Lcd.drawLine(underline1[inputleft], 95, underline2[inputleft], 95, letterColor);
  M5.Lcd.drawLine(underline1[inputright], 95, underline2[inputright], 95, letterColor);
}

void changeNumberUp(int i){
  if (input[i] < 25) {
    input[i]++;
  } else {
    input[i] = 0;
  }
  //M5.Lcd.fillScreen(backgroundColor);
}
void changeNumberDown(int i){
  if (input[i] > 0) {
    input[i]--;
  } else {
    input[i] = 25;
  }
  //M5.Lcd.fillScreen(backgroundColor);
}

void checkCode(){
  int one = input[2];
  int ten = input[1] * 10;
  int hundred = input[0] * 100;
  int playercode = one + ten + hundred;
  
  /*
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(200, 0);
  M5.Lcd.print(playercode);*/

  if (playercode == code){
    start = false;
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Green;
      FastLED.show();  // must be executed for neopixel becoming effective
      delay(50);
    }
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Black;
      FastLED.show();  // must be executed for neopixel becoming effective
      delay(50);
    }
    M5.Lcd.fillScreen(winColor);
    pubSubClient.publish("DDU4/FAMS/bomb", "goede2");
  }

  M5.Lcd.setTextSize(5);
}

String byteArrayToString(byte byteArray[], int size) {
  String result = "";
  for (int i = 0; i <= size; i++) {
    result += char(byteArray[i]);
  }
  return result;
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    Serial.printf("Message arrived [%s]:\n", topic);
    String str = byteArrayToString(payload, sizeof(payload));
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.printf("topic: %s \n", topic);
    Serial.println(str);
    if (str == "start") {
      start = true;
      for (int i = 0; i < sizeof(input); i++){
        input[i] = i;
      }
      Serial.println("Game starts now");
      display();
    }
    else if (str == "stopt") {
      start = false;
      Serial.println("Game stops now");
      M5.Lcd.fillScreen(BLACK);
    }
    M5.Lcd.println();
}