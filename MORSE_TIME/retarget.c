/*
 * retarget.c
 *
 * ITM-based printf retargeting for ARM Cortex-M4.
 * Routes stdout/stderr to ITM stimulus port 0 over SWO.
 * View output in JLinkSWOViewer or JLinkRTTViewerExe (SWO tab).
 */

#include <sys/unistd.h> // for STDOUT_FILENO, STDERR_FILENO
#include "XMC4500.h"    // for ITM and CoreDebug registers

// Use the CMSIS ITM_SendChar (already defined in core_cm4.h)
// No need to redefine it here.
#if defined(__GNUC__)
int _write(int fd, const char *ptr, int len)
{
  // Only handle stdout/stderr
  if (fd != STDOUT_FILENO && fd != STDERR_FILENO) 
  {
    return -1;
  }
  
  for (int i = 0; i < len; ++i) 
  {
    char c = ptr[i];
    // Translate \n to \r\n for terminal compatibility
    if (c == '\n') 
    {
      ITM_SendChar('\r');
    }
    ITM_SendChar((uint32_t)c);
  }

  return len;
}
#endif

#if defined(__ICCARM__)
size_t __write(int Handle, const unsigned char * Buf, size_t Bufsize)
{
  if (Handle != STDOUT_FILENO && Handle != STDERR_FILENO) {
    return 0;
  }
  for (size_t i = 0; i < Bufsize; ++i) {
    char c = Buf[i];
    if (c == '\n') {
      ITM_SendChar('\r');
    }
    ITM_SendChar((uint32_t)c);
  }
  return Bufsize;
}
#endif

#if defined(__CC_ARM)
#include <rt_misc.h>

#pragma import(__use_no_semihosting_swi)

struct __FILE { int handle; };
FILE __stdout;
FILE __stdin;

int fputc(int c, FILE *f) {
  if (c == '\n') {
    ITM_SendChar('\r');
  }
  ITM_SendChar((uint32_t)c);
  return c;
}

int ferror(FILE *f) {
  return EOF;
}

void _ttywrch(int c) {
  ITM_SendChar((uint32_t)c);
}

void _sys_exit(int return_code) {
  while(1);
}
#endif
