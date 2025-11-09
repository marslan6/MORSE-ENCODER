//
// ITM_IO.c
//
// ITM-based printf retargeting for ARM Cortex-M4.
// Routes stdout/stderr to ITM stimulus port 0 over SWO.
// Supports GCC, ArmCC, and IAR.
//

#include <sys/unistd.h>
#include "XMC4500.h"      // ITM and CoreDebug (CMSIS device header)

// ---------------- ITM Initialization ----------------
// Enables tracing, configures ITM, and sets up SWO.
// SWO baud ≈ 2 MHz assuming 120 MHz system clock.
//
void ITMInit(void)
{
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;    // Enable trace
  ITM->TER = 1UL;                                    // Enable stimulus port 0
  ITM->TCR = (1UL << ITM_TCR_ITMENA_Pos) |
             (1UL << ITM_TCR_SYNCENA_Pos) |
             (1UL << ITM_TCR_DWTENA_Pos) |
             (1UL << ITM_TCR_TraceBusID_Pos);        // ITM control setup
  TPI->ACPR = 59;                                    // 120 MHz / (59+1) = 2 MHz
  TPI->SPPR = 2;                                     // NRZ SWO
  TPI->FFCR = 0x00;                                  // Raw ITM packets
}

// ---------------- GCC / Newlib ---------------- 
#if defined(__GNUC__)
int _write(int fd, const char *ptr, int len)
{
  if (fd != STDOUT_FILENO && fd != STDERR_FILENO) 
  {
    return -1;
  }

  for (int i = 0; i < len; ++i)
  {
    char c = ptr[i];
    if (c == '\n') 
    {
      ITM_SendChar('\r');
    } 

    ITM_SendChar((uint32_t)c);
  }
  return len;
}
#endif

// ---------------- ArmCC / Keil ---------------- 
#if defined(__CC_ARM)
#include <rt_misc.h>
#pragma import(__use_no_semihosting_swi)

struct __FILE { int handle; };
FILE __stdout;
FILE __stdin;

int fputc(int c, FILE *f)
{
  if (c == '\n') ITM_SendChar('\r');
  ITM_SendChar((uint32_t)c);
  return c;
}

int ferror(FILE *f) { return EOF; }

void _ttywrch(int c) { ITM_SendChar((uint32_t)c); }

void _sys_exit(int return_code) { while(1); }
#endif

// ---------------- IAR ---------------- 
#if defined(__ICCARM__)
size_t __write(int Handle, const unsigned char *Buf, size_t Bufsize)
{
  if (Handle != STDOUT_FILENO && Handle != STDERR_FILENO) 
  {
    return 0;
  }

  for (size_t i = 0; i < Bufsize; ++i)
  {
    char c = Buf[i];
    if (c == '\n') 
    {
      ITM_SendChar('\r');
    }

    ITM_SendChar((uint32_t)c);
  }
  return Bufsize;
}
#endif
