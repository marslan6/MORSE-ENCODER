/*
 * retarget.c
 *
 * ITM-based printf retargeting for ARM Cortex-M4.
 * Routes stdout/stderr to ITM stimulus port 0 over SWO.
 * View output in JLinkSWOViewer or JLinkRTTViewerExe (SWO tab).
 *
 * ---------------------------------------------------------------------------
 * Layout of this file (toolchain-specific implementations):
 *   1) ArmCC / Keil (__CC_ARM)
 *   2) GCC / Newlib (__GNUC__)
 *   3) IAR (__ICCARM__)
 *
 * Only one section is compiled depending on the active toolchain macro.
 * No function names or logic were changed—only ordering and comments.
 * ---------------------------------------------------------------------------
 */
#include <sys/unistd.h> // for STDOUT_FILENO, STDERR_FILENO
#include "XMC4500.h"    // for ITM and CoreDebug registers (CMSIS device header)

/* ----------------------------------------------------------------------------
 * 1) GCC / Newlib implementation
 *    - Overrides _write to route stdout/stderr to ITM.
 * ---------------------------------------------------------------------------*/
#if defined(__GNUC__)
int _write(int fd, const char *ptr, int len)
{
  /* Only handle stdout/stderr; return error for other file descriptors */
  if (fd != STDOUT_FILENO && fd != STDERR_FILENO) 
  {
    return -1;
  }
  
  /* Stream out each character; translate LF to CRLF for most terminals */
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
#endif /* __GNUC__ */

/* ----------------------------------------------------------------------------
 * 2) ArmCC / Keil (classic) implementation
 *    - Provides fputc redirection and minimal syscall stubs.
 * ---------------------------------------------------------------------------*/
#if defined(__CC_ARM)
#include <rt_misc.h>

#pragma import(__use_no_semihosting_swi)

/* Minimal FILE structure to satisfy ArmCC runtime */
struct __FILE { int handle; };
FILE __stdout;
FILE __stdin;

/* Core putchar redirection: sends characters via ITM (adds \r before \n) */
int fputc(int c, FILE *f) 
{
  if (c == '\n') 
  {
    ITM_SendChar('\r');
  }
  ITM_SendChar((uint32_t)c);
  return c;
}

/* Error hook required by ArmCC runtime (no extra behavior needed here) */
int ferror(FILE *f) 
{
  return EOF;
}

/* Low-level character output used by some runtime routines */
void _ttywrch(int c) 
{
  ITM_SendChar((uint32_t)c);
}

/* Exit stub to avoid semihosting; loops forever */
void _sys_exit(int return_code) 
{
  while(1);
}
#endif /* __CC_ARM */

/* ----------------------------------------------------------------------------
 * 3) IAR implementation
 *    - Overrides __write to route stdout/stderr to ITM.
 * ---------------------------------------------------------------------------*/
#if defined(__ICCARM__)
size_t __write(int Handle, const unsigned char * Buf, size_t Bufsize)
{
  /* Only handle stdout/stderr; ignore other handles */
  if (Handle != STDOUT_FILENO && Handle != STDERR_FILENO) 
  {
    return 0;
  }

  /* Stream out each character; translate LF to CRLF for most terminals */
  for (size_t i = 0; i < Bufsize; ++i) 
  {
    char c = Buf[i];
    if (c == '\n') {
      ITM_SendChar('\r');
    }
    ITM_SendChar((uint32_t)c);
  }
  return Bufsize;
}
#endif /* __ICCARM__ */

/* End of file */
