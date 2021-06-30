
#include "stack.h"


//returns the specific color that is within the cone by looking at one pixel
uint32_t getConeColor(uint8_t coneNum) {
  if (coneNum >= 1 && coneNum <= 20) {
    int pixStart = (coneNum - 1) * 7; //determines which pixel out of 140 to fill, inside coneNum
    return pixels.getPixelColor(pixStart);
  }
  return 0; //if for some reason the coneNum isn't in the range of 1 to 20 then zero is returned
} //end Sam 7-14-20



bool addEventToStack(uint8_t cone, uint32_t color, uint16_t timeOfEvent){
  bool set = false;
  for(int i = 0; i < MAX_NUM_EVENTS && !set; i++){
    if(event_cone[i] == OPEN_EVENT_CODE){
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

  for(int i = 0; i < MAX_CONE_NUM; i++){
    if(times[i][1] == NO_EVENT_PLANNED){
      uint8_t coneNum = i + 1;
      times[i][0] = currentTime;
      colors[i][0] = getConeColor(i + 1);
      int32_t proximity = 65535;
      uint8_t closestEvent = 255;

      for(int j = 0; j < MAX_NUM_EVENTS; j++){
        if(event_cone[j] == coneNum){
#ifdef DEBUG_PRINT
          Serial.print("Cone Num "); Serial.print(coneNum); Serial.print(" == "); Serial.println(event_cone[j]);
#endif
          int32_t tempProximity = event_times[j] + ((event_times[j] < currentTime) ? lengthOfShow : 0) - currentTime;
          if(tempProximity < proximity){
            proximity = tempProximity;
            times[i][1] = event_times[j];
            colors[i][1] = event_colors[j];
            closestEvent = j;
          }
        }
      }

      if(closestEvent != 255){
#ifdef DEBUG_PRINT
        Serial.print("Setting cone "); Serial.print(coneNum); Serial.print(" to "); Serial.print((colors[i][1] >> 16) & 0xFF); Serial.print(", ");
        Serial.print((colors[i][1] >> 8) & 0xFF); Serial.print(", "); Serial.println((colors[i][1] >> 0) & 0xFF); Serial.print(" at time "); Serial.println(times[i][1]);
        Serial.print(" from "); Serial.print((colors[i][0] >> 16) & 0xFF); Serial.print(", ");
        Serial.print((colors[i][0] >> 8) & 0xFF); Serial.print(", "); Serial.println((colors[i][0] >> 0) & 0xFF); Serial.print(" at time "); Serial.println(times[i][0]);
#endif
       
        event_cone[closestEvent] = OPEN_EVENT_CODE;
        // we just put this event on the transition phase
        // so open it up to be used again
      }
      
    }
  }  
}





float t(uint16_t x, uint16_t x1, uint16_t x2){
  //Serial.print((x - x1)); Serial.print(" / "); Serial.println((x2 - x1));
  //Serial.println((float) (x - x1) / (float) (x2 - x1));
  
  return ((float) (x - x1) / (float) (x2 - x1));
}

float cubicNatural(uint16_t x, uint16_t x1, uint16_t x2, uint8_t y1, uint8_t y2){
  float tOfX = t(x, x1, x2);
  //Serial.print("y1: "); Serial.print(y1); Serial.print(" y2: "); Serial.println(y2);
  //Serial.print("t(x) = "); Serial.println(tOfX); //Serial.print(", "); Serial.print((1 - tOfX) * y1); Serial.print(", "); Serial.print(tOfX * y2); Serial.print(", "); Serial.println(tOfX * (1 - tOfX) * ((1 - tOfX) * (y1 - y2) + tOfX * (y2 - y1)));
  return (1 - tOfX) * y1 + tOfX * y2 + tOfX * (1 - tOfX) * ((1 - tOfX) * (y1 - y2) + tOfX * (y2 - y1));
}


bool transitionCone(uint8_t coneNum, bool repeat){
  uint8_t arrayIndex = coneNum - 1;

  if(times[arrayIndex][1] == NO_EVENT_PLANNED){
    return false;
  }
  
  uint8_t r[2] = {(colors[arrayIndex][0] >> 8) & 0xFF, (colors[arrayIndex][1] >> 8) & 0xFF};
  uint8_t g[2] = {(colors[arrayIndex][0] >> 16) & 0xFF, (colors[arrayIndex][1] >> 16) & 0xFF};
  uint8_t b[2] = {(colors[arrayIndex][0]) & 0xFF, (colors[arrayIndex][1]) & 0xFF};
  uint8_t w[2] = {(colors[arrayIndex][0] >> 24) & 0xFF, (colors[arrayIndex][1] >> 24) & 0xFF};

  uint16_t currentTime = (millis() - start) % lengthOfShow;

  uint16_t startTime = times[arrayIndex][0];
  uint16_t finalTime = times[arrayIndex][1];
  
  if(times[arrayIndex][0] > times[arrayIndex][1] && currentTime > times[arrayIndex][0]){
    finalTime += lengthOfShow;
  } else if(times[arrayIndex][0] > times[arrayIndex][1] && currentTime < times[arrayIndex][0]){
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

  if(currentTime >= finalTime){
    if(repeat) addEventToStack(coneNum, colors[arrayIndex][1], times[arrayIndex][1]);
    
    colors[arrayIndex][0] = colors[arrayIndex][1];
    times[arrayIndex][0] = times[arrayIndex][1];
    times[arrayIndex][1] = NO_EVENT_PLANNED;
  }

  return true;

}
