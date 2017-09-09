#include <Adafruit_SSD1306.h>
#include <DS3231.h>
#include <Wire.h>
DS3231 rtc(SDA, SCL);
#include <SPI.h>                                      //SPI library
#include "fix_fft.h"
#include <Adafruit_GFX.h>                             //graphics library for OLED
#define OLED_RESET 4                                  //OLED reset, not hooked up but required by library
Adafruit_SSD1306 display(OLED_RESET);
#include <dht.h>
dht DHT;
#define DHT11_PIN 2

char im[128], data[128];                              //variables for the FFT
char x = 0, ylim = 32;                                //variables for drawing the graphics
int i = 0, val;



void setup()
{
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  rtc.begin();
 
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);          //begin OLED @ hex addy 0x3C
// rtc.setDOW(SUNDAY);     // Set Day-of-Week to SUNDAY
// rtc.setTime(23, 30, 0);     // Set the time to 12:00:00 (24hr format)
 //rtc.setDate(3, 9, 2017);   // Set the date to June 6th, 2017
  display.setTextSize(1);
  display.clearDisplay();
  display.setTextColor(WHITE);
  analogReference(DEFAULT);                           // Use default (5v) aref voltage, (3.3v)for pro mini 

}

void loop()
{
  if (analogRead(A1)>1000)
  {
    int min = 0, max = 1024;                            //set minumum & maximum ADC values
    for (i = 0; i < 128; i++) {                         //take 128 samples
      val = analogRead(A0);                             //get audio from Analog 0
      data[i] = val / 4 - 128;                          //each element of array is val/4-128
      im[i] = 0;                                        //
      if (val > max) max = val;                         //capture maximum level
      if (val < min) min = val;                         //capture minimum level
    };

    fix_fft(data, im, 7, 0);                            //perform the FFT on data

    display.clearDisplay();                             //clear display
    for (i = 1; i < 64; i++) {                          // In the current design, 60Hz and noise
      int dat = sqrt(data[i] * data[i] + im[i] * im[i]);//filter out noise and hum
      display.drawLine(i * 2 + x, ylim, i * 2 + x, ylim - dat, WHITE); // draw bar graphics for freqs above 500Hz to buffer
    };
    display.setCursor(0, 0);                            //set cursor to top of screen
    display.print("");             //print title to buffer
    display.display();                                  //show the buffer

  }
  else
  {
    int chk = DHT.read11(DHT11_PIN);
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(20, 0);
    display.print(rtc.getTimeStr());
    display.setTextSize(1);
    display.setCursor(0, 15);
    display.print(rtc.getDateStr());
    display.setTextSize(1);
    display.setCursor(70, 15);
    display.print(rtc.getDOWStr());
    display.setTextSize(1);
    display.setCursor(0, 25);
    display.print("H: ");
    display.print(DHT.humidity, 1);
    display.print("%\t");
     display.setCursor(54,25 );
    display.print(" Temp: ");
    display.print(DHT.temperature, 1);
    display.print("C");
    display.display();
     delay(1000);
  }
}
