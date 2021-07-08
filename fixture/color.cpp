#include "color.h"

//completely turns off all of the pixels
void clear() {
  for (int i = 0; i < NUM_CONES; ++i) {
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
  for (int i = 0; i < NUM_CONES; ++i) {
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
  for (int i = 0; i < NUM_CONES; ++i) {
    coneColor(i, restrictRandom8Bit(), restrictRandom8Bit(), restrictRandom8Bit(), restrictRandom8Bit() );
  }
  pixels.show();
} //end 7/18/20




//returns the specific color that is within the cone by looking at one pixel
Color getConeColor(Cone cone_num) {
  if (cone_num >= 0 && cone_num < NUM_CONES) {
    uint8_t pixStart = (cone_num) * NUM_PIXELS_PER_GROUP; //determines which pixel out of 140 to fill, inside cone_num
    return pixels.getPixelColor(pixStart);
  }
  return 0; //if for some reason the cone_num isn't in range then zero is returned
} 



//Only sets ONE pixel to a certain color at a time
void pixelColor(int pix_num , IndividualColor g, IndividualColor r, IndividualColor b, IndividualColor w) { // pix_num = Pixel Number, g = green, r=red, b = blue, w = white
  if (pix_num >= 0 && pix_num < NUM_PIXELS) { //forces the user to enter a valid pixel number
    Color color = pixels.Color(g, r, b, w); // make a color
    pixels.setPixelColor(pix_num, color); //set a single pixel color
  }
}   //end (Sam 6/15/2020)




// accepts a pixel number, correlates the pixel's number to the cone its in and lights all of the pixels within that cone to the specfied color
void pixToConeColor (int pix_num , IndividualColor g, IndividualColor r, IndividualColor b, IndividualColor w) { // pix_num = Pixel Number, g = green, r= red, b = blue, w = white
  if (pix_num >= 0 && pix_num < NUM_PIXELS) { //forces the user to enter a valid pixel number
    Color color = pixels.Color(g, r, b, w); // make a color
    int pixStart = (pix_num / NUM_PIXELS_PER_GROUP) * NUM_PIXELS_PER_GROUP; //determines which pixel out of 140 to fill
    pixels.fill(color, pixStart, NUM_PIXELS_PER_GROUP);
  }
}   



//accepts an integer (representing the number on each cone) and lights up just that cone to the specified color
void coneColor(Cone cone, IndividualColor g, IndividualColor r, IndividualColor b, IndividualColor w) { // pix_num = Pixel Number, g = green, r= red, b = blue, w = white
  coneColor(cone, pixels.Color(g,r,b,w));
}   



void coneColor(Cone cone, Color color) { // pix_num = Pixel Number, g = green, r= red, b = blue, w = white
  // Serial.print("filling cone ");Serial.print(cone);Serial.print(" with color ");Serial.println(color);
  if (cone < NUM_CONES) {
    pixels.fill(color, (cone) * NUM_PIXELS_PER_GROUP, NUM_PIXELS_PER_GROUP); //fills seven pixels with the desired color and specific location
    most_recent_colors[cone] = color;
  }
}   


//sets the cones of the array to preset colors, works for 2, 3, and 5 fold
void setGroupPresetColor(uint8_t *coneArray, uint8_t degreeOfSymmetry) {
  if (degreeOfSymmetry > 5) {
    Serial.println(F("degreeOfSymmetry must be less than or equal to 5 in setGroupPresetColor function."));
  }
  else {
    for (int i = 0; i < degreeOfSymmetry; i++) {
      Color changeCone = colorPresets[i];
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

void doPulseMode(){
  Serial.println("pulse mode");
  allLEDs(100);
}





/////// from https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
// David H.'s code for this problem.  


// typedef struct {
//     double r;       // a fraction between 0 and 1
//     double g;       // a fraction between 0 and 1
//     double b;       // a fraction between 0 and 1
// } rgb;
// 


// hsv   rgb2hsv(rgb in);
// rgb   hsv2rgb(hsv in);

HSV rgb2hsv(RGB in)
{
    HSV         out;
    double      min, max, delta;

    min = in.red < in.green ? in.red : in.green;
    min = min  < in.blue ? min  : in.blue;

    max = in.red > in.green ? in.red : in.green;
    max = max  > in.blue ? max  : in.blue;

    out.value = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
        out.saturation = 0;
        out.hue = 0; // undefined, maybe nan?
        return out;
    }
    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        out.saturation = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0              
        // s = 0, h is undefined
        out.saturation = 0.0;
        out.hue = NAN;                            // its now undefined
        return out;
    }
    if( in.red >= max )                           // > is bogus, just keeps compilor happy
        out.hue = ( in.green - in.blue ) / delta;        // between yellow & magenta
    else
    if( in.green >= max )
        out.hue = 2.0 + ( in.blue - in.red ) / delta;  // between cyan & yellow
    else
        out.hue = 4.0 + ( in.red - in.green ) / delta;  // between magenta & cyan

    out.hue *= 60.0;                              // degrees

    if( out.hue < 0.0 )
        out.hue += 360.0;

    return out;
}


RGB hsv2rgb(HSV in)
{
    double      hh, p, q, t, ff;
    long        i;
    RGB         out;

    if(in.saturation <= 0.0) {       // < is bogus, just shuts up warnings
        out.red = in.value;
        out.green = in.value;
        out.blue = in.value;
        return out;
    }
    hh = in.hue;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.value * (1.0 - in.saturation);
    q = in.value * (1.0 - (in.saturation * ff));
    t = in.value * (1.0 - (in.saturation * (1.0 - ff)));

    switch(i) {
    case 0:
        out.red = in.value;
        out.green = t;
        out.blue = p;
        break;
    case 1:
        out.red = q;
        out.green = in.value;
        out.blue = p;
        break;
    case 2:
        out.red = p;
        out.green = in.value;
        out.blue = t;
        break;

    case 3:
        out.red = p;
        out.green = q;
        out.blue = in.value;
        break;
    case 4:
        out.red = t;
        out.green = p;
        out.blue = in.value;
        break;
    case 5:
    default:
        out.red = in.value;
        out.green = p;
        out.blue = q;
        break;
    }
    return out;     
}
