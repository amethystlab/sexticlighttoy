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
#include "diagnostic.h"



/*---------------------------------------------------------------------*/







// called once at the beginning
void setup() {
  setupSerial();
  setupPixels();
  setupCommunication();
  setupEvents();
  setupTime();

  delay(100);
  Serial.println("Finished Setup");
}









// called ad nauseum
void loop() {
  // doPulseMode();
  print_state();
  doDiagnosticMode();
  
  // getMode();
  // 
  // switch(mode){
  //   case Rotational:
  //     {doRotationalMode(); break;}
  //   case Reflectional:
  //     {doReflectionalMode(); break;}
  //   case ColorSet:
  //     {doColorSetMode(); break;}
  //   case Event:
  //     {doEventMode(); break;}
  //   case Pulse:
  //     {doPulseMode(); break;}
  //   case Diagnostic:
  //     {doDiagnosticMode(); break;}
  //   default:
  //     break;
  // } // switch
  // 
  // 
  // if(mode != Event){
  //   transitionAllCones();
  // }

} // ends the loop() function
