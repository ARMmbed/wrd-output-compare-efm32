/* mbed
 * Copyright (c) 2006-2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "letimer/LETimer.h"

#include "emlib/em_letimer.h"
#include "emlib/em_cmu.h"
#include "emlib/em_gpio.h"


#include <stdio.h>

#define LETIMER_ROUTE_OUT_MASK 0x03UL
#define Platform_Time_Base 32768


/* Default configuration for LETIMER0 */
static const LETIMER_Init_TypeDef letimerInit =
{
    .enable         = true,                   /* Start counting when init completed */
    .debugRun       = true,                  /* Counter shall not keep running during debug halt. */
    .rtcComp0Enable = false,                  /* Don't start counting on RTC COMP0 match. */
    .rtcComp1Enable = false,                  /* Don't start counting on RTC COMP1 match. */
    .comp0Top       = true,                   /* Load COMP0 register into CNT when counter underflows. COMP is used as TOP */
    .bufTop         = false,                  /* Don't load COMP1 into COMP0 when REP0 reaches 0. */
    .out0Pol        = 0,                      /* Idle value for output 0. */
    .out1Pol        = 0,                      /* Idle value for output 1. */
    .ufoa0          = letimerUFOAToggle,      /* Toggle output on output 0 */
    .ufoa1          = letimerUFOAToggle,      /* Toggle output on output 1 */
    .repMode        = letimerRepeatFree       /* Count until stopped */
};


void LETimer(PinName pin, uint32_t period)
{
    /*  Convert milliseconds to timer ticks.

        NOTE: Return call if period is invalid.
    */
    uint32_t timerTicks = period * Platform_Time_Base / 1000;

#if 0
    if (timerTicks > 0xFFFFUL)
    {
      return ytError(YTBadRequest);
    }
#endif

    /* Enable clocks */
    CMU_ClockEnable(cmuClock_LETIMER0, true);
    CMU_ClockEnable(cmuClock_GPIO, true);


    /*  Assert the pin name is valid and find module/location.
        Configure pin to push pull so the LETIMER can override it.

        NOTE: This switch statement defaults to a return call.
    */
    /*  Get enabled output pins. Zero out location. */
    uint8_t module;
    uint32_t route = LETIMER0->ROUTE & LETIMER_ROUTE_OUT_MASK;

    switch(pin)
    {
        case PB11:
                  module = 0;
                  route |= (LETIMER_ROUTE_OUT0PEN | LETIMER_ROUTE_LOCATION_LOC1);
                  GPIO_PinModeSet(gpioPortB, 11, gpioModePushPull, 0);
                  break;
        case PB12:
                  module = 1;
                  route |= (LETIMER_ROUTE_OUT1PEN | LETIMER_ROUTE_LOCATION_LOC1);
                  GPIO_PinModeSet(gpioPortB, 12, gpioModePushPull, 0);
                  break;
        case PC4:
                  module = 0;
                  route |= (LETIMER_ROUTE_OUT0PEN | LETIMER_ROUTE_LOCATION_LOC3);
                  GPIO_PinModeSet(gpioPortC, 4, gpioModePushPull, 0);
                  break;
        case PC5:
                  module = 1;
                  route |= (LETIMER_ROUTE_OUT1PEN | LETIMER_ROUTE_LOCATION_LOC3);
                  GPIO_PinModeSet(gpioPortC, 5, gpioModePushPull, 0);
                  break;
        case PD6:
                  module = 0;
                  route |= (LETIMER_ROUTE_OUT0PEN | LETIMER_ROUTE_LOCATION_LOC0);
                  GPIO_PinModeSet(gpioPortD, 6, gpioModePushPull, 0);
                  break;
        case PD7:
                  module = 1;
                  route |= (LETIMER_ROUTE_OUT1PEN | LETIMER_ROUTE_LOCATION_LOC0);
                  GPIO_PinModeSet(gpioPortD, 7, gpioModePushPull, 0);
                  break;
        case PF0:
                  module = 0;
                  route |= (LETIMER_ROUTE_OUT0PEN | LETIMER_ROUTE_LOCATION_LOC2);
                  GPIO_PinModeSet(gpioPortF, 0, gpioModePushPull, 0);
                  break;
        case PF1:
                  module = 1;
                  route |= (LETIMER_ROUTE_OUT1PEN | LETIMER_ROUTE_LOCATION_LOC2);
                  GPIO_PinModeSet(gpioPortF, 1, gpioModePushPull, 0);
                  break;
         default:
                  /* invalid pin */
                  return;
    }

    /*  Pin and period is valid. Set register to new route. */
    LETIMER0->ROUTE = route;

    /*  Set initial compare values for COMP0.
        The LETIMER module counts down from these values and toggles the pin
        when the counter underflows.
    */
    LETIMER_CompareSet(LETIMER0, 0, timerTicks);
    LETIMER_CompareSet(LETIMER0, 1, timerTicks);

    /* LETIMERn_REPx must be non-zero for pin to be toggled */
    LETIMER_RepeatSet(LETIMER0, module, 1);

    /* Initialize and start LETIMER */
    LETIMER_Init(LETIMER0, &letimerInit);
}

