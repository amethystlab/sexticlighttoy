#include "color.h"

//completely turns off all of the pixels
void clear() {
  for (int i = 1; i <= NUM_LED_GROUPS; i++) {
    coneColor (i, 0, 0, 0, 0); //sets all the pixels, in each cone, to brightness of 0
  }
} //end (sam 7/18/20)


//returns a randomized color value
uint8_t random8BitValue() {
  uint8_t color = rand() % 256;
  return color;
} //end Sam (7/13/20)



//assigns a random color to each and every cone
void allConeRandom() {
  for (int i = 1; i <= NUM_LED_GROUPS; i++) {
    coneColor(i, random8BitValue(), random8BitValue(), random8BitValue(), random8BitValue() ); //bit not byte
  }
  pixels.show();
} //end (Sam 7/13/20)


//only returns a random color value given it uses the 3 numbers in the colorValueArray
uint8_t restrictRandom8Bit() {
  uint8_t colorValueArray[3] = {0, 127, 255};
  return colorValueArray[rand() % 3];
} //end 7/18/20

//assigns a random color to each cone but can only use combinations of values including 0, 127, and 255
void restrictAllConeRandom() {
  uint8_t colorValueArray[3] = {0, 127, 255};
  for (int i = 1; i <= NUM_LED_GROUPS; i++) {
    coneColor(i, restrictRandom8Bit(), restrictRandom8Bit(), restrictRandom8Bit(), restrictRandom8Bit() );
  }
  pixels.show();
} //end 7/18/20






//Only sets ONE pixel to a certain color at a time
void pixelColor(int pixNum , uint8_t g, uint8_t r, uint8_t b, uint8_t w) { // pixNum = Pixel Number, g = green, r=red, b = blue, w = white
  if (pixNum >= 0 && pixNum <= 255) { //forces the user to enter a valid pixel number
    uint32_t color = pixels.Color(g, r, b, w); // make a color
    pixels.setPixelColor(pixNum, color); //set a single pixel color
    pixels.show(); //update the colors
  }
}   //end (Sam 6/15/2020)




// accepts a pixel number, correlates the pixel's number to the cone its in and lights all of the pixels within that cone to the specfied color
void pixToConeColor (int pixNum , uint8_t g, uint8_t r, uint8_t b, uint8_t w) { // pixNum = Pixel Number, g = green, r= red, b = blue, w = white
  if (pixNum >= 0 && pixNum <= 255) { //forces the user to enter a valid pixel number
    uint32_t color = pixels.Color(g, r, b, w); // make a color
    int pixStart = (pixNum / 7) * 7; //determines which pixel out of 140 to fill
    pixels.fill(color, pixStart, 7);
    pixels.show(); //update the colors
  }
}   //end (Sam 6/20/2020)



//accepts an integer (representing the number on each cone) and lights up just that cone to the specified color
void coneColor (int coneNum, uint8_t g, uint8_t r, uint8_t b, uint8_t w) { // pixnum = Pixel Number, g = green, r= red, b = blue, w = white
  if (coneNum >= 1 && coneNum <= 20) {
    uint32_t color = pixels.Color(g, r, b, w); // make a color
    int pixStart = (coneNum - 1) * 7; //determines which pixel out of 140 to fill
    pixels.fill(color, pixStart, 7); //fills seven pixels with the desired color and specific location
    }
}   //end (Sam 7/1/2020)




//sets the cones of the array to preset colors, works for 2, 3, and 5 fold
void setGroupPresetColor(uint8_t *coneArray, uint8_t degreeOfSymmetry) {
  if (degreeOfSymmetry > 5) {
    Serial.println("degreeOfSymmetry must be less than or equal to 5 in setGroupPresetColor function.");
  }
  else {
    for (int i = 0; i < degreeOfSymmetry; i++) {
      uint32_t changeCone = colorPresets[i];
      coneColor(*coneArray, (changeCone >> 16) & 0xFF, (changeCone >> 8) & 0xFF, changeCone & 0xFF, (changeCone >> 24) & 0xFF); //moves color cone to cone
      coneArray++;
    }
  }
}




// sets all pixes to red
void allred(unsigned long period) {
  uint16_t i, j = 0;

  uint16_t num_steps = 256;
  uint8_t a = 1;
  while (j < num_steps) {
    currentMillis = millis();
    if (currentMillis - previousMillis >= period) {
      a = float(++j) / num_steps * 256;
      previousMillis = currentMillis;

      for (i = 0; i < pixels.numPixels(); i++) {
        pixels.setPixelColor(i, 0, a, 0, 0); //set a single pixel color
      }
      pixels.show(); //update the color
    } // if
  } // j
} // fun

// sets all pixes to 
void allblue(unsigned long period) {
  uint16_t i, j = 0;

  uint16_t num_steps = 256;
  uint8_t a = 1;
  while (j < num_steps) {
    currentMillis = millis();
    if (currentMillis - previousMillis >= period) {
      a = float(++j) / num_steps * 256;
      previousMillis = currentMillis;

      for (i = 0; i < pixels.numPixels(); i++) {
        pixels.setPixelColor(i, 0, 0, a, 0);
      }
      pixels.show();
    } // if
  } // j
} // fun

// sets all pixes to 
void allmagenta(unsigned long period) {
  uint16_t i, j = 0;

  uint16_t num_steps = 256;
  uint8_t a = 1;
  while (j < num_steps) {
    currentMillis = millis();
    if (currentMillis - previousMillis >= period) {
      a = float(++j) / num_steps * 256;
      previousMillis = currentMillis;

      for (i = 0; i < pixels.numPixels(); i++) {
        pixels.setPixelColor(i, 0, a, a, 0);
      }
      pixels.show();
    } // if
  } // j
} // fun

// sets all pixes to 
void allgreen(unsigned long period) {
  uint16_t i, j = 0;

  uint16_t num_steps = 256;
  uint8_t a = 1;
  while (j < num_steps) {
    currentMillis = millis();
    if (currentMillis - previousMillis >= period) {
      a = float(++j) / num_steps * 256;
      previousMillis = currentMillis;

      for (i = 0; i < pixels.numPixels(); i++) {
        pixels.setPixelColor(i, a, 0, 0, 0);
      }
      pixels.show();
    } // if
  } // j
} // fun

// sets all pixes to 
void allwhite(unsigned long period) {
  uint16_t i, j = 0;

  uint16_t num_steps = 256;
  uint8_t a = 1;
  while (j < num_steps) {
    currentMillis = millis();
    if (currentMillis - previousMillis >= period) {
      a = float(++j) / num_steps * 256;
      previousMillis = currentMillis;

      for (i = 0; i < pixels.numPixels(); i++) {
        pixels.setPixelColor(i, 0, 0, 0, a);
      }
      pixels.show();
    } // if
  } // j
} // fun

void allLEDs(unsigned long period) {
  uint16_t i, j = 0;

  uint16_t num_steps = 256;
  uint8_t a = 1;
  while (j < num_steps) {
    currentMillis = millis();
    if (currentMillis - previousMillis >= period) {
      a = float(++j) / num_steps * 256;
      previousMillis = currentMillis;

      for (i = 0; i < pixels.numPixels(); i++) {
        pixels.setPixelColor(i, a, a, a, a);
      }
      pixels.show();
    } // if
  } // j
} // fun
