

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
  for (int i = 0; i < MAX_CONNECTION_NUM; ++i){
    if (i!=skip_this_conn)
      second_level[c++] = get_connection(cycles[0], i);
  }
  
  skip_this_conn = find_connection_index(cycles[1], cycles[0]);
  for (int i = 0; i < MAX_CONNECTION_NUM; ++i){
    if (i!=skip_this_conn)
      second_level[c++] = get_connection(cycles[1], i);
  }
  
  

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
  skip_this_conn = find_connection_index(fifth_level[0], third_level[0]); // 
  for (int i = 0; i < MAX_CONNECTION_NUM; ++i){
    if (i!=skip_this_conn)
      sixth_level[c++] = get_connection(fifth_level[0], i);
  }
  
  skip_this_conn = find_connection_index(fifth_level[1], third_level[1]);
  for (int i = 0; i < MAX_CONNECTION_NUM; ++i){
    if (i!=skip_this_conn)
      sixth_level[c++] = get_connection(fifth_level[1], i);
  }
  
  
  c = 0; // reset the counter
  for (int i=0; i<2; ++i){
    for (int j=2; j<4; ++j){
      uint8_t common_cone = get_mutual_connection(sixth_level[i], sixth_level[j]);
      if (common_cone!=NO_SUCH_CONE_ERR && common_cone!=fifth_level[0] && common_cone!=fifth_level[1]){
        // Serial.print("found common for level 3: "); Serial.println(common_cone);
        seventh_level[c++] = common_cone;
      }
    } // j
  } // i
  

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




// TODO: Investigate iterative path rather than
//       iterating through every single path of length
//       two from the cone in array location 0

//  uint8_t potentialConnectionsNext[2];
//  uint8_t potentialConnectionsThird[2];
//  int j = 0;
//
//  // Find the cones connected to the "next" cone
//  // that are not the source cone
//  for(int i = 0; i < MAX_CONNECTION_NUM; i++){
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
//  for(int i = 0; i < MAX_CONNECTION_NUM; i++){
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

  // cycles[2] = second_level[0];
  // cycles[3] = second_level[3];
  // cycles[4] = second_level[1];
  // cycles[5] = second_level[2];
  // cycles[6] = fourth_level[0];
  // cycles[7] = fourth_level[3];
  // cycles[8] = fourth_level[1];
  // cycles[9] = fourth_level[2];
  // cycles[10] = sixth_level[0];
  // cycles[11] = sixth_level[1];
  // cycles[12] = sixth_level[2];
  // cycles[13] = sixth_level[3];
  // cycles[14] = seventh_level[0];
  // cycles[15] = seventh_level[1];

  ///////////////////////////////

  // cycles[16] = third_level[0];
  // cycles[17] = third_level[1];
  // cycles[18] = fifth_level[0];
  // cycles[19] = fifth_level[1];
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
  Serial.print(F("Setting "));
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

  for(int i = index; i < NUM_CONES; i++){
    // set the remaining cones colors to white
    coneColor(cycles[i], 255, 255, 255, 255);
#ifdef DEBUG_PRINT
    Serial.print(cycles[i]);
    Serial.print(", ");
#endif
  }
#ifdef DEBUG_PRINT
  Serial.println(F("to white"));
#endif
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


void getNextPair(uint8_t *arr, uint8_t one, uint8_t two){
    uint8_t first = one;
    uint8_t second = two;

    uint8_t connection_num = 255;
    bool found = false;
    for(int i = 0; i < MAX_CONNECTION_NUM && !found; i++){
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
      first = (first % NUM_LED_GROUPS) + 1;
    }

    found = false;
    
    while(!found){
      for(int i = connection_num; i < MAX_CONNECTION_NUM && !found; i++){
        second = get_connection(first, i);
        if(second > first){
          found = true;
        }
      }

      if(!found){
        first = (first % NUM_LED_GROUPS) + 1;
        connection_num = 0;
      }
    }

    arr[0] = first;
    arr[1] = second;

    // iterate the axis cone (so the center of rotation
    // becomes cone two if it was cone one before, etc
}


// only capable of incrementing positive -- cannot go reverse.
void incrementAxis(Direction dir){
  static Cone fiveFoldAxis = 0;
  static Cone threeFoldAxis = 1;
  static Cone twoFoldAxis[2] = {1, 2};
  static Cone reflectionAxis[2] = {1, 2};

  switch(symmetry){
    
    case Reflect:
      if(dir) getNextPair(reflectionAxis, reflectionAxis[0], reflectionAxis[1]);
      cycles[0] = reflectionAxis[0];
      cycles[1] = reflectionAxis[1];
#ifdef DEBUG_PRINT
      Serial.print(F("NEW AXIS: ")); Serial.print(reflectionAxis[0]); Serial.print(", "); Serial.println(reflectionAxis[1]);
#endif
      set_reflection_cycles();
      break;
      
    case TwoFold:
      if(dir) getNextPair(twoFoldAxis, twoFoldAxis[0], twoFoldAxis[1]);

#ifdef DEBUG_PRINT
      Serial.print("NEW AXIS: "); Serial.print(twoFoldAxis[0]); Serial.print(", "); Serial.println(twoFoldAxis[1]);
#endif
      set_twofold_cycles(twoFoldAxis[0],twoFoldAxis[1]);
      break;
      
    case ThreeFold:
      if (dir) 
        threeFoldAxis = (threeFoldAxis + 1) % NUM_LED_GROUPS;
      set_threefold_cycles(threeFoldAxis);
      break;
      
    case FiveFold:
      if (dir) 
        fiveFoldAxis = (fiveFoldAxis + 1) % NUM_FIVE_FOLD_AXES;
      set_fivefold_cycles(fiveFoldAxes[fiveFoldAxis][0], fiveFoldAxes[fiveFoldAxis][1], POSITIVE);
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
      
    case ThreeFold:
      offset = 1;
      maximum = 20;
      numPerRotation = 3;
      break;
      
    case FiveFold:
      offset = 0;
      maximum = 20;
      numPerRotation = 5;
      break;
      
    case Reflect:
      offset = 0;
      maximum = 16;
      numPerRotation = 2;
      break;
  }
  
  int index = offset;
  getCurrentTime();
  boolean shouldContinue = true;

  while (index + numPerRotation <= maximum && shouldContinue){
    // iterate through the cycles array, rotating each group

#ifndef GRADUAL_TRANSITION
    nFoldRotateColor(cycles + index, numPerRotation, reverse);
#else
    shouldContinue = nFoldRotateColorTransition(cycles + index, numPerRotation, reverse, g_current_time, 1000);
#endif
    index += numPerRotation;
  }
}


  
  
  
  

//Takes the color from cone1 and transfers it to cone2, cone2 goes to cone3, cone3 goes to cone1
void ThreeFoldRotateColor(Cone cone1, Cone cone2, Cone cone3) { //must make sure the cone# is in the correct order
  if (cone1 >= 0 && cone1 < NUM_CONES && cone2 >= 1 && cone2 <= 20 && cone3 >= 1 && cone3 <= 20) { //makes sure all the inputed cone numbers are 1-20
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
void nFoldRotateColor(uint8_t *coneArray, uint8_t num_cones_to_rotate, bool reverse) { //num_cones_to_rotate refers to number of cones in the array
  if (num_cones_to_rotate > 0) {
    
    uint32_t color = getConeColor(coneArray[0]);
    uint32_t overwrittenColor = getConeColor(coneArray[0]);

    uint8_t prevIndex = 0;

    for (int i = 0; i < num_cones_to_rotate; i++) {
      if (coneArray[i] < NUM_CONES) { //makes sure all the inputed cone numbers are between 0-19

        color = overwrittenColor;
        uint8_t index = -1;
        if(!reverse){
          index = (prevIndex + 1) % num_cones_to_rotate;
        } else {
          index = (prevIndex - 1 + num_cones_to_rotate) % num_cones_to_rotate;
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
    Serial.println(F("numCone is not greater than one. Choose new number in nFoldRotateColor."));
#endif
  }
} //end 7/18/20




bool nFoldRotateColorTransition(uint8_t *coneArray, uint8_t numCone, bool reverse, uint16_t currentTime, uint16_t leng) { //numCone refers to number of cones in the array
  if (numCone > 1) {

    for(int i = 0; i < numCone; i++){
      if(frame_times[1] != NO_EVENT_PLANNED){     
        return false;
      }
    }
    
    uint32_t color = getConeColor(coneArray[0]);
    uint32_t overwrittenColor = getConeColor(coneArray[0]);

    uint8_t prevIndex = 0;

    for (int i = 0; i < numCone; i++) {
      if (coneArray[i] < NUM_CONES) { //makes sure all the inputed cone numbers are between 0-19

        color = overwrittenColor;
        uint8_t index;
        if(!reverse){
          index = (prevIndex + 1) % numCone;
        } else {
          index = (prevIndex - 1 + numCone) % numCone;
        }

        overwrittenColor = getConeColor(coneArray[index]);

        frame_times[0] = g_current_time;
        

        frame_times[1] = g_current_time + leng;

        frame_colors[coneArray[index]][0] = getConeColor(coneArray[index]);
        frame_colors[coneArray[index]][1] = color;
        
        //coneColor(coneArray[index], (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, (color >> 24) & 0xFF); //moves color cone to cone

        

        prevIndex = index;
      }
    }

    return true;
  }
  else {
#ifdef DEBUG_PRINT
    Serial.println(F("numCone is not greater than one. Choose new number in nFoldRotateColor."));
#endif
  }
} 






//Uses the encoder to rotate the three fold symmetry 120 degrees about the axis point
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

  
void doRotationalMode()
{

  if(previousMode != Rotational) {
    symmetry = TwoFold;

    // Serial.println("Rotational Mode");

    incrementAxis(0);
    
    set_cycle_presets();
    pixels.show();
    previousMode = Rotational;
  }
  

  if(abs(rotary_counter - previousEncoderValue) > TICKS_PER_ROTATION && is_button_down)
  {
    // Serial.println("Rotate Axis");
    previousEncoderValue = rotary_counter;
    incrementAxis(1);
    set_cycle_presets();

    pixels.show();
  } else if((rotary_counter - previousEncoderValue) > TICKS_PER_ROTATION) {
#ifdef DEBUG_PRINT
    Serial.print(rotary_counter - previousEncoderValue); Serial.print(" > "); Serial.println(TICKS_PER_ROTATION);
    Serial.println(F("Rotate Positive"));
#endif
    previousEncoderValue = rotary_counter;
    rotate(false);
    pixels.show();
  } else if((previousEncoderValue - rotary_counter) > TICKS_PER_ROTATION){
#ifdef DEBUG_PRINT
    Serial.print(F("Rotate Negative ")); Serial.print(rotary_counter); Serial.print(" - "); Serial.print(previousEncoderValue); Serial.print(" < -"); Serial.println(TICKS_PER_ROTATION);
#endif
    previousEncoderValue = rotary_counter;
    rotate(true);
    pixels.show();
  }

  setSymmetryModeFromButtons();
  
  pixels.show();
} //  case doRotationalMode


void setSymmetryModeFromButtons(){
  
  if (is_button_pressed(0)&&is_button_pressed(1)){

    // Serial.println("Set reflectional");
    symmetry = Reflect;
    set_cycle_presets();
  }
  else if(is_button_pressed(0)){

      // Serial.println("Set twofold");

    symmetry = TwoFold;
    set_cycle_presets();
  }
  else if(is_button_pressed(1)){
    // Serial.println("Set threefold");

    symmetry = ThreeFold;
    set_cycle_presets();
  } 
  else if(is_button_pressed(2)){
    // Serial.println("Set fivefold");
    symmetry = FiveFold;
    set_cycle_presets();
  }
  // incrementAxis(0);
}

void doReflectionalMode(){
    symmetry = Reflect;
  
    if(previousMode != Reflectional) {
      Serial.println(F("Reflectional Mode"));
      incrementAxis(0);
      set_cycle_presets();
      pixels.show();
      previousMode = Reflectional;
    }
  

    if(abs(rotary_counter - previousEncoderValue) > TICKS_PER_ROTATION && is_button_down)
    {
      #ifdef DEBUG_PRINT
      Serial.println(F("Change reflection axis"));
      #endif
      previousEncoderValue = rotary_counter;
      incrementAxis(1);
      set_cycle_presets();

      pixels.show();
    } else if(abs(rotary_counter - previousEncoderValue) > TICKS_PER_ROTATION) {
      #ifdef DEBUG_PRINT
      Serial.println(F("Reflect"));
      #endif
      previousEncoderValue = rotary_counter;
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
