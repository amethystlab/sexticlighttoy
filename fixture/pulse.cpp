#include "pulse.h"


void pulse_pot_color(Time time_per_step) {
  uint16_t j = 0;
  uint8_t num_steps = MAX_UINT8-1;
  float scale = 1;
  while (j <= num_steps) {

    g_current_millis = millis();
    if (g_current_millis - g_previous_millis >= time_per_step) {
      scale = float(j++) / num_steps;
      g_previous_millis = g_current_millis;

      for (uint8_t i = 0; i < NUM_PIXELS; i++) {
        pixels.setPixelColor(i, scale*(pot1/4), scale*(pot0/4), scale*(pot2/4), 0); // divide by 4 to scale from 0-1023 to 0-255 for those pots.
      }
      pixels.show();
    } // if
  } // j

  j = 0;
  while (j < num_steps) {

    g_current_millis = millis();
    if (g_current_millis - g_previous_millis >= time_per_step) {
      scale = 1 - (float(j++) / num_steps);
      g_previous_millis = g_current_millis;

      for (uint8_t i = 0; i < NUM_PIXELS; i++) {
        pixels.setPixelColor(i, scale*(pot1/4), scale*(pot0/4), scale*(pot2/4), 0);
      }
      pixels.show();
    } // if
  } // j
} // fun





void pulse_fixed_color(Time time_per_step, IndividualColor red, IndividualColor green, IndividualColor blue, IndividualColor white) {
  uint16_t j = 0;

  uint8_t num_steps = MAX_UINT8-1;
  float scale = 1;
  while (j <= num_steps) {
    g_current_millis = millis();
    if (g_current_millis - g_previous_millis >= time_per_step) {
      scale = float(j++) / num_steps;
      g_previous_millis = g_current_millis;

      for (uint8_t i = 0; i < pixels.numPixels(); i++) {
        pixels.setPixelColor(i, scale*green, scale*red, scale*blue, scale*white);
      }
      pixels.show();
    } // if
  } // j

  j = 0;
  while (j < num_steps) {
    g_current_millis = millis();
    if (g_current_millis - g_previous_millis >= time_per_step) {
      scale = 1 - (float(j++) / num_steps);
      g_previous_millis = g_current_millis;

      for (uint8_t i = 0; i < pixels.numPixels(); i++) {
        pixels.setPixelColor(i, scale*green, scale*red, scale*blue, scale*white);
      }
      pixels.show();
    } // if
  } // j
} // fun






// sets all pixes to red
void pulse_red(Time time_per_step) {
  pulse_fixed_color(time_per_step,255,0,0,0);
} // fun

// sets all pixes to 
void pulse_green(Time time_per_step) {
  pulse_fixed_color(time_per_step,255,0,0,0);
} // fun

// sets all pixes to 
void pulse_blue(Time time_per_step) {
  pulse_fixed_color(time_per_step,0,0,255,0);
} // fun



// sets all pixes to 
void pulse_magenta(Time time_per_step) {
  pulse_fixed_color(time_per_step,0,255,255,0);
} // fun



// sets all pixes to 
void pulse_white(Time time_per_step) {
  pulse_fixed_color(time_per_step,0,0,0,255);
} // fun

void pulse_all(Time time_per_step) {
  pulse_fixed_color(time_per_step, 255, 255, 255, 255);
} // fun




void doPulseMode(){
  Serial.println(F("pulse mode"));
  pulse_pot_color(10);
}
