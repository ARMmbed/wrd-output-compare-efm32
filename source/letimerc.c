#import "letimer/LeTimerC.h"

#import "emlib/em_letimer.h"
#import "emlib/em_cmu.h"
#import "emlib/em_gpio.h"


#include <stdio.h>

#define LETIMER_ROUTE_OUT_MASK 0x03UL
#define YT_Platform_Time_Base 32768

enum PinName{
    Pin_Not_Connected = 0,
    Pin_NC = 0,

    Pin_PA0,
    Pin_PA1,
    Pin_PA2,
    Pin_PA3,  // U0_TX #2
    Pin_PA4,  // U0_RX #2
    Pin_PA5,  // LEU1_TX #1
    Pin_PA6,  // LEU1_RX #1
    
    Pin_PB0,
    Pin_PB1,
    Pin_PB2,
    Pin_PB3,  // US2_TX #1
    Pin_PB4,  // US2_RX #1
    Pin_PB5,  // US2_Clk #1
    Pin_PB6,  // US2_Cs #1
    Pin_PB7,  // US0_TX #4, US1_Clk #0
    Pin_PB8,  // US0_RX #4, US1_Cs #0
    Pin_PB9,  // U1_TX #2
    Pin_PB10, // U1_RX #2
    Pin_PB11,
    Pin_PB12,
    Pin_PB13, // US0_Clk #4/5, LEU0_TX #1
    Pin_PB14, // US0_Cs #4/5, LEU0_RX #1

    Pin_PC0,  // US0_TX #5, US1_TX #0
    Pin_PC1,  // US0_RX #5, US1_RX #0
    Pin_PC2,  // US2_TX #0
    Pin_PC3,  // US2_RX #0
    Pin_PC4,  // US2_Clk #0
    Pin_PC5,  // US2_Cs #0
    Pin_PC6,  // LEU1_TX #0
    Pin_PC7,  // LEU1_RX #0
    Pin_PC8,  // US0_Cs #2
    Pin_PC9,  // US0_Clk #2
    Pin_PC10, // US0_RX #2
    Pin_PC11, // US0_TX #2

    Pin_PD0,  // US1_TX #1
    Pin_PD1,  // US1_RX #1
    Pin_PD2,  // US1_Clk #1
    Pin_PD3,  // US1_Cs #1
    Pin_PD4,  // LEU0_TX #0
    Pin_PD5,  // LEU0_RX #0
    Pin_PD6,  // US1_RX #2
    Pin_PD7,  // US1_TX #2
    Pin_PD8, 

    Pin_PE0,  // U0_TX #1
    Pin_PE1,  // U0_RX #1
    Pin_PE2,  // U1_T1 #3
    Pin_PE3,  // U1_RX #3
    Pin_PE4,  // US0_Cs #1
    Pin_PE5,  // US0_Clk #1
    Pin_PE6,  // US0_RX #1
    Pin_PE7,  // US0_TX #1
    Pin_PE8,
    Pin_PE9,
    Pin_PE10, // US0_TX #0
    Pin_PE11, // US0_RX #0
    Pin_PE12, // US0_RX #3, US0_Clk #0
    Pin_PE13, // US0_TX #3, US0_Cs #0
    Pin_PE14, // LEU0_TX #2
    Pin_PE15, // LEU0_RX #2

    Pin_PF0,  // US1_Clk #2, LEU0_TX #3
    Pin_PF1,  // US1_Cs #2, LEU0_RX #3 
    Pin_PF2,  // LEU0_TX #4
    Pin_PF3,
    Pin_PF4,
    Pin_PF5,
    Pin_PF6,  // U0_TX #0 
    Pin_PF7,  // U0_RX #0
    Pin_PF8,
    Pin_PF9,
    Pin_PF10, // U1_TX #1
    Pin_PF11,  // U1_RX #1 
    Pin_PF12,

    Pin_Max,
};


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


YTError LeTimer(int pin, uint32_t period)
{
  /*  Convert milliseconds to timer ticks. 

      NOTE: Return call if period is invalid.
  */
  uint32_t timerTicks = period * YT_Platform_Time_Base / 1000;

  if (timerTicks > 0xFFFFUL)
  {
    return ytError(YTBadRequest);
  }

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
    case Pin_PB11:
                  module = 0;
                  route |= (LETIMER_ROUTE_OUT0PEN | LETIMER_ROUTE_LOCATION_LOC1);
                  GPIO_PinModeSet(gpioPortB, 11, gpioModePushPull, 0);
                  break;
    case Pin_PB12:
                  module = 1;
                  route |= (LETIMER_ROUTE_OUT1PEN | LETIMER_ROUTE_LOCATION_LOC1);
                  GPIO_PinModeSet(gpioPortB, 12, gpioModePushPull, 0);
                  break;
    case Pin_PC4:
                  module = 0;
                  route |= (LETIMER_ROUTE_OUT0PEN | LETIMER_ROUTE_LOCATION_LOC3);
                  GPIO_PinModeSet(gpioPortC, 4, gpioModePushPull, 0);
                  break;
    case Pin_PC5:
                  module = 1;
                  route |= (LETIMER_ROUTE_OUT1PEN | LETIMER_ROUTE_LOCATION_LOC3);
                  GPIO_PinModeSet(gpioPortC, 5, gpioModePushPull, 0);
                  break;
    case Pin_PD6:
                  module = 0;
                  route |= (LETIMER_ROUTE_OUT0PEN | LETIMER_ROUTE_LOCATION_LOC0);
                  GPIO_PinModeSet(gpioPortD, 6, gpioModePushPull, 0);
                  break;
    case Pin_PD7:
                  module = 1;
                  route |= (LETIMER_ROUTE_OUT1PEN | LETIMER_ROUTE_LOCATION_LOC0);
                  GPIO_PinModeSet(gpioPortD, 7, gpioModePushPull, 0);
                  break;
    case Pin_PF0:
                  module = 0;
                  route |= (LETIMER_ROUTE_OUT0PEN | LETIMER_ROUTE_LOCATION_LOC2);
                  GPIO_PinModeSet(gpioPortF, 0, gpioModePushPull, 0);
                  break;
    case Pin_PF1:
                  module = 1;
                  route |= (LETIMER_ROUTE_OUT1PEN | LETIMER_ROUTE_LOCATION_LOC2);
                  GPIO_PinModeSet(gpioPortF, 1, gpioModePushPull, 0);
                  break;
         default:
                  /* invalid pin */
                  return ytError(YTBadRequest);
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

  return ytError(YTNoError);
}

