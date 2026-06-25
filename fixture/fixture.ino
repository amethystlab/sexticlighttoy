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


#include "globals.h"
#include "device.h"
#include "stack.h"
#include "rotation.h"

// then the files that define the various modes you can use by switching switches
#include "manual_rotate.h"
#include "auto_rotate.h"
#include "pulse.h"



/*---------------------------------------------------------------------*/







// called once at the beginning
void setup() {
  setupSerial();
  setupPixels();
  setupCommunication();
  setupFrames();
  setupTime();

  delay(100); // waste some time for communication to get synced.
  Serial.println(F("Finished Setup"));
}









// called ad nauseum
void loop() {
  // doPulseMode();

  // doManualRotate();

#ifdef DEBUG_CONTROLLER_STATE
  // throttled dump of the latest controller inputs received over I2C, so we can
  // verify the controller is actually talking to the fixture without flooding.
  static unsigned long last_state_print = 0;
  if (millis() - last_state_print >= 300) {
    last_state_print = millis();
    print_state();
  }
#endif

  getModeFromSwitches();

  // keep the color rotation continuous when switching between auto and manual
  // rotate. both render the same way and differ only in which offset drives the
  // hue, so hand the offset across on the transition instead of letting it jump.
  if (g_mode != g_previous_dispatch_mode){
    bool now_manual = (g_mode == ManualRotate || g_mode == ManualRotateHighlight);
    bool now_auto   = (g_mode == AutoRotate   || g_mode == AutoRotateHighlight);
    bool was_manual = (g_previous_dispatch_mode == ManualRotate || g_previous_dispatch_mode == ManualRotateHighlight);
    bool was_auto   = (g_previous_dispatch_mode == AutoRotate   || g_previous_dispatch_mode == AutoRotateHighlight);

    if (now_manual && !was_manual){
      // entering manual: start the encoder-driven offset right where the colors are.
      g_manual_color_offset = g_auto_color_rotation;
      g_manual_target       = (int32_t)g_auto_color_rotation;
      g_manual_last_encoder = encoder_counter;
      g_previous_manual_rotate_time = getCurrentTime();
    }
    else if (now_auto && !was_auto){
      // entering auto: continue rotating from where manual left the colors.
      g_auto_color_rotation = positive_mod((int32_t)g_manual_color_offset, MAX_UINT16);
      g_previous_auto_rotate_time = getCurrentTime();
    }
    g_previous_dispatch_mode = g_mode;
  }

  switch(g_mode){
    // case Rotational:
    //   {doRotationalMode(); break;}
    // case Reflectional:
    //   {doReflectionalMode(); break;}
    // case ColorSet:
    //   {doColorSetMode(); break;}
    // case EventMode:
    //   {doEventMode(); break;}
    case ManualRotateHighlight:
      {doManualRotate(true); break;}
    case AutoRotateHighlight:
      {doAutoRotate(true); break;}
      
    case ManualRotate:
      {doManualRotate(false); break;}
    case AutoRotate:
      {doAutoRotate(false); break;}

    // lots of switch positions make it pulse.  i hate it.
    case Something1:
    case Something2:
    case Something3:
    case Pulse:
      {doPulseMode(); break;}

    default:
      // do nothing
      break;
  } // switch
//   // 
//   // 
//   // if (ModeUsesEvents(mode)){
//   //   transitionAllCones();
//   // }
} // ends the loop() function

bool ModeUsesEvents(ObjectMode mode){
  switch (mode) {
    case Rotational:
    case Reflectional:
    case ColorSet:
      return true;
    default:
      return false;
  }
}
