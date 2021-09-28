#include "device.h"


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
  is_button_down = Wire.read(); //reads signal
  button_down_time = get_int32(); //time the button has been pressed down (starts over when button is released)
  rotary_counter = get_int32(); //amount the dial has been turned
}

void get_buttons()
{
  buttons = 0;
  for (int ii = 0; ii < NUM_BUTTONS; ++ii) {
     buttons = buttons | (Wire.read() << ii);
  }
}

void get_switches()
{
  switches = 0;
  for (int ii = 0; ii < NUM_SWITCHES; ++ii) {
     switches = switches | (Wire.read() << ii);
  }
}

bool is_button_pressed(uint8_t num){
      return buttons & (1 << num);
}

bool is_switch_on(uint8_t num){
    return switches & (1 << num);
}

void get_pots()
{
  pot0 = get_uint16();
  pot1 = get_uint16();
  pot2 = get_uint16();
}


void print_state()
{

  Serial.print("encoder:  ");
  Serial.print(is_button_down, DEC); Serial.print(" ");
  Serial.print(button_down_time, DEC); Serial.print(" ");
  Serial.print(rotary_counter, DEC);
  Serial.println("");

  Serial.print("buttons:  ");
  for (int ii = 0; ii < 3; ++ii) {
    Serial.print(is_button_pressed(ii));
  }
  Serial.println("");


  Serial.print("switches:  ");
  for (int ii = 0; ii < 3; ++ii) {
    Serial.print(is_switch_on(ii));
  }
  Serial.println("");


  Serial.print("potentiometers:  ");
  Serial.print(pot0);    Serial.print(" ");
  Serial.print(pot1);    Serial.print(" ");
  Serial.print(pot2);    Serial.print(" ");
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








//when the rotary encounter is turned (in either direction) a new cone will be selected and the color is controlled by the potentiometers
void coneSelect(){
  int cone_num = abs(((rotary_counter / 2) % 20) + 1); //each time the encoder is turned 2 values it switches to a new cone, (clockwise to get larger #cone, counterclockwise to get smaller #cone)
  uint8_t g = pot1 / 4; //integer division by four to translate range of 0-1023 (potentiometers) to 0-255 (pixel brightness)
  uint8_t r = pot0 / 4;
  uint8_t b = pot2 / 4;
  
  coneColor(cone_num, g, r, b, 0); //gets the cone_num from the variable above
#ifdef DEBUG_PRINT
  Serial.println("Setting Cone "); Serial.print(cone_num); Serial.print(" to "); Serial.print(r); Serial.print(" "); Serial.print(g); Serial.print(" "); Serial.println(b);
#endif
} //end (Sam 7/7/2020)



//when the rotary encoder is pressed all the pixels are shut off, the selected cone returns to cone #1, and the encoder value returns to zero(last part done in other code)
void reset() {
  if (button_down_time > 3000) { //hold button down for 3 seconds to activate
    clear();
  }
} // end (Sam 7/8/2020)


void getMode(){
  previousMode = mode;
  Serial.println(F("mode: "));  Serial.print(mode);  Serial.print(F(" ")); Serial.print(switches,BIN);Serial.print(F("\n")); 
  // print_state();
  mode = switches;  // just interpret the uint8_t as an integer.  the user of the controller gets to use binary!
}

void setupPixels(){
  pixels.begin();
  clear();
  pixels.setBrightness(255); // 0 is off.  255 is full.  this is a limit on the brightness for the entire run.

}

void setupCommunication(){
  Wire.begin(FIXTURE_ADDRESS);                // join i2c bus with address
  Wire.onReceive(receiveEvent); // register event
}

void setupSerial(){
  Serial.begin(115200);           // start serial for output
  Serial.setTimeout(20);
}


void setupTime(){
}



Time getCurrentTime(){
  g_current_time = millis();
  return g_current_time;
}

void deadCodeGraveyard(){
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
}
