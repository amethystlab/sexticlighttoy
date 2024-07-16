
#include "stack.h"



void printFrames(){
  Serial.println(F("frame state:"));

  Serial.print(F("time: "));Serial.print(frame_times[0]); Serial.print(F(" --> ")); Serial.println(frame_times[1]);

  for (Cone i = 0; i<NUM_CONES; ++i){
    Serial.print(F("cone ")); Serial.print(i);Serial.print(F(": ")); Serial.print(frame_colors[0][i]);Serial.print(F(" ")); Serial.print(F(" --> ")); Serial.println(frame_colors[1][i]);;
  }
  Serial.println("");
}




void printConeColorsFromMemory(){
  Serial.println(F("current cone colors:"));
  for (Cone i{0}; i<NUM_CONES; ++i){
    Serial.println(getConeColor(i));
  }
}



void setStartConeColorsFromCurrent(){
  Serial.println(F("setting start colors from current"));
  for (Cone i{0}; i<NUM_CONES; ++i){
    frame_colors[0][i] = most_recent_colors[i];
  }

}


void setStartTimeToNow(){
  frame_times[0] = g_current_time;
}


bool setNextFrameTime(Time duration){
  frame_times[1] = frame_times[0] + duration;
}


bool setNextFrameColor(Cone cone, Color color){
  #ifdef DEBUG_PRINT 
  Serial.print(F("setting next frame color for cone "));Serial.print(cone);Serial.print(F(" to "));Serial.println(color);
  #endif
  frame_colors[1][cone] = color;
}

// called once at the beginning of the program
void setupFrames(){

  frame_times[0] = NO_EVENT_PLANNED;
  frame_times[1] = NO_EVENT_PLANNED;

  for (Cone i = 0; i < NUM_CONES; ++i){
    frame_colors[0][i] = BLACK;
    frame_colors[1][i] = BLACK;
    most_recent_colors[i] = BLACK;
  }


}




















float t(Time x, Time x1, Time x2){
  //Serial.print((x - x1)); Serial.print(" / "); Serial.println((x2 - x1));
  //Serial.println((float) (x - x1) / (float) (x2 - x1));
  
  return ((float) (x - x1) / (float) (x2 - x1));
}

float cubicNatural(Time x, Time x1, Time x2, uint8_t y1, uint8_t y2){
  float tOfX = t(x, x1, x2);
  //Serial.print("y1: "); Serial.print(y1); Serial.print(" y2: "); Serial.println(y2);
  //Serial.print("t(x) = "); Serial.println(tOfX); //Serial.print(", "); Serial.print((1 - tOfX) * y1); Serial.print(", "); Serial.print(tOfX * y2); Serial.print(", "); Serial.println(tOfX * (1 - tOfX) * ((1 - tOfX) * (y1 - y2) + tOfX * (y2 - y1)));
  return (1 - tOfX) * y1 + tOfX * y2 + tOfX * (1 - tOfX) * ((1 - tOfX) * (y1 - y2) + tOfX * (y2 - y1));
}




// this function uses a globally-set time.  you must have set this time prior
// to calling this function
// or it will not work correctly.
bool transitionCone(Cone cone){


  // the starting and ending colors for the event being transitioned
  IndividualColor r[2] = {(frame_colors[0][cone] >> 8)  & 0xFF,  (frame_colors[1][cone] >> 8)  & 0xFF};
  IndividualColor g[2] = {(frame_colors[0][cone] >> 16) & 0xFF,  (frame_colors[1][cone] >> 16) & 0xFF};
  IndividualColor b[2] = {(frame_colors[0][cone])       & 0xFF,  (frame_colors[1][cone])       & 0xFF};
  IndividualColor w[2] = {(frame_colors[0][cone] >> 24) & 0xFF,  (frame_colors[1][cone] >> 24) & 0xFF};



  // unpack from the stored array
  Time start_time = frame_times[0];
  Time final_time = frame_times[1];
  
  
  // the current time is later than the final time of the current event, 
  // so setting the final_time to the current time makes the
  // cone's color get set to the final color for the event.
  if(g_current_time < final_time) 
  {
      
  #ifdef CUBIC_INTERP
    IndividualColor final_red = (IndividualColor) round(cubicNatural(g_current_time, start_time, final_time, r[0], r[1]));
    IndividualColor final_green = (IndividualColor) round(cubicNatural(g_current_time, start_time, final_time, g[0], g[1]));
    IndividualColor final_blue = (IndividualColor) round(cubicNatural(g_current_time, start_time, final_time, b[0], b[1]));
    IndividualColor final_white = (IndividualColor) round(cubicNatural(g_current_time, start_time, final_time, w[0], w[1]));
    //Serial.print("final red: "); Serial.println(final_red);

  #else
    // linear interpolation
    float mult = ((float)(g_current_time - start_time))/((float)(final_time - start_time));

    IndividualColor final_red = mult * (r[1] - r[0]) + r[0];
    IndividualColor final_green = mult * (g[1] - g[0]) + g[0];
    IndividualColor final_blue = mult * (b[1] - b[0]) + b[0];
    IndividualColor final_white = mult * (w[1] - w[0]) + w[0];
  #endif

    coneColor(cone, final_green, final_red, final_blue, final_white);
  }
  else {
    coneColor(cone, g[1], r[1], b[1], w[1]);
  }

  return true;
}

bool transitionConeHSV(Cone cone){




  // the starting and ending colors for the event being transitioned
  IndividualColor r[2] = {(frame_colors[0][cone] >> 8)  & 0xFF,  (frame_colors[1][cone] >> 8)  & 0xFF};
  IndividualColor g[2] = {(frame_colors[0][cone] >> 16) & 0xFF,  (frame_colors[1][cone] >> 16) & 0xFF};
  IndividualColor b[2] = {(frame_colors[0][cone])       & 0xFF,  (frame_colors[1][cone])       & 0xFF};
  IndividualColor w[2] = {(frame_colors[0][cone] >> 24) & 0xFF,  (frame_colors[1][cone] >> 24) & 0xFF};


  Serial.print(F("hsv interpolating between two colors: "));
  Serial.print(F("start:")); Serial.print(r[0]); Serial.print(" "); Serial.print(g[0]); Serial.print(" ");Serial.print(b[0]); Serial.println(" ");
  Serial.print(F("final:")); Serial.print(r[1]); Serial.print(" "); Serial.print(g[1]); Serial.print(" ");Serial.print(b[1]); Serial.println(" ");


  // unpack from the stored array
  Time start_time = frame_times[0];
  Time final_time = frame_times[1];
  

    //bail if nothing to do
  if (g_current_time >= final_time){
    coneColor(cone, g[1], r[1], b[1], w[1]);
    return true;
  }
  else{
    HSV start_hsv, final_hsv, current_hsv; // these are in [0,360]x[0,1]x[0,1] space
    start_hsv = rgb2hsv(RGB(r[0],g[0],b[0]));
    final_hsv = rgb2hsv(RGB(r[1],g[1],b[1]));


    float mult = ((float)(g_current_time - start_time))/((float)(final_time - start_time));

    current_hsv.hue        = mult*(final_hsv.hue-start_hsv.hue) + start_hsv.hue;
    current_hsv.saturation = mult*(final_hsv.saturation-start_hsv.saturation) + start_hsv.saturation;
    current_hsv.value      = mult*(final_hsv.value-start_hsv.value) + start_hsv.value;

    // RGB current_rgb = hsv2rgb(current_hsv);

    // final_rgb.Print();

    coneColor(cone,Adafruit_NeoPixel::ColorHSV(current_hsv.HueAsInt(), current_hsv.SatAsInt(), current_hsv.ValAsInt()));

    // coneColor(cone,current_rgb.GreenAsInt(), current_rgb.RedAsInt(), current_rgb.BlueAsInt(), 0); //mult * (w[1] - w[0]) + w[0]


    return true;
  }




}





// loops over all cones in the fixture, and calls `transitionCone` for each.
void transitionAllCones(){
  
  getCurrentTime();  // sets the variable g_current_time -- the g is for global
  
  // pixels.clear();

  for(int i = 0; i < NUM_CONES; i++){
    transitionCone(i);
  }

  pixels.show();
}

// loops over all cones in the fixture, and calls `transitionCone` for each.
void transitionAllConesHSV(){
  Serial.println("hsv transioning");
  getCurrentTime();  // sets the variable g_current_time -- the g is for global
  
  // pixels.clear();

  for(int i = 0; i < NUM_CONES; i++){
    transitionConeHSV(i);
  }

  pixels.show();
}






 
