
#include <IRLib.h>
// IR ENTER BUTTON FD906F
// PIN
int LED_GREEN = 6; //Fotowiderstand
int LED_RED = 5; //Thermometer
int LED_YELLOW = 4; // IR Reciever
int SENSOR_INPUT = A0;
int TEMP_INPUT = A1;
int IR_INPUT = 7;

int sensorWert = 0;
int tempWert = 0;

IRrecv My_Receiver(IR_INPUT);
IRdecode My_Decoder;

void setup() {
  // put your setup code here, to run once:
  //pinMode(TEST_PIN, INPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  My_Receiver.enableIRIn();
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  irWert = analogRead(IR_INPUT);
  sensorWert = analogRead(SENSOR_INPUT);
  tempWert = analogRead(TEMP_INPUT);
  //Serial.println(sensorWert);
  //Serial.println(tempWert);
  if (My_Receiver.GetResults(&My_Decoder)) {
    My_Decoder.decode();    //Decode the data
    //Serial.println(My_Decoder.value); //Show the results on serial monitor
    if (My_Decoder.value == 16617583){
      //Do On IREnter
      digitalWrite(LED_YELLOW, HIGH);
      delay(3000);
      digitalWrite(LED_YELLOW,LOW);
    }
    My_Receiver.resume();     //Restart the receiver
  }
  
  //Serial.println(irWert);
  if (sensorWert >= 300){
    digitalWrite(LED_GREEN, HIGH);
  } else if (sensorWert < 300){
    digitalWrite(LED_GREEN, LOW);
  }

  if (tempWert >= 170){
    digitalWrite(LED_RED, HIGH);
  } else if (tempWert < 170){
    digitalWrite(LED_RED, LOW);
  }
  
  //delay(3000);
}
