#include <IRLib.h>
#include <Adafruit_NeoPixel.h>
// IR ENTER BUTTON FD906F || 16617583
//PIN Zuweisung Output
uint8_t LED_GREEN = 6; //Fotowiderstand, TEST, Helligkeit der LEDS
uint8_t LED_RED = 5; //Thermometer, TEST, Geschwindigkeit des Ventilator
uint8_t LED_YELLOW = 4; // IR Reciever
uint8_t LED_STRIP = 8; //Wert zuweisen
uint8_t LED_STRIP_ELEMENT = 4;

//PIN Zuweisungen Input
int SCHRANKE = A2; // 450 + keine Schranke , 450 - Schranke, AUSSCHALTEN
int PHOTO_INPUT = A0; // desto höher der Wert desto dunkler
int TEMP_INPUT = A1; // desto höher der Wert desto wärmer
uint8_t IR_INPUT = 7; // Einschalten
int OHMMETER = A3; // Messen des Widerstands, FARBE 

// VALUES
uint8_t photoValue = 0;
uint8_t tempValue = 0;
int raw = 0;
uint8_t Vin = 5;
float Vout= 0;
float R1= 10000;
float R2= 0;
float buffer= 0;
bool state = false; // false = off, true = on;
bool debug = false;
//VARS
IRrecv My_Receiver(IR_INPUT);
IRdecode My_Decoder;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_STRIP_ELEMENT, LED_STRIP, NEO_GRB + NEO_KHZ800);



void setup() {
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  My_Receiver.enableIRIn();
  strip.begin();
  strip.show();
  Serial.begin(9600);
}

void loop() {
  raw = analogRead(OHMMETER);
  photoValue = analogRead(PHOTO_INPUT);
  tempValue = analogRead(TEMP_INPUT);
  // ----- LED Brightness Feature
  //Photosensor
  if (photoValue >= 255){
    photoValue = 255;
  }
  strip.setBrightness(photoValue);

  // ----- Block Add Feature
  //Getting unknown OHM
  if (raw){
    buffer = raw * Vin;
    Vout= (buffer)/1024.0;
    buffer= (Vin/Vout) -1;
    R2 = R1 * buffer;
    Serial.print("Vout: ");
    Serial.println(Vout);
    Serial.print("R2: ");
    Serial.println(R2);
  } else {
    R2 = 0;
  }
  //assign COLOR STATE
  if(R2 > 550 && R2 < 600){ //3 Bloecke BLAU
    fireLight(strip.Color(0, 0, 255), strip.Color(0,160,160), 150);
  } else if(R2 > 1000 && R2 < 1100){ //1 Block GELB 
    //todo
  } else if(R2 > 1700 && R2 < 1800){ //1 Block ORANGE
    //todo
  } else if(R2 > 5800 && R2 < 5900){ //1 Block ROT
    //todo
  } else if(R2 > 630 && R2 < 680){ //2 Bloecke DUNKELROT
    //todo
  } else if(R2 > 850 && R2 < 900){ //2 Bloecke LILA
    //todo
  } else if(R2 > 1200 && R2 < 1500){ //2 Bloecke TUERKIS
    //todo
  } else if (state){ //Default
    fireLight(strip.Color(255, 0, 0), strip.Color(255,69,0), 150);  
  }

  // ----- SWITCH DEVICE -> ON
  if (My_Receiver.GetResults(&My_Decoder)) {
    My_Decoder.decode();
    //Serial.println(My_Decoder.value);
    if (My_Decoder.value == 16617583){
      //Do On IREnter
      state = true;
      digitalWrite(LED_YELLOW, HIGH);
      digitalWrite(LED_YELLOW,LOW);
    }
    My_Receiver.resume();     //Restart the receiver
  }

  // ----- SWITCH DEVICE -> OFF
  if (analogRead(SCHRANKE)< 540){
    LEDoff();
    state = false;
  }
  
  // ----- Visual DEBUG Lights
  // Photosensor OUTPUT
  if (photoValue >= 150){
    digitalWrite(LED_GREEN, HIGH);
  } else if (photoValue < 150){
    digitalWrite(LED_GREEN, LOW);
  }
  
  // Heatsensor OUTPUT
  if (tempValue >= 170){
    digitalWrite(LED_RED, HIGH);
  } else if (tempValue < 170){
    digitalWrite(LED_RED, LOW);
  }
  
  if (debug){
    debugMode();
    delay (1000);
  }  
} // Loop END

void LEDoff(){
  for (uint16_t i=0; i < strip.numPixels(); i++){
    strip.setPixelColor(i,0);
  }
  strip.show();
}

void debugMode(){
  Serial.print("Photo: ");
  Serial.println(photoValue);
  Serial.print("Temperatur: ");
  Serial.println(tempValue);
  Serial.print("Lichtschranke: ");
  Serial.println(analogRead(SCHRANKE));
}

void fireLight(uint32_t c, uint32_t f, uint8_t wait) {
    for (int q=0; q < 2; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+2) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.setBrightness(analogRead(PHOTO_INPUT));
      strip.show();
      if (analogRead(SCHRANKE)< 540){
        LEDoff();
        state = false;
        break;
      }
      delay(wait);
      for (uint16_t i=0; i < strip.numPixels(); i=i+2) {
        strip.setPixelColor(i+q, f);        //turn every third pixel off
      }
   }
}
