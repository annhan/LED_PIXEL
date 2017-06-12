#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <LPD6803.h>
#include <Ticker.h>

#include "ets_sys.h"
#include "osapi.h"

#define USE_US_TIMER

//wifi access point
const char* ssid = "mHomeVP";
const char* password = "123789456";

// Number of RGB LEDs in strand:
int nLEDs = 64;
/*struct ledHieuUng={
  int R[24]={255,0,0,0,0,0,0,0,0,0,0,0,
  int G24]={255
  int B[24]={255
}*/
// Chose 2 pins for output; can be any valid output pins:
int dataPin  = 13;
int clockPin = 14;

// ESP stuff
MDNSResponder mdns;
ESP8266WebServer server(80);

// First parameter is the number of LEDs in the strand.  The LED strips
// are 32 LEDs per meter but you can extend or cut the strip.  Next two
// parameters are SPI data and clock pins:
LPD6803 strip = LPD6803(nLEDs, dataPin, clockPin);

int stateLED[41][37]={{255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255, 0,0,0,0,0}, //1
                      {0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255, 0,0,0,0,0},//2
                      {191,223,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ,0,0,0,0,0,0,0,255, 0,0,0,0,0},//3 0,0,0,0,0,0,0,255
                      {159,191,223,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ,0,0,0,0,0,0,255,223, 0,0,0,0,0},//4 0,0,0,0,0,0,255,223
                      {127,159,191,223,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,223, 0,0,0,0,0},//5 0,0,0,0,0,0,255,223
                      {95,127,159,191,223,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,223, 0,0,0,0,0},//6 0,0,0,0,0,0,255,223
                      {63,95,127,159,191,223,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,233,191, 0,0,0,0,0},//7 0,0,0,0,0,255,233,191
                      {31,63,95,127,159,191,223,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,233,191, 0,0,0,0,0},//8 0,0,0,0,0,255,233,191
                      {0,31,63,95,127,159,191,223,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,233,191, 0,0,0,0,0},//9 0,0,0,0,0,255,233,191
                      {0,0,31,63,95,127,159,191,223,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,233,191,159, 0,0,0,0,0},//10 0,0,0,0,255,233,191,159
                      {0,0,0,31,63,95,127,159,191,223,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,233,191,159, 0,0,0,0,0},//11 0,0,0,0,255,233,191,159
                      {0,0,0,0,31,63,95,127,159,191,223,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,233,191,159, 0,0,0,0,0},//12  0,0,0,0,255,233,191,159
                      {0,0,0,0,0,31,63,95,127,159,191,223,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,233,191,159,127, 0,0,0,0,0},//13 0,0,0,255,233,191,159,127
                      {0,0,0,0,0,0,31,63,95,127,159,191,223,255,0,0,0,0,0,0,0,0,0,0,0,0,0,255,233,191,159,127, 0,0,0,0,0},//14 0,0,0,255,233,191,159,127
                      {0,0,0,0,0,0,0,31,63,95,127,159,191,223,255,0,0,0,0,0,0,0,0,0,0,0,0,255,233,191,159,127, 0,0,0,0,0},//15 0,0,0,255,233,191,159,127
                      {0,0,0,0,0,0,0,0,31,63,95,127,159,191,223,255,0,0,0,0,0,0,0,0,0,0,255,233,191,159,127,95, 0,0,0,0,0},//16 0,0,255,233,191,159,127,95
                      {0,0,0,0,0,0,0,0,0,31,63,95,127,159,191,223,255,0,0,0,0,0,0,0,0,0,255,233,191,159,127,95, 0,0,0,0,0},//17 0,0,255,233,191,159,127,95
                      {0,0,0,0,0,0,0,0,0,0,31,63,95,127,159,191,223,255,0,0,0,0,0,0,0,0,255,233,191,159,127,95, 0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0,0,31,63,95,127,159,191,223,255,0,0,0,0,0,0,255,233,191,159,127,95,63, 0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0,0,0,31,63,95,127,159,191,223,255,0,0,0,0, 0,255,233,191,159,127,95,63, 0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0,0,0,0,31,63,95,127,159,191,223,255,0,0,0,  0,255,233,191,159,127,95,63, 0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,63,95,127,159,191,223,255,0,0,  255,233,191,159,127,95,63,31, 0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,63,95,127,159,191,223,255,0,  255,233,191,159,127,95,63,31, 0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,63,95,127,159,191,223,255,  255,233,191,159,127,95,63,31, 0,0,0,0,0  },

                      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,95,127,159,191,223,255,  255,233,191,159,127,95,63,0, 0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,95,127,159,191,223,255,  255,233,191,159,127,95,0,0, 0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,127,159,191,223,255,  255,233,191,159,127,0,0,0, 0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,159,191,223,255,  255,233,191,159,0,0,0,0, 0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,191,223,255,   255,233,191,0,0,0,0,0, 0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,223,255,  255,233,0,0,0,0,0,0, 0,0,0,0,0 },
                      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,  255,0,0,0,0,0,0,0, 0,0,0,0,0 },
                      
                      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 255,0,0,0,0 },
                      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 255,0,0,0,0 },
                      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,255,0,0,0 },
                      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,255,0,0,0 },
                      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,255,0,0 },
                      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,255,0,0 },
                      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,255,0 },
                      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,255,0 },
                      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,255 },
                      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,255 }};

void handleRoot() {
  server.send(200, "text/plain", "hello from esp8266!");
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
void setHU()
{
  LPD6803::color_t color;
  color.r = 255;
  color.g = 255;
  color.b = 255;
  int ii=0;
    if (server.hasArg ("R"))
  {
    ii = server.arg ("R").toInt ();
  }
  for (int i=0; i < ii; ++i)
  {
      strip.setPixelColor (i, color);
  }
  strip.show ();
}
void setColor()
{
  LPD6803::color_t color;
  color.r = 0;
  color.g = 0;
  color.b = 0;

  if (server.hasArg ("R"))
  {
    color.r = server.arg ("R").toInt () >> 3;
  }
  if (server.hasArg ("G"))
  {
    color.g = server.arg ("G").toInt () >> 3;
  }
  if (server.hasArg ("B"))
  {
    color.b = server.arg ("B").toInt () >> 3;
  }

  for (int i=0; i < strip.getNrPixels (); ++i)
  {
      strip.setPixelColor (i, color);
  }
  strip.show ();
}

void setPixelColor()
{
  int index = 0;
  LPD6803::color_t color;
  color.r = 0;
  color.g = 0;
  color.b = 0;

  bool accept_call = true;
  if (server.hasArg ("i"))
  {
     index = server.arg ("i").toInt ();

     int row = index / 8;
     int col = index % 8;
     int odd_row = row % 2;

     index = row*8 + odd_row*col + (1-odd_row)*(7-col);
  }
  else
  {
    accept_call = false;
  }

  if (server.hasArg ("R"))
  {
    color.r = server.arg ("R").toInt () >> 3;
  }
  if (server.hasArg ("G"))
  {
    color.g = server.arg ("G").toInt () >> 3;
  }
  if (server.hasArg ("B"))
  {
    color.b = server.arg ("B").toInt () >> 3;
  }

  if (accept_call)
  {
    server.send(200, "text/plain", "");
  }
  else
  {
    server.send(404, "text/plain", "i (0-based index) and R, G and B (dec, optional)");
  }

  strip.setPixelColor (index, color);
  strip.show (index, index + 1);
}

void chayvao(){
    LPD6803::color_t color;
  color.r = 0;
  color.g = 0;
  color.b = 0;
    for (int i=0; i < strip.getNrPixels (); ++i)
  {
    color.g=0;
                color.r=0;
                color.b=0;
      strip.setPixelColor (i, color);
  }
  strip.show ();
  int randomtt=random(0,2);
 // color.g=random(0,255);
               //         color.r=random(0,255);
               //         color.b=random(0,255);
          for (int state=0; state<41; state++)
        {
          for (int led=0; led<37; led++)
          {
                switch (randomtt){
                  case 0:
                        color.g=stateLED[state][led];
                        strip.setPixelColor (led, color);
                  break;
                         color.r=stateLED[state][led];
                        strip.setPixelColor (led, color);
                  case 1:
                        color.b=stateLED[state][led];
                        strip.setPixelColor (led, color);
                  break;
                  case 2:
                        color.g=stateLED[state][led];
                        color.r=stateLED[state][led];
                        strip.setPixelColor (led, color);
                                   case 3:
                        color.g=stateLED[state][led];
                        color.b=stateLED[state][led];
                        strip.setPixelColor (led, color);
                                          case 4:
                        color.b=stateLED[state][led];
                        color.r=stateLED[state][led];
                        strip.setPixelColor (led, color);
                                          case 5:
                        color.g=stateLED[state][led];
                        color.b=stateLED[state][led];
                        color.r=stateLED[state][led];
                        strip.setPixelColor (led, color);
                  break;
                }

                
          }
          strip.show ();
          delay(100);
        }
          color.r = 0;
  color.g = 0;
  color.b = 0;
    for (int i=0; i < strip.getNrPixels (); ++i)
  {
    color.g=0;
                color.r=0;
                color.b=0;
      strip.setPixelColor (i, color);
  }
  strip.show ();
 }

 
void setup ()
{  
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  int tam=0;
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
    tam++;
    if (tam >50){break;}
    
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }
  
  server.on("/", handleRoot);
  server.on("/setColor", setColor);
  server.on("/setHU", setHU);
  server.on("/setPixelColor", setPixelColor);
  server.onNotFound(handleNotFound);
  
  //start server
  server.begin();
  Serial.println("HTTP server started");

  // Start up the LED counter
  strip.begin();

  // Update the strip, to start they are all 'off'
  strip.show();
  Serial.println("LEDS initalized");

  timer1_attachInterrupt (OnTimerOneInterrupt);
  timer1_write (800);
  timer1_enable (TIM_DIV1, TIM_EDGE, TIM_LOOP);
}

void loop(void)
{
  if (strip.outputReady ())
  {
    server.handleClient();
  }
chayvao();
  delay (500);
}

void OnTimerOneInterrupt ()
{
  strip.update ();
}

// fill the dots with said color
// good for testing purposes
void colorSet(LPD6803::color_t i_color)
{
  
}

/* Helper functions */

// Create a 15 bit color value from R,G,B
unsigned int Color(byte r, byte g, byte b)
{
  //Take the lowest 5 bits of each value and append them end to end
  //return( ((unsigned int)g & 0x1F )<<10 | ((unsigned int)b & 0x1F)<<5 | (unsigned int)r & 0x1F);
  unsigned int data;
  
  data = g & 0x1F;
  data <<= 5;
  data |= b & 0x1F;
  data <<= 5;
  data |= r & 0x1F;
  data |= 0x8000;

  return data;
}
void rainbowCycle1() {
  int i, j;
  byte WheelPos;
  LPD6803::color_t color;
  for (j=0; j < 96 * 5; j++) {     // 5 cycles of all 96 colors in the wheel
    for (i=0; i < strip.getNrPixels (); i++) {
      // tricky math! we use each pixel as a fraction of the full 96-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 96 is to make the wheel cycle around
 
  WheelPos = ( ((i * 96 / strip.getNrPixels ()) + j) % 96) ;
        switch(WheelPos >> 5)
  {
    case 0:
      color.r=50- WheelPos %128;   //Red down
      color.g=WheelPos % 128;      // Green up
      color.b=0;                  //blue off
      break; 
    case 1:
      color.g=50- WheelPos % 128;  //green down
      color.b=WheelPos % 128;      //blue up
      color.r=0;                  //red off
      break; 
    case 2:
      color.b=50- WheelPos % 128;  //blue down 
      color.r=WheelPos % 128;      //red up
      color.g=0;                  //green off
      break; 
  }
         strip.setPixelColor (i, color);
    }  
    strip.show();   // write all the pixels out
  delay(100);
  }
}
