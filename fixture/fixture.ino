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

#include <Adafruit_NeoPixel.h> // see https://github.com/adafruit/Adafruit_NeoPixel
#include <Wire.h> // see https://www.arduino.cc/en/Tutorial/MasterReader

void setGroupPresetColor(uint8_t *coneArray, uint8_t numberOfCones);
void nFoldRotateColor(uint8_t *coneArray, uint8_t numCone);
void clear();
void coneColor (int coneNum, uint8_t g, uint8_t r, uint8_t b, uint8_t w);

#define NEOPIXEL_PIN 12

// some global variables
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(140, NEOPIXEL_PIN,  NEO_RGBW + NEO_KHZ800);
unsigned long previousMillis = 0, currentMillis = 0;

uint16_t pot1, pot2, pot3;

bool buttons[3], switches[3];

unsigned long button_down_time = 0L; // [ms] the time the encoder button has been pushed so far.  is 0 if the button's not being pressed
bool button_down = 0; // is the button in the encoder being pushed?
int32_t rotary_counter = 0; // current "position" of rotary encoder (increments clockwise, decrements anticlockwise)


#define FIXTURE_ADDRESS 4

uint32_t colorPresets [5] = {0x00FF00, 0xFF0000, 0x0000FF, 0xFFFFFF, 0xFFFF00}; //use an array of 5 since at most we are looking at 5-fold rotational symmetry (g,r,b,w,y)
//white hexa = FFFFFF, red hexa = FF0000, green hexa = 00FF00, blue hexa = 0000FF, yellow hexa = FFFF00, magenta hexa = FF00FF


uint8_t threeFoldGroups [6][3] = {{5, 14, 2}, {15, 12, 4}, {6, 13, 3}, {7, 20, 10}, {8, 16, 11}, {17, 19, 9}}; //[6] indicates amount of arrays, [3] arrays in the array

uint8_t fiveFoldGroups [4][5] = {{1,2,3,4,5}, {14,12,10,8,6}, {9,7,15,13,11}, {18,17,16,20,19}}; //the numbering of these groups are in clockwise-order starting at "top" cone

//{1,2,3,4,5} = r,g,b,w,y
//{14,12,10,8,6} = r,g,b,w,y
//{9,7,15,13,11} = r,g,b,w,y
//{18,17,16,20,19} = r,g,b,w,y

/*---------------------------------------------------------------------*/
// MACROS TO EASE WITH THE PROCESS OF WRITING THIS OUT AS UINT16_T
#define MAX_UINT5 31
#define CONNECTION_NUM_SETUP(num_connection, num_cone) ((num_cone << 5*(num_connection)) & (MAX_UINT5 << 5*(num_connection)))
#define MIN_CONE_NUM 1
#define MAX_CONE_NUM 20
#define MAX_CONNECTION_NUM 2

uint16_t connections[20] = {
  CONNECTION_NUM_SETUP(0, 4) | CONNECTION_NUM_SETUP(1, 1) | CONNECTION_NUM_SETUP(2, 7),
  CONNECTION_NUM_SETUP(0, 9) | CONNECTION_NUM_SETUP(1, 0) | CONNECTION_NUM_SETUP(2, 2),
  CONNECTION_NUM_SETUP(0, 11) | CONNECTION_NUM_SETUP(1, 1) | CONNECTION_NUM_SETUP(2, 3),
  CONNECTION_NUM_SETUP(0, 4) | CONNECTION_NUM_SETUP(1, 13) | CONNECTION_NUM_SETUP(2, 2),
  CONNECTION_NUM_SETUP(0, 3) | CONNECTION_NUM_SETUP(1, 0) | CONNECTION_NUM_SETUP(2, 5),
  CONNECTION_NUM_SETUP(0, 6) | CONNECTION_NUM_SETUP(1, 14) | CONNECTION_NUM_SETUP(2, 4),
  CONNECTION_NUM_SETUP(0, 5) | CONNECTION_NUM_SETUP(1, 7) | CONNECTION_NUM_SETUP(2, 19),
  CONNECTION_NUM_SETUP(0, 8) | CONNECTION_NUM_SETUP(1, 6) | CONNECTION_NUM_SETUP(2, 0),
  CONNECTION_NUM_SETUP(0, 18) | CONNECTION_NUM_SETUP(1, 7) | CONNECTION_NUM_SETUP(2, 9),
  CONNECTION_NUM_SETUP(0, 8) | CONNECTION_NUM_SETUP(1, 1) | CONNECTION_NUM_SETUP(2, 10),
  CONNECTION_NUM_SETUP(0, 17) | CONNECTION_NUM_SETUP(1, 9) | CONNECTION_NUM_SETUP(2, 11),
  CONNECTION_NUM_SETUP(0, 12) | CONNECTION_NUM_SETUP(1, 10) | CONNECTION_NUM_SETUP(2, 2),
  CONNECTION_NUM_SETUP(0, 16) | CONNECTION_NUM_SETUP(1, 11) | CONNECTION_NUM_SETUP(2, 13),
  CONNECTION_NUM_SETUP(0, 14) | CONNECTION_NUM_SETUP(1, 12) | CONNECTION_NUM_SETUP(2, 3),
  CONNECTION_NUM_SETUP(0, 15) | CONNECTION_NUM_SETUP(1, 13) | CONNECTION_NUM_SETUP(2, 5),
  CONNECTION_NUM_SETUP(0, 16) | CONNECTION_NUM_SETUP(1, 14) | CONNECTION_NUM_SETUP(2, 19),
  CONNECTION_NUM_SETUP(0, 12) | CONNECTION_NUM_SETUP(1, 15) | CONNECTION_NUM_SETUP(2, 17),
  CONNECTION_NUM_SETUP(0, 10) | CONNECTION_NUM_SETUP(1, 16) | CONNECTION_NUM_SETUP(2, 18),
  CONNECTION_NUM_SETUP(0, 17) | CONNECTION_NUM_SETUP(1, 19) | CONNECTION_NUM_SETUP(2, 8),
  CONNECTION_NUM_SETUP(0, 18) | CONNECTION_NUM_SETUP(1, 15) | CONNECTION_NUM_SETUP(2, 6),
};
/*
 * Each uint16_t stores 3 numbers.
 * The index represents a specific cone on the barth sextic,
 * whereas the values stored inside the uint16_t hold the cone numbers
 * that are connected
 * 
 * ________________________________________________________________________________
 * | First Connected Cone | Second Connected Cone | Third Connected Cone  |        |
 * |-------------------------------------------------------------------------------|
 * | 5 Bytes              | 5 Bytes               | 5 Bytes               | 1 Byte |
 * |-------------------------------------------------------------------------------|
 * 
 * The connections are ordered such that if you were to hold the geometric object
 * from the cone specified by the index, the ordering of the connections would follow
 * a circle around the cone, going from left to right.
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
    
    Serial.print("NUM CONE: ");
    Serial.println(num, DEC);

    // Light up the current cone (cone number num) green
    coneColor(num, 255, 0, 0, 0);
    for(int i = 0; i <= MAX_CONNECTION_NUM; i++){
      // Light up the cones connected to the current cone red
      uint8_t conn = get_connection(num, i);
      Serial.print("CONNECTED CONE: ");
      Serial.println(conn, DEC);
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
      
      if(dir){
        // if direction is 1 / positive

        // As the list of connections is ordered to be the cycle around the cone
        // specified by the index, we can simply take the next element in the list
        third = get_connection(src, (i + 1) % MAX_CONNECTION_NUM);
      }
      else {
        third = get_connection(src, (i - 1 + MAX_CONNECTION_NUM) % MAX_CONNECTION_NUM);
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

// Function to set an array of length five to the five elements of 
// the pentagon defined by a source cone, a cone connected to that 
// source cone, and the direction to iterate from there.
void set_pentagon(uint8_t *arr, uint8_t src, uint8_t next, uint8_t dir){

  // Set initial three elements of the pentagon by using the set
  // parameters, and the function used to find the third cone
  // from the direction
  arr[0] = next;
  arr[1] = src;
  arr[2] = findThird(src, next, dir);

  //    Source Cone
  //      /    \
  //Next /      \  Third 
  //Cone\       /  Cone
  //     \_____/

  // TODO: Investigate iterative path rather than
  //       iterating through every single path of length
  //       two from the cone in array location 0

  uint8_t potentialConnectionsNext[2];
  uint8_t potentialConnectionsThird[2];
  int j = 0;

  // Find the cones connected to the "next" cone
  // that are not the source cone
  for(int i = 0; i <= MAX_CONNECTION_NUM; i++){
    uint8_t conn = get_connection(cycles[0], i);
    if(conn != src){
      potentialConnectionsNext[j++] = conn;
      if(j > 2) Serial.print("ERROR");
    }
  }

  // Find the cones connected to the "third" cone
  // that are not the source cone
  j = 0;
  for(int i = 0; i <= MAX_CONNECTION_NUM; i++){
    uint8_t conn = get_connection(cycles[2], i);
    if(conn != src){
      potentialConnectionsThird[j++] = conn;
      if(j > 2) Serial.print("ERROR");
    }
  }

  bool done = false;
  for(int i = 0; i < 2 && !done; i++){
    for(int j = 0; j <= MAX_CONNECTION_NUM && !done; j++){
      // iterate over all of the connections of the cones that are
      // connected to the "next" cone
      uint8_t conn = get_connection(potentialConnectionsNext[i], j);
      for(int k = 0; k < 2 && !done; k++){

        // For each cone that is connected to a connection of the "next"
        // cone, check if it is connected to the "third" cone. If it is,
        // then you have found a path of length two from the "next" cone
        // to the "third" cone, completing the pentagon
        
        Serial.print("PAIR: ");
        Serial.print(potentialConnectionsNext[i], DEC);
        Serial.println(conn, DEC);

        if(conn == potentialConnectionsThird[k]){
          arr[4] = potentialConnectionsNext[i];
          arr[3] = conn;
          done = true;
        }
      }
    }
  }
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
uint8_t find_connection_excluding_prev_cycles(int cone, uint8_t min_avoid, uint8_t max_avoid)
{
    for(int i = 0; i <= MAX_CONNECTION_NUM; i++){
      uint8_t conn = get_connection(cone, i);

      // find a connection of the cone in question
      bool foundConnection = false;

      // check if the cone connection is in any of the cycle array in the
      // indices specified by the parameters
      for(int j = min_avoid; j <= max_avoid && !foundConnection; j++){
        if(conn == cycles[j]) foundConnection = true;
      }

      // if the cone is connected to the cone input in the parameter
      // and is not stored in the cycles array within the specified indices
      // return that cone number
      if(!foundConnection) return conn;
    }

    // if no such cone exists, return an error code
    return NO_SUCH_CONE_ERR;
}

// Given two cones one and two, find the two cones that create
// a path of length two from cone one to cone two
//
// TODO: Replace code in set_pentagon with this function, as
//       they are mostly equivalent
uint8_t *find_middle_two(uint8_t coneOne, uint8_t coneTwo){
  static uint8_t arr[2];
  uint8_t conns[2][3];

  // Find the cones connected to the coneOne
  for(int i = 0; i < 3; i++){
    conns[0][i] = get_connection(coneOne, i);
  }
  
   // Find the cones connected to the coneTwo 
  for(int i = 0; i < 3; i++){
    conns[1][i] = get_connection(coneTwo, i);
  }

  bool done = false;
  for(int i = 0; i < 3 && !done; i++){
    for(int j = 0; j < 3 && !done; j++){
      // iterate over all of the connections of the cones that are
      // connected to coneOne
      uint8_t conn = get_connection(conns[0][i], j);
      for(int k = 0; k < 3 && !done; k++){

        // For each cone that is connected to a connection of coneOne
        // check if it is connected to the coneTwo cone. If it is,
        // then you have found a path of length two from coneOne
        // to coneTwo
        
        if(conn == conns[1][k]){
          arr[0] = conns[0][i];
          arr[1] = conn;
          done = true;
        }
      }
    }
    
  }

  Serial.print("MIDDLE TWO OF "); Serial.print(coneOne, DEC); Serial.print(" and "); Serial.println(coneTwo, DEC);
  Serial.print(arr[0], DEC); Serial.print(" and "); Serial.println(arr[1], DEC);
  return arr;
  
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

    cycles[i + 4] = arr[0];
    cycles[i + 7] = arr[1];
  }

  for(int i = 0; i < 3; i++){
    // the next two sets of cycles are found by finding the sets of two
    // cones in between the cones set in the above for loop
    arr = find_middle_two(cycles[(i + 1) % 3 + 4], cycles[i + 7]);
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
    
    cycles[5 + i] = find_connection_excluding_prev_cycles(cycles[i], 0, 4);
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
    cycles[15 + i] = find_connection_excluding_prev_cycles(cycles[10 + i], 5, 9);
  }
}

void set_cycle_presets(int offset){
  int index = offset;
  Serial.print("Setting ");
  for(int i = 0; i < offset; i++){
    // Before the offset (cones that are the axis cone / not in 
    // cycles) set all cones to white
    coneColor(cycles[i], 255, 255, 255, 255);
    Serial.print(cycles[i]);
    Serial.print(", ");
  }

  while(index + num_per_rotation <= MAX_CONE_NUM){
    // set the remaining cycle groups to the preset colors
    
    setGroupPresetColor(cycles + index, num_per_rotation);
    index += num_per_rotation;
  }

  for(int i = index; i < MAX_CONE_NUM; i++){
    // set the remaining cones colors to white
    coneColor(cycles[i], 255, 255, 255, 255);
    Serial.print(cycles[i]);
    Serial.print(", ");
  }
  Serial.println("to white");
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

void rotate(int offset){
  int index = offset;

  while(index + num_per_rotation <= MAX_CONE_NUM){
    // iterate through the cycles array, rotating each group
    nFoldRotateColor(cycles + index, num_per_rotation);
    index += num_per_rotation;
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

    Serial.print("Cycles: ");
    for(int i = 0; i < 20; i++){
      Serial.print(cycles[i], DEC);
      Serial.print(", ");
    }
    Serial.println("");

    set_cycle_presets(1);
    // color the cones appropriately


    pixels.show();

    
  }

  if(switches[1] != prevStateTwo){
    // if the second switch changes positions, perform
    // a 3 fold "rotation" of the lights about the axis
    rotate(1);
    pixels.show();
  }

}

void diagnostic_check_pentagons(){
  num_per_rotation = 5;
  
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
    Serial.print("NUM CONE: ");
    Serial.println(num, DEC);

    uint8_t next = get_connection(num, 0);
    set_pentagon(cycles, num, next, POSITIVE);
    // set the axis pentagon in the cycles array

    set_pentagon_cycles();
    // set the appropriate cycles in the cycles array
    
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

    set_cycle_presets(0);
    // color the cones in the cycles appropriately
    
    pixels.show(); 
  }

  if(switches[1] != prevStateTwo){
    // if the second switch changes positions, perform
    // a 3 fold "rotation" of the lights about the axis
    rotate(0);
    pixels.show();
  }

}





/*---------------------------------------------------------------------*/





// called once at the beginning
void setup() {
  pixels.begin();
  pixels.setBrightness(255); // 0 is off.  255 is full.  this is a limit on the brightness for the entire run.


  Wire.begin(FIXTURE_ADDRESS);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output

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

}

// called ad nauseum
void loop() {
  //    red(10);
  //    green(10);
  //    blue(10);
  //    magenta(10);
  //    white(10);
  //    allLEDs(10);

  //pixelColor(1, 255, 0, 0, 0); //(Pixel Number, green, red, blue, white)
  //pixToConeColor (8, 0, 255, 0, 0);
  //coneColor(3, 0, 0, 255, 0);
  //coneSelect();
  //allConeRandom();

  //coneColor(2, 0, 0, 0, 255);
  //  coneColor(3, 0, 255, 0, 0);
  //  coneColor(4, 0, 0, 255, 0);
  //  coneColor(7, 0, 0, 0, 255);
  //  coneColor(8, 255, 0, 0, 0);
  //  coneColor(9, 0, 255, 255, 0);
  //  ThreeFoldSymmetry();
  //  delay(500);
  //  ThreeFoldSymmetry();
  //  delay(500);
  //  ThreeFoldSymmetry();
  //  delay(500);

  //  uint8_t coneArray[5] = {2, 3, 4, 5, 6};
  //  for (int i = 0; i < 4; i++) {
  //    nFoldRotateColor(coneArray, 5);
  //    delay(500);
  //  }




  
  delay(100);
  diagnostic_check_threefold();
  //diagnostic_check_pentagons();
  //rotationalThreeFoldSymEncoder();
  //rotationalFiveFoldSymEncoder();

}

//sets the cones of the array to preset colors, works for 2, 3, and 5 fold
void setGroupPresetColor(uint8_t *coneArray, uint8_t numberOfCones) {
  if (numberOfCones > 5) {
    Serial.println("Number of cones must be less than or equal to 5 in setGroupPresetColor function.");
  }
  else {
    for (int i = 0; i < numberOfCones; i++) {
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

  //print_state();
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
void nFoldRotateColor(uint8_t *coneArray, uint8_t numCone) { //numCone refers to number of cones in the array
  if (numCone > 1) {
    uint32_t changeCone1;
    uint32_t changeCone2;
    uint8_t *conePointer = coneArray;
    uint8_t *startArray = coneArray;
    changeCone1 = getConeColor(*conePointer); //dereference the cone pointer, this now looks at the cone number
    for (int i = 0; i < numCone - 1; i++) {
      if (*conePointer >= 1 && *conePointer <= 20) { //makes sure all the inputed cone numbers are between 1-20
        conePointer++; //incrimenting the pointer
        changeCone2 = getConeColor(*conePointer);
        coneColor(*conePointer, (changeCone1 >> 16) & 0xFF, (changeCone1 >> 8) & 0xFF, changeCone1 & 0xFF, (changeCone1 >> 24) & 0xFF); //moves color cone to cone
        changeCone1 = changeCone2;
      }
    }
    coneColor(*startArray, (changeCone1 >> 16) & 0xFF, (changeCone1 >> 8) & 0xFF, changeCone1 & 0xFF, (changeCone1 >> 24) & 0xFF); //moves color from last cone in array to first cone (start of array)
  }
  else {
    Serial.println("numCone is not greater than one. Choose new number in nFoldRotateColor.");
  }
} //end 7/18/20

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
void fiveFoldSymmetry() {
  for (int i = 0; i < 4; i++) { //use 4 because there are 4 groups of pentagons
    nFoldRotateColor(fiveFoldGroups[i], 5);
  }
}

//this is 100% a guess. Trying to get the rotational encoder to rotate both colockwise and counterclockwise around the pentagons (5fold)
void rotationalFiveFoldSymEncoder() {
  static int32_t lastRotation = 0; //the last rotation value of the encoder
  if (rotary_counter - lastRotation > NUM_TICKS) { //NUM_TICKS defined above
    fiveFoldSymmetry();
    lastRotation = rotary_counter;
  }
  else if (lastRotation - rotary_counter > NUM_TICKS) { //moved more than 3 in the opposite direction
    fiveFoldSymmetry(); //need 4 to get to the left (rotate the the same direction just ending on a different cone technically
    fiveFoldSymmetry();
    fiveFoldSymmetry();
    fiveFoldSymmetry();
    lastRotation = rotary_counter;
  } //if rotary encoder is turned any amount between 5 and -5 nothing will happen
  pixels.show();
}



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
