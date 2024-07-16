

#include "rotation.h"



void printCycles(){
  Serial.println(F("cycles:"));
  for (int i=0; i<NUM_CONES; ++i)
    Serial.println(cycles[i]);
}


uint8_t get_connection(uint8_t cone, uint8_t connection_num)
{
  // Serial.print("getting connection "); Serial.print(connection_num); Serial.print(" for cone "); Serial.println(cone);
  if(cone >= NUM_CONES) return CONE_DNE;
  if(connection_num > MAX_CONNECTION_NUM) return CONNECTION_DNE;
  
  return ((connections[cone] >> (5*connection_num)) & MAX_UINT5);
}






void set_twofold_cycles(Cone cone1, Cone cone2){
  // This function sets the cycle array for two fold rotations, assuming the seed 
  // pair has already been set
  
  const uint8_t cycle_lengths[7] = {2,4,2,4,2,4,2};
  
  // the first level is already in `cycles` from `set_twofold_cycles`, by hypothesis
  uint8_t* first_level = cycles; // contains 2
  uint8_t* second_level = cycles+partial_sum(cycle_lengths,1); // contains 4
  uint8_t* third_level = cycles+partial_sum(cycle_lengths,2); // contains 2
  uint8_t* fourth_level = cycles+partial_sum(cycle_lengths,3); // contains 4
  uint8_t* fifth_level = cycles+partial_sum(cycle_lengths,4); // contains 2
  uint8_t* sixth_level = cycles+partial_sum(cycle_lengths,5); // contains 4
  uint8_t* seventh_level = cycles+partial_sum(cycle_lengths,6); // contains 2

  uint8_t c;
  
  first_level[0] = cone1;
  first_level[1] = cone2;
  
  c = 0; // reset the counter
  
  uint8_t skip_this_conn = find_connection_index(cycles[0], cycles[1]); // 

  second_level[c++] = find_next_cone_in_direction(cycles[0], cycles[1], POSITIVE);
  second_level[c++] = find_next_cone_in_direction(cycles[0], cycles[1], NEGATIVE);

  second_level[c++] = find_next_cone_in_direction(cycles[1], cycles[0], POSITIVE);
  second_level[c++] = find_next_cone_in_direction(cycles[1], cycles[0], NEGATIVE);

  
  

  c = 0; // reset the counter
  for (int i=0; i<2; ++i){
    for (int j=2; j<4; ++j){
      uint8_t common_cone = get_mutual_connection(second_level[i], second_level[j]);
      if (common_cone!=NO_SUCH_CONE_ERR && common_cone!=first_level[0] && common_cone!=first_level[1]){
        // Serial.print("found common for level 3: "); Serial.println(common_cone);
        third_level[c++] = common_cone;
      }
    } // j
  } // i
  
  
  
  c = 0;
  uint8_t *arr = find_middle_two(second_level[0], second_level[1]);
  fourth_level[c++] = arr[0];
  fourth_level[c++] = arr[1];
  
  arr = find_middle_two(second_level[2], second_level[3]);
  fourth_level[c++] = arr[0];
  fourth_level[c++] = arr[1];


  fifth_level[0] = find_connection_excluding_prev_cycles(second_level, third_level[0], 0, 3);
  fifth_level[1] = find_connection_excluding_prev_cycles(second_level, third_level[1], 0, 3);

  c = 0;

  sixth_level[c++] = find_next_cone_in_direction(third_level[0], fifth_level[0], POSITIVE);
  sixth_level[c++] = find_next_cone_in_direction(third_level[0], fifth_level[0], NEGATIVE);

  sixth_level[c++] = find_next_cone_in_direction(third_level[1], fifth_level[1], POSITIVE);
  sixth_level[c++] = find_next_cone_in_direction(third_level[1], fifth_level[1], NEGATIVE);


  
  c = 0; // reset the counter

  seventh_level[c++] = get_mutual_connection(sixth_level[1],sixth_level[2]);
  seventh_level[c++] = get_mutual_connection(sixth_level[0],sixth_level[3]);

  

#ifdef DEBUG_PRINT
  Serial.print(F("LEVEL 1: ")); Serial.print(cycles[0]); Serial.print(" "); Serial.println(cycles[1]);
  Serial.print(F("LEVEL 2: ")); Serial.print(second_level[0]); Serial.print(" "); Serial.print(second_level[1]); Serial.print(" "); Serial.print(second_level[2]); Serial.print(" "); Serial.println(second_level[3]);
  Serial.print(F("LEVEL 3: ")); Serial.print(third_level[0]); Serial.print(" "); Serial.println(third_level[1]);
  Serial.print(F("LEVEL 4: ")); Serial.print(fourth_level[0]); Serial.print(" "); Serial.print(fourth_level[1]); Serial.print(" "); Serial.print(fourth_level[2]); Serial.print(" "); Serial.println(fourth_level[3]);
  Serial.print(F("LEVEL 5: ")); Serial.print(fifth_level[0]); Serial.print(" "); Serial.println(fifth_level[1]);
  Serial.print(F("LEVEL 6: ")); Serial.print(sixth_level[0]); Serial.print(" "); Serial.print(sixth_level[1]); Serial.print(" "); Serial.print(sixth_level[2]); Serial.print(" "); Serial.println(sixth_level[3]);
  Serial.print(F("LEVEL 7: ")); Serial.print(seventh_level[0]); Serial.print(" "); Serial.println(seventh_level[1]);
  
  printCycles();
#endif

}





void set_threefold_cycles(Cone root_cone){
  // the root/seed cone is the first element of `cycles`
  
  const uint8_t cycle_lengths[8] = {1,3,3,3,3,3,3,1};
  
  uint8_t* first_level = cycles; // contains 1
  uint8_t* second_level = cycles+partial_sum(cycle_lengths,1); // contains 3
  uint8_t* third_level = cycles+partial_sum(cycle_lengths,2); // contains 3
  uint8_t* fourth_level = cycles+partial_sum(cycle_lengths,3); // contains 3
  uint8_t* fifth_level = cycles+partial_sum(cycle_lengths,4); // contains 3
  uint8_t* sixth_level = cycles+partial_sum(cycle_lengths,5); // contains 3
  uint8_t* seventh_level = cycles+partial_sum(cycle_lengths,6); // contains 3
  uint8_t* eighth_level = cycles+partial_sum(cycle_lengths,7); // contains 1 thing
  
  (*first_level) = root_cone;
  
  // the first cycle is the cycle determined by the connections of the seed cone
  for(int i = 0; i < 3; i++){
    second_level[i] = get_connection(first_level[0], i);
  }

  uint8_t *arr;

  for(int i = 0; i < 3; i++){
    // the next two sets of cycles are found by finding the sets of two
    // cones in between the cones set in the above for loop
    
    arr = find_middle_two(second_level[i], second_level[((i+1)%3) ]);
    
    third_level[i] = arr[0];
    fourth_level[i] = arr[1];
    
#ifdef DEBUG_PRINT
    Serial.print(F("array out: "));
    for(int i = 0; i < 2; i++){
      Serial.print(arr[0], DEC); Serial.print(", "); Serial.println(arr[1], DEC);
    }
#endif
  }

  for(int i = 0; i < 3; i++){
    // the next two sets of cycles are found by finding the sets of two
    // cones in between the cones set in the above for loop
    arr = find_middle_two(third_level[i], fourth_level[(i+1)%3]);
    fifth_level[i] = arr[0];
    sixth_level[i] = arr[1];
  }

  for(int i = 0; i < 3; i++){
    // the last cycle is generated by looking at the cones in the two previously 
    // set cycles, and finding the mutual connection of the adjacent cones
    seventh_level[i] = get_mutual_connection(fifth_level[i], sixth_level[(i + 1) % 3]);
  }

  // as the source is set as cycles[0], we must find the other axis cone
  // and input it into the array, which is done in this function 

  set_missing_in_cycles(19); // why does this need an argument?


#ifdef DEBUG_PRINT
  Serial.println(F("Threefold cycles:"));

  Serial.print(F("LEVEL 1: ")); Serial.println(cycles[0]); 
  Serial.print(F("LEVEL 2: ")); Serial.print(second_level[0]); Serial.print(" "); Serial.print(second_level[1]); Serial.print(" "); Serial.println(second_level[2]);
  Serial.print(F("LEVEL 3: ")); Serial.print(third_level[0]); Serial.print(" "); Serial.print(third_level[1]); Serial.print(" "); Serial.println(third_level[2]);
  Serial.print(F("LEVEL 4: ")); Serial.print(fourth_level[0]); Serial.print(" "); Serial.print(fourth_level[1]); Serial.print(" "); Serial.println(fourth_level[2]);
  Serial.print(F("LEVEL 5: ")); Serial.print(fifth_level[0]); Serial.print(" "); Serial.print(fifth_level[1]); Serial.print(" "); Serial.println(fifth_level[2]);
  Serial.print(F("LEVEL 6: ")); Serial.print(sixth_level[0]); Serial.print(" "); Serial.print(sixth_level[1]); Serial.print(" "); Serial.println(sixth_level[2]);
  Serial.print(F("LEVEL 7: ")); Serial.print(seventh_level[0]); Serial.print(" "); Serial.print(seventh_level[1]); Serial.print(" "); Serial.println(seventh_level[2]);
  Serial.print(F("LEVEL 8: ")); Serial.println(eighth_level[0]);
#endif
}



//      next Cone
//        /    \
//source /      \   Cone
// Cone  \      /   in Negative direction 
//        \____/
void set_fivefold_cycles(Cone src, Cone next, Direction dir){

  cycles[0] = src;
  cycles[1] = next;

  for(int i = 2; i < 5; i++){
    cycles[i] = find_next_cone_in_direction(cycles[i - 2], cycles[i - 1], dir);
  }

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

  Serial.println(F("Fivefold cycles:"));

  Serial.print(F("\nLEVEL 1: ")); for (uint8_t i{0}; i<5; ++i) {Serial.print(cycles[i]);  Serial.print(" ");}
  Serial.print(F("\nLEVEL 2: ")); for (uint8_t i{0}; i<5; ++i) {Serial.print(cycles[i+5]);  Serial.print(" ");}
  Serial.print(F("\nLEVEL 3: ")); for (uint8_t i{0}; i<5; ++i) {Serial.print(cycles[i+10]);  Serial.print(" ");}
  Serial.print(F("\nLEVEL 4: ")); for (uint8_t i{0}; i<5; ++i) {Serial.print(cycles[i+15]);  Serial.print(" ");}
  Serial.println("");

}



void set_reflection_cycles(){
  // This function sets the cycle array for reflections, assuming the seed 
  // pair has already been set
  
  const uint8_t cycle_lengths[7] = {2,4,2,4,2,4,2};
  
  // the first level is already in `cycles` from `set_twofold_cycles`, by hypothesis
  uint8_t* first_level = cycles; // contains 2
  uint8_t* second_level = cycles+partial_sum(cycle_lengths,1); // contains 4
  uint8_t* third_level = cycles+partial_sum(cycle_lengths,2); // contains 2
  uint8_t* fourth_level = cycles+partial_sum(cycle_lengths,3); // contains 4
  uint8_t* fifth_level = cycles+partial_sum(cycle_lengths,4); // contains 2
  uint8_t* sixth_level = cycles+partial_sum(cycle_lengths,5); // contains 4
  uint8_t* seventh_level = cycles+partial_sum(cycle_lengths,6); // contains 2


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
  Serial.print(F("LEVEL 1: ")); Serial.print(cycles[0]); Serial.print(" "); Serial.println(cycles[1]);
  Serial.print(F("LEVEL 2: ")); Serial.print(second_level[0]); Serial.print(" "); Serial.print(second_level[1]); Serial.print(" "); Serial.print(second_level[2]); Serial.print(" "); Serial.println(second_level[3]);
  Serial.print(F("LEVEL 3: ")); Serial.print(third_level[0]); Serial.print(" "); Serial.println(third_level[1]);
  Serial.print(F("LEVEL 4: ")); Serial.print(fourth_level[0]); Serial.print(" "); Serial.print(fourth_level[1]); Serial.print(" "); Serial.print(fourth_level[2]); Serial.print(" "); Serial.println(fourth_level[3]);
  Serial.print(F("LEVEL 5: ")); Serial.print(fifth_level[0]); Serial.print(" "); Serial.println(fifth_level[1]);
  Serial.print(F("LEVEL 6: ")); Serial.print(sixth_level[0]); Serial.print(" "); Serial.print(sixth_level[1]); Serial.print(" "); Serial.print(sixth_level[2]); Serial.print(" "); Serial.println(sixth_level[3]);
  Serial.print(F("LEVEL 7: ")); Serial.print(seventh_level[0]); Serial.print(" "); Serial.println(seventh_level[1]);
#endif

}



// Function used to find the third element (cone number) of a pentagon on
// the geometric structure, given the cone oriented on top of the object,
// the second cone in the pentagon sequence, and the direction around the
// top oriented cone from the second cone to where the third should be found

Cone find_next_cone_in_direction(Cone src, Cone next, Direction dir){
  //find the "next" cone in the list of connections of the 
  //source cone - if it does not exist, return an error
  // Serial.print("going in direction "); Serial.print(dir); Serial.print(" from cone "); Serial.print(src); Serial.print("-->"); Serial.println(next);
  
  for(int i = 0; i < MAX_CONNECTION_NUM; i++){

    if (get_connection(next, i) == src){
      // When we find the "next" cone in the list of connected cones
      // find the cone that is oriented clockwise (negative) or counterclockwise
      // (positive) from the "next" cone, when orienting the object
      // such that the src cone is on top
      
      // As the list of connections is ordered to be the cycle around the cone
      // specified by the index, we can simply take the next element in the list
      Cone result = get_connection(next, ((i+MAX_CONNECTION_NUM) + dir) % MAX_CONNECTION_NUM);
      // Serial.print("next in direction: ");Serial.print(src);Serial.print("->");Serial.print(next);Serial.print("->");Serial.println(result);
      return result;
    }
  }

  return SRC_NEXT_UNCONNECTED_ERR;
}




  



// Find the cone that is connected to both cone1 and cone2
Cone get_mutual_connection(Cone cone1, Cone cone2){
  // Serial.print("finding mutual "); Serial.print(cone1);Serial.print(" ");Serial.println(cone2);
  for(int i = 0; i < MAX_CONNECTION_NUM; i++){
    // Iterate throught the connections of cone1
    Cone potential_common_cone = get_connection(cone1, i);
    for(int j = 0; j < MAX_CONNECTION_NUM; j++){
      // check to see if any of the connections of cone2 match
      // with the current connection from cone1 - if so, return
      // the appropriate cone number
      if(potential_common_cone == get_connection(cone2, j)) return potential_common_cone;
    }
  }

  // if we found no mutual cones, none exist, and we return an error code
  return NO_SUCH_CONE_ERR;
}

// Function used to iterate through the connections of a cone
// specifically not returning those stored in specific indices
// of the cycles array (ie cones that we already know their cycle)
uint8_t find_connection_excluding_prev_cycles(uint8_t* arr, int cone, uint8_t min_avoid, uint8_t max_avoid){
    for(int i = 0; i < MAX_CONNECTION_NUM; i++){
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


uint8_t find_connection_index(Cone src, uint8_t conn){
  for(int i = 0; i < MAX_CONNECTION_NUM; i++){
    if(get_connection(src, i) == conn) return i;
  }

  return NO_CONNECTION;
}

// tells whether going positive or negative, from cone1 -> middle -> cone2
Direction find_direction(Cone cone1, Cone middle, Cone cone2){
  for (int i = 0; i < MAX_CONNECTION_NUM; ++i){
    Cone connected_cone = get_connection(middle, i);
    if (connected_cone == cone1){
      if (get_connection(middle, (i + 1) % (MAX_CONNECTION_NUM )) == cone2){ // do we have to go in the positive direction in the stored connections? to find cone2?
        return POSITIVE;
      } 
      else if (get_connection(middle, ( (i+MAX_CONNECTION_NUM)) - 1 % (MAX_CONNECTION_NUM)) == cone2){
        return NEGATIVE;
      }
    }
  }
  return NO_SUCH_DIRECTION;
}


// Given two cones one and two, find the two cones that create
// a path of length two from cone one to cone two
//
// TODO: Replace code in set_fivefold with this function, as
//       they are mostly equivalent
Cone* find_middle_two(Cone cone1, Cone cone2){

  Cone middlecone = get_mutual_connection(cone1, cone2);
  

  static Cone arr[2]; // should this really be static?

  Direction dir = find_direction(cone1, middlecone, cone2);
  // Serial.print("sequence "); 
  // Serial.print(" going from cone "); Serial.print(cone1); Serial.print(" -> "); Serial.print(middlecone);  Serial.print(" -> "); Serial.print(cone2); Serial.print(" is direction: "); Serial.println(dir); 
  
  Cone a = find_next_cone_in_direction(middlecone, cone2, dir);
  Cone b = find_next_cone_in_direction(cone2, a, dir);
  
  arr[0] = a; // for some reason, assigning directly into arr wasn't working.   
  arr[1] = b;
  
  // Serial.print("pentagon : ("); Serial.print(cone1); Serial.print(" -> "); Serial.print(middlecone); Serial.print(" -> "); Serial.print(cone2);
  // Serial.print(") is "); Serial.print(arr[0]); Serial.print(" and "); Serial.println(arr[1]);
  
  return arr;
}





// As we know the cycle array will contain the numbers 0-19
// we know the sum of the array will be  0 + 1 + 2 + 3 + ... + 19 = 190
// and that (Sum of the Array without one of those numbers) = 190 - that number
// giving us missing number = 190 - sum of the array without the missing number
void set_missing_in_cycles(uint8_t missingIndex){

  uint8_t sum_of_array = 0;
  
  for(int i = 0; i < NUM_CONES; ++i){
    if(i != missingIndex){
      sum_of_array += cycles[i];
    }
  }

  // once we have found the missing number, set it in the array
  cycles[missingIndex] = sumFromZeroToNineteen - sum_of_array;
}


  
  
  
  





void setSymmetryModeFromButtons(){
  #ifdef DEBUG_PRINT
  Serial.println(F("getting symm mode from buttons"));
  #endif
  
  g_previous_symmetry = g_symmetry; // cache the old one

  if (is_button_pressed(0)&&is_button_pressed(1)){
    Serial.println("Set reflectional");
    g_symmetry = Reflect;
  }
  else if(is_button_pressed(0)){
      Serial.println("Set twofold");
    g_symmetry = TwoFold;
  }
  else if(is_button_pressed(1)){
    Serial.println("Set threefold");
    g_symmetry = ThreeFold;
  } 
  else if(is_button_pressed(2)){
    Serial.println("Set fivefold");
    g_symmetry = FiveFold;
    
  }
}

void doReflectionalMode(){
    g_symmetry = Reflect;
  
    if(previousMode != Reflectional) {
      Serial.println(F("Reflectional Mode"));
      incrementAxis(0);
      set_cycle_presets();
      pixels.show();
      previousMode = Reflectional;
    }
  

    if(abs(encoder_counter - previous_encoder_value) > TICKS_PER_ROTATION && is_encoder_button_down)
    {
      #ifdef DEBUG_PRINT
      Serial.println(F("Change reflection axis"));
      #endif
      previous_encoder_value = encoder_counter;
      incrementAxis(1);
      set_cycle_presets();

      pixels.show();
    } else if(abs(encoder_counter - previous_encoder_value) > TICKS_PER_ROTATION) {
      #ifdef DEBUG_PRINT
      Serial.println(F("Reflect"));
      #endif
      previous_encoder_value = encoder_counter;
      rotate(false);
      pixels.show();
    }
}


void doColorSetMode(){

    if(previousMode != ColorSet) {
      Serial.println(F("ColorSet Mode"));
      clear();
      previousMode = ColorSet;
    }

  
    coneSelect();
    pixels.show();
}
