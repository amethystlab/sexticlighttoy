
#include "stack.h"


//returns the specific color that is within the cone by looking at one pixel
Color getConeColor(Cone coneNum) {
  if (coneNum >= 0 && coneNum < NUM_CONES) {
    uint8_t pixStart = (coneNum) * NUM_PIXELS_PER_GROUP; //determines which pixel out of 140 to fill, inside coneNum
    return pixels.getPixelColor(pixStart);
  }
  return 0; //if for some reason the coneNum isn't in range then zero is returned
} 



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

  for (int cone = 0; cone < MAX_CONE_NUM; cone++){
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


bool transitionCone(Cone coneNum, bool repeat){

  if (times[arrayIndex][1] == NO_EVENT_PLANNED){
    return false;
  }
  
  uint8_t r[2] = {(colors[coneNum][0] >> 8) & 0xFF, (colors[coneNum][1] >> 8) & 0xFF};
  uint8_t g[2] = {(colors[coneNum][0] >> 16) & 0xFF, (colors[coneNum][1] >> 16) & 0xFF};
  uint8_t b[2] = {(colors[coneNum][0]) & 0xFF, (colors[coneNum][1]) & 0xFF};
  uint8_t w[2] = {(colors[coneNum][0] >> 24) & 0xFF, (colors[coneNum][1] >> 24) & 0xFF};

  Time currentTime = (millis() - start) % lengthOfShow;

  Time startTime = times[coneNum][0];
  Time finalTime = times[coneNum][1];
  
  if(times[coneNum][0] > times[coneNum][1] && currentTime > times[coneNum][0]){
    finalTime += lengthOfShow;
  } else if(times[coneNum][0] > times[coneNum][1] && currentTime < times[coneNum][0]){
    finalTime += lengthOfShow;
    currentTime += lengthOfShow;
  }
  
  if(currentTime >= finalTime) currentTime = finalTime;

#ifdef CUBIC_INTERP
  uint8_t finalRed = (uint8_t) round(cubicNatural(currentTime, startTime, finalTime, r[0], r[1]));
  uint8_t finalGreen = (uint8_t) round(cubicNatural(currentTime, startTime, finalTime, g[0], g[1]));
  uint8_t finalBlue = (uint8_t) round(cubicNatural(currentTime, startTime, finalTime, b[0], b[1]));
  uint8_t finalWhite = (uint8_t) round(cubicNatural(currentTime, startTime, finalTime, w[0], w[1]));
  //Serial.print("final red: "); Serial.println(finalRed);

#else

  float mult = ((float)(currentTime - startTime))/((float)(finalTime - startTime));

  uint8_t finalRed = mult * (r[1] - r[0]) + r[0];
  uint8_t finalGreen = mult * (g[1] - g[0]) + g[0];
  uint8_t finalBlue = mult * (b[1] - b[0]) + b[0];
  uint8_t finalWhite = mult * (w[1] - w[0]) + w[0];
#endif

  coneColor(coneNum, finalGreen, finalRed, finalBlue, finalWhite);

//#ifdef DEBUG_PRINT
//  Serial.print("Current Time: "); Serial.print(currentTime); Serial.print(" End of Transition: "); Serial.println(finalTime);
//#endif

  if (currentTime >= finalTime){
    if (repeat) 
      addEventToStack(coneNum, colors[coneNum][1], times[coneNum][1]);
    
    colors[coneNum][0] = colors[coneNum][1];
    times[coneNum][0] = times[coneNum][1];
    times[coneNum][1] = NO_EVENT_PLANNED;
  }

  return true;

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


void transitionAllCones(){
  for(int i = 0; i < NUM_CONES; i++){
    transitionCone(i, false);
  }

  pixels.show();
}
