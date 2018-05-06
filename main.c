/*
 * LCD3 firmware
 *
 * Copyright (C) Casainho, 2018.
 *
 * Released under the GPL License, Version 3
 */

#include <stdint.h>
#include <stdio.h>
#include "stm8s.h"
#include "gpio.h"
#include "timers.h"
#include "adc.h"
#include "ht1622.h"

/////////////////////////////////////////////////////////////////////////////////////////////
//// Functions prototypes

// main -- start of firmware and main loop
int main (void);

// With SDCC, interrupt service routine function prototypes must be placed in the file that contains main ()
// in order for an vector for the interrupt to be placed in the the interrupt vector space.  It's acceptable
// to place the function prototype in a header file as long as the header file is included in the file that
// contains main ().  SDCC will not generate any warnings or errors if this is not done, but the vector will
// not be in place so the ISR will not be executed when the interrupt occurs.

// Calling a function from interrupt not always works, SDCC manual says to avoid it. Maybe the best is to put
// all the code inside the interrupt

// Local VS global variables
// Sometimes I got the following error when compiling the firmware: motor.asm:750: Error: <r> relocation error
// when I have this code inside a function: "static uint8_t ui8_cruise_counter = 0;"
// and the solution was define the variable as global instead
// Another error example:
// *** buffer overflow detected ***: sdcc terminated
// Caught signal 6: SIGABRT

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

int main (void)
{
  static uint16_t ui16_temp;
  static uint8_t ui8_temp;

  //set clock at the max 16MHz
  CLK_HSIPrescalerConfig (CLK_PRESCALER_HSIDIV1);

  gpio_init ();
  timer2_init ();
  adc_init ();
  ht1622_init ();

  GPIO_WriteHigh(LCD3_ENABLE_BACKLIGHT_POWER__PORT, LCD3_ENABLE_BACKLIGHT_POWER__PIN);
  GPIO_WriteLow(LCD3_ENABLE_BACKLIGHT__PORT, LCD3_ENABLE_BACKLIGHT__PIN);

  while (1)
  {
    ui16_temp = ui16_adc_read_battery_voltage_10b ();

    ui16_temp = GPIO_ReadInputPin(LCD3_BUTTON_DOWN__PORT, LCD3_BUTTON_DOWN__PIN);
    if (ui16_temp == 0)
    {
      if (ui8_temp)
      {
        ht1622_enable_all_segments (1);
        ui8_temp = 0;
      }
      else
      {
        ht1622_enable_all_segments (0);
        ui8_temp = 1;
      }

      while (!GPIO_ReadInputPin(LCD3_BUTTON_DOWN__PORT, LCD3_BUTTON_DOWN__PIN)) ;
    }
  }

  return 0;
}
