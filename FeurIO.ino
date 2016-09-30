#include <IRLib.h>
// IR ENTER BUTTON FD906F || 16617583
// PIN
uint8_t LED_GREEN = 6; //Fotowiderstand
uint8_t LED_RED = 5; //Thermometer
uint8_t LED_YELLOW = 4; // IR Reciever
int SENSOR_INPUT = A0;
int TEMP_INPUT = A1;
uint8_t IR_INPUT = 7;
// VALUES
uint8_t sensorWert = 0;
uint8_t tempWert = 0;

//VARS
IRrecv My_Receiver(IR_INPUT);
IRdecode My_Decoder;

void setup() {
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  My_Receiver.enableIRIn();
  Serial.begin(9600);
}

void loop() {
  sensorWert = analogRead(SENSOR_INPUT);
  tempWert = analogRead(TEMP_INPUT);
  Serial.print("Photo: ");
  Serial.println(sensorWert);
  Serial.print("Temperatur: ");
  Serial.println(tempWert);
  if (My_Receiver.GetResults(&My_Decoder)) {
    My_Decoder.decode();
    //Serial.println(My_Decoder.value);
    if (My_Decoder.value == 16617583){
      //Do On IREnter
      digitalWrite(LED_YELLOW, HIGH);
      delay(3000);
      digitalWrite(LED_YELLOW,LOW);
    }
    My_Receiver.resume();     //Restart the receiver
  }

  // Check Photosensor
  if (sensorWert >= 150){
    digitalWrite(LED_GREEN, HIGH);
  } else if (sensorWert < 150){
    digitalWrite(LED_GREEN, LOW);
  }

  // Check Heatsensor
  if (tempWert >= 170){
    digitalWrite(LED_RED, HIGH);
  } else if (tempWert < 170){
    digitalWrite(LED_RED, LOW);
  }
  
  delay(3000);
}
