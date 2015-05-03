#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>
#include <Servo.h>
#include <HttpClient.h>
#include <Process.h>


YunServer server;
Servo servo1; // Create a servo object
Servo servo2; // Create a servo object

#define LED 13                // LED collegato al pin digitale 13
#define BUTTON 8              // pin di input dove è collegato il pulsante
int  val = 0;  
int state = LOW;      // the current state of the output pin
int reading;           // the current reading from the input pin
int previous = LOW;    // the previous reading from the input pin

// the follow variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long time = 0;         // the last time the output pin was toggled
long debounce = 200;   // the debounce time, increase if the output flickers

/**
 * Setup
 */
void setup() {
  servo1.attach(5); // Attaches the servo on Pin 5
  servo2.attach(6); // Attaches the servo on Pin 6
  Bridge.begin();
  server.listenOnLocalhost();
  server.begin();
  pinMode(LED, OUTPUT);       // imposta il pin digitale come output
  pinMode(BUTTON, INPUT); 
}

/**
 * Loop
 */
void loop() {
  HttpClient HTTPClient;
  reading = digitalRead(BUTTON);
  if (reading == HIGH && previous == LOW && millis() - time > debounce) {
    if (state == HIGH){
      
      state = LOW;
      Process p;
      p.runShellCommand("curl --user leonardo:password1. http://192.168.0.15:8080/0/detection/pause");  
      while(p.running()); 
      delay(2000);
      //HTTPClient.get("http://192.168.0.15:8080/0/detection/pause");
    } 
    else {
      
      state = HIGH;
      Process p;
      p.runShellCommand("curl --user leonardo:password1. http://192.168.0.15:8080/0/detection/start");  
      while(p.running()); 
      delay(2000);
      //HTTPClient.get("http://192.168.0.15:8080/0/detection/start");
    }
    time = millis();    
  }

  digitalWrite(LED, state);

  previous = reading;
  
  //Serial.println("loop");
  YunClient client = server.accept();

  if (client) {
    Serial.println("client");
    process(client);
    client.stop();
  }

  delay(15);

}

void process(YunClient client) {
  String command = client.readStringUntil('/');

  // Check if the url contains the word "servo"
  Serial.println(command);
  if (command == "servo") {
    servoCommand(client);
  }

}

void servoCommand(YunClient client) {
  int pin, value;
  
  // Get the servo Pin (5 or 6)
  pin = client.parseInt();

  // Check if the url string contains a value (/servo/6/VALUE)
  if (client.read() == '/') {
    value = client.parseInt();
    //Serial.println(value);
    if (pin == 5){
      servo1.write(value); // Rotate the servo 5
    }
    //Attualmente è usato solo questo
    else if (pin == 6){
      servo2.write(value); // Rotate the servo 6
      }

  }

}

