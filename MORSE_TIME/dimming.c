#include <xmc_common.h>

void initCCU4(void);
void connectLED(void);

int main(void) {
  initCCU4();

  while(1);
  return 0;
}

void initCCU4(void) {
  /* Release CCU4 instance 0 from reset*/
  SCU_RESET->PRCLR0 = SCU_RESET_PRCLR0_CCU40RS_Msk;
  /* Enable clock to CCUs*/
  SCU_CLK->CLKSET = SCU_CLK_CLKSET_CCUCEN_Msk;
  /* Enable prescaler in CCU4 instance 0*/
  CCU40->GIDLC = CCU4_GIDLC_SPRB_Msk;
  /* Configure period and compare in CCU4 instance 0 slice 2*/
  CCU40_CC42->PRS = 0xFFFF;
  CCU40_CC42->CRS = (1 - 0.99) * 0xFFFF;
  /* Request shadow transfer for CCU4 instance 0 slice 2*/
  CCU40->GCSS = CCU4_GCSS_S2SE_Msk;
  /* Connect LED1 to CCU40.OUT2 */
  connectLED();
  /* Enable timer slice 2 in CCU4 instance 0*/
  CCU40->GIDLC = CCU4_GIDLC_CS2I_Msk;
  /* Start slice 2 in CCU4 instance 0 by setting run bit*/
  CCU40_CC42->TCSET = CCU4_CC4_TCSET_TRBS_Msk;
}

void connectLED(void) {
  /* Bit mask for alternate function 3 with push-pull output */
  static const uint8_t PP_ALT3 = 0b10011;
  PORT1->IOCR0 = (PORT1->IOCR0 & ~PORT1_IOCR0_PC1_Msk) | (PP_ALT3 << PORT1_IOCR0_PC1_Pos);
}
