#include <IRLib.h>
#include <Adafruit_NeoPixel.h>
// IR ENTER BUTTON FD906F || 16617583
//PIN Zuweisung Output
uint8_t LED_GREEN = 6; //Fotowiderstand, TEST, Helligkeit der LEDS
uint8_t LED_RED = 5; //Thermometer, TEST, Geschwindigkeit des Ventilator
uint8_t LED_YELLOW = 4; // IR Reciever
uint8_t LED_STRIP = 8; //Wert zuweisen

//PIN Zuweisungen Input
int SCHRANKE = A2; // 450 + keine Schranke , 450 - Schranke, AUSSCHALTEN
int SENSOR_INPUT = A0; // desto höher der Wert desto dunkler
int TEMP_INPUT = A1; // desto höher der Wert desto wärmer
uint8_t IR_INPUT = 7; // Einschalten
int OHMMETER = A3; // Messen des Widerstands, FARBE 
/* 1k = 1000~1100 gelb
 * 1.7k = 1700 ~ 1800 orange
 * 5.7k = 5800 ~ 5900 rot
 * 1k || 1.7k = 629 = 630 ~ 680 dunkelrot
 * 1k || 5.7k = 850 ~ 900 lila
 * 1.7k || 5.7k = 1060 = 1200-1500 türkis
 * 1k || 1.7k || 5.7k = 567 = 550~600 blau
 * Farbreihenfolge = gelb, orange, rot , dunkelrot, lila, türkis, blau
 * Default: R2 = 0;
 */

// VALUES
uint8_t sensorWert = 0;
uint8_t tempWert = 0;
int raw = 0;
int Vin = 5;
float Vout= 0;
float R1= 10000;
float R2= 0;
float buffer= 0;
bool state = false; // false = off, true = on;
//VARS
IRrecv My_Receiver(IR_INPUT);
IRdecode My_Decoder;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(4, LED_STRIP, NEO_GRB + NEO_KHZ800);

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
  sensorWert = analogRead(SENSOR_INPUT);
  tempWert = analogRead(TEMP_INPUT);
  Serial.print("Photo: ");
  Serial.println(sensorWert);
  Serial.print("Temperatur: ");
  Serial.println(tempWert);
  Serial.print("Lichtschranke: ");
  Serial.println(analogRead(SCHRANKE));

  if (sensorWert >= 255){
    sensorWert = 255;
  }
  if(R2 > 550 && R2 < 600){
    fireLight(strip.Color(0, 0, 255), strip.Color(0,160,160), 150);
  } else if (state){
    fireLight(strip.Color(255, 0, 0), strip.Color(255,69,0), 150);  
  }
  if (analogRead(SCHRANKE)< 540){
    LEDoff();
    state = false;
  }
  strip.setBrightness(sensorWert);
   //Check Ohm
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
   
  //Check IR Signal
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
  
  delay(1000);
} // Loop END

void LEDoff(){
  for (uint16_t i=0; i < strip.numPixels(); i++){
    strip.setPixelColor(i,0);
  }
  strip.show();
}

void fireLight(uint32_t c, uint32_t f, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 2; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+2) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.setBrightness(analogRead(SENSOR_INPUT));
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
}
