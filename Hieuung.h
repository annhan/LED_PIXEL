


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

