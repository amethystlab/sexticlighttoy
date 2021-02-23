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
#include <Wire.h> // see https://www.arduino.cc/en/Tutorial/MasterReader

void setGroupPresetColor(uint8_t *coneArray, uint8_t numberOfCones);
void nFoldRotateColor(uint8_t *coneArray, uint8_t numCone, bool reverse);
void clear();
void coneColor (int coneNum, uint8_t g, uint8_t r, uint8_t b, uint8_t w);

#define NEOPIXEL_PIN 12 // Depends on which pin you used on your Arduino when wiring..

// some global variables
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(140, NEOPIXEL_PIN,  NEO_RGBW + NEO_KHZ800); // 140, because 20*7 = 140.
unsigned long previousMillis = 0, currentMillis = 0;

uint16_t pot1, pot2, pot3;

bool buttons[3], switches[3];

unsigned long button_down_time = 0L; // [ms] the time the encoder button has been pushed so far.  is 0 if the button's not being pressed
bool button_down = 0; // is the button in the encoder being pushed?
int32_t rotary_counter = 0; // current "position" of rotary encoder (increments clockwise, decrements anticlockwise)

#define FIXTURE_ADDRESS 4


// define some hexadecimal colors.  Handy!
#define WHITE 0xFFFFFF

#define RED 0xFF0000
#define GREEN 0x00FF00
#define BLUE 0x0000FF

#define YELLOW 0xFFFF00
#define MAGENTA 0xFF00FF
#define CYAN 0x00FFFF

#define BLACK 0x000000

uint32_t colorPresets [5] = {GREEN, RED, BLUE, WHITE, YELLOW}; 
//use an array of 5 since at most we are looking at 5-fold rotational symmetry (g,r,b,w,y)



//uint8_t threeFoldGroups [6][3] = {{5, 14, 2}, {15, 12, 4}, {6, 13, 3}, {7, 20, 10}, {8, 16, 11}, {17, 19, 9}}; //[6] indicates amount of arrays, [3] arrays in the array
//uint8_t fiveFoldGroups [4][5] = {{1,2,3,4,5}, {14,12,10,8,6}, {9,7,15,13,11}, {18,17,16,20,19}}; //the numbering of these groups are in clockwise-order starting at "top" cone

//{1,2,3,4,5} = r,g,b,w,y
//{14,12,10,8,6} = r,g,b,w,y
//{9,7,15,13,11} = r,g,b,w,y
//{18,17,16,20,19} = r,g,b,w,y



typedef enum SymmetryType {
  TwoFold,
  ThreeFold,
  FiveFold,
  Reflect
};

typedef enum ObjectMode {
  ColorSet,
  Rotational,
  Reflectional,
  Event
};

#define MIN_CONE_NUM 1
#define MAX_CONE_NUM 20

#define GRADUAL_TRANSITION

///////////////////////
#define NO_EVENT_PLANNED 65535

uint32_t colors[20][2];
uint16_t times[20][2];

#define MAX_NUM_EVENTS 40
uint8_t event_cone[MAX_NUM_EVENTS];
uint32_t event_colors[MAX_NUM_EVENTS];
uint16_t event_times[MAX_NUM_EVENTS];

unsigned long start;
uint16_t lengthOfShow;


#define OPEN_EVENT_CODE 0
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



//////////////////////


ObjectMode mode = Event;
SymmetryType symmetry = TwoFold; 

/*---------------------------------------------------------------------*/
// MACROS TO EASE WITH THE PROCESS OF WRITING THIS OUT AS UINT16_T
#define MAX_UINT5 31

#define CONNECTION_NUM_SETUP(num_connection, num_cone) ((num_cone << 5*(num_connection)) & (MAX_UINT5 << 5*(num_connection)))
#define MAKE_CONNECTION(a,b,c) CONNECTION_NUM_SETUP(0, a) | CONNECTION_NUM_SETUP(1, b) | CONNECTION_NUM_SETUP(2, c)

#define MAX_CONNECTION_NUM 2


#define FIXTURE 1  // select which physical wiring path you implemented.  
// Maybe it's one of these?  Cross those fingers!  Explanation below.

// define the connections between cones.
#if FIXTURE==1 // Will's.  The first one silviana made.
uint16_t connections[20] = {
  MAKE_CONNECTION(4,1,7),
  MAKE_CONNECTION(9,0,2),
  MAKE_CONNECTION(11,1,3),
  MAKE_CONNECTION(4,13,2),
  MAKE_CONNECTION(3,0,5),

  MAKE_CONNECTION(6,14,4),
  MAKE_CONNECTION(5,7,19),
  MAKE_CONNECTION(8,6,0),
  MAKE_CONNECTION(18,7,9),
  MAKE_CONNECTION(8,1,10),

  MAKE_CONNECTION(17,9,11),
  MAKE_CONNECTION(12,10,2),
  MAKE_CONNECTION(16,11,13),
  MAKE_CONNECTION(14,12,3),
  MAKE_CONNECTION(15,13,5),

  MAKE_CONNECTION(16,14,19),
  MAKE_CONNECTION(12,15,17),
  MAKE_CONNECTION(10,16,18),
  MAKE_CONNECTION(17,19,8),
  MAKE_CONNECTION(18,15,6)
};

#elif FIXTURE==2 // Samantha's
uint16_t connections[20] = {
  MAKE_CONNECTION(4,1,13),
  MAKE_CONNECTION(0,2,11)
  MAKE_CONNECTION(9,1,3),
  MAKE_CONNECTION(7,2,4),
  MAKE_CONNECTION(0,5,3),

  MAKE_CONNECTION(14,6,4),
  MAKE_CONNECTION(16,7,5),
  MAKE_CONNECTION(8,3,6),
  MAKE_CONNECTION(17,9,7),
  MAKE_CONNECTION(10,2,8),

  MAKE_CONNECTION(18,11,9),
  MAKE_CONNECTION(12,1,10),
  MAKE_CONNECTION(19,13,11),
  MAKE_CONNECTION(14,0,12),
  MAKE_CONNECTION(15,5,13),

  MAKE_CONNECTION(19,16,14),
  MAKE_CONNECTION(17,6,15),
  MAKE_CONNECTION(18,8,16),
  MAKE_CONNECTION(19,10,17),
  MAKE_CONNECTION(15,12,18)
};
#endif


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

uint8_t cycles[20];
uint8_t num_per_rotation = 3;
/*
 * Array to store the cyclic groups of rotation and reflection
*/

#define CONE_DNE 255 // Error code returned by get_connection when provided a nonexistant cone number 
#define CONNECTION_DNE 254; // Error code returned by get_connection when provided a connection number that is too large
uint8_t get_connection(uint8_t cone, uint8_t connection_num)
{
  if(cone < MIN_CONE_NUM || cone > MAX_CONE_NUM) return CONE_DNE;
  if(connection_num > MAX_CONNECTION_NUM) return CONNECTION_DNE;
  
  return ((connections[cone - 1] >> (5*connection_num)) & MAX_UINT5) + 1;
}


void diagnostic_check(){
  // Store the current cone we are focusing on, as well as the 
  // previous state of the switch
  static int num = 1;
  static bool prevState = false;

  if(switches[0] != prevState){
    // if the switch is flipped from its previous position
    prevState = switches[0];
    clear();

    num = (num % MAX_CONE_NUM) + 1;
    //Iterate to the next cone number

#ifdef DEBUG_PRINT
    Serial.print("NUM CONE: ");
    Serial.println(num, DEC);
#endif

    // Light up the current cone (cone number num) green
    coneColor(num, 255, 0, 0, 0);
    for(int i = 0; i <= MAX_CONNECTION_NUM; i++){
      // Light up the cones connected to the current cone red
      uint8_t conn = get_connection(num, i);
#ifdef DEBUG_PRINT
      Serial.print("CONNECTED CONE: ");
      Serial.println(conn, DEC);
#endif
      coneColor(conn, 0, 255, 0, 0);
    }

    pixels.show();

    
  }
}

#define SRC_NEXT_UNCONNECTED_ERR 255
#define POSITIVE 1
#define NEGATIVE 0


// Function used to find the third element (cone number) of a pentagon on
// the geometric structure, given the cone oriented on top of the object,
// the second cone in the pentagon sequence, and the direction around the
// top oriented cone from the second cone to where the third should be found

uint8_t findThird(uint8_t src, uint8_t next, uint8_t dir){
  uint8_t third = SRC_NEXT_UNCONNECTED_ERR;
  bool found = false;

  for(int i = 0; i <= MAX_CONNECTION_NUM && !found; i++){
    uint8_t conn = get_connection(src, i);

    //find the "next" cone in the list of connections of the 
    //source cone - if it does not exist, return an error
    
    if(conn == next){
      // When we find the "next" cone in the list of connected cones
      // find the cone that is oriented clockwise (negative) or counterclockwise
      // (positive) from the "next" cone, when orienting the object
      // such that the src cone is on top
      
      if(dir == POSITIVE){
        // if direction is 1 / positive

        // As the list of connections is ordered to be the cycle around the cone
        // specified by the index, we can simply take the next element in the list
        third = get_connection(src, (i + 1) % (MAX_CONNECTION_NUM + 1));
      }
      else {
        third = get_connection(src, (i - 1 + MAX_CONNECTION_NUM + 1) % (MAX_CONNECTION_NUM + 1));
      }
      found = true;
    }
  }

  return third;
}


#define THREEFOLD_AXIS_CONE_INDEX 0
void set_threefold(uint8_t src){
  // Set the seed for the threefold rotatoin
  cycles[THREEFOLD_AXIS_CONE_INDEX] = src;
}

void set_twofold(uint8_t one, uint8_t two){
  cycles[0] = one;
  cycles[1] = two;
}

// Function to set an array of length five to the five elements of 
// the pentagon defined by a source cone, a cone connected to that 
// source cone, and the direction to iterate from there.
void set_pentagon(uint8_t *arr, uint8_t src, uint8_t next, uint8_t dir){

  // Set initial three elements of the pentagon by using the set
  // parameters, and the function used to find the third cone
  // from the direction
  arr[0] = next;
  arr[1] = src;
  
  for(int i = 2; i < 5; i++){
    arr[i] = findThird(arr[i - 1], arr[i - 2], POSITIVE);
  }

  

  /*------------------------------------------------------------*/
  //    Source Cone
  //      /    \
  //Next /      \  Third 
  //Cone \      /  Cone
  //      \____/

  // TODO: Investigate iterative path rather than
  //       iterating through every single path of length
  //       two from the cone in array location 0

//  uint8_t potentialConnectionsNext[2];
//  uint8_t potentialConnectionsThird[2];
//  int j = 0;
//
//  // Find the cones connected to the "next" cone
//  // that are not the source cone
//  for(int i = 0; i <= MAX_CONNECTION_NUM; i++){
//    uint8_t conn = get_connection(cycles[0], i);
//    if(conn != src){
//      potentialConnectionsNext[j++] = conn;
//      if(j > 2) Serial.print("ERROR");
//    }
//  }
//
//  // Find the cones connected to the "third" cone
//  // that are not the source cone
//  j = 0;
//  for(int i = 0; i <= MAX_CONNECTION_NUM; i++){
//    uint8_t conn = get_connection(cycles[2], i);
//    if(conn != src){
//      potentialConnectionsThird[j++] = conn;
//      if(j > 2) Serial.print("ERROR");
//    }
//  }
//
//  bool done = false;
//  for(int i = 0; i < 2 && !done; i++){
//    for(int j = 0; j <= MAX_CONNECTION_NUM && !done; j++){
//      // iterate over all of the connections of the cones that are
//      // connected to the "next" cone
//      uint8_t conn = get_connection(potentialConnectionsNext[i], j);
//      for(int k = 0; k < 2 && !done; k++){
//
//        // For each cone that is connected to a connection of the "next"
//        // cone, check if it is connected to the "third" cone. If it is,
//        // then you have found a path of length two from the "next" cone
//        // to the "third" cone, completing the pentagon
//        
//        Serial.print("PAIR: ");
//        Serial.print(potentialConnectionsNext[i], DEC);
//        Serial.println(conn, DEC);
//
//        if(conn == potentialConnectionsThird[k]){
//          arr[4] = potentialConnectionsNext[i];
//          arr[3] = conn;
//          done = true;
//        }
//      }
//    }
//  }
}

#define NO_SUCH_CONE_ERR 250

// Find the cone that is connected to both coneOne and coneTwo
uint8_t get_mutual_connection(uint8_t coneOne, uint8_t coneTwo){
  
  for(int i = 0; i <= MAX_CONNECTION_NUM; i++){
    // Iterate throught the connections of coneOne
    uint8_t connOne = get_connection(coneOne, i);
    for(int j = 0; j <= MAX_CONNECTION_NUM; j++){
      // check to see if any of the connections of ConeTwo match
      // with the current connection from cone one - if so, return
      // the appropriate cone number
      if(connOne == get_connection(coneTwo, j)) return connOne;
    }
  }

  // if we found no mutual cones, none exist, and we return an error code
  return NO_SUCH_CONE_ERR;
}

// Function used to iterate through the connections of a cone
// specifically not returning those stored in specific indices
// of the cycles array (ie cones that we already know their cycle)
uint8_t find_connection_excluding_prev_cycles(uint8_t* arr, int cone, uint8_t min_avoid, uint8_t max_avoid)
{
    for(int i = 0; i <= MAX_CONNECTION_NUM; i++){
      uint8_t conn = get_connection(cone, i);

      // find a connection of the cone in question
      bool foundConnection = false;

      // check if the cone connection is in any of the cycle array in the
      // indices specified by the parameters
      for(int j = min_avoid; j <= max_avoid && !foundConnection; j++){
        if(conn == arr[j]) foundConnection = true;
      }

      // if the cone is connected to the cone input in the parameter
      // and is not stored in the cycles array within the specified indices
      // return that cone number
      if(!foundConnection) return conn;
    }

    // if no such cone exists, return an error code
    return NO_SUCH_CONE_ERR;
}

#define NO_CONNECTION 243
uint8_t find_connection_index(uint8_t src, uint8_t conn){
  for(int i = 0; i <= MAX_CONNECTION_NUM; i++){
    if(get_connection(src, i) == conn) return i;
  }

  return NO_CONNECTION;
}

#define NO_SUCH_DIRECTION 255
uint8_t find_direction(uint8_t src, uint8_t first, uint8_t second){
  for(int i = 0; i <= MAX_CONNECTION_NUM; i++){
    uint8_t conn = get_connection(src, i);
    if(conn == first){
      if(get_connection(src, (i + 1) % (MAX_CONNECTION_NUM + 1)) == second){
        return POSITIVE;
      } else if (get_connection(src, (i - 1 + (MAX_CONNECTION_NUM + 1)) % (MAX_CONNECTION_NUM + 1)) == second){
        return NEGATIVE;
      }
    }
  }
  return NO_SUCH_DIRECTION;
}


// Given two cones one and two, find the two cones that create
// a path of length two from cone one to cone two
//
// TODO: Replace code in set_pentagon with this function, as
//       they are mostly equivalent
uint8_t *find_middle_two(uint8_t coneOne, uint8_t coneTwo){
  uint8_t src = get_mutual_connection(coneOne, coneTwo);

  static uint8_t arr[2];

  uint8_t dir = find_direction(src, coneOne, coneTwo);
  
  arr[0] = findThird(coneTwo, src, dir);
  arr[1] = findThird(arr[0], coneTwo, dir);

#ifdef DEBUG_PRINT
  Serial.print("Top of Pentagon: "); Serial.print(src); Serial.print(" Direction: "); Serial.println(dir); 
  Serial.print("Middle Two Of: "); Serial.print(coneOne); Serial.print(" and "); Serial.print(coneTwo); Serial.print(" is "); Serial.print(arr[0]); Serial.print(" and "); Serial.println(arr[1]);
#endif

  return arr;
  
  /*-------------------------------------------------------------------------*/

//  static uint8_t arr[2];
//  uint8_t conns[2][3];
//  
//  // Find the cones connected to the coneOne
//  for(int i = 0; i < 3; i++){
//    conns[0][i] = get_connection(coneOne, i);
//  }
//  
//   // Find the cones connected to the coneTwo 
//  for(int i = 0; i < 3; i++){
//    conns[1][i] = get_connection(coneTwo, i);
//  }
//
//  bool done = false;
//  for(int i = 0; i < 3 && !done; i++){
//    for(int j = 0; j < 3 && !done; j++){
//      // iterate over all of the connections of the cones that are
//      // connected to coneOne
//      uint8_t conn = get_connection(conns[0][i], j);
//      for(int k = 0; k < 3 && !done; k++){
//
//        // For each cone that is connected to a connection of coneOne
//        // check if it is connected to the coneTwo cone. If it is,
//        // then you have found a path of length two from coneOne
//        // to coneTwo
//        
//        if(conn == conns[1][k]){
//          arr[0] = conns[0][i];
//          arr[1] = conn;
//          done = true;
//        }
//      }
//    }
//    
//  }
//
//  Serial.print("MIDDLE TWO OF "); Serial.print(coneOne, DEC); Serial.print(" and "); Serial.println(coneTwo, DEC);
//  Serial.print(arr[0], DEC); Serial.print(" and "); Serial.println(arr[1], DEC);
//  return arr;
  
}

void set_threefold_cycles(){

  // the first cycle is the cycle determined by the connections of the seed cone
  for(int i = 0; i < 3; i++){
    cycles[i + 1] = get_connection(cycles[THREEFOLD_AXIS_CONE_INDEX], i);
  }

  uint8_t *arr;

  for(int i = 0; i < 3; i++){
    // the next two sets of cycles are found by finding the sets of two
    // cones in between the cones set in the above for loop
    
    arr = find_middle_two(cycles[1 + i], cycles[1 + (i + 1) % 3]);

#ifdef DEBUG_PRINT
    Serial.print("array out: ");
    for(int i = 0; i < 2; i++){
      Serial.print(arr[0], DEC); Serial.print(", "); Serial.println(arr[1], DEC);
    }
#endif

    cycles[i + 4] = arr[0];
    cycles[i + 7] = arr[1];
  }

  for(int i = 0; i < 3; i++){
    // the next two sets of cycles are found by finding the sets of two
    // cones in between the cones set in the above for loop
    arr = find_middle_two(cycles[i + 4], cycles[(i + 1) % 3 + 7]);
    cycles[i + 10] = arr[0];
    cycles[i + 13] = arr[1];
  }

  for(int i = 0; i < 3; i++){
    // the last cycle is generated by looking at the cones in the two previously 
    // set cycles, and finding the mutual connection of the adjacent cones
    cycles[i + 16] = get_mutual_connection(cycles[i + 10], cycles[(i + 1) % 3 + 13]);
  }

  // as the source is set as cycles[0], we must find the other axis cone
  // and input it into the array, which is done in this function 
  set_missing_in_cycles(19);
}

void set_pentagon_cycles(){
  // This function sets the cycle array for five fold rotations, assuming the seed 
  // pentagon has already been set
  
  for(int i = 0; i < 5; i++){
    // As each cone in the first cycle is directly connected to a cone in the 
    // second cycle in the same ordering, we can just find the connection
    // of each cone in the first cycle that isn't in the first cycle
    
    cycles[5 + i] = find_connection_excluding_prev_cycles(cycles, cycles[i], 0, 4);
  }

  for(int i = 0; i < 5; i++){
    // Using the previously set cycle, we can simply look at the mutual connections
    // of the adjacent cones in the previously set cycle to find the next cycle
    cycles[10 + i] = get_mutual_connection(cycles[5 + i], cycles[5 + ((i + 1) % 5)]);
  }

  for(int i = 0; i < 5; i++){
    // As each cone in the fourth cycle is directly connected to a cone in the 
    // third cycle in the same ordering, we can just find the connection
    // of each cone in the first cycle that isn't in the first cycle
    cycles[15 + i] = find_connection_excluding_prev_cycles(cycles, cycles[10 + i], 5, 9);
  }
}

void set_twofold_cycles(){
  // This function sets the cycle array for two fold rotations, assuming the seed 
  // pentagon has already been set
  
  uint8_t second_level[4];
  uint8_t third_level[2];
  uint8_t fourth_level[4];
  uint8_t fifth_level[2];
  uint8_t sixth_level[4];
  uint8_t seventh_level[2];



  uint8_t conn_num = find_connection_index(cycles[0], cycles[1]);
  for(int i = 0; i < 2; i++){
    second_level[i] = get_connection(cycles[0], (conn_num + 1 + i) % (MAX_CONNECTION_NUM + 1));
  }
  
  conn_num = find_connection_index(cycles[1], cycles[0]);
  for(int i = 0; i < 2; i++){
    second_level[i + 2] = get_connection(cycles[1], (conn_num + 1 + i) % (MAX_CONNECTION_NUM + 1));
  }

  third_level[0] = get_mutual_connection(second_level[0], second_level[3]);
  third_level[1] = get_mutual_connection(second_level[1], second_level[2]);

  uint8_t *arr = find_middle_two(second_level[0], second_level[1]);
  fourth_level[0] = arr[0];
  fourth_level[1] = arr[1];
  
  arr = find_middle_two(second_level[2], second_level[3]);
  fourth_level[2] = arr[0];
  fourth_level[3] = arr[1];

  fifth_level[0] = find_connection_excluding_prev_cycles(second_level, third_level[0], 0, 3);
  fifth_level[1] = find_connection_excluding_prev_cycles(second_level, third_level[1], 0, 3);

  conn_num = find_connection_index(fifth_level[0], third_level[0]);
  for(int i = 0; i < 2; i++){
     sixth_level[i] = get_connection(fifth_level[0], (conn_num + 1 + i) % (MAX_CONNECTION_NUM + 1));
  }

  conn_num = find_connection_index(fifth_level[1], third_level[1]);
  for(int i = 0; i < 2; i++){
     sixth_level[i + 2] = get_connection(fifth_level[1], (conn_num + 1 + i) % (MAX_CONNECTION_NUM + 1));
  }

  seventh_level[0] = get_mutual_connection(sixth_level[0], sixth_level[3]);
  seventh_level[1] = get_mutual_connection(sixth_level[1], sixth_level[2]);

#ifdef DEBUG_PRINT
  Serial.print("LEVEL 1: "); Serial.print(cycles[0]); Serial.print(" "); Serial.println(cycles[1]);
  Serial.print("LEVEL 2: "); Serial.print(second_level[0]); Serial.print(" "); Serial.print(second_level[1]); Serial.print(" "); Serial.print(second_level[2]); Serial.print(" "); Serial.println(second_level[3]);
  Serial.print("LEVEL 3: "); Serial.print(third_level[0]); Serial.print(" "); Serial.println(third_level[1]);
  Serial.print("LEVEL 4: "); Serial.print(fourth_level[0]); Serial.print(" "); Serial.print(fourth_level[1]); Serial.print(" "); Serial.print(fourth_level[2]); Serial.print(" "); Serial.println(fourth_level[3]);
  Serial.print("LEVEL 5: "); Serial.print(fifth_level[0]); Serial.print(" "); Serial.println(fifth_level[1]);
  Serial.print("LEVEL 6: "); Serial.print(sixth_level[0]); Serial.print(" "); Serial.print(sixth_level[1]); Serial.print(" "); Serial.print(sixth_level[2]); Serial.print(" "); Serial.println(sixth_level[3]);
  Serial.print("LEVEL 7: "); Serial.print(seventh_level[0]); Serial.print(" "); Serial.println(seventh_level[1]);
#endif


  cycles[2] = second_level[0];
  cycles[3] = second_level[2];
  cycles[4] = second_level[1];
  cycles[5] = second_level[3];
  cycles[6] = third_level[0];
  cycles[7] = third_level[1];
  cycles[8] = fourth_level[0];
  cycles[9] = fourth_level[2];
  cycles[10] = fourth_level[1];
  cycles[11] = fourth_level[3];
  cycles[12] = fifth_level[0];
  cycles[13] = fifth_level[1];
  cycles[14] = sixth_level[0];
  cycles[15] = sixth_level[2];
  cycles[16] = sixth_level[1];
  cycles[17] = sixth_level[3];
  cycles[18] = seventh_level[0];
  cycles[19] = seventh_level[1];
}

void set_reflection_cycles(){
  // This function sets the cycle array for two fold rotations, assuming the seed 
  // pentagon has already been set
  
  uint8_t second_level[4];
  uint8_t third_level[2];
  uint8_t fourth_level[4];
  uint8_t fifth_level[2];
  uint8_t sixth_level[4];
  uint8_t seventh_level[2];



  uint8_t conn_num = find_connection_index(cycles[0], cycles[1]);
  for(int i = 0; i < 2; i++){
    second_level[i] = get_connection(cycles[0], (conn_num + 1 + i) % (MAX_CONNECTION_NUM + 1));
  }
  
  conn_num = find_connection_index(cycles[1], cycles[0]);
  for(int i = 0; i < 2; i++){
    second_level[i + 2] = get_connection(cycles[1], (conn_num + 1 + i) % (MAX_CONNECTION_NUM + 1));
  }

  third_level[0] = get_mutual_connection(second_level[0], second_level[3]);
  third_level[1] = get_mutual_connection(second_level[1], second_level[2]);

  uint8_t *arr = find_middle_two(second_level[0], second_level[1]);
  fourth_level[0] = arr[0];
  fourth_level[1] = arr[1];
  
  arr = find_middle_two(second_level[2], second_level[3]);
  fourth_level[2] = arr[0];
  fourth_level[3] = arr[1];

  fifth_level[0] = find_connection_excluding_prev_cycles(second_level, third_level[0], 0, 3);
  fifth_level[1] = find_connection_excluding_prev_cycles(second_level, third_level[1], 0, 3);

  conn_num = find_connection_index(fifth_level[0], third_level[0]);
  for(int i = 0; i < 2; i++){
     sixth_level[i] = get_connection(fifth_level[0], (conn_num + 1 + i) % (MAX_CONNECTION_NUM + 1));
  }

  conn_num = find_connection_index(fifth_level[1], third_level[1]);
  for(int i = 0; i < 2; i++){
     sixth_level[i + 2] = get_connection(fifth_level[1], (conn_num + 1 + i) % (MAX_CONNECTION_NUM + 1));
  }

  seventh_level[0] = get_mutual_connection(sixth_level[0], sixth_level[3]);
  seventh_level[1] = get_mutual_connection(sixth_level[1], sixth_level[2]);

#ifdef DEBUG_PRINT
  Serial.print("LEVEL 1: "); Serial.print(cycles[0]); Serial.print(" "); Serial.println(cycles[1]);
  Serial.print("LEVEL 2: "); Serial.print(second_level[0]); Serial.print(" "); Serial.print(second_level[1]); Serial.print(" "); Serial.print(second_level[2]); Serial.print(" "); Serial.println(second_level[3]);
  Serial.print("LEVEL 3: "); Serial.print(third_level[0]); Serial.print(" "); Serial.println(third_level[1]);
  Serial.print("LEVEL 4: "); Serial.print(fourth_level[0]); Serial.print(" "); Serial.print(fourth_level[1]); Serial.print(" "); Serial.print(fourth_level[2]); Serial.print(" "); Serial.println(fourth_level[3]);
  Serial.print("LEVEL 5: "); Serial.print(fifth_level[0]); Serial.print(" "); Serial.println(fifth_level[1]);
  Serial.print("LEVEL 6: "); Serial.print(sixth_level[0]); Serial.print(" "); Serial.print(sixth_level[1]); Serial.print(" "); Serial.print(sixth_level[2]); Serial.print(" "); Serial.println(sixth_level[3]);
  Serial.print("LEVEL 7: "); Serial.print(seventh_level[0]); Serial.print(" "); Serial.println(seventh_level[1]);
#endif

  cycles[2] = second_level[0];
  cycles[3] = second_level[3];
  cycles[4] = second_level[1];
  cycles[5] = second_level[2];
  cycles[6] = fourth_level[0];
  cycles[7] = fourth_level[3];
  cycles[8] = fourth_level[1];
  cycles[9] = fourth_level[2];
  cycles[10] = sixth_level[0];
  cycles[11] = sixth_level[1];
  cycles[12] = sixth_level[2];
  cycles[13] = sixth_level[3];
  cycles[14] = seventh_level[0];
  cycles[15] = seventh_level[1];

  ///////////////////////////////

  cycles[16] = third_level[0];
  cycles[17] = third_level[1];
  cycles[18] = fifth_level[0];
  cycles[19] = fifth_level[1];
}


void set_cycle_presets(){

  int offset;
  int maximum;
  int numPerRotation;
  
  switch(symmetry){
    case TwoFold:
      numPerRotation = 2;
      offset = 0;
      maximum = 20;
      break;
    case FiveFold:
      offset = 0;
      maximum = 20;
      numPerRotation = 5;
    break;
    case ThreeFold:
      offset = 1;
      maximum = 20;
      numPerRotation = 3;
    break;
    case Reflect:
      offset = 0;
      maximum = 16;
      numPerRotation = 2;
    break;
  }

  
  int index = offset;
#ifdef DEBUG_PRINT
  Serial.print("Setting ");
#endif

  for(int i = 0; i < offset; i++){
    // Before the offset (cones that are the axis cone / not in 
    // cycles) set all cones to white
    coneColor(cycles[i], 255, 255, 255, 255);
#ifdef DEBUG_PRINT
    Serial.print(cycles[i]);
    Serial.print(", ");
#endif
  }

  while(index + numPerRotation <= maximum){
    // set the remaining cycle groups to the preset colors
    
    setGroupPresetColor(cycles + index, numPerRotation);
    index += numPerRotation;
  }

  for(int i = index; i < MAX_CONE_NUM; i++){
    // set the remaining cones colors to white
    coneColor(cycles[i], 255, 255, 255, 255);
#ifdef DEBUG_PRINT
    Serial.print(cycles[i]);
    Serial.print(", ");
#endif
  }
#ifdef DEBUG_PRINT
  Serial.println("to white");
#endif
}


void set_missing_in_cycles(uint8_t missingIndex){
  // As we know the cycle array will contain the numbers 1-20
  // we know the sum of the array will be  1 + 2 + 3 + ... + 20 = 210
  // and that (Sum of the Array without one of those numbers) = 210 - that number
  // giving us missing number = 210 - sum of the array without the missing number
  
  uint8_t sum_of_array = 0;
  uint8_t sumFromOneToTwenty = 210;
  for(int i = 0; i < MAX_CONE_NUM; i++){
    if(i != missingIndex){
      sum_of_array += cycles[i];
    }
  }

  // once we have found the missing number, set it in the array
  cycles[missingIndex] = sumFromOneToTwenty - sum_of_array;
}

void getNextPair(uint8_t *arr, uint8_t one, uint8_t two){
    uint8_t first = one;
    uint8_t second = two;

    uint8_t connection_num = 255;
    bool found = false;
    for(int i = 0; i <= MAX_CONNECTION_NUM && !found; i++){
      if(get_connection(first, i) == second){
        connection_num = i;
        found = true;
      }
    }

    if(connection_num == 255) {
      return NO_CONNECTION;
    }
    
    connection_num++;
    
    if(second > MAX_CONNECTION_NUM){
      second = 0;
      first = (first % MAX_CONE_NUM) + 1;
    }

    found = false;
    
    while(!found){
      for(int i = connection_num; i <= MAX_CONNECTION_NUM && !found; i++){
        second = get_connection(first, i);
        if(second > first){
          found = true;
        }
      }

      if(!found){
        first = (first % MAX_CONE_NUM) + 1;
        connection_num = 0;
      }
    }

    arr[0] = first;
    arr[1] = second;

    // iterate the axis cone (so the center of rotation
    // becomes cone two if it was cone one before, etc
}

#define NUM_FIVE_FOLD_AXES 12
uint8_t fiveFoldAxes[NUM_FIVE_FOLD_AXES][2] = {{1, 5}, {6, 15}, {14, 13}, {12, 11}, {1, 6}, {8, 7}, {15, 16}, {13, 17}, {11, 18}, {9, 19}, {7, 20}, {18, 17}};

void incrementAxis(int8_t dir){
  static uint8_t fiveFoldAxis = 0;
  static uint8_t threeFoldAxis = 1;
  static uint8_t twoFoldAxis[2] = {1, 2};
  static uint8_t reflectionAxis[2] = {1, 2};

  switch(symmetry){
    case Reflect:
      if(dir) getNextPair(reflectionAxis, reflectionAxis[0], reflectionAxis[1]);
      cycles[0] = reflectionAxis[0];
      cycles[1] = reflectionAxis[1];
#ifdef DEBUG_PRINT
      Serial.print("NEW AXIS: "); Serial.print(reflectionAxis[0]); Serial.print(", "); Serial.println(reflectionAxis[1]);
#endif
      set_reflection_cycles();
      break;
    case TwoFold:
      if(dir) getNextPair(twoFoldAxis, twoFoldAxis[0], twoFoldAxis[1]);
      cycles[0] = twoFoldAxis[0];
      cycles[1] = twoFoldAxis[1];
#ifdef DEBUG_PRINT
      Serial.print("NEW AXIS: "); Serial.print(twoFoldAxis[0]); Serial.print(", "); Serial.println(twoFoldAxis[1]);
#endif
      set_twofold_cycles();
      break;
    case ThreeFold:
      if(dir) threeFoldAxis = (threeFoldAxis % MAX_CONE_NUM) + 1;
      cycles[0] = threeFoldAxis;
      set_threefold_cycles();
      break;
    case FiveFold:
      if(dir) fiveFoldAxis = (fiveFoldAxis + 1) % NUM_FIVE_FOLD_AXES;
      set_pentagon(cycles, fiveFoldAxes[fiveFoldAxis][0], fiveFoldAxes[fiveFoldAxis][1], POSITIVE);
      set_pentagon_cycles();
      break;
  }
}

void rotate(bool reverse){

  int offset;
  int maximum;
  int numPerRotation;
  
  switch(symmetry){
    case TwoFold:
      numPerRotation = 2;
      offset = 0;
      maximum = 20;
      break;
    case FiveFold:
      offset = 0;
      maximum = 20;
      numPerRotation = 5;
    break;
    case ThreeFold:
      offset = 1;
      maximum = 20;
      numPerRotation = 3;
    break;
    case Reflect:
      offset = 0;
      maximum = 16;
      numPerRotation = 2;
    break;
  }
  
  int index = offset;
  unsigned long currentTime = (millis() - start) % lengthOfShow;
  boolean shouldContinue = true;

  while(index + numPerRotation <= maximum && shouldContinue){
    // iterate through the cycles array, rotating each group

#ifndef GRADUAL_TRANSITION
    nFoldRotateColor(cycles + index, numPerRotation, reverse);
#else
    shouldContinue = nFoldRotateColorTransition(cycles + index, numPerRotation, reverse, currentTime, 1000);
#endif
    index += numPerRotation;
  }
}

void diagnostic_check_threefold(){
  num_per_rotation = 3;

  static int num = 1;
  static bool prevState = false;
  static bool prevStateTwo = false;

  // Store the previous state of the switches
  // as well as the current axis cone

  if(switches[0] != prevState){
    // if we flip the first switch
    
    prevState = switches[0];
    clear();

    num = (num % MAX_CONE_NUM) + 1;
    // iterate the axis cone (so the center of rotation
    // becomes cone two if it was cone one before, etc
    
    set_threefold(num);
    // set the axis cone in the cycles array
    set_threefold_cycles();
    // set the appropriate cycles in the cycles array

#ifdef DEBUG_PRINT
    Serial.print("Cycles: ");
    for(int i = 0; i < 20; i++){
      Serial.print(cycles[i], DEC);
      Serial.print(", ");
    }
    Serial.println("");
#endif

    set_cycle_presets();
    // color the cones appropriately


    pixels.show();

    
  }

  if(switches[1] != prevStateTwo){
    // if the second switch changes positions, perform
    // a 3 fold "rotation" of the lights about the axis
    rotate(false);
    pixels.show();
  }

}

void diagnostic_check_pentagons(){
  symmetry = FiveFold;
  num_per_rotation = 5;
  
    static int num = 1;
  static bool prevState = false;
  static bool prevStateTwo = false;
  static bool prevStateThree = false;

  // Store the previous state of the switches
  // as well as the current axis cone

  if(switches[0] != prevState){
    // if we flip the first switch
    
    prevState = switches[0];
    clear();

    num = (num % MAX_CONE_NUM) + 1;
    // iterate the axis cone (so the center of rotation
    // becomes cone two if it was cone one before, etc
#ifdef DEBUG_PRINT
    Serial.print("NUM CONE: ");
    Serial.println(num, DEC);
#endif

    uint8_t next = get_connection(num, 0);
    set_pentagon(cycles, num, next, POSITIVE);
    // set the axis pentagon in the cycles array

    set_pentagon_cycles();
    // set the appropriate cycles in the cycles array
#ifdef DEBUG_PRINT
    Serial.print("PENTAGON: ");
    for(int i = 0; i < 5; i++){
      Serial.print(cycles[i], DEC);
      Serial.print(", ");
    }
    Serial.println("");
    Serial.print("Cycles: ");
    for(int i = 0; i < 20; i++){
      Serial.print(cycles[i], DEC);
      Serial.print(", ");
    }
    Serial.println("");
#endif

    set_cycle_presets();
    // color the cones in the cycles appropriately
    
    pixels.show(); 
  }

  if(switches[1] != prevStateTwo){
    // if the second switch changes positions, perform
    // a 3 fold "rotation" of the lights about the axis
    prevStateTwo = switches[1];
    rotate(false);
    pixels.show();
  }

  if(switches[2] != prevStateThree){
    // if the second switch changes positions, perform
    // a 3 fold "rotation" of the lights about the axis
    prevStateThree = switches[2];
    rotate(true);
    pixels.show();
  }

}

void diagnostic_check_twofold(){
  num_per_rotation = 2;
  
  static uint8_t cone_num = 1;
  static uint8_t connection_num = 0;
  
  static bool prevState = false;
  static bool prevStateTwo = false;

  // Store the previous state of the switches
  // as well as the current axis cone

  if(switches[0] != prevState){
    // if we flip the first switch
    
    prevState = switches[0];
    clear();

    uint8_t secondCone = -1;
    bool found = false;

    connection_num++;
    if(connection_num > MAX_CONNECTION_NUM){
      connection_num = 0;
      cone_num = (cone_num % MAX_CONE_NUM) + 1;
    }
    
    while(!found){
      for(int i = connection_num; i <= MAX_CONNECTION_NUM && !found; i++){
        secondCone = get_connection(cone_num, i);
        if(secondCone > cone_num){
          found = true;
        }
      }

      if(!found){
        cone_num = (cone_num % MAX_CONE_NUM) + 1;
        connection_num = 0;
      }
    }

    // iterate the axis cone (so the center of rotation
    // becomes cone two if it was cone one before, etc

   
    set_twofold(cone_num, secondCone);
    // set the axis in the cycles array

    set_reflection_cycles();
    // set the appropriate cycles in the cycles array
    
    set_cycle_presets();
    // color the cones in the cycles appropriately
    
    pixels.show(); 
  }

  if(switches[1] != prevStateTwo){
    // if the second switch changes positions, perform
    // a 3 fold "rotation" of the lights about the axis

    prevStateTwo = switches[1];
    
    rotate(false);
    pixels.show();
  }

}

void diagnostic_check_find_third(){
  static uint8_t src = 1;
  static uint8_t next = 8;
  
  static bool prevState = false;

  // Store the previous state of the switches
  // as well as the current axis cone

  //uint8_t findThird(uint8_t src, uint8_t next, uint8_t dir){


  if(switches[0] != prevState){
    // if we flip the first switch
    clear();
    uint8_t arr[5];
    set_pentagon(arr, 1, 2, POSITIVE);
    
    for(int i = 0; i < 5; i++) coneColor(arr[i], 255,255,255,255);
    
//    uint8_t third = findThird(src, next, POSITIVE);
//    next = src;
//    src = third;
//
//    coneColor(src, 255, 0, 255, 255);
//    coneColor(next, 0, 255, 255, 255);
//
//    Serial.print("src: "); Serial.print(src); Serial.print(" next: "); Serial.println(next);

    pixels.show();

    prevState = switches[0];
  }
}



/*---------------------------------------------------------------------*/


#define TEST(actual, expected) Serial.print("expected: "); Serial.print(expected); Serial.print("actual: "); Serial.println(actual)
#define WILLIAM_DEBUG




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

#define CUBIC_INTERP
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


uint8_t srcCone = 1;
uint8_t nextCone = 8;

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

}

//sets the cones of the array to preset colors, works for 2, 3, and 5 fold
void setGroupPresetColor(uint8_t *coneArray, uint8_t degreeOfSymmetry) {
  if (degreeOfSymmetry > 5) {
    Serial.println("degreeOfSymmetry must be less than or equal to 5 in setGroupPresetColor function.");
  }
  else {
    for (int i = 0; i < degreeOfSymmetry; i++) {
      uint32_t changeCone = colorPresets[i];
      coneColor(*coneArray, (changeCone >> 16) & 0xFF, (changeCone >> 8) & 0xFF, changeCone & 0xFF, (changeCone >> 24) & 0xFF); //moves color cone to cone
      coneArray++;
    }
  }
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  get_pots();
  get_buttons();
  get_switches();
  get_encoder();

//  print_state();
}
void get_encoder()
{
  button_down = Wire.read(); //reads signal
  button_down_time = get_int32(); //time the button has been pressed down (starts over when button is released)
  rotary_counter = get_int32(); //amount the dial has been turned
}

void get_buttons()
{
  for (int ii = 0; ii < 3; ++ii) {
    buttons[ii] = Wire.read();
  }
}

void get_switches()
{
  for (int ii = 0; ii < 3; ++ii) {
    switches[ii] = Wire.read();
  }
}

void get_pots()
{
  pot1 = get_uint16();
  pot2 = get_uint16();
  pot3 = get_uint16();
}


void print_state()
{
//#ifdef DEBUG_PRINT
  Serial.print("encoder:  ");
  Serial.print(button_down, DEC); Serial.print(" ");
  Serial.print(button_down_time, DEC); Serial.print(" ");
  Serial.print(rotary_counter, DEC);
  Serial.println("");

  Serial.print("buttons:  ");
  for (int ii = 0; ii < 3; ++ii) {
    Serial.print(buttons[ii]);
  }
  Serial.println("");


  Serial.print("switches:  ");
  for (int ii = 0; ii < 3; ++ii) {
    Serial.print(switches[ii]);
  }
  Serial.println("");


  Serial.print("potentiometers:  ");
  Serial.print(pot1);    Serial.print(" ");
  Serial.print(pot2);    Serial.print(" ");
  Serial.print(pot3);    Serial.print(" ");
  Serial.println("");

  Serial.println("");
//#endif
}
// this and the other `get` function (get_int32) could easily be replaced by one template function, and a sizeof call
uint16_t get_uint16()
{

  static byte recv[2];

  recv[0] = Wire.read();
  recv[1] = Wire.read();

  uint16_t r = recv[0];
  r = (r << 8) | recv[1];

  return r;
}

int32_t get_int32()
{
  static byte recv[2];


  recv[0] = Wire.read();
  recv[1] = Wire.read();
  recv[2] = Wire.read();
  recv[3] = Wire.read();

  int32_t r = recv[0];
  r = (r << 8) | recv[1];
  r = (r << 8) | recv[2];
  r = (r << 8) | recv[3];

  return r;
}

//Only sets ONE pixel to a certain color at a time
void pixelColor(int pixNum , uint8_t g, uint8_t r, uint8_t b, uint8_t w) { // pixNum = Pixel Number, g = green, r=red, b = blue, w = white
  if (pixNum >= 0 && pixNum <= 255) { //forces the user to enter a valid pixel number
    uint32_t color = pixels.Color(g, r, b, w); // make a color
    pixels.setPixelColor(pixNum, color); //set a single pixel color
    pixels.show(); //update the colors
  }
}   //end (Sam 6/15/2020)




// accepts a pixel number, correlates the pixel's number to the cone its in and lights all of the pixels within that cone to the specfied color
void pixToConeColor (int pixNum , uint8_t g, uint8_t r, uint8_t b, uint8_t w) { // pixNum = Pixel Number, g = green, r= red, b = blue, w = white
  if (pixNum >= 0 && pixNum <= 255) { //forces the user to enter a valid pixel number
    uint32_t color = pixels.Color(g, r, b, w); // make a color
    int pixStart = (pixNum / 7) * 7; //determines which pixel out of 140 to fill
    pixels.fill(color, pixStart, 7);
    pixels.show(); //update the colors
  }
}   //end (Sam 6/20/2020)



//accepts an integer (representing the number on each cone) and lights up just that cone to the specified color
void coneColor (int coneNum, uint8_t g, uint8_t r, uint8_t b, uint8_t w) { // pixnum = Pixel Number, g = green, r= red, b = blue, w = white
  if (coneNum >= 1 && coneNum <= 20) {
    uint32_t color = pixels.Color(g, r, b, w); // make a color
    int pixStart = (coneNum - 1) * 7; //determines which pixel out of 140 to fill
    pixels.fill(color, pixStart, 7); //fills seven pixels with the desired color and specific location
    }
}   //end (Sam 7/1/2020)



//returns the specific color that is within the cone by looking at one pixel
uint32_t getConeColor(uint8_t coneNum) {
  if (coneNum >= 1 && coneNum <= 20) {
    int pixStart = (coneNum - 1) * 7; //determines which pixel out of 140 to fill, inside coneNum
    return pixels.getPixelColor(pixStart);
  }
  return 0; //if for some reason the coneNum isn't in the range of 1 to 20 then zero is returned
} //end Sam 7-14-20



//when the rotary encounter is turned (in either direction) a new cone will be selected and the color is controlled by the potentiometers
void coneSelect () {
  int coneNum = abs(((rotary_counter / 2) % 20) + 1); //each time the encoder is turned 2 values it switches to a new cone, (clockwise to get larger #cone, counterclockwise to get smaller #cone)
  uint8_t g = pot2 / 4; //integer division by four to translate range of 0-1023 (potentiometers) to 0-255 (pixel brightness)
  uint8_t r = pot1 / 4;
  uint8_t b = pot3 / 4;
  
  coneColor(coneNum, g, r, b, 0); //gets the coneNum from the variable above
#ifdef DEBUG_PRINT
  Serial.println("Setting Cone "); Serial.print(coneNum); Serial.print(" to "); Serial.print(r); Serial.print(" "); Serial.print(g); Serial.print(" "); Serial.println(b);
#endif
} //end (Sam 7/7/2020)



//when the rotary encoder is pressed all the pixels are shut off, the selected cone returns to cone #1, and the encoder value returns to zero(last part done in other code)
#define NUM_LED_GROUPS 20 //20 cones total on fixture
void reset() {
  if (button_down_time > 3000) { //hold button down for 3 seconds to activate
    clear();
  }
} // end (Sam 7/8/2020)



//completely turns off all of the pixels
void clear() {
  for (int i = 1; i <= NUM_LED_GROUPS; i++) {
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
  for (int i = 1; i <= NUM_LED_GROUPS; i++) {
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
  for (int i = 1; i <= NUM_LED_GROUPS; i++) {
    coneColor(i, restrictRandom8Bit(), restrictRandom8Bit(), restrictRandom8Bit(), restrictRandom8Bit() );
  }
  pixels.show();
} //end 7/18/20


//Takes the color from cone1 and transfers it to cone2, cone2 goes to cone3, cone3 goes to cone1
void ThreeFoldRotateColor(uint8_t cone1, uint8_t cone2, uint8_t cone3) { //must make sure the cone# is in the correct order
  if (cone1 >= 1 && cone1 <= 20 && cone2 >= 1 && cone2 <= 20 && cone3 >= 1 && cone3 <= 20) { //makes sure all the inputed cone numbers are 1-20
    uint32_t changeCone1 = getConeColor(cone1);
    uint32_t changeCone2 = getConeColor(cone2);
    coneColor(cone2, (changeCone1 >> 16) & 0xFF, (changeCone1 >> 8) & 0xFF, changeCone1 & 0xFF, (changeCone1 >> 24) & 0xFF); //moves color from cone1 to cone2
    changeCone1 = getConeColor(cone3); //instead of making new variable, reuse changeCone1 but just set to get info from cone3
    coneColor(cone3, (changeCone2 >> 16) & 0xFF, (changeCone2 >> 8) & 0xFF, changeCone2 & 0xFF, (changeCone2 >> 24) & 0xFF); //moves color from cone2 to cone3
    coneColor(cone1, (changeCone1 >> 16) & 0xFF, (changeCone1 >> 8) & 0xFF, changeCone1 & 0xFF, (changeCone1 >> 24) & 0xFF); //moves color from cone3 to cone1
  }
} //end Sam 7-14-20



//Calls the various 3 fold rotational symmetry groupings (hard coded)
void ThreeFoldSymmetry() {
  ThreeFoldRotateColor(2, 5, 14);
  ThreeFoldRotateColor(15, 12, 4);
  ThreeFoldRotateColor(6, 13, 3);
  ThreeFoldRotateColor(7, 20, 10);
  ThreeFoldRotateColor(8, 16, 11);
  ThreeFoldRotateColor(9, 17, 19);
  pixels.show();
} //end Sam 7-15-20

//changes the color of a certain number of cones, one-by-one, this only rotates one "group" at a time
void nFoldRotateColor(uint8_t *coneArray, uint8_t numCone, bool reverse) { //numCone refers to number of cones in the array
  if (numCone > 1) {
    
    uint32_t color = getConeColor(coneArray[0]);
    uint32_t overwrittenColor = getConeColor(coneArray[0]);

    uint8_t prevIndex = 0;

    for (int i = 0; i < numCone; i++) {
      if (coneArray[i] >= MIN_CONE_NUM && coneArray[i] <= MAX_CONE_NUM) { //makes sure all the inputed cone numbers are between 1-20

        color = overwrittenColor;
        uint8_t index = -1;
        if(!reverse){
          index = (prevIndex + 1) % numCone;
        } else {
          index = (prevIndex - 1 + numCone) % numCone;
        }

        overwrittenColor = getConeColor(coneArray[index]);
        coneColor(coneArray[index], (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, (color >> 24) & 0xFF); //moves color cone to cone
        Serial.print(coneArray[index]); Serial.print(": "); Serial.println(color);

        prevIndex = index;
      }
    }
  }
  else {
#ifdef DEBUG_PRINT
    Serial.println("numCone is not greater than one. Choose new number in nFoldRotateColor.");
#endif
  }
} //end 7/18/20

bool nFoldRotateColorTransition(uint8_t *coneArray, uint8_t numCone, bool reverse, uint16_t currentTime, uint16_t leng) { //numCone refers to number of cones in the array
  if (numCone > 1) {

    for(int i = 0; i < numCone; i++){
      if(times[coneArray[i] - 1][1] != NO_EVENT_PLANNED){     
        return false;
      }
    }
    
    uint32_t color = getConeColor(coneArray[0]);
    uint32_t overwrittenColor = getConeColor(coneArray[0]);

    uint8_t prevIndex = 0;

    for (int i = 0; i < numCone; i++) {
      if (coneArray[i] >= MIN_CONE_NUM && coneArray[i] <= MAX_CONE_NUM) { //makes sure all the inputed cone numbers are between 1-20

        color = overwrittenColor;
        uint8_t index = -1;
        if(!reverse){
          index = (prevIndex + 1) % numCone;
        } else {
          index = (prevIndex - 1 + numCone) % numCone;
        }

        overwrittenColor = getConeColor(coneArray[index]);

        times[coneArray[index] - 1][0] = currentTime;
        colors[coneArray[index] - 1][0] = getConeColor(coneArray[index]);

        times[coneArray[index] - 1][1] = (currentTime + leng) % lengthOfShow;
        colors[coneArray[index] - 1][1] = color;
        
        //coneColor(coneArray[index], (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, (color >> 24) & 0xFF); //moves color cone to cone

        

        prevIndex = index;
      }
    }

    return true;
  }
  else {
#ifdef DEBUG_PRINT
    Serial.println("numCone is not greater than one. Choose new number in nFoldRotateColor.");
#endif
  }
} 

//Uses the encoder to rotate the three fold symmetry 120 degrees about the axis point
# define NUM_TICKS 3
void rotationalThreeFoldSymEncoder() {
  static int32_t lastRotation = 0; //the last rotation value of the encoder
  if (rotary_counter - lastRotation > NUM_TICKS) {
    ThreeFoldSymmetry();
    lastRotation = rotary_counter;
  }
  else if (lastRotation - rotary_counter > NUM_TICKS) { //moved more than 3 in the opposite direction
    ThreeFoldSymmetry();
    ThreeFoldSymmetry();
    lastRotation = rotary_counter;
  } //if rotary encoder is turned any amount between 5 and -5 nothing will happen
}

//Calls the various 5 fold rotational symmetry groupings using arrays (hard coded)
//void fiveFoldSymmetry(bool reverse) {
//  for (int i = 0; i < 4; i++) { //use 4 because there are 4 groups of pentagons
//    nFoldRotateColor(fiveFoldGroups[i], 5, reverse);
//  }
//}

//this is 100% a guess. Trying to get the rotational encoder to rotate both colockwise and counterclockwise around the pentagons (5fold)
//void rotationalFiveFoldSymEncoder() {
//  static int32_t lastRotation = 0; //the last rotation value of the encoder
//  if (rotary_counter - lastRotation > NUM_TICKS) { //NUM_TICKS defined above
//    fiveFoldSymmetry(false);
//    lastRotation = rotary_counter;
//  }
//  else if (lastRotation - rotary_counter > NUM_TICKS) { //moved more than 3 in the opposite direction
//    fiveFoldSymmetry(false); //need 4 to get to the left (rotate the the same direction just ending on a different cone technically
//    fiveFoldSymmetry(false);
//    fiveFoldSymmetry(false);
//    fiveFoldSymmetry(false);
//    lastRotation = rotary_counter;
//  } //if rotary encoder is turned any amount between 5 and -5 nothing will happen
//  pixels.show();
//}



void red(unsigned long period) {
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

void blue(unsigned long period) {
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

void magenta(unsigned long period) {
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

void green(unsigned long period) {
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

void white(unsigned long period) {
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
