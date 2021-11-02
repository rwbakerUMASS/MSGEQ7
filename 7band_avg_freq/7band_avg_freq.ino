#include <bitswap.h>
#include <chipsets.h>
#include <color.h>
#include <colorpalettes.h>
#include <colorutils.h>
#include <controller.h>
#include <cpp_compat.h>
#include <dmx.h>
#include <FastLED.h>
#include <fastled_config.h>
#include <fastled_delay.h>
#include <fastled_progmem.h>
#include <fastpin.h>
#include <fastspi.h>
#include <fastspi_bitbang.h>
#include <fastspi_dma.h>
#include <fastspi_nop.h>
#include <fastspi_ref.h>
#include <fastspi_types.h>
#include <hsv2rgb.h>
#include <led_sysdefs.h>
#include <lib8tion.h>
#include <noise.h>
#include <pixelset.h>
#include <pixeltypes.h>
#include <platforms.h>
#include <power_mgt.h>



//Declare Spectrum Shield pin connections
#define DATA_PIN    6
//#define CLK_PIN   4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    150
CRGB leds[NUM_LEDS];
#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

#define STROBE 4
#define RESET 5
#define DC_One A0
#define DC_Two A1

//Define spectrum variables
int freq_amp;
int Frequencies_One[7];
int Frequencies_Two[7];
int i;
long sum;
long avg;
int hue;
void setup() {
  delay(3000); // 3 second delay for recovery

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  Serial.begin(9600);
  //Set spectrum Shield pin configurations
  pinMode(STROBE, OUTPUT);
  pinMode(RESET, OUTPUT);
  pinMode(DC_One, INPUT);
  pinMode(DC_Two, INPUT);
  digitalWrite(STROBE, HIGH);
  digitalWrite(RESET, HIGH);

  //Initialize Spectrum Analyzers
  digitalWrite(STROBE, LOW);
  delay(1);
  digitalWrite(RESET, HIGH);
  delay(1);
  digitalWrite(STROBE, HIGH);
  delay(1);
  digitalWrite(STROBE, LOW);
  delay(1);
  digitalWrite(RESET, LOW);
}
void loop() {
  // put your main code here, to run repeatedly:

  Read_Frequencies();
  sum = 0;
  avg = 0;
  for (i = 0; i < 10000; i++) {
    sum += Frequencies_One[0];
  }
  avg = sum / 10000;
    if (avg < 400) {
      avg = 0;
    }else{
      for (i = 0; i < 7; i++) {
    Serial.print(Frequencies_One[i]);
    Serial.print(" ");
  }
      avg=1000;
    }
//  for (i = 0; i < 7; i++) {
//    Serial.print(Frequencies_One[i]);
//    Serial.print(" ");
//  }
  Serial.print(avg);
  Serial.println();
  for (i = NUM_LEDS; i > 0; i--) {
    leds[i] = leds[i - 1];
  }
  leds[0] = avgColor();
  FastLED.show();
  FastLED.delay(1000 / FRAMES_PER_SECOND);
}

CHSV avgColor() {
  if (hue == 255) {
    hue = 0;
  }
  hue += 3;
  //    Serial.println(avg);
  avg /= 4;
  return CHSV(hue, 255, avg);

}


void Read_Frequencies() {
  //Read frequencies for each band
  for (freq_amp = 0; freq_amp < 7; freq_amp++)
  {
    Frequencies_One[freq_amp] = analogRead(DC_One);
    Frequencies_Two[freq_amp] = analogRead(DC_Two);
    digitalWrite(STROBE, HIGH);
    digitalWrite(STROBE, LOW);
  }
}
