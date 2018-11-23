#include <RTCZero.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN 7
#define NPIXELS 60
#define LED_BRIGHTNESS 64

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
RTCZero rtc;
int h=0,m=0,s=0;

void setup() {
  Serial.begin(9600);
  rtc.begin();
  rtc.setHours((__TIME__[0] - '0') * 10 + __TIME__[1] - '0');
  rtc.setMinutes((__TIME__[3] - '0') * 10 + __TIME__[4] - '0');
  rtc.setSeconds((__TIME__[6] - '0') * 10 + __TIME__[7] - '0');
  strip.begin();
  strip.setBrightness(LED_BRIGHTNESS);
  strip.show();
}

void loop() {

  byte hpos[3]; //array with 3 entry, byte type stores an 8-bit unsigned number, from 0 to 255.
  getTime(); //sets h,m,s variables
  hpos[0] = (h*5+m/12)%60; //posizione della lancetta delle ore
  hpos[1] = (hpos[0]-1 < 0) ? 59 : hpos[0]-1;  //led precedente alla posizione delle ore (simil buffer circolare)
  hpos[2] = (hpos[0]+1 > 59) ? 0 : hpos[0]+1; //led successivo alla posizione delle ore
  
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
  print2digits(rtc.getHours());
  Serial.print(":");
  print2digits(rtc.getMinutes());
  Serial.print(":");
  print2digits(rtc.getSeconds());
  Serial.println();
}

void print2digits(int number) {
  if (number < 10) {
    Serial.print("0"); // print a 0 before if the number is < than 10
  }
  Serial.print(number);
}

void getTime() {
  //h = rtc.getHours() + 1; // ora legale
  h = (rtc.getHours()>11) ? rtc.getHours()-12 : rtc.getHours(); // ora solare
  m = rtc.getMinutes();
  s = rtc.getSeconds();
  delay(200);
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos, byte divider) {
  if(WheelPos < 85) {
   return strip.Color((WheelPos * 3>>divider), (255 - WheelPos * 3)>>divider, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color((255 - WheelPos * 3)>>divider, 0, (WheelPos * 3)>>divider);
  } else {
   WheelPos -= 170;
   return strip.Color(0, (WheelPos * 3)>>divider, (255 - WheelPos * 3)>>divider);
  }
}
