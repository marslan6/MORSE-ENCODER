#include <xmc_common.h>

void initCCU4(void);
void connectLED(void);

int main(void) {

  initCCU4();

  while(1);
  return 0;
}

void initCCU4() {
  /* Release CCU4 instance 0 from reset (manual 22.6.1 step 2) */

  /* Enable clock to CCUs (manual 22.6.1 step 3) */

  /* Enable prescaler in CCU4 instance 0 (manual 22.6.1 step 4) */

  /* Configure period and compare in CCU4 instance 0 slice 2 (manual 22.6.1 step 6) */

  /* Request shadow transfer for CCU4 instance 0 slice 2 (manual 22.6.1 step 6) */

  /* Connect LED1 to CCU40.OUT2 */
  connectLED();
  /* Enable timer slice 2 in CCU4 instance 0 (manual 22.6.1 step 8) */

  /* Start slice 2 in CCU4 instance 0 by setting run bit (manual 22.6.1 step 9) */

}

void connectLED() {
  /* Bit mask for alternate function 3 with push-pull output */
  static const uint8_t PP_ALT3 = 0b10011;
  PORT1->IOCR0 = (PORT1->IOCR0 & ~PORT1_IOCR0_PC1_Msk) | (PP_ALT3 << PORT1_IOCR0_PC1_Pos);
}

