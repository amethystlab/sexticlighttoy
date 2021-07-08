
#include "stack.h"



void printFrames(){
  Serial.println(F("frame state:"));

  Serial.print(frame_times[0]); Serial.print(F(" --> ")); Serial.println(frame_times[1]);

  for (Cone i = 0; i<NUM_CONES; ++i){
    Serial.print(frame_colors[i][0]);Serial.print(" "); Serial.print(F(" --> ")); Serial.println(frame_colors[i][1]);;
  }
}




void printConeColorsFromMemory(){
  Serial.println(F("current cone colors:"));
  for (Cone i{0}; i<NUM_CONES; ++i){
    Serial.println(getConeColor(i));
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





// called once at the beginning of the program
void setupFrames(){

  frame_times[0] = NO_EVENT_PLANNED;
  frame_times[1] = NO_EVENT_PLANNED;

  for (Cone i = 0; i < NUM_CONES; i++){
    
    
    frame_colors[i][0] = BLACK;
    frame_colors[i][1] = BLACK;
  }
}



bool setNextFrameTime(Time duration){

}


bool setNextFrameColor(Cone cone, Color color){

}






void renderFrame(){

  for(int i = 0; i < NUM_CONES; i++){
    transitionCone(i); 
  }

  pixels.show();
}  // event








bool transitionCone(Cone cone){

  if (frame_times[1] == NO_EVENT_PLANNED){
    Serial.print(F("due to NO_EVENT_PLANNED, skipping cone ")); Serial.println(cone);
    return false;
  }
  
  // the starting and ending colors for the event being transitioned
  IndividualColor r[2] = {(frame_colors[cone][0] >> 8)  & 0xFF,  (frame_colors[cone][1] >> 8)  & 0xFF};
  IndividualColor g[2] = {(frame_colors[cone][0] >> 16) & 0xFF,  (frame_colors[cone][1] >> 16) & 0xFF};
  IndividualColor b[2] = {(frame_colors[cone][0])       & 0xFF,  (frame_colors[cone][1])       & 0xFF};
  IndividualColor w[2] = {(frame_colors[cone][0] >> 24) & 0xFF,  (frame_colors[cone][1] >> 24) & 0xFF};

  // answer these questions:
  //
  // why are we subtracting `start`?  
  // why are we modding by lengthOfShow?
  // should the call to `millis()` be factored out, so that all cones being transisioned at once 
  // will use the same time?  otherwise, they use slightly different times...

  // unpack from the stored array
  Time start_time = frame_times[0];
  Time final_time = frame_times[1];
  
  
  // the current time is later than the final time of the current event, 
  // so setting the final_time to the current time makes the
  // cone's color get set to the final color for the event.
  if(g_current_time < final_time) 
  {
    Serial.print(F("transitioning cone ")); Serial.print(cone); Serial.print(F(" at time ")); Serial.println(g_current_time);
      
    #ifdef CUBIC_INTERP
    IndividualColor final_red = (IndividualColor) round(cubicNatural(current_time, start_time, final_time, r[0], r[1]));
    IndividualColor final_green = (IndividualColor) round(cubicNatural(current_time, start_time, final_time, g[0], g[1]));
    IndividualColor final_blue = (IndividualColor) round(cubicNatural(current_time, start_time, final_time, b[0], b[1]));
    IndividualColor final_white = (IndividualColor) round(cubicNatural(current_time, start_time, final_time, w[0], w[1]));
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
    //#ifdef DEBUG_PRINT
    //  Serial.print("Current Time: "); Serial.print(current_time); Serial.print(" End of Transition: "); Serial.println(final_time);
    //#endif
  }
  else {
    Serial.print(F("cone ")); Serial.print(cone); Serial.print(F("over time")); Serial.println(g_current_time);
    coneColor(cone, g[1], r[1], b[1], w[1]);
  }

  return true;
}






// loops over all cones in the fixture, and calls `transitionCone` for each.
void transitionAllCones(){
  
  getCurrentTime();
  
  for(int i = 0; i < NUM_CONES; i++){
    transitionCone(i);
  }

  pixels.show();
}







 
