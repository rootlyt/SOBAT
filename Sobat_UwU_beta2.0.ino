#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <HX711.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define DOUT D4
#define CLK D3
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)

const char* ssid = "LIA2"; 
const char* password = "mikalucu";
const char* mqtt_server = "farmer.cloudmqtt.com";
const char* mqtt_user = "obvflhog";
const char* mqtt_pass = "-GF6x9OkO5nH";
const int xPin = A0;

const unsigned char sobatBoot [] PROGMEM = {
  // ini bentuk hexa pixel titik2 dari bootlogo sobat...
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x0c, 0x0e, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x1c, 0x04, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x3e, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x7f, 0x00, 0x7f, 0xaf, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x7f, 0x00, 0x7f, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0xff, 0x80, 0x07, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0xff, 0x00, 0xff, 0x83, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0xfe, 0x01, 0xff, 0x83, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0xff, 0xc7, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc7, 0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe3, 0xef, 0xff, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xc0, 0x3f, 0xff, 0xe3, 0xff, 0xff, 0xde, 0x3f, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0x00, 0x3f, 0xff, 0xe3, 0xdf, 0xff, 0xde, 0x3f, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xfe, 0x0c, 0x3f, 0xff, 0xe3, 0xff, 0xff, 0xde, 0x3f, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xfe, 0x3f, 0xff, 0xff, 0xe3, 0xef, 0xff, 0xfe, 0x3f, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xfe, 0x3f, 0xfe, 0x03, 0xe0, 0x1f, 0xe0, 0x18, 0x0f, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xfe, 0x3f, 0xfe, 0x01, 0xe0, 0x0f, 0xc0, 0x18, 0x0f, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xfe, 0x1f, 0xf8, 0x30, 0xe1, 0x87, 0x86, 0x1e, 0x3f, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0x00, 0x78, 0xf8, 0x23, 0xe3, 0x11, 0x1e, 0x3f, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0x80, 0x38, 0x3c, 0x62, 0xa1, 0x11, 0x1e, 0x3f, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xfe, 0x29, 0x3c, 0x63, 0xa3, 0x00, 0x1e, 0x3f, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0x09, 0x78, 0x61, 0xc3, 0x00, 0x1e, 0x3f, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xfc, 0x09, 0x74, 0x21, 0xa3, 0x00, 0x1e, 0x3f, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0x08, 0x6c, 0x63, 0xa3, 0x10, 0x1e, 0x3f, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0x20, 0xd8, 0x63, 0xc5, 0x18, 0x1e, 0x3f, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xfe, 0x00, 0x04, 0x00, 0xe0, 0x03, 0x80, 0x1e, 0x0f, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xfe, 0x00, 0x42, 0x01, 0xe0, 0x0f, 0xc0, 0x1e, 0x0f, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0x00, 0x81, 0x03, 0xe0, 0x1f, 0xe0, 0x17, 0x0f, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xfc, 0x04, 0xff, 0xfe, 0x7f, 0xff, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xf8, 0x1e, 0xff, 0xff, 0xff, 0xff, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xf0, 0x3f, 0xff, 0xfe, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xe0, 0x3f, 0xff, 0xfd, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xc0, 0x7f, 0xff, 0xf3, 0xff, 0xff, 0xe0, 0x7f, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xc0, 0xef, 0xff, 0xc7, 0xff, 0xdf, 0x80, 0x7f, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xc0, 0x6f, 0xfc, 0x0f, 0xff, 0xce, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xe0, 0x1f, 0x80, 0x1f, 0xff, 0x80, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xf8, 0x06, 0x00, 0x3f, 0xff, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xfc, 0x02, 0x00, 0x7f, 0xff, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x01, 0xff, 0xfe, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x07, 0xff, 0xfc, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x3f, 0xff, 0xfc, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x3f, 0xff, 0xf8, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3f, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

ESP8266WebServer server(80); //ini inisialisasi local server
String page = ""; // nampilin page yang isinya script goblok2an html sama css lah

HX711 scale(DOUT, CLK);// adc dari sensor berat bosqu
float calibration_factor = -344.10; //ini manual nyari atau bisa dicari pakek program, akuratin lagi kalo mau ndak berubah2 tapi ini udah paling akurat yg aku cari
int GRAM;
int KONVERSI;
int data;
int TOTAL;

WiFiClient espClient; // constant mqtt bosqu
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() { //inisialisasi wifi
  delay (100);
  Serial.println();
  
  Serial.print("Inisialisasi ke ");
  Serial.println(ssid);
  //WiFi.forceSleepWake();//biar melek bosq
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }
    randomSeed(micros());
    Serial.println("Wifi tersambung");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }

void reconnect() { //koneksi ke mqtt reconnect
  while (!client.connected()) {
    
    Serial.print("Mencoba koneksi MQTT .");
    String clientId = "ESP8266Client-"; // membuat id random untuk client
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) { //menyambung 
      Serial.println("Tersambung");
      client.publish("outTopic", "Tes Tersambung");//ngepublish kalo kesambung
      client.subscribe("inTopic");
    } else{
      Serial.print("failed, rc= ");//kalo gagal
      Serial.print(client.state());
      Serial.println(" Mencoba kembali dalam 5 detik");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length){ // fiungsi callback
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void oled(){//udah jelas ini oled ya Allah 😢
  display.setTextSize(5);
  display.setCursor(0,0);
  display.clearDisplay();
  display.println(GRAM);
 
  display.setTextSize(1);
  display.setCursor(0,42);
  display.println("Berat dalam Gram.");
  display.setCursor(0,55);
  display.println("Sobat(V2.0)Alpha_05");
  display.display();
  delay(100);  
}

void gyro(){
  //ini buat matiin oled. wkwk belom ada sleep oled dll. masi power saving goblok2 an 
  int x = analogRead(xPin);
  Serial.println(x);
  if(x>235 && x<330){
  Serial.print("buka ");
  oled();
  } else if (x>342 && x<510){
  Serial.print("ttup ");
  display.clearDisplay(); 
  display.display(); 
  }
  delay(50);
}

void bootlogo (){ // ini nanti manggil bootlogo hexa tadi di draw di oled sesuai pixel
  display.clearDisplay(); //for Clearing the display
  display.drawBitmap(0, 0, sobatBoot, 128, 64, WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
  display.display();
  delay(2000);
  
}

void serbar(){
  //mau nambahin fitur tapi lupa
}

void setup() {
 Serial.begin(115200);
   if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 Tak ditemukan bois"));
    for(;;);
  }
  delay(500);//oled
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  Serial.println("bisa oled/n");
 bootlogo();
 setup_wifi();
 
 client.setServer(mqtt_server, 14782);// ini ada port server
 client.setCallback(callback);
 reconnect();
 scale.set_scale();
 scale.tare();

  server.begin();
  display.clearDisplay();
  display.println("Local IP : ");
  display.display();
  delay(100);  
  display.println(WiFi.localIP());
  display.display();
  delay(4000);
  Serial.println("Local Server S.O.B.A.T");
  display.clearDisplay();
  display.println("Local Server Started");
  display.display();
  delay(1000);

  server.on("/", [](){
  page = "<!DOCTYPE html>\n"
"<html>\n"
"\n"
"<head>\n"
"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
"    <link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.4.0/css/bootstrap.min.css\">\n"
"        <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js\"></script>\n"
"        <script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.4.0/js/bootstrap.min.js\"></script>\n"
"</head>\n"
"\n"
"<style>\n"
"    body {font-family: Arial, Helvetica, sans-serif;}\n"
"        /* Full-width input fields */\n"
"        input[type=text], input[type=password] \n"
"        {width: 100%;padding: 12px 20px;margin: 8px 0;display: inline-block;border: 1px solid #ccc;box-sizing: border-box;}\n"
"\n"
"        /* Set a style for all buttons \n"
"        button {background-color: #4CAF50;color: white;padding: 14px 20px;margin: 8px 0;border: none;cursor: pointer;width: 100%;}*/\n"
"\n"
"        .button {background-color: #4CAF50; /* Green */border: none;color: white;padding: 15px 32px;text-align: center;text-decoration: none;display: inline-block;font-size: 16px;margin: 4px 2px;cursor: pointer;}\n"
"    .button2 {background-color: #008CBA;} /* Blue */\n"
"    .button3 {background-color: #f44336;} /* Red */ \n"
"    .button4 {background-color: #e7e7e7; color: black;} /* Gray */ \n"
"    .button5 {background-color: #555555;} /* Black */\n"
"\n"
"        button:hover\n"
"        {opacity: 0.8;}\n"
"\n"
"        /* Extra styles for the cancel button */\n"
"        .cancelbtn \n"
"        {width: auto;padding: 10px 18px;background-color: #f44336;}\n"
"\n"
"        /* Center the image and position the close button */\n"
"        .imgcontainer \n"
"        {text-align: center;margin: 24px 0 12px 0;position: relative;}\n"
"\n"
"        img.avatar \n"
"        {width: 40%;border-radius: 50%;}\n"
"\n"
"        .container \n"
"        {padding: 16px;}\n"
"\n"
"        span.psw \n"
"        {float: right;padding-top: 16px;}\n"
"\n"
"        /* The Modal (background) */\n"
"        .modal \n"
"        {display: none; /* Hidden by default */position: fixed; /* Stay in place */z-index: 1; /* Sit on top */\n"
"left: 0;top: 0;width: 100%; /* Full width */height: 100%; /* Full height */overflow: auto; /* Enable scroll if needed */background-color: rgb(0,0,0); /* Fallback color */background-color: rgba(0,0,0,0.4); /* Black w/ opacity */padding-top: 60px;}\n"
"\n"
"        /* Modal Content/Box */\n"
"        .modal-content \n"
"        {background-color: #fefefe;margin: 5% auto 15% auto; /* 5% from the top, 15% from the bottom and centered */border: 1px solid #888;width: 80%; /* Could be more or less, depending on screen size */}\n"
"\n"
"        /* The Close Button (x) */\n"
"        .close \n"
"        {position: absolute;right: 25px;top: 0;color: #000;font-size: 35px;font-weight: bold;}\n"
"\n"
"        .close:hover,\n"
"        .close:focus \n"
"        {color: red;cursor: pointer;}\n"
"\n"
"        /* Add Zoom Animation */\n"
"        .animate \n"
"        {-webkit-animation: animatezoom 0.6s;animation: animatezoom 0.6s}\n"
"\n"
"        @-webkit-keyframes animatezoom \n"
"        {from {-webkit-transform: scale(0)} to {-webkit-transform: scale(1)}}\n"
"        \n"
"        @keyframes animatezoom \n"
"        {from {transform: scale(0)} to {transform: scale(1)}}\n"
"\n"
"        /* Change styles for span and cancel button on extra small screens */\n"
"        @media screen and (max-width: 300px) \n"
"        {span.psw {display: block;float: none;}.cancelbtn {width: 100%;}}\n"
"\n"
"\n"
"    img {display: block; margin-left: auto; margin-right: auto; width: 253px; height: 230pxs}\n"
"    div {text-align: center; width: 100%}\n"
"    table {height: 135px; margin-left: auto; margin-right: auto; width: 428px}\n"
"    .aaa {width: 135px}\n"
"    .bbb {width: 136px}\n"
"</style>\n"
"\n"
"<body>\n"
"\n"
"<div>\n"
"    <img src=\"https://cdn.discordapp.com/attachments/690812218668744774/722368928063684658/Untitled.png\">\n"
"    <h1>S.O.B.A.T ONLINE SERVICE</h1>\n"
"    <h4>Sistem Administrasi Online Data S.O.B.A.T Perusahaan X</h4>\n"
"\n"
"    \n"
"        <button onclick=\"document.getElementById('id00').style.display='block'\" class=\"button\" style=\"width:auto;\">Silahkan Masuk</button>\n"
"        <div id=\"id00\" class=\"modal\">\n"
"            <form class=\"modal-content animate\" action=\"\" method=\"post\">\n"
"                <div class=\"imgcontainer\">\n"
"                    <span onclick=\"document.getElementById('id00').style.display='none'\" class=\"close\" title=\"Close Modal\">&times;</span>\n"
"                </div>\n"
"\n"
"                <div class=\"container\">\n"
"                        <label for=\"uname\"><b>Nama Akun</b></label>\n"
"                        <input type=\"text\" placeholder=\"Masukkan Nama Akun Anda\" name=\"uname\" required>\n"
"            \n"
"                        <label for=\"psw\"><b>Kata Sandi</b></label>\n"
"                        <input type=\"password\" placeholder=\"Masukkan Kata Sandi Anda\" name=\"psw\" required>\n"
"                            \n"
"                        <input id=\"akses\" type=\"submit\" value=\"Masuk\" class=\"button\">\n"
"                </div>\n"
"            </form>\n"
"        </div>\n"
"        <script>\n"
"        // Get the modal\n"
"        var modal = document.getElementById('id00');\n"
"        // When the user clicks anywhere outside of the modal, close it\n"
"        window.onclick = function(event) \n"
"        {if (event.target == modal) {modal.style.display = \"none\";}}\n"
"        // When Login is Pressed, Said the login was succesful\n"
"        akses.onclick = function() \n"
"        {alert('SELAMAT DATANG KEMBALI \"nama pendaftaran anda\"');}\n"
"        </script>\n"
"\n"
"<p></p>\n"
"\n"
"        <button onclick=\"document.getElementById('id01').style.display='block'\" class=\"button button4\" style=\"width:auto;\">Harga Pasar Terkini</button>\n"
"            <div id=\"id01\" class=\"modal\">\n"
"                <form class=\"modal-content animate\" method=\"post\">\n"
"                    <div class=\"imgcontainer\">\n"
"                        <span onclick=\"document.getElementById('id01').style.display='none'\" class=\"close\" title=\"Close Modal\">&times;</span>\n"
"                    </div>\n"
"\n"
"                    <div class=\"container\">\n"
"                        <label for=\"uname\"><b>Kertas</b></label>\n"
"                            <p> Kertas HVS  : Rp 1.000,-/Kg </p>\n"
"                            <p> Koran Bekas : Rp 850,-/Kg </p>\n"
"                            <p> Karton Bekas: Rp 1.200,-/Kg </p>\n"
"\n"
"                        <label for=\"uname\"><b>Plastik</b></label>\n"
"                            <p> Botol Plastik        : Rp 2.000,-/Kg </p>\n"
"                            <p> Gelas Mineral Plastik: Rp 3.000,-/Kg </p>\n"
"                            <p> Karah Botol Plastik  : Rp 200,-/Kg </p>\n"
"\n"
"                        <label for=\"uname\"><b>Kaleng</b></label>\n"
"                            <p> Kaleng Minuman: Rp 8.000,-/Kg </p>\n"
"                            <p> Kaleng Keras  : Rp 1.000,-/Kg </p>\n"
"                    </div>\n"
"            </form>\n"
"        </div>\n"
"        <script>\n"
"        // Get the modal\n"
"        var modal = document.getElementById('id01');\n"
"        // When the user clicks anywhere outside of the modal, close it\n"
"        window.onclick = function(event) \n"
"        {if (event.target == modal) {modal.style.display = \"none\";}}\n"
"        </script>\n"
"\n"
"\n"
"        <button onclick=\"document.getElementById('id02').style.display='block'\" class=\"button button4\" style=\"width:auto;\">Status SOBAT</button>\n"
"            <div id=\"id02\" class=\"modal\">\n"
"                <form class=\"modal-content animate\" method=\"post\">\n"
"                    <div class=\"imgcontainer\">\n"
"                        <span onclick=\"document.getElementById('id02').style.display='none'\" class=\"close\" title=\"Close Modal\">&times;</span>\n"
"                    </div>\n"
"\n"
"                    <div class=\"container\">\n"
"                        <button class=\"button button\" type=\"button\" disabled>SOBAT 1 AKTIF</button>\n"
"                        <p></p>\n"
"\n"
"                        <button class=\"button button\" type=\"button\" disabled>SOBAT 2 AKTIF</button>\n"
"                        <p></p>\n"
"\n"
"                        <button class=\"button button\" type=\"button\" disabled>SOBAT 3 AKTIF</button>\n"
"                        <p></p>\n"
"\n"
"                        <button class=\"button button3\" type=\"button\" disabled>SOBAT 4 TIDAK AKTIF</button>\n"
"                        <p></p>\n"
"\n"
"                        <button class=\"button button5\" type=\"button\" disabled>SOBAT 5 GANGGUAN TEKNIS</button>\n"
"                        <p></p>                          \n"
"                    </div>\n"
"            </form>\n"
"        </div>\n"
"        <script>\n"
"        // Get the modal\n"
"        var modal = document.getElementById('id02');\n"
"        // When the user clicks anywhere outside of the modal, close it\n"
"        window.onclick = function(event) \n"
"        {if (event.target == modal) {modal.style.display = \"none\";}}\n"
"        </script>\n"
"\n"
"\n"
"        <button onclick=\"document.getElementById('id03').style.display='block'\" class=\"button button4\" style=\"width:auto;\">Hasil Timbangan</button>\n"
"            <div id=\"id03\" class=\"modal\">\n"
"                <form class=\"modal-content animate\" method=\"post\">\n"
"                    <div class=\"imgcontainer\">\n"
"                        <span onclick=\"document.getElementById('id03').style.display='none'\" class=\"close\" title=\"Close Modal\">&times;</span>\n"
"                    </div>\n"
"\n"
"                    <div class=\"container\">\n"
"                        <label for=\"uname\"><b>BERAT SOBAT 1</b></label>\n"
"                            <p> "+String(data)+" Gram </p>\n"
"\n"
"                        <label for=\"uname\"><b>BERAT SOBAT 2</b></label>\n"
"                            <p> "+String(data)+" Gram </p>\n"
"\n"
"                        <label for=\"uname\"><b>BERAT SOBAT 3</b></label>\n"
"                            <p> "+String(data)+" Gram </p>\n"
"\n"
"                        <label for=\"uname\"><b>BERAT SOBAT 4</b></label>\n"
"                            <p> *NULL* Gram </p>\n"
"\n"
"                        <label for=\"uname\"><b>BERAT SOBAT 5</b></label>\n"
"                            <p> 0 Gram </p>    \n"
"                    </div>\n"
"            </form>\n"
"        </div>\n"
"        <script>\n"
"        // Get the modal\n"
"        var modal = document.getElementById('id03');\n"
"        // When the user clicks anywhere outside of the modal, close it\n"
"        window.onclick = function(event) \n"
"        {if (event.target == modal) {modal.style.display = \"none\";}}\n"
"        </script>\n"
"\n"
"\n"
"        <button onclick=\"document.getElementById('id04').style.display='block'\" class=\"button button4\" style=\"width:auto;\">Buku Tabungan</button>\n"
"            <div id=\"id04\" class=\"modal\">\n"
"                <form class=\"modal-content animate\" method=\"post\">\n"
"                    <div class=\"imgcontainer\">\n"
"                        <span onclick=\"document.getElementById('id04').style.display='none'\" class=\"close\" title=\"Close Modal\">&times;</span>\n"
"                    </div>\n"
"\n"
"                    <div class=\"container\">\n"
"                        <label for=\"uname\"><b>JUMLAH KESELURUHAN PENDAPATAN</b></label>\n"
"                            <p> "+String(KONVERSI*3)+" Rupiah </p>\n"
"\n"
"                        <label for=\"uname\"><b>PENDAPATAN SOBAT 1</b></label>\n"
"                            <p> "+String(KONVERSI)+" Rupiah </p>\n"
"\n"
"                        <label for=\"uname\"><b>PENDAPATAN SOBAT 2</b></label>\n"
"                            <p> "+String(KONVERSI)+" Rupiah </p>\n"
"\n"
"                        <label for=\"uname\"><b>PENDAPATAN SOBAT 3</b></label>\n"
"                            <p> "+String(KONVERSI)+" Rupiah </p>\n"
"\n"
"                        <label for=\"uname\"><b>PENDAPATAN SOBAT 4</b></label>\n"
"                            <p> *NULL* Rupiah </p>\n"
"\n"
"                        <label for=\"uname\"><b>PENDAPATAN SOBAT 5</b></label>\n"
"                            <p> 0 Rupiah </p>\n"
"                    </div>\n"
"            </form>\n"
"        </div>\n"
"        <script>\n"
"        // Get the modal\n"
"        var modal = document.getElementById('id04');\n"
"        // When the user clicks anywhere outside of the modal, close it\n"
"        window.onclick = function(event) \n"
"        {if (event.target == modal) {modal.style.display = \"none\";}}\n"
"        </script>\n"
"\n"
"\n"
"<p></p>\n"
"        <button onclick=\"document.getElementById('id99').style.display='block'\" class=\"button button5\" style=\"width:auto;\">Hubungi Kami</button>\n"
"            <div id=\"id99\" class=\"modal\">\n"
"                <form class=\"modal-content animate\" method=\"post\">\n"
"                    <div class=\"imgcontainer\">\n"
"                        <span onclick=\"document.getElementById('id99').style.display='none'\" class=\"close\" title=\"Close Modal\">&times;</span>\n"
"                    </div>\n"
"\n"
"                    <div class=\"container\">\n"
"                        <label for=\"uname\"><b>Alamat Surat Elektronik</b></label>\n"
"                        <input type=\"text\" placeholder=\"Masukkan Surel Anda\" name=\"uname\" required>\n"
"            \n"
"                        <label for=\"psw\"><b>Perihal</b></label>\n"
"                        <input type=\"password\" placeholder=\"Perihal Topik yang Dibahas\" name=\"psw\" required>\n"
"\n"
"                        <label for=\"uname\"><b>Isi Pesan</b></label>\n"
"                        <input type=\"text\" placeholder=\"...\" name=\"uname\" required> \n"
"\n"
"                        <input id=\"akses\" type=\"submit\" value=\"Kirim\" class=\"button\">\n"
"                    </div>\n"
"                </form>\n"
"            </div>\n"
"        <script>\n"
"        // Get the modal\n"
"        var modal = document.getElementById('id99');\n"
"        // When the user clicks anywhere outside of the modal, close it\n"
"        window.onclick = function(event) \n"
"        {if (event.target == modal) {modal.style.display = \"none\";}}\n"
"        // When Login is Pressed, Said the login was succesful\n"
"        akses.onclick = function() \n"
"        {alert('PESAN TELAH TERIKIRIM');}\n"
"        </script>\n"
"\n"
"    \n"
"    <input id=\"exit\" class=\"button button3\" style=\"width:auto;\" type=\"button\" value=\"Keluar\">\n"
"    <script>\n"
"    exit.onclick = function() {\n"
"        alert('SUKSES KELUAR');\n"
"    };\n"
"    </script>\n"
"\n"
"\n"
"        <button onclick=\"document.getElementById('id999').style.display='block'\" class=\"button button5\" style=\"width:auto;\">Pengaturan</button>\n"
"            <div id=\"id999\" class=\"modal\">\n"
"                <form class=\"modal-content animate\" method=\"post\">\n"
"                    <div class=\"imgcontainer\">\n"
"                        <span onclick=\"document.getElementById('id999').style.display='none'\" class=\"close\" title=\"Close Modal\">&times;</span>\n"
"                    </div>\n"
"\n"
"                    <div class=\"container\">\n"
"                        <p> ANDA BELUM MASUK </p>   \n"
"                    </div>\n"
"                </form>\n"
"            </div>\n"
"        <script>\n"
"        // Get the modal\n"
"        var modal = document.getElementById('id999');\n"
"        // When the user clicks anywhere outside of the modal, close it\n"
"        window.onclick = function(event) \n"
"        {if (event.target == modal) {modal.style.display = \"none\";}}\n"
"        // When Login is Pressed, Said the login was succesful\n"
"        </script>\n"
"\n"
"\n"
"\n"
"\n"
"    <h6><span id=\"days\"></span>, <span id=\"time\"></span> <span id=\"tz_name\"></span></h6>\n"
"    <script> \n"
"    var tz_offset = -(new Date().getTimezoneOffset() / 60);\n"
"    switch(tz_offset)\n"
"    {\n"
"    case 7:\n"
"    case 8:\n"
"    case 9:\n"
"        tz_table = ['WIB', 'WITA', 'WIT'];\n"
"        tz_name = tz_table[tz_offset - 7];\n"
"    break;\n"
"    default:\n"
"        tz_name = 'UTC'+(tz_offset >= 0 ? \"+\" : \"\")+tz_offset;\n"
"    }\n"
"    document.getElementById('tz_name').innerHTML = tz_name;\n"
"    var id = setInterval(function()\n"
"    {days_table = ['Minggu', 'Senin', 'Selasa', 'Rabu', 'Kamis', 'Jumat', 'Sabtu'];var date = new Date();var hours = date.getHours();var minutes = ('0'+date.getMinutes()).slice(-2);var seconds = ('0'+date.getSeconds()).slice(-2);document.getElementById('days').innerHTML = days_table[date.getDay()];document.getElementById('time').innerHTML = hours+':'+minutes+':'+seconds;}, 1000);\n"
"    </script>\n"
"</div>\n"
"</body>\n"
"</html>";
    server.send(200, "text/html", page);
  });
 
}

void loop() {
  if (!client.connected()){
    reconnect();
  }
  client.loop();

  scale.set_scale(calibration_factor); //timbangan
  GRAM = scale.get_units(), 2;
  Serial.println(GRAM);
  data = scale.get_units(), 2;
  KONVERSI = GRAM * 2;
  int g = GRAM;
  int k = KONVERSI;
  int TOTAL = KONVERSI+KONVERSI+KONVERSI;

  gyro();
  
 delay(1000);
 String gg = String(g);//ngepublish dari sini
 String kk = String(k);

 Serial.print("Mempublish pesan: ");
 Serial.println(msg);

 int numg = g;
 char csgr[16];
 itoa(numg, csgr, 10);
 int numk = k;
 char cskr[16];
 itoa(numk, cskr, 10);

 delay(1000);
 server.handleClient(); // intinya ngestart server local supaya bisa handling client yg request
 client.publish("ber", csgr);
 client.publish("kon", cskr);
}
