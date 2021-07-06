
#include "stack.h"



void printStack(){
  // Serial.println("current event stack:");
  for (uint8_t i{0}; i<MAX_NUM_EVENTS; ++i){
    printEvent(event_stack[i]);
  }
}

void printEvent(Event e){
  Serial.print(e.cone); Serial.print(" "); Serial.print(e.color); Serial.print(" "); Serial.println(e.duration);
}

void printActive(){
  Serial.println("active state:");
  for (Cone i{0}; i<NUM_CONES; ++i){
    Serial.print(active_colors[i][0]);Serial.print(" ");Serial.print(active_times[i][0]); Serial.print(" --> ");
    Serial.print(active_colors[i][1]);Serial.print(" ");Serial.println(active_times[i][1]);
  }
}

void printConeColorsFromMemory(){
  // Serial.println("cone colors:");
  for (Cone i{0}; i<NUM_CONES; ++i){
    Serial.println(getConeColor(i));
  }
}
// sets first available event to change the cone to color.
bool addEventToStack(Cone cone, Color color, Time duration){
  
  for (int i = 0; i < MAX_NUM_EVENTS; ++i){
    if (event_stack[i].cone == OPEN_EVENT_CODE){
 
      event_stack[i].cone = cone;
      event_stack[i].color = color;
      event_stack[i].duration = duration;

      // Serial.print("EVENT ADDED TO STACK at position"); Serial.println(i);

      return true;
    }
  }
  return false;
}

void eventStackToActive(){
  unsigned long current_time = millis();
  
  // Serial.print("current time: "); Serial.println(current_time);
  // printActive();
  
  for (Cone cone = 0; cone < NUM_CONES; ++cone){
    if (current_time > active_times[cone][1]){ // availability of a cone is indicated by the current time being later than the "to" time in the array.
      

      // loop over all events on the stack,
      // if matches current cone,
      // make be next active, and remove from stack.
      for (uint8_t j = 0; j < MAX_NUM_EVENTS; ++j){
        if (event_stack[j].cone == cone){
#ifdef DEBUG_PRINT
          Serial.print("Cone Num "); Serial.print(cone_num); Serial.print(" == "); Serial.println(event_cone[j]);
#endif    
            // set the old active time and color to be the current ones.
            active_times[cone][0] = current_time;
            active_colors[cone][0] = getConeColor(cone); // old color is whatever the pixels think it is.

            active_times[cone][1]  = current_time + event_stack[j].duration;
            active_colors[cone][1] = event_stack[j].color;
            
            
            // now, remove that event from the stack
            // this should probably be a function, especially if it is used elsewhere.
            for (uint8_t z{j}; z<MAX_NUM_EVENTS-1; ++z){
              event_stack[z] = event_stack[z+1];
            }
            break;
        }
      } // for events on stack

    } // if cone has no event planned
  }  // for each cone 
  
} // function



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
  for (uint8_t i = 0; i < MAX_NUM_EVENTS; i++){
    event_stack[i] = Event();
  }

  for (Cone i = 0; i < NUM_CONES; i++){
    active_times[i][0] = NO_EVENT_PLANNED;
    active_times[i][1] = NO_EVENT_PLANNED;
    
    active_colors[i][0] = BLACK;
    active_colors[i][1] = BLACK;
  }
}


void doEventMode(){

  eventStackToActive();

  for(int i = 0; i < NUM_CONES; i++){
    transitionCone(i); 
  }

  pixels.show();

}  // event








bool transitionCone(Cone cone){

  if (active_times[cone][1] == NO_EVENT_PLANNED){
    Serial.print("due to NO_EVENT_PLANNED, skipping cone "); Serial.println(cone);
    return false;
  }
  
  // the starting and ending colors for the event being transitioned
  IndividualColor r[2] = {(active_colors[cone][0] >> 8)  & 0xFF,  (active_colors[cone][1] >> 8)  & 0xFF};
  IndividualColor g[2] = {(active_colors[cone][0] >> 16) & 0xFF,  (active_colors[cone][1] >> 16) & 0xFF};
  IndividualColor b[2] = {(active_colors[cone][0])       & 0xFF,  (active_colors[cone][1])       & 0xFF};
  IndividualColor w[2] = {(active_colors[cone][0] >> 24) & 0xFF,  (active_colors[cone][1] >> 24) & 0xFF};

  // answer these questions:
  //
  // why are we subtracting `start`?  
  // why are we modding by lengthOfShow?
  // should the call to `millis()` be factored out, so that all cones being transisioned at once 
  // will use the same time?  otherwise, they use slightly different times...
  Time current_time = millis();

  // unpack from the stored array
  Time start_time = active_times[cone][0];
  Time final_time = active_times[cone][1];
  
  
  // the current time is later than the final time of the current event, 
  // so setting the final_time to the current time makes the
  // cone's color get set to the final color for the event.
  if(current_time < final_time) 
  {
    Serial.print("transitioning cone "); Serial.print(cone); Serial.print(" at time "); Serial.println(current_time);
      
    #ifdef CUBIC_INTERP
    IndividualColor final_red = (IndividualColor) round(cubicNatural(current_time, start_time, final_time, r[0], r[1]));
    IndividualColor final_green = (IndividualColor) round(cubicNatural(current_time, start_time, final_time, g[0], g[1]));
    IndividualColor final_blue = (IndividualColor) round(cubicNatural(current_time, start_time, final_time, b[0], b[1]));
    IndividualColor final_white = (IndividualColor) round(cubicNatural(current_time, start_time, final_time, w[0], w[1]));
    //Serial.print("final red: "); Serial.println(final_red);

  #else
    // linear interpolation
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
  }
  else {
    Serial.print("cone "); Serial.print(cone); Serial.print("over time"); Serial.println(current_time);
    coneColor(cone, g[1], r[1], b[1], w[1]);
  }

  return true;
}






// loops over all cones in the fixture, and calls `transitionCone` for each.
void transitionAllCones(){
  
  for(int i = 0; i < NUM_CONES; i++){
    transitionCone(i);
  }

  pixels.show();
}







 
