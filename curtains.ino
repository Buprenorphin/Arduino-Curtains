#include <ESP8266WiFi.h>
const char* ssid = "TP-LINK_Dyna";
const char* password = "dyna_password";

WiFiServer server(80);

// btn_s
const int ender_pin    = 14; //D5
const int btn_up_pin   = 12; //D6
const int btn_down_pin = 13; //D7
// Motor
const int motor_pin_1  = 16; //D0
const int motor_pin_2  = 5;  //D1
const int motor_pin_3  = 4;  //D2
const int motor_pin_4  = 0;  //D3

const int step_up   = 40000;
const int step_down = 25500; 
int loglevel        = 1;
int steps_left;
int last_step_time; 
int last_step_time2;  
int step_number;
int global_down_step_number = step_up;
int global_up_step_number   = 0;
int directionmotor = 1;
int step_delay     = 3;
int step_delay2    = 10;
int moveroll       = 1;
int ender_state    = 1;
int i;
int btn_up_state   = 1;
int btn_down_state = 1;

void moveupcommand() {
  moveroll = 1;
  directionmotor  = 1;
}

void movedowncommand() {
  moveroll = 1;
  directionmotor = 0;
}

void motorstop() {
  moveroll = 0;
  digitalWrite(motor_pin_1, LOW);
  digitalWrite(motor_pin_2, LOW);
  digitalWrite(motor_pin_3, LOW);
  digitalWrite(motor_pin_4, LOW);
}

void motor(int thisStep) {
  switch (thisStep) {
    case 0:    // 0001
      digitalWrite(motor_pin_1, LOW);
      digitalWrite(motor_pin_2, LOW);
      digitalWrite(motor_pin_3, LOW);
      digitalWrite(motor_pin_4, HIGH);
    break;
    case 1:    // 0011
      digitalWrite(motor_pin_1, LOW);
      digitalWrite(motor_pin_2, LOW);
      digitalWrite(motor_pin_3, HIGH);
      digitalWrite(motor_pin_4, HIGH);
    break;
    case 2:    //0010
      digitalWrite(motor_pin_1, LOW);
      digitalWrite(motor_pin_2, LOW);
      digitalWrite(motor_pin_3, HIGH);
      digitalWrite(motor_pin_4, LOW);
    break;
    case 3:    //0110
      digitalWrite(motor_pin_1, LOW);
      digitalWrite(motor_pin_2, HIGH);
      digitalWrite(motor_pin_3, HIGH);
      digitalWrite(motor_pin_4, LOW);
    break;
    case 4:    // 0100
      digitalWrite(motor_pin_1, LOW);
      digitalWrite(motor_pin_2, HIGH);
      digitalWrite(motor_pin_3, LOW);
      digitalWrite(motor_pin_4, LOW);
    break;
    case 5:    // 1100
      digitalWrite(motor_pin_1, HIGH);
      digitalWrite(motor_pin_2, HIGH);
      digitalWrite(motor_pin_3, LOW);
      digitalWrite(motor_pin_4, LOW);
    break;
    case 6:    //1000
      digitalWrite(motor_pin_1, HIGH);
      digitalWrite(motor_pin_2, LOW);
      digitalWrite(motor_pin_3, LOW);
      digitalWrite(motor_pin_4, LOW);
    break;
    case 7:    //1001
      digitalWrite(motor_pin_1, HIGH);
      digitalWrite(motor_pin_2, LOW);
      digitalWrite(motor_pin_3, LOW);
      digitalWrite(motor_pin_4, HIGH);
    break;
  }
}

void go() {
  //Read values of buttons and enders
  ender_state    = digitalRead(ender_pin);
  btn_up_state   = digitalRead(btn_up_pin);
  btn_down_state = digitalRead(btn_down_pin);  

//If quantity of steps is more than allowed and direction = DOWN
if (global_down_step_number > step_down && directionmotor == 0)
  {
    if (loglevel > 0)  
    Serial.println("Roll stop by limit step down");
    motorstop();
}

//If button "UP" is pressed
if (btn_up_state == 0) {
  Serial.println(WiFi.status());  
}

//If ender state = 0 and direction = UP
 if (ender_state == 0 && directionmotor == 1) {
    if (loglevel > 0) Serial.println("Roll stop by ender, moving up");
    motorstop();  
    global_down_step_number = 1;
 }  
  
//Moving (moveroll = 1)
 if (moveroll == 1){
   if (millis() - last_step_time2 >= step_delay2)
      { 
        if (loglevel > 0)
        last_step_time2 = millis();  
      }
   if (millis() - last_step_time >= step_delay) 
      {
        last_step_time = millis();
        //If miving UP
          if (directionmotor==1){
             motor(step_number);
             step_number++;
             global_down_step_number--;
             if (step_number > 7) step_number = 0;
           }
        //If moving down
          if (directionmotor == 0) {
             if (step_number == 0) step_number = 7;
             motor(step_number);
             step_number--;
             global_down_step_number++;
          }
       }
  }
}

void setup() {

last_step_time = millis();

pinMode(ender_pin,    INPUT_PULLUP);
pinMode(btn_up_pin,   INPUT_PULLUP);
pinMode(btn_down_pin, INPUT_PULLUP);
 
pinMode(motor_pin_1,  OUTPUT);
pinMode(motor_pin_2,  OUTPUT);
pinMode(motor_pin_3,  OUTPUT);
pinMode(motor_pin_4,  OUTPUT);
 
digitalWrite(motor_pin_1, 0);
digitalWrite(motor_pin_2, 0);
digitalWrite(motor_pin_3, 0);
digitalWrite(motor_pin_4, 0);

Serial.begin(57600);

// Connect to WiFi network
Serial.println();
Serial.println();
Serial.print("Connecting to ");
Serial.println(ssid);
WiFi.begin(ssid, password);
  
while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
}
 Serial.println("");
 Serial.println("WiFi connected");
  
 // Start the server
 server.begin();
 Serial.println("Server started");

 // Print the IP address
 Serial.println(WiFi.localIP());
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    go();  
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
  
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  
  // Match the request
  go();  
  int val;
  if (req.indexOf("/down") != -1) {
    val = 0;
    moveroll=1;
    directionmotor=0;
    if (loglevel > 0)
      Serial.println("Recived command to down");
  }
  else if (req.indexOf("/up") != -1) {   
    val = 1;
    moveupcommand();
      if (loglevel > 0)
        Serial.println("Recived command to up");
  }
  else if (req.indexOf("/stop") != -1) {   
    val = 1;
    motorstop();
    if (loglevel > 0)
      Serial.println("Recived command to stop");
  }
 else if (req.indexOf("/log+") != -1)
 loglevel  = loglevel + 1;
 else if (req.indexOf("/log-") != -1)
 loglevel  = loglevel - 1;
 
 
  else {
    Serial.println("Invalid request");
    client.stop();
    return;
  }

  client.flush();

  // Prepare the response

  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\n";
  //s += "GPIO is now ";
  //s += (val)?"high ":"low ";
  //s += global_down_step_number;
  //s += moveroll;
  s += "<style>h2 {text-align:center;} a {border:1px solid #0000ff; border-radius:15px; text-decoration:none; background:#0000ff; color:#ffffff; font-weight:bold; padding:10px 0; margin:25px 0px; display:block; width:100%; text-align:center; font-size:3vw;} </style>";
  s += "\n <h2>CURTAINS REMOTE CONTROL</h3>";
  s += "\n <a href='/up'>UP</a> ";
  s += "\n <a href='/down'>DOWN</a> ";
  s += "\n <a href='/stop'>STOP</a> ";
  s += "</html>\n";
  
  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

  // The client will be disconnected when the function returns and 'client' object is detroyed
}
