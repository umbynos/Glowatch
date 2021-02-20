#include <Adafruit_NeoPixel.h>
#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include <RTCZero.h>

#define SECRET_SSID ""
#define SECRET_PASS ""
#define LDR_PIN A0
#define LED_PIN 5
#define NPIXELS 60
#define LED_BRIGHTNESS 64

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
WiFiConnectionHandler ArduinoIoTPreferredConnection(SECRET_SSID, SECRET_PASS);
RTCZero rtc_clock; //will access the same memory address as rtc defined in ArduinoIoTCloud.cpp no need to set that
int h=0,m=0,s=0;
int light = 0;

void setup() {
  setDebugMessageLevel(3);
  Serial.begin(9600);
  rtc_clock.begin();
  //rtc_clock.setHours((__TIME__[0] - '0') * 10 + __TIME__[1] - '0');
  //rtc_clock.setMinutes((__TIME__[3] - '0') * 10 + __TIME__[4] - '0');
  //rtc_clock.setSeconds((__TIME__[6] - '0') * 10 + __TIME__[7] - '0');
  ArduinoCloud.begin(ArduinoIoTPreferredConnection); // initialize a connection to the Arduino IoT Cloud
  strip.begin();
  rainbowCycle(20);
  strip.setBrightness(LED_BRIGHTNESS);
  strip.show();
}

void loop() {
  ArduinoCloud.update();
  byte hpos[3]; //array with 3 entry, byte type stores an 8-bit unsigned number, from 0 to 255.
  getTime(); //sets h,m,s variables
  hpos[0] = (h*5+m/12)%60; //posizione della lancetta delle ore
  hpos[1] = (hpos[0]-1 < 0) ? 59 : hpos[0]-1;  //led precedente alla posizione delle ore (simil buffer circolare)
  hpos[2] = (hpos[0]+1 > 59) ? 0 : hpos[0]+1; //led successivo alla posizione delle ore

  light = map(analogRead(LDR_PIN), 300, 1000, 5, 8);
  light = pow(2, light);
  light = constrain(light, 32, 255);
  strip.setBrightness(light);
  
  for (int i=0;i<NPIXELS;i++) {
    if (s==i)
      strip.setPixelColor(s,strip.Color(0,255,255));//coloro la lancetta dei secondi di blu
    else if (i == hpos[0])
      strip.setPixelColor( i,Wheel((i+128)&255,0)); //lancetta delle ore
    else if ((i == hpos[1]) || (i == hpos[2]))
      strip.setPixelColor( i,Wheel((hpos[0]+128)&255,2)); //lancetta delle ore -> 3 led
    else if (i < m)
      strip.setPixelColor(i, Wheel((i+32) & 255,0));//arco dei minuti
    else
      strip.setPixelColor(i, strip.Color(0,0,0));//il resto dell'orologio rimane spento    
  }
  strip.show();
  debugPrint();
}

void debugPrint(){
  print2digits(rtc_clock.getHours());
  Serial.print(":");
  print2digits(rtc_clock.getMinutes());
  Serial.print(":");
  print2digits(rtc_clock.getSeconds());
  Serial.println();
  uint32_t rtcEpoch = rtc_clock.getEpoch();
  Serial.println(rtcEpoch);
}

void print2digits(int number) {
  if (number < 10) {
    Serial.print("0"); // print a 0 before if the number is < than 10
  }
  Serial.print(number);
}

void getTime() {
  //h = rtc_clock.getHours() + 1; // ora legale
  h = (rtc_clock.getHours()>11) ? rtc_clock.getHours()-12 : rtc_clock.getHours(); // ora solare
  m = rtc_clock.getMinutes();
  s = rtc_clock.getSeconds();
  delay(200);
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
//divider reduces brightness by half every increment
uint32_t Wheel(byte WheelPos, byte divider) {
  if(WheelPos < 85) {
   return strip.Color((WheelPos * 3>>divider), (255 - WheelPos * 3)>>divider, 0);//R-G
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color((255 - WheelPos * 3)>>divider, 0, (WheelPos * 3)>>divider);//-RB
  } else {
   WheelPos -= 170;
   return strip.Color(0, (WheelPos * 3)>>divider, (255 - WheelPos * 3)>>divider);//G-B
  }
}
  void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*3; j++) { // 3 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255,0));
    }
    strip.show();
    delay(wait);
  }
}
