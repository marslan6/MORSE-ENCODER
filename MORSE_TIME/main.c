#include <xmc_common.h>
#include <stdio.h>

#include "MORSE_ALPHABET.h"
#include "MORSE_ENCODER.h"

static bool SysTickConfigMilliseconds(void); 
static void ConnectLED(const XMC_GPIO_CONFIG_t* led_config);
const XMC_GPIO_CONFIG_t LED_config = {
                                        .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
                                        .output_level = XMC_GPIO_OUTPUT_LEVEL_LOW,
                                        .output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE
                                    };

int main(void) 
{
  const char* word = "I CAN MORSE";
  ConnectLED(&LED_config);
  ITMInit();

  if (SysTickConfigMilliseconds() == false) 
    return 0;

  while(1)
    ConvertWordToMorseWord(word);

  return 0;
}

static bool inline SysTickConfigMilliseconds(void) 
{
  // Set the SysTick reload value for 1 millisecond per tick
  uint32_t returnCode = SysTick_Config(SystemCoreClock / 1000); 
  // returnCode 0 is a success
  return (returnCode == 0);
}

void ConnectLED(const XMC_GPIO_CONFIG_t* led_config)
{
  XMC_GPIO_Init(XMC_GPIO_PORT1, 1, led_config);
}