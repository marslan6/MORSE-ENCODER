#include <xmc_common.h>
#include <stdio.h>

//////////////////////// PROJECT INCLUDES ////////////////////////
#include "MORSE_ALPHABET.h"
#include "MORSE_ENCODER.h"

//////////////////////// TYPEDEF ////////////////////////
typedef struct
{
  XMC_GPIO_PORT_t *const port; 
  const uint8_t pin;
} t_BUTTON;

//////////////////////// VARIABLES ////////////////////////
static t_BUTTON button_1 = {.port = XMC_GPIO_PORT1, .pin = 14};
static t_BUTTON button_2 = {.port = XMC_GPIO_PORT1, .pin = 15};
static uint32_t button_1_last_press_time = 0;
static uint32_t button_1_second_last_press_time= 0;

//////////////////////// FUNCTIONS ////////////////////////
static bool SysTickConfigMilliseconds(void); 
static void ConnectLED(XMC_GPIO_PORT_t *const port, const uint8_t pin, const XMC_GPIO_CONFIG_t* led_config);
static void ConnectButton(t_BUTTON button);
static bool IsButtonPressed(t_BUTTON button);

//////////////////////// CONFIG VARIABLES ////////////////////////
const XMC_GPIO_CONFIG_t LED_config = {
                                        .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
                                        .output_level = XMC_GPIO_OUTPUT_LEVEL_LOW,
                                        .output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE
                                    };
const XMC_GPIO_CONFIG_t BUTTON_config = {
                                        .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
                                        .output_level = XMC_GPIO_OUTPUT_LEVEL_LOW,
                                        .output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE
                                    };
//////////////////////////////////////////////////////////////////

int main(void) 
{
  const char* word = "I CAN MORSE";
  ConnectLED(XMC_GPIO_PORT1, 1, &LED_config);
  ConnectButton(button_1);
  ConnectButton(button_2);
  ITMInit();

  if (SysTickConfigMilliseconds() == false) 
    return 0;
  
  while (1)
  { 
    if (IsButtonPressed(button_1))
    {
      ConvertWordToMorseWord(word);
    }
    else if (IsButtonPressed(button_2))
    {
      char text[11]; // Maximum 10 Digits + '\0'
      uint32_t number = button_1_last_press_time - button_1_second_last_press_time; // Always difference will be sent
      sprintf(text, "%lu", number); // Copy into char array
      //printf("Time Diff: %lu\n", number);
      ConvertWordToMorseWord(text);
    }
  }

  return 0;
}

static bool inline SysTickConfigMilliseconds(void) 
{
  // Set the SysTick reload value for 1 millisecond per tick
  uint32_t returnCode = SysTick_Config(SystemCoreClock / 1000); 
  // returnCode 0 is a success
  return (returnCode == 0);
}

static void ConnectLED(XMC_GPIO_PORT_t *const port, const uint8_t pin, const XMC_GPIO_CONFIG_t* led_config)
{
  XMC_GPIO_Init(port, pin, led_config);
}

static void ConnectButton(t_BUTTON button)
{
  XMC_GPIO_SetMode (button.port, button.pin, XMC_GPIO_MODE_INPUT_TRISTATE);
  XMC_GPIO_EnableDigitalInput(button.port, button.pin);
}

static bool IsButtonPressed(t_BUTTON button)
{
  bool first_button_check = false;
  bool second_button_check = false;

  uint32_t current_time = GetTime();
  first_button_check = (XMC_GPIO_GetInput(button.port, button.pin) == 0u);
  Delay_ms(DEBOUNCE);
  second_button_check = (XMC_GPIO_GetInput(button.port, button.pin) == 0u);

  if ((button.pin == button_1.pin) && first_button_check && second_button_check)
  {
    button_1_second_last_press_time = button_1_last_press_time;
    button_1_last_press_time = current_time;
  }

  return (first_button_check && second_button_check);
}