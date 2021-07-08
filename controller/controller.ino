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


// see https://www.arduino.cc/en/Tutorial/MasterWriter
// see https://github.com/sparkfun/Rotary_Encoder_Breakout-Illuminated/blob/master/Firmware/RGB_Rotary_Encoder/RGB_Rotary_Encoder.ino


#include <PinChangeInterrupt.h>


#define ROT_LED_R 2      // red LED
#define ROT_LED_G 5     // green LED
#define ROT_LED_B 6       // blue LED

#define ROT_B A0        // rotary B
#define ROT_A 3          // rotary A // do not change this one, it's associated with an interrupt
#define ROT_SW 4         // rotary pushbutton

// the other 2 pins are wired to + and -



// RGB LED colors (for common anode LED, 0 is on, 1 is off)
#define OFF B111
#define RED B110
#define GREEN B101
#define YELLOW B100
#define BLUE B011
#define PURPLE B010
#define CYAN B001
#define WHITE B000

#define BUTTON_DEBOUNCE_MS 10

// volatile because they're written to by interrupts

// rotary things
volatile int32_t rotary_counter = 0; // current "position" of rotary encoder (increments clockwise)
volatile bool rotary_change = false; // will turn true if rotary_counter has changed

// button things
volatile bool button_pressed = false; // will turn true if the button has been pushed
volatile bool button_released = false; // will turn true if the button has been released (sets button_down_time)
volatile bool is_button_down = false;
volatile unsigned long int button_down_start, button_down_time;
volatile unsigned long int button_down_end;


volatile unsigned char led_color = OFF; //LED color




#include <Wire.h>

#define FIXTURE_ADDRESS 4 // an arbitrary address to which to send the data.  this is because i2c can send to multiple devices on the same bus using addresses




uint16_t pot0, pot1, pot2; // the values of the potentiometers, between 0 and 1023.  see https://www.arduino.cc/reference/en/language/functions/analog-io/analogread/

uint8_t buttons[3]; // perhaps these should be just one integer, and use bitwise ops.  that would be an optimization.
uint8_t switches[3]; // perhaps these should be just one integer, and use bitwise ops.  that would be an optimization.

#define pot0_PIN A1
#define pot1_PIN A2
#define pot2_PIN A3

#define BUTTON1_PIN    7
#define BUTTON2_PIN    8
#define BUTTON3_PIN    9

#define SWITCH1_PIN    10
#define SWITCH2_PIN    11
#define SWITCH3_PIN    12




void setup()
{
  setup_generic();
  setup_communication();
  setup_devices();
}

void setup_generic()
{
  Serial.begin(115200);           // start serial for output
}

void setup_devices()
{

  setup_potentiometers();
  setup_switches();
  setup_buttons();
  setup_encoder();
}


void setup_potentiometers()
{
    // Set up the 3 potentiometers as INPUT:
  pinMode(pot0_PIN, INPUT); 
  pinMode(pot1_PIN, INPUT); 
  pinMode(pot2_PIN, INPUT); 
}

void setup_switches()
{
// setup for the switches
  pinMode(SWITCH1_PIN,  INPUT_PULLUP); // using pullup since the switches and buttons are connected to ground, and to avoid having to use pul
  pinMode(SWITCH2_PIN,  INPUT_PULLUP);
  pinMode(SWITCH3_PIN,  INPUT_PULLUP);
}

void setup_buttons()
{
  // setup for the pushbuttons
  pinMode(BUTTON1_PIN,  INPUT_PULLUP);
  pinMode(BUTTON2_PIN,  INPUT_PULLUP);
  pinMode(BUTTON3_PIN,  INPUT_PULLUP); 
}

void setup_encoder()
{

  // Set up all the I/O pins. Unused pins are commented out.
  pinMode(ROT_B, INPUT_PULLUP);
  pinMode(ROT_A, INPUT_PULLUP);
  
  pinMode(ROT_SW, INPUT);
  // The rotary switch is common anode with external pulldown, do not turn on pullup
  pinMode(ROT_LED_B, OUTPUT);
  pinMode(ROT_LED_G, OUTPUT);
  pinMode(ROT_LED_R, OUTPUT);

  setLED(led_color);  // this is the LED in the encoder

  
  Serial.begin(115200); // Use serial for debugging
  Serial.println("Begin RGB Rotary Encoder Testing");

  // We use the standard external interrupt pin for the rotary,
  // but we'll use the pin change interrupt library for the button.

  attachInterrupt(digitalPinToInterrupt(ROT_A), rotaryIRQ, CHANGE);
  
  attachPCINT(digitalPinToPCINT(ROT_SW), buttonIRQ, CHANGE);
//  PCintPort::attachInterrupt(ROT_SW, &buttonIRQ, CHANGE);
}

void setup_communication()
{
  Wire.begin(); // join i2c bus (address optional for master)
}






void loop()
{
  ReadAll();
  SendAll();
  
  print_state();

  delay(50); // make this longer to make the delay between sends shorter.  
  // don't want to overwhelm the fixture, though!!!
}


void print_state()
{

  Serial.print("encoder:\t");
  Serial.print(is_button_down,DEC); Serial.print("\t");
  Serial.print(button_down_time,DEC); Serial.print("\t");
  Serial.print(rotary_counter,DEC);
  Serial.println("");

  Serial.print("buttons:\t");
  for (int ii=0; ii<3; ++ii){
   Serial.print(buttons[ii]);
  }
  Serial.println("");


  Serial.print("switches:\t");
  for (int ii=0; ii<3; ++ii){
   Serial.print(switches[ii]);
  }
  Serial.println("");


  Serial.print("potentiometers:\t");
  Serial.print(pot0);    Serial.print("\t");
  Serial.print(pot1);    Serial.print("\t");
  Serial.print(pot2);    Serial.print("\t");
  Serial.println("");

  Serial.println("");
}



void ReadAll()
{
  ReadPots();
  ReadButtons();
  ReadSwitches();
  // notably, no read encoder here because it's handled via an interrupt

}


void SendAll()
{
  Wire.beginTransmission(FIXTURE_ADDRESS); // transmit to device #FIXTURE_ADDRESS
  WritePots();
  WriteButtons();
  WriteSwitches();
  WriteEncoder();
  Wire.endTransmission();    // stop transmitting
}


void write_int16(uint16_t i)
{
  static byte sendarray[2]; 
  sendarray[0] = (i >> 8) & 0xFF;
  sendarray[1] = i & 0xFF;
  Wire.write(sendarray, 2);   
}

// see https://thewanderingengineer.com/2015/05/06/sending-16-bit-and-32-bit-numbers-with-arduino-i2c/
void write_int32(int32_t i)
{
  static byte sendarray[4];
  sendarray[0] = (i >> 24) & 0xFF;
  sendarray[1] = (i >> 16) & 0xFF;
  sendarray[2] = (i >> 8) & 0xFF;
  sendarray[3] = i & 0xFF;

  Wire.write(sendarray, 4);   
}






void ReadPots(){
  pot0 = analogRead(pot0_PIN);
  pot1 = analogRead(pot1_PIN);
  pot2 = analogRead(pot2_PIN);
}


void WritePots()
{
  write_int16(pot0);
  write_int16(pot1);
  write_int16(pot2);
}


void ReadButtons()
{
  buttons[0] = !digitalRead(BUTTON1_PIN);
  buttons[1] = !digitalRead(BUTTON2_PIN);
  buttons[2] = !digitalRead(BUTTON3_PIN);
}

void WriteButtons()
{
  Wire.write(buttons, 3);
}




void ReadSwitches()
{
  switches[0] = !digitalRead(SWITCH1_PIN);
  switches[1] = !digitalRead(SWITCH2_PIN);
  switches[2] = !digitalRead(SWITCH3_PIN);
}

void WriteSwitches()
{
  Wire.write(switches, 3);
}



// there's no ReadEncoder button, because its state is set via interrupts

void WriteEncoder()
{
  Wire.write(is_button_down);
  
  if (is_button_down)
  {
    button_down_time = millis()-button_down_start;
    write_int32(button_down_time);
  }
  else
    write_int32(0);

  write_int32(rotary_counter);
}



void setLED(unsigned char color)
// Set RGB LED to one of eight colors (see #defines above)
{
  digitalWrite(ROT_LED_R, color & B001);
  digitalWrite(ROT_LED_G, color & B010);
  digitalWrite(ROT_LED_B, color & B100);
}




// this code adapted from https://github.com/sparkfun/Rotary_Encoder_Breakout-Illuminated/blob/master/Firmware/RGB_Rotary_Encoder/RGB_Rotary_Encoder.ino
void buttonIRQ()
{
  // Process rotary encoder button presses and releases, including
  // debouncing (extra "presses" from noisy switch contacts).
  // If button is pressed, the button_pressed flag is set to true.
  // (Manually set this to false after handling the change.)
  // If button is released, the button_released flag is set to true,
  // and button_down_time will contain the duration of the button
  // press in ms. (Set this to false after handling the change.)



  uint8_t trigger = getPinChangeInterruptTrigger(digitalPinToPCINT(ROT_SW));
  
  if ((trigger == RISING) && (is_button_down == false))
    // Button was up, but is currently being pressed down
  {
    // Discard button presses too close together (debounce)
    button_down_start = millis();
    if (button_down_start > (button_down_end + BUTTON_DEBOUNCE_MS)) // BUTTON_DEBOUNCE_MS debounce timer
    {
      is_button_down = true;
      button_pressed = true;
//      Serial.println("pressed");
    }
  }
  else if ((trigger == FALLING) && (is_button_down == true))
    // Button was down, but has just been released
  {
    // Discard button releases too close together (debounce)
    button_down_end = millis();
    if (button_down_end > (button_down_start + BUTTON_DEBOUNCE_MS)) // BUTTON_DEBOUNCE_MS debounce timer
    {
      is_button_down = false;
      button_released = true;
      button_down_time = button_down_end - button_down_start;
//      Serial.println("released");
    }
  }
}



// this code adapted from https://github.com/sparkfun/Rotary_Encoder_Breakout-Illuminated/blob/master/Firmware/RGB_Rotary_Encoder/RGB_Rotary_Encoder.ino
void rotaryIRQ()
{
  // Process input from the rotary encoder.
  // The rotary "position" is held in rotary_counter, increasing for CW rotation (changes by one per detent).
  // If the position changes, rotary_change will be set true. (You may manually set this to false after handling the change).

  // This function will automatically run when rotary encoder input A transitions in either direction (low to high or high to low)
  // By saving the state of the A and B pins through two interrupts, we'll determine the direction of rotation
  // int rotary_counter will be updated with the new value, and bool rotary_change will be true if there was a value change
  // Based on concepts from Oleg at circuits@home (http://www.circuitsathome.com/mcu/rotary-encoder-interrupt-service-routine-for-avr-micros)
  // Unlike Oleg's original code, this code uses only one interrupt and has only two transition states;
  // it has less resolution but needs only one interrupt, is very smooth, and handles switchbounce well.

  static unsigned char rotary_state = 0; // current and previous encoder states

  rotary_state <<= 2;  // remember previous state
  rotary_state |= (digitalRead(ROT_A) | (digitalRead(ROT_B) << 1));  // mask in current state
  rotary_state &= 0x0F; // zero upper nybble

  if (rotary_state == 0x06) // from 10 to 01, increment counter. Also try 0x09, 0x06 if unreliable
  {
    rotary_counter++;
    rotary_change = true;
//    Serial.println("increase");
  }
  else if (rotary_state == 0x03) // from 00 to 11, decrement counter. Also try 0x0C if unreliable
  {
    rotary_counter--;
    rotary_change = true;
//    Serial.println("decrease");
  }
}
