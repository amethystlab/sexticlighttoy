
#include "stack.h"


//returns the specific color that is within the cone by looking at one pixel
Color getConeColor(Cone coneNum) {
  if (coneNum >= 0 && coneNum < NUM_CONES) {
    uint8_t pixStart = (coneNum) * NUM_PIXELS_PER_GROUP; //determines which pixel out of 140 to fill, inside coneNum
    return pixels.getPixelColor(pixStart);
  }
  return 0; //if for some reason the coneNum isn't in range then zero is returned
} 


// sets first available event to change the cone to color.
bool addEventToStack(Cone cone, Color color, Time timeOfEvent){
  bool set = false;
  for (int i = 0; i < MAX_NUM_EVENTS && !set; i++){
    if (event_cone[i] == OPEN_EVENT_CODE){
#ifdef DEBUG_PRINT
        Serial.println("EVENT ADDED TO STACK!");
#endif
      
      event_cone[i] = cone;
      event_colors[i] = color;
      event_times[i] = timeOfEvent;
      set = true;
    }
  }
  return set;
}

void eventStackToTransition(){
  unsigned long currentTime = (millis() - start) % lengthOfShow;

  for (int cone = 0; cone < NUM_CONES; cone++){
    if (times[cone][1] == NO_EVENT_PLANNED){

      times[cone][0] = currentTime;
      colors[cone][0] = getConeColor(cone);
      int32_t proximity = 65535; // 2^16-2
      uint8_t closestEvent = 255;

      for (int j = 0; j < MAX_NUM_EVENTS; j++){
        if (event_cone[j] == cone){
#ifdef DEBUG_PRINT
          Serial.print("Cone Num "); Serial.print(coneNum); Serial.print(" == "); Serial.println(event_cone[j]);
#endif
          int32_t tempProximity = event_times[j] + ((event_times[j] < currentTime) ? lengthOfShow : 0) - currentTime;
          if(tempProximity < proximity){
            proximity = tempProximity;
            times[cone][1] = event_times[j];
            colors[cone][1] = event_colors[j];
            closestEvent = j;
          }
        }
      }

      if(closestEvent != 255){
#ifdef DEBUG_PRINT
        Serial.print("Setting cone "); Serial.print(coneNum); Serial.print(" to "); Serial.print((colors[cone][1] >> 16) & 0xFF); Serial.print(", ");
        Serial.print((colors[cone][1] >> 8) & 0xFF); Serial.print(", "); Serial.println((colors[cone][1] >> 0) & 0xFF); Serial.print(" at time "); Serial.println(times[cone][1]);
        Serial.print(" from "); Serial.print((colors[cone][0] >> 16) & 0xFF); Serial.print(", ");
        Serial.print((colors[cone][0] >> 8) & 0xFF); Serial.print(", "); Serial.println((colors[cone][0] >> 0) & 0xFF); Serial.print(" at time "); Serial.println(times[cone][0]);
#endif
       
        event_cone[closestEvent] = OPEN_EVENT_CODE;
        // we just put this event on the transition phase
        // so open it up to be used again
      }
      
    }
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
void setupEvents(){
  for(int i = 0; i < MAX_NUM_EVENTS; i++){
    event_cone[i] = OPEN_EVENT_CODE;
  }

  for(int i = 0; i < MAX_CONE_NUM; i++){
    times[i][1] = NO_EVENT_PLANNED;
  }
  
  //for(int i = 0; i < MAX_CONE_NUM; i++){
  //  addEventToStack(i, 0xFF000000, 2500);
  //  addEventToStack(i, 0xFFFF0000, 7500);
  //}


  //  for(int i = 0; i < MAX_CONE_NUM; i++){
  //    colors[i][1] = 0xFF00FF00;
  //    times[i][1] = 5000;
  //  }
}


void doEventMode(){
//    for(int i = 0; i < 20; i++){
//      Serial.print(colors[i][0]); Serial.print(" to "); Serial.print(colors[i][1]);
//      Serial.print(" from "); Serial.print(times[i][0]); Serial.print(" to "); Serial.println(times[i][1]);
//    }
//    delay(500);

//      Serial.print("MILLIS: "); Serial.println(millis());

  eventStackToTransition();

  for(int i = 0; i < NUM_CONES; i++){
    transitionCone(i, true);
  }

  pixels.show();

}  // event








bool transitionCone(Cone cone, bool repeat){

  if (times[cone][1] == NO_EVENT_PLANNED){
    return false;
  }
  
  // the starting and ending colors for the event being transitioned
  IndividualColor r[2] = {(colors[cone][0] >> 8) & 0xFF, (colors[cone][1] >> 8) & 0xFF};
  IndividualColor g[2] = {(colors[cone][0] >> 16) & 0xFF, (colors[cone][1] >> 16) & 0xFF};
  IndividualColor b[2] = {(colors[cone][0]) & 0xFF, (colors[cone][1]) & 0xFF};
  IndividualColor w[2] = {(colors[cone][0] >> 24) & 0xFF, (colors[cone][1] >> 24) & 0xFF};

  // answer these questions:
  //
  // why are we subtracting `start`?  
  // why are we modding by lengthOfShow?
  // should the call to `millis()` be factored out, so that all cones being transisioned at once 
  // will use the same time?  otherwise, they use slightly different times...
  Time current_time = (millis() - start) % lengthOfShow;
  
  // unpack from the stored array
  Time start_time = times[cone][0];
  Time final_time = times[cone][1];
  
  // this block feels weird to me.  why would the times be out of order?  shouldn't they be always increasing?
  if (times[cone][0] > times[cone][1] && current_time > times[cone][0]){
    // times are out of order, so roll forward the final time
    final_time += lengthOfShow;
  } 
  else if (times[cone][0] > times[cone][1] && current_time < times[cone][0]){
    // times are out of order, so roll forward the final time and the current time.
    final_time += lengthOfShow;
    current_time += lengthOfShow;
  }
  
  if(current_time >= final_time) current_time = final_time;

#ifdef CUBIC_INTERP
  IndividualColor final_red = (IndividualColor) round(cubicNatural(current_time, start_time, final_time, r[0], r[1]));
  IndividualColor final_green = (IndividualColor) round(cubicNatural(current_time, start_time, final_time, g[0], g[1]));
  IndividualColor final_blue = (IndividualColor) round(cubicNatural(current_time, start_time, final_time, b[0], b[1]));
  IndividualColor final_white = (IndividualColor) round(cubicNatural(current_time, start_time, final_time, w[0], w[1]));
  //Serial.print("final red: "); Serial.println(final_red);

#else

  float mult = ((float)(current_time - start_time))/((float)(final_time - start_time));

  IndividualColor final_red = mult * (r[1] - r[0]) + r[0];
  IndividualColor final_green = mult * (g[1] - g[0]) + g[0];
  IndividualColor final_blue = mult * (b[1] - b[0]) + b[0];
  IndividualColor final_white = mult * (w[1] - w[0]) + w[0];
#endif

  coneColor(cone, final_green, final_red, final_blue, final_white);

//#ifdef DEBUG_PRINT
//  Serial.print("Current Time: "); Serial.print(current_time); Serial.print(" End of Transition: "); Serial.println(final_time);
//#endif

  if (current_time >= final_time){
    if (repeat) 
      addEventToStack(cone, colors[cone][1], times[cone][1]);
    
    // roll over the color and time for the current cone, and indicate next one is open.
    colors[cone][0] = colors[cone][1];
    times[cone][0] = times[cone][1];
    times[cone][1] = NO_EVENT_PLANNED;
  }

  return true;

}






// loops over all cones in the fixture, and calls `transitionCone` for each.
void transitionAllCones(){
  for(int i = 0; i < NUM_CONES; i++){
    transitionCone(i, false);
  }

  pixels.show();
}







 
