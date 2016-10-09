#include <IRLib.h>
#include <Adafruit_NeoPixel.h>

//LED Pin Zuweisung
uint8_t LED_PIN_PEL = 12;
uint8_t LED_PIN_TIER = 13;
uint8_t LED_STRIP [4] = {11, 10, 9, 8};
uint8_t FAN_PIN = 7;
uint8_t PELTIER_PIN = 6;

//PIN Zuweisungen Input#
uint8_t PHOTO_INPUT = A0; // desto höher der Wert desto dunkler
uint8_t TEMP_INPUT = A1; // desto höher der Wert desto wärmer
uint8_t SCHRANKE = A2; // 450 + keine Schranke , 450 - Schranke, AUSSCHALTEN
uint8_t OHMMETER = A3; // Messen des Widerstands, FARBE
uint8_t IR_INPUT = 3; // Einschalten

// VALUES
uint8_t LED_STRIP_ELEMENT = 3;
Adafruit_NeoPixel strip [4];
Adafruit_NeoPixel strip_pel;
Adafruit_NeoPixel strip_tier;

uint8_t photoValue = 0;
uint8_t tempValue = 0;
uint16_t rawOhmmeter = 0;
uint8_t Vin = 5;
float Vout = 0;
float R1 = 10000;
float R2 = 0;
float buffer = 0;
bool state = false; // false = off, true = on;
bool debug = false;
IRrecv My_Receiver(IR_INPUT);
IRdecode My_Decoder;


void setup() {
  pinMode(FAN_PIN, OUTPUT);
  pinMode(PELTIER_PIN, OUTPUT);
  strip_pel = Adafruit_NeoPixel(1, LED_PIN_PEL, NEO_GRB + NEO_KHZ800);
  strip_tier = Adafruit_NeoPixel(1, LED_PIN_TIER, NEO_GRB + NEO_KHZ800);
  strip[0] = Adafruit_NeoPixel(LED_STRIP_ELEMENT, LED_STRIP[0], NEO_GRB + NEO_KHZ800);
  strip[1] = Adafruit_NeoPixel(LED_STRIP_ELEMENT, LED_STRIP[1], NEO_GRB + NEO_KHZ800);
  strip[2] = Adafruit_NeoPixel(LED_STRIP_ELEMENT, LED_STRIP[2], NEO_GRB + NEO_KHZ800);
  strip[3] = Adafruit_NeoPixel(LED_STRIP_ELEMENT, LED_STRIP[3], NEO_GRB + NEO_KHZ800);
  for (uint8_t i = 0; i <= LED_STRIP_ELEMENT; i++) {
    strip[i].begin();
    strip[i].show();
  }
  My_Receiver.enableIRIn();
  if (debug) {
    Serial.begin(9600);
  }
}

void loop() {
  strip_pel.setPixelColor(0, strip_pel.Color(255, 255, 255));
  strip_tier.setPixelColor(0, strip_tier.Color(255, 255, 255));
  strip_pel.begin();
  strip_tier.begin();
  strip_tier.show();
  strip_pel.show();
  digitalWrite(LED_PIN_PEL, HIGH);
  digitalWrite(LED_PIN_TIER, HIGH);
  rawOhmmeter = analogRead(OHMMETER);
  photoValue = analogRead(PHOTO_INPUT);
  tempValue = analogRead(TEMP_INPUT);
  //Brightness
  if (photoValue >= 255) {
    photoValue = 255;
  }

  if (tempValue >= 250 ) {
    tempValue = 250;
  }
  for (uint8_t i = 0; i <= LED_STRIP_ELEMENT; i++) {
    strip[i].setBrightness(photoValue);
  }
  if (state) {
    //todo map tempValue to fanSpeed
    analogWrite(FAN_PIN, 255); // <- tempValue
    digitalWrite(PELTIER_PIN, HIGH);
  }


  //fireLight(strip[0].Color(255, 0, 0), strip[0].Color(255, 130, 0), 0);

  // LED
  for (uint8_t i = 0; i <= LED_STRIP_ELEMENT; i++) {
    if (R2 > 2000 && R2 < 2300) { //1 Block ORANGE - N
      fireLight(strip[i].Color(242, 164, 14), strip[i].Color(238, 56, 14), i);
    } else if (R2 > 5500 && R2 < 7000) { //1 Block ROT - N
      fireLight(strip[i].Color(238, 56, 14), strip[i].Color(160, 10, 10), i);
    } else if (R2 > 1200 && R2 < 2000) { //2 Bloecke TUERKIS - N
      fireLight(strip[i].Color(14, 231, 231), strip[i].Color(14, 231, 101), i);
    } else if (state) { //Default ON Statements
      fireLight(strip[i].Color(255, 0, 0), strip[i].Color(255, 130, 0), i);
    }
  }
  //Ohmmeter
  if (rawOhmmeter) {
    buffer = rawOhmmeter * Vin;
    Vout = (buffer) / 1024.0;
    buffer = (Vin / Vout) - 1;
    R2 = R1 * buffer;
    Serial.print("Vout: ");
    Serial.println(Vout);
    Serial.print("R2: ");
    Serial.println(R2);
  } else {
    R2 = 0;
  }
  // IR Reciever -> ON
  if (My_Receiver.GetResults(&My_Decoder)) {
    My_Decoder.decode();
    Serial.print("--------------------------------------------");
    Serial.println(My_Decoder.value);
    if (My_Decoder.value == 16617583) {
      //Do On IREnter
      state = true;
    }
    My_Receiver.resume();     //Restart the receiver
  }
  // OFF
  if (analogRead(SCHRANKE) < 540) {
    for (uint8_t i = 0; i <= LED_STRIP_ELEMENT; i++) {
      ledOff(i);
    }
    digitalWrite(FAN_PIN, LOW);
    digitalWrite(PELTIER_PIN, LOW);
    state = false;
  }

  // Debug
  if (debug) {
    debugMode();
    delay (1000);
  }
}

void debugMode() {
  Serial.print("Photo: ");
  Serial.println(photoValue);
  Serial.print("Temperatur: ");
  Serial.println(tempValue);
  Serial.print("Lichtschranke: ");
  Serial.println(analogRead(SCHRANKE));
}

void ledOff(uint8_t pin) {
  for (uint8_t i = 0; i < LED_STRIP_ELEMENT; i++) {
    strip[pin].setPixelColor(i, 0);
  }
  strip[pin].show();
}

void fireLight(uint32_t c, uint32_t f, uint8_t pin) {
  strip[pin].setPixelColor(0, c);
  strip[pin].setPixelColor(1, f);
  strip[pin].setPixelColor(2, c);
  delay(50);
  //strip[pin].begin();
  strip[pin].show();
  //stripTest.begin();
  //stripTest.show();
  //delay(150);
  //for (uint8_t i=0; i < strip[pin].numPixels(); i=i+2) {
  //  strip[pin].setPixelColor(i+q, f);        //turn every third pixel off
  //}
  //}
}
