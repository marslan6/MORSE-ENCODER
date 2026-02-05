# MORSE CODE ENCODER

![ISA](https://img.shields.io/badge/ISA-ARM_Cortex--M4-blue)
![Language](https://img.shields.io/badge/Language-C-green)
![Platform](https://img.shields.io/badge/Platform-XMC4500-orange)
![Toolchain](https://img.shields.io/badge/Toolchain-ARM_GCC-yellow)

A real-time Morse code encoder implementation for the XMC4500 ARM Cortex-M4 microcontroller. This project converts ASCII text strings into visual Morse code signals using precise timing control via the SysTick timer and GPIO-driven LED output.

The system encodes alphanumeric characters (A-Z, 0-9) into their corresponding Morse code representations, outputting dot and dash sequences through an LED with standard Morse timing conventions.

---

## Table of Contents

- [Overview](#overview)
- [Morse Code Theory](#morse-code-theory)
- [System Architecture](#system-architecture)
- [Timing Diagrams](#timing-diagrams)
- [Hardware Requirements](#hardware-requirements)
- [Software Dependencies](#software-dependencies)
- [Project Structure](#project-structure)
- [Build & Flash](#build--flash)
- [Debugging](#debugging)
- [Configuration](#configuration)
- [Technical Details](#technical-details)

---

## Overview

### What It Does

1. **Initializes SysTick timer** for millisecond-precision timing
2. **Parses input text strings** character by character
3. **Converts each character** to its Morse code equivalent (dots and dashes)
4. **Controls LED output** with precise on/off timing
5. **Maintains standard Morse timing** for symbols, letters, words, and sentences
6. **Outputs debug information** via ITM/SWO for monitoring

### Key Features

- Real-time Morse code generation
- Configurable timing parameters (dot duration, gaps)
- Support for uppercase letters (A-Z) and digits (0-9)
- ITM-based printf debugging over SWO
- Clean modular architecture with separate encoder and alphabet modules

---

## Morse Code Theory

### Standard Timing Conventions

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         MORSE CODE TIMING RULES                             │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   Symbol Duration:                                                          │
│                                                                             │
│   DOT (.)    ████░░░░░░░░░░░░░░░░░░░░░░  (1 unit  = 100ms)                  │
│   DASH (-)   ████████████░░░░░░░░░░░░░░  (3 units = 300ms)                  │
│                                                                             │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   Gap Duration:                                                             │
│                                                                             │
│   Intra-symbol gap:  ░░░░  (1 unit  = 100ms)  Between dots/dashes           │
│   Letter gap:        ░░░░░░░░░░░░  (3 units = 300ms)  Between letters       │
│   Word gap:          ░░░░░░░░░░░░░░░░░░░░░░░░░░░░  (7 units = 700ms)        │
│   Sentence gap:      ░░░░░░░░░░░░░░░░░░░░░░░░...  (50 units = 5000ms)       │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Character Encoding Examples

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         MORSE ALPHABET EXAMPLES                             │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   Letter    Morse       LED Pattern (█ = ON, ░ = OFF)                       │
│   ───────   ──────      ─────────────────────────────────────               │
│                                                                             │
│   A         .-          █░███░░░                                            │
│   B         -...        ███░█░█░█░░░                                        │
│   C         -.-.        ███░█░███░█░░░                                      │
│   S         ...         █░█░█░░░                                            │
│   O         ---         ███░███░███░░░                                      │
│                                                                             │
│   1         .----       █░███░███░███░███░░░                                │
│   2         ..---       █░█░███░███░███░░░                                  │
│   0         -----       ███░███░███░███░███░░░                              │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Supported Character Set

| Range | Characters | Count |
|-------|------------|-------|
| Letters | A B C D E F G H I J K L M N O P Q R S T U V W X Y Z | 26 |
| Digits | 0 1 2 3 4 5 6 7 8 9 | 10 |
| Space | (word separator) | 1 |
| **Total** | | **37** |

---

## System Architecture

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           SYSTEM ARCHITECTURE                               │
└─────────────────────────────────────────────────────────────────────────────┘

┌──────────────────────────────────────────────────────────────────────────────┐
│                              XMC4500 MCU                                     │
│                                                                              │
│  ┌────────────────┐     ┌────────────────┐     ┌────────────────────────┐   │
│  │    main.c      │     │ MORSE_ENCODER  │     │   MORSE_ALPHABET.h     │   │
│  │                │     │                │     │                        │   │
│  │ • System Init  │────►│ • Symbol Send  │────►│ • A-Z Lookup Table     │   │
│  │ • SysTick Cfg  │     │ • Word Convert │     │ • 0-9 Lookup Table     │   │
│  │ • Main Loop    │     │ • Timing Delay │     │ • ASCII Index Mapping  │   │
│  └────────────────┘     └───────┬────────┘     └────────────────────────┘   │
│                                 │                                            │
│                                 ▼                                            │
│  ┌────────────────┐     ┌────────────────┐     ┌────────────────────────┐   │
│  │   SysTick      │     │    GPIO        │     │      ITM_IO.c          │   │
│  │   Handler      │     │   Driver       │     │                        │   │
│  │                │     │                │     │ • Printf Retarget      │   │
│  │ • 1ms Tick     │     │ • LED Control  │     │ • SWO Output           │   │
│  │ • timer_ms++   │     │ • P1.1 Output  │     │ • Debug Trace          │   │
│  └────────────────┘     └───────┬────────┘     └────────────────────────┘   │
│                                 │                                            │
└─────────────────────────────────┼────────────────────────────────────────────┘
                                  │
                                  ▼
                         ┌────────────────┐
                         │   LED (P1.1)   │
                         │                │
                         │  ● ON  = HIGH  │
                         │  ○ OFF = LOW   │
                         └────────────────┘
```

---

## Timing Diagrams

### Word Encoding: "I CAN MORSE"

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    WORD ENCODING TIMING DIAGRAM                             │
└─────────────────────────────────────────────────────────────────────────────┘

Input: "I CAN MORSE"

I (..)
─────────────────────────────────────────────────────────────────────────────►
    │                                                                    time
    ▼
LED: █░█░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░...
     ├─┤ ├─┤
     1u  1u
      dot  dot
            ├────────────────────┤
                  WORD GAP (7u = 700ms)

C (-.-.)
LED: ░░░░░░░░░░░░░░░░░░░░░███░█░███░█░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░...
                          ├───┤ ├─┤ ├───┤ ├─┤
                           3u  1u  3u  1u
                          dash dot dash dot
                                          ├────────┤
                                          LETTER GAP (3u)

A (.-)
LED: ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░█░███░░░░░░░░░░░░░░░░░░░░░░░░...
                                         ├─┤ ├───┤
                                         1u   3u
                                        dot  dash
                                               ├────────┤
                                               LETTER GAP (3u)

... continues for remaining letters

Legend:
  █ = LED ON (HIGH)
  ░ = LED OFF (LOW)
  u = unit (100ms base timing)
```

### Single Character Encoding: "C" (-.-.)

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                   CHARACTER "C" TIMING BREAKDOWN                            │
└─────────────────────────────────────────────────────────────────────────────┘

    ┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐
    │  0  │ 100 │ 200 │ 300 │ 400 │ 500 │ 600 │ 700 │ 800 │ 900 │1000 │1100 │ ms
    └─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘
         
LED: ████████████░░░░█████░░░░████████████░░░░█████░░░░░░░░░░░░░░░░░░░...
     ├──────────┤    ├───┤    ├──────────┤    ├───┤    ├──────────┤
         DASH     GAP  DOT  GAP    DASH    GAP  DOT     LETTER GAP
        (300ms) (100ms)(100ms)(100ms)(300ms)(100ms)(100ms)  (300ms)

Total duration: 300 + 100 + 100 + 100 + 300 + 100 + 100 + 300 = 1400ms
```

---

## Hardware Requirements

### Microcontroller

| Component | Specification |
|-----------|---------------|
| **MCU** | Infineon XMC4500 |
| **Core** | ARM Cortex-M4 (32-bit) |
| **Clock** | 120 MHz |
| **Flash** | 1024 KB |
| **RAM** | 160 KB |
| **Debug** | SWD via J-Link |

### Pin Configuration

```
XMC4500 Microcontroller
        ┌────────────────────┐
        │                    │
   P1.1 │ ●──────► LED1      │  (Morse Code Output)
        │                    │
   SWD  │ ◄═════► J-Link     │  (Debug/Programming)
        │                    │
   SWO  │ ──────► ITM Trace  │  (Printf Debug Output)
        │                    │
        └────────────────────┘
```

### Required Equipment

- XMC4500 development board (or compatible)
- Segger J-Link debugger
- LED connected to GPIO P1.1 (or onboard LED)
- USB cable for J-Link connection

---

## Software Dependencies

### Toolchain

| Tool | Version | Purpose |
|------|---------|---------|
| arm-none-eabi-gcc | 7.0+ | ARM cross-compiler |
| arm-none-eabi-gdb | 7.0+ | Debugger |
| gdb-multiarch | 7.0+ | Alternative debugger |
| make | 4.0+ | Build system |
| JLinkExe | 6.0+ | Flash programming |
| JLinkGDBServer | 6.0+ | GDB debug server |
| JLinkSWOViewerExe | 6.0+ | ITM/SWO trace viewer |

### Libraries

| Library | Version | Purpose |
|---------|---------|---------|
| XMC Peripheral Library | 2.1.16 | Low-level GPIO drivers |
| CMSIS | 4.5+ | ARM Cortex interface |
| Newlib | System | Standard C library |

### Installation (Ubuntu/Debian)

```bash
# Install ARM toolchain
sudo apt-get install gcc-arm-none-eabi gdb-arm-none-eabi gdb-multiarch

# Install J-Link tools (download from Segger website)
# https://www.segger.com/downloads/jlink/

# Set XMC library path
export XMC_LIBDIR=/opt/XMClib/XMC_Peripheral_Library_v2.1.16
```

---

## Project Structure

```
MORSE/
├── readme.md                 # This documentation
├── MORSE_TIME/               # Main implementation
│   ├── main.c                # Entry point, system initialization
│   ├── MORSE_ENCODER.c       # Morse encoding logic, timing control
│   ├── MORSE_ENCODER.h       # Encoder function declarations, timing constants
│   ├── MORSE_ALPHABET.h      # ASCII to Morse lookup table
│   ├── ITM_IO.c              # Printf retargeting via ITM/SWO
│   ├── XMC4500_MACROS_HELP.h # Hardware macro helpers
│   ├── Makefile              # Build configuration
│   ├── student.mk            # Source file configuration
│   └── GDBStudentCommands    # GDB automation commands
├── MORSE_TIME_B/             # Alternative implementation
│   └── ...                   # Same structure as MORSE_TIME
└── .vscode/                  # VS Code configuration
    ├── settings.json         # Project settings
    ├── tasks.json            # Build tasks
    ├── launch.json           # Debug configuration
    └── c_cpp_properties.json # IntelliSense configuration
```

### Source File Descriptions

| File | Lines | Purpose |
|------|-------|---------|
| `main.c` | ~45 | System initialization, SysTick config, main loop |
| `MORSE_ENCODER.c` | ~95 | Core encoding logic, GPIO control, timing delays |
| `MORSE_ENCODER.h` | ~25 | Timing constants, function declarations |
| `MORSE_ALPHABET.h` | ~45 | ASCII-indexed Morse code lookup table |
| `ITM_IO.c` | ~90 | Printf retargeting for GCC, ArmCC, IAR |

---

## Build & Flash

### Build Commands

```bash
# Navigate to project directory
cd MORSE_TIME

# Clean previous build
make clean

# Compile the project
make

# Flash to device via J-Link
make program

# Debug with GDB
make debug
```

### Build Output

```
arm-none-eabi-gcc -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mthumb ...
Building: main.c
Building: MORSE_ENCODER.c
Building: ITM_IO.c
Linking: build/main.elf
Creating: build/main.hex

   text    data     bss     dec     hex filename
   8192     512    1024    9728    2600 build/main.elf
```

### Flash Process

```
$ make program
Connecting to J-Link...
Erasing flash...
Programming build/main.hex...
Verifying...
Done.
```

---

## Debugging

### VS Code Configuration

#### settings.json

```json
{
    "PROJECT.NAME": "MORSE_TIME"
}
```

#### launch.json

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "J-Link (XMC4500)",
            "type": "cortex-debug",
            "request": "launch",
            "servertype": "jlink",
            "device": "XMC4500",
            "interface": "swd",
            "runToEntryPoint": "main",
            "cwd": "${workspaceFolder}",
            "executable": "${workspaceFolder}/${config:PROJECT.NAME}/build/main.elf",
            "gdbPath": "/usr/bin/gdb-multiarch",
            "serverpath": "JLinkGDBServer"
        }
    ]
}
```

#### c_cpp_properties.json

```json
{
    "configurations": [
        {
            "name": "ARM-GCC",
            "includePath": [
                "${workspaceFolder}/**",
                "/opt/XMClib/XMC_Peripheral_Library_v2.1.16/XMCLib/inc",
                "/opt/XMClib/XMC_Peripheral_Library_v2.1.16/CMSIS/Include",
                "/opt/XMClib/XMC_Peripheral_Library_v2.1.16/CMSIS/Infineon/XMC4500_series/Include"
            ],
            "defines": ["XMC4500_F100x1024"],
            "compilerPath": "/usr/bin/arm-none-eabi-gcc",
            "cStandard": "gnu99",
            "intelliSenseMode": "linux-gcc-arm"
        }
    ],
    "version": 4
}
```

### ITM/SWO Debug Output

The project uses **ITM (Instrumentation Trace Macrocell)** to route `printf` output over SWD/SWO—no extra UART wiring required.

#### Quick Start

1. Flash your board:
   ```bash
   make program
   ```

2. Open JLinkSWOViewer:
   ```bash
   JLinkSWOViewerExe
   ```

3. Configure JLinkSWOViewer:
   - **Device**: XMC4500-1024
   - **Target Interface**: SWD
   - **CPU Clock**: Click MEASURE
   - **SWO Clock**: Click MEASURE

4. Click **OK**, then **Start** to view printf output.

### Starting a Debug Session

1. Open `main.c` in VS Code
2. Press **F5** to start the Cortex-Debug session
3. Use breakpoints, step through code, and inspect variables

---

## Configuration

### student.mk

```makefile
# Project name
LD_NAME = main

# Source files
SRCS = main.c MORSE_ENCODER.c ITM_IO.c

# Library sources
LIBSRCS = xmc_gpio.c xmc4_gpio.c

# XMC library path
XMC_LIBDIR = /opt/XMClib/XMC_Peripheral_Library_v2.1.16

# Compiler flags (debug optimized)
SCFLAGS = -std=gnu99 -Og -fno-omit-frame-pointer -fno-inline
```

### Timing Parameters (MORSE_ENCODER.h)

| Parameter | Value | Description |
|-----------|-------|-------------|
| `DOT` | 100 ms | Base timing unit |
| `DASH` | 300 ms | 3 × DOT duration |
| `INTRA_SYMBOL_GAP` | 100 ms | Gap between dots/dashes |
| `LETTER_GAP` | 300 ms | Gap between letters |
| `WORD_GAP` | 700 ms | Gap between words |
| `SENTENCE_GAP` | 5000 ms | Gap after sentence |

---

## Technical Details

### SysTick Timer Configuration

```c
// Configure SysTick for 1ms interrupts
uint32_t returnCode = SysTick_Config(SystemCoreClock / 1000);
// SystemCoreClock = 120 MHz
// Reload Value = 120,000,000 / 1,000 = 120,000 cycles per tick
```

### Delay Implementation

```c
static volatile uint32_t timer_ms = 0;

void SysTick_Handler(void) {
    timer_ms++;
}

static void Delay_ms(uint32_t sleep_duration_ms) {
    uint32_t now = timer_ms;
    while ((timer_ms - now) < sleep_duration_ms);
}
```

### GPIO LED Control

```c
// LED ON (transmitting dot/dash)
XMC_GPIO_SetOutputHigh(XMC_GPIO_PORT1, 1);

// LED OFF (gap/pause)
XMC_GPIO_SetOutputLow(XMC_GPIO_PORT1, 1);
```

### Morse Alphabet Lookup

```c
// ASCII-indexed lookup table for O(1) access
static const char* MORSE_ALPHABET[] = {
    [' '] = " ",
    ['A'] = ".-",
    ['B'] = "-...",
    ['C'] = "-.-.",
    // ... etc
    ['0'] = "-----",
    ['1'] = ".----",
    // ... etc
};

// Usage
const char* morse = MORSE_ALPHABET['C'];  // Returns "-.-."
```

### ITM Configuration

```c
void ITMInit(void) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;  // Enable trace
    ITM->TER = 1UL;                                   // Enable stimulus port 0
    ITM->TCR = (1UL << ITM_TCR_ITMENA_Pos) |
               (1UL << ITM_TCR_SYNCENA_Pos) |
               (1UL << ITM_TCR_DWTENA_Pos);
    TPI->ACPR = 59;                                   // 120 MHz / 60 = 2 MHz SWO
    TPI->SPPR = 2;                                    // NRZ encoding
}
```

---

## Memory Map

```
┌─────────────────────────────────────────┐  0x1000_0000
│                                         │
│              FLASH (1 MB)               │
│                                         │
│   .text   (code)                        │
│   .rodata (Morse alphabet table)        │
│                                         │
├─────────────────────────────────────────┤  0x1FFF_0000
│                                         │
│              SRAM (160 KB)              │
│                                         │
│   .data   (initialized globals)         │
│   .bss    (timer_ms, etc.)              │
│                                         │
│   Stack                                 │
│                                         │
└─────────────────────────────────────────┘  0x2000_0000
```

---

## Troubleshooting

| Issue | Solution |
|-------|----------|
| Debug session won't start | Verify `gdbPath` and `serverpath` in launch.json |
| J-Link not detected | Check USB connection, install J-Link drivers |
| No ITM output | Run `Edit > Configure > Measure > OK` in JLinkSWOViewer |
| Build errors | Ensure `XMC_LIBDIR` path is correct in student.mk |
| LED not blinking | Verify GPIO P1.1 configuration and LED connection |

---

## References

- [XMC4500 Reference Manual](https://www.infineon.com/xmc4500)
- [ARM Cortex-M4 Technical Reference](https://developer.arm.com/documentation/ddi0439)
- [International Morse Code Standard](https://www.itu.int/rec/R-REC-M.1677)
- [Segger J-Link Documentation](https://www.segger.com/products/debug-probes/j-link/)
- [Cortex-Debug VS Code Extension](https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug)
