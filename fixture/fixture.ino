// This file is part of sexticlighttoy.
//
// sexticlighttoy is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// sexticlighttoy is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with sexticlighttoy.  If not, see <https://www.gnu.org/licenses/>.

//#define DEBUG_PRINT

#include <Adafruit_NeoPixel.h> // see https://github.com/adafruit/Adafruit_NeoPixel







#include "globals.h"
#include "device.h"
#include "stack.h"
#include "rotation.h"
#include "diagnostic.h"

//////////////////////



/*
 * Each uint16_t stores 3 numbers.
 * The index represents a specific cone on the barth sextic,
 * whereas the values stored inside the uint16_t hold the cone numbers
 * that are connected
 * 
 * ________________________________________________________________________________
 * | First Connected Cone | Second Connected Cone | Third Connected Cone  |        |
 * |-------------------------------------------------------------------------------|
 * | 5 Bits               | 5 Bits                | 5 Bits                | 1 Bit  |
 * |-------------------------------------------------------------------------------|
 * 
 * The connections are ordered such that if you were to hold the geometric object
 * from the cone specified by the index, the ordering of the connections would follow
 * a circle around the cone, ANTI-clockwise from above, looking from the point of the cone
 * to the center of the fixture.
 *
 * Math question (combinatorics): How many valid numberings are there?
 * 
 * TODO: INDICATE ONE OFF INDEXING HANDLED BY GET_CONNECTION FUNCTION
 *  
 * This is packed into uint16_ts to save space, as the arduino platform is limited in memory.
 * 
 */







/*---------------------------------------------------------------------*/







// called once at the beginning
void setup() {
  Serial.println("BEGIN");
  pixels.begin();
  pixels.setBrightness(255); // 0 is off.  255 is full.  this is a limit on the brightness for the entire run.


  Wire.begin(FIXTURE_ADDRESS);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output
  Serial.setTimeout(20);

  for(int i = 0; i < MAX_NUM_EVENTS; i++){
    event_cone[i] = OPEN_EVENT_CODE;
  }

  for(int i = 0; i < MAX_CONE_NUM; i++){
    times[i][1] = NO_EVENT_PLANNED;
  }

  Serial.println("STEP 1");

//for(int i = 1; i <= MAX_CONE_NUM; i++){
//  addEventToStack(i, 0xFF000000, 2500);
//  addEventToStack(i, 0xFFFF0000, 7500);
//}


    Serial.println("STEP 2");


//  for(int i = 0; i < MAX_CONE_NUM; i++){
//    colors[i][1] = 0xFF00FF00;
//    times[i][1] = 5000;
//  }

  lengthOfShow = 10000;
  start = millis();

//  clear();
//  uint8_t arr[5] = {1,2,3,4,5};
//  setGroupPresetColor(arr, 5);
//  symmetry = FiveFold;
//  pixels.show();

  //commented out the three fold because we have the five fold in the set up, hang onto this for mode feature code
  //  //Three fold rotational symmetry set up
  //  for (int i = 0; i < 6; i++) {
  //    setGroupPresetColor(threeFoldGroups[i], 3);
  //  }
  //  coneColor(1, 0, 0, 0, 255); //sets the axis for 3-fold to white
  //  coneColor(18, 0, 0, 0, 255); //sets the axis for 3-fold to white

  //Five fold rotational symmetry set up
//  for (int i = 0; i < 4; i++) {
//    setGroupPresetColor(fiveFoldGroups[i], 5);
//  }

  Serial.println("Finished Setup");
}






// called ad nauseum
#define TICKS_PER_ROTATION 3
void loop() {

  static int32_t previousEncoderValue = 0; 

  static ObjectMode previousMode;

  if(switches[0]){
    mode = Reflectional;
  } else if(switches[1]){
    mode = Rotational;
  } else if(switches[2]){
    mode = ColorSet;
  } else {
    mode = Event;
  }

  
  switch(mode){
    case Rotational:
    
      if(previousMode != Rotational) {
        symmetry = TwoFold;
        Serial.println("Rotational Mode");

        incrementAxis(0);
        
        set_cycle_presets();
        pixels.show();
        previousMode = Rotational;
      }
      
    
      if(abs(rotary_counter - previousEncoderValue) > TICKS_PER_ROTATION && button_down)
      {
        Serial.println("Rotate Axis");
        previousEncoderValue = rotary_counter;
        incrementAxis(1);
        set_cycle_presets();

        pixels.show();
      } else if((rotary_counter - previousEncoderValue) > TICKS_PER_ROTATION) {
#ifdef DEBUG_PRINT
        Serial.print(rotary_counter - previousEncoderValue); Serial.print(" > "); Serial.println(TICKS_PER_ROTATION);
        Serial.println("Rotate Positive");
#endif
        previousEncoderValue = rotary_counter;
        rotate(false);
        pixels.show();
      } else if((previousEncoderValue - rotary_counter) > TICKS_PER_ROTATION){
#ifdef DEBUG_PRINT
        Serial.print("Rotate Negative "); Serial.print(rotary_counter); Serial.print(" - "); Serial.print(previousEncoderValue); Serial.print(" < -"); Serial.println(TICKS_PER_ROTATION);
#endif
        previousEncoderValue = rotary_counter;
        rotate(true);
        pixels.show();
      }

      if(buttons[0]){
        Serial.println("Set twofold");
        symmetry = TwoFold;
        incrementAxis(0);
        set_cycle_presets();
        pixels.show();
      } else if(buttons[2]){
        Serial.println("Set threefold");
        symmetry = ThreeFold;
        incrementAxis(0);
        set_cycle_presets();

        pixels.show();
      } else if(buttons[1]){
        Serial.println("Set fivefold");
        symmetry = FiveFold;
        incrementAxis(0);
        set_cycle_presets();

        pixels.show();
      }
      
    break;
    case Reflectional:
      symmetry = Reflect;
    
      if(previousMode != Reflectional) {
        Serial.println("Reflectional Mode");
        incrementAxis(0);
        set_cycle_presets();
        pixels.show();
        previousMode = Reflectional;
      }
    

      if(abs(rotary_counter - previousEncoderValue) > TICKS_PER_ROTATION && button_down)
      {
        Serial.println("Change reflection axis");
        previousEncoderValue = rotary_counter;
        incrementAxis(1);
        set_cycle_presets();

        pixels.show();
      } else if(abs(rotary_counter - previousEncoderValue) > TICKS_PER_ROTATION) {
        Serial.println("Reflect");
        previousEncoderValue = rotary_counter;
        rotate(false);
        pixels.show();
      }
    break;
    case ColorSet:
      if(previousMode != ColorSet) {
        Serial.println("ColorSet Mode");
        clear();
        previousMode = ColorSet;
      }

    
      coneSelect();
      pixels.show();
    break;
    case Event:
//    for(int i = 0; i < 20; i++){
//      Serial.print(colors[i][0]); Serial.print(" to "); Serial.print(colors[i][1]);
//      Serial.print(" from "); Serial.print(times[i][0]); Serial.print(" to "); Serial.println(times[i][1]);
//    }
//    delay(500);

//      Serial.print("MILLIS: "); Serial.println(millis());

      eventStackToTransition();

      for(int i = 1; i <= MAX_CONE_NUM; i++){
        transitionCone(i, true);
      }

      pixels.show();
    break;
  }

  if(mode != Event){
    for(int i = 1; i <= MAX_CONE_NUM; i++){
      transitionCone(i, false);
    }

    pixels.show();
  }

} // ends the loop() function
