#include <xmc_common.h>
#include <stdio.h>

#include "MORSE_ALPHABET.h"
#include "MORSE_ENCODER.h"

/*
  const XMC_GPIO_CONFIG_t LED_config = \
        {.mode=XMC_GPIO_MODE_OUTPUT_PUSH_PULL,\
         .output_level=XMC_GPIO_OUTPUT_LEVEL_LOW,\
         .output_strength=XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE};

  XMC_GPIO_Init(XMC_GPIO_PORT1, 0, &LED_config);
*/

static void ITM_Init(void);
void initCCU4(void);
void connectLED(void);

int main(void) 
{
  // Initialize ITM for printf over SWO (before any printf calls)
  ITM_Init();
  
  const char* word = "I CAN MORSE";
  printf("ITM printf ready over SWO!\r\n");

  while(1)
  {
    ConvertWordToMorseWord(word);
  }
  return 0;
}

/* ----------------------------------------------------------------------------
 * ITM initialization for printf over SWO (Serial Wire Output)
 *  - Enables tracing in CoreDebug
 *  - Configures ITM (stimulus port 0, control bits)
 *  - Sets up TPIU for NRZ SWO at ~2 MHz (assumes 120 MHz sysclk)
 * ---------------------------------------------------------------------------*/
static void ITM_Init(void)
{
  /* ---------------- CoreDebug: enable trace ---------------- */
  /* Enable TRCENA (Trace Enable) in Debug Exception and Monitor Control Register */
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

  /* ---------------- ITM stimulus ports --------------------- */
  /* Enable stimulus port 0 (bit 0 in TER) */
  ITM->TER = 1UL;

  /* ---------------- ITM: trace control --------------------- */
  /* ITM enabled, sync enable, DWT event enable, TraceBusID set (nonzero) */
  ITM->TCR = (1UL << ITM_TCR_ITMENA_Pos) |
             (1UL << ITM_TCR_SYNCENA_Pos) |
             (1UL << ITM_TCR_DWTENA_Pos) |
             (1UL << ITM_TCR_TraceBusID_Pos);

  /* ---------------- TPIU: SWO transport -------------------- */
  /* Target SWO baud ≈ 2 MHz: sysclk/(ACPR+1) = 120 MHz / 60 = 2 MHz */
  TPI->ACPR = 59;   /* Asynchronous clock prescaler */
  TPI->SPPR = 2;    /* Selected Pin Protocol: 2 = NRZ (UART-style SWO) */
  TPI->FFCR = 0x00; /* Formatter disabled for raw ITM packets */
}

void initCCU4(void) 
{
  /* Release CCU4 instance 0 from reset*/
  SCU_RESET->PRCLR0 = SCU_RESET_PRCLR0_CCU40RS_Msk;
  /* Enable clock to CCUs*/
  SCU_CLK->CLKSET = SCU_CLK_CLKSET_CCUCEN_Msk;
  /* Enable prescaler in CCU4 instance 0*/
  CCU40->GIDLC = CCU4_GIDLC_SPRB_Msk;
  /* Configure period and compare in CCU4 instance 0 slice 2*/
  CCU40_CC42->PRS = 0xFFFF;
  CCU40_CC42->CRS = (1 - 0.01) * 0xFFFF;
  /* Request shadow transfer for CCU4 instance 0 slice 2*/
  CCU40->GCSS = CCU4_GCSS_S2SE_Msk;
  /* Connect LED1 to CCU40.OUT2 */
  connectLED();
  /* Enable timer slice 2 in CCU4 instance 0*/
  CCU40->GIDLC = CCU4_GIDLC_CS2I_Msk;
  /* Start slice 2 in CCU4 instance 0 by setting run bit*/
  CCU40_CC42->TCSET = CCU4_CC4_TCSET_TRBS_Msk;
}

void connectLED(void) 
{
  /* Bit mask for alternate function 3 with push-pull output */
  static const uint8_t PP_ALT3 = 0b10011;
  PORT1->IOCR0 = (PORT1->IOCR0 & ~PORT1_IOCR0_PC1_Msk) | (PP_ALT3 << PORT1_IOCR0_PC1_Pos);
}
