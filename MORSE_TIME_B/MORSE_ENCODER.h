#ifndef MORSE_ENCODER
#define MORSE_ENCODER

// A dash is equal to three dots.
// The wait time between the signals forming the same letter is equal to one dot.
// The wait time between two letters is equal to three dots.
// The wait time between two words is equal to seven dots.

#define DEBOUNCE 20                 // 20ms
#define DOT 100                     // 100ms
#define DASH (3 * DOT)              // 300ms
#define INTRA_SYMBOL_GAP (1 * DOT)  // 100ms
#define LETTER_GAP (3 * DOT)        // 300ms
#define WORD_GAP (7 * DOT)          // 700ms
#define SENTENCE_GAP (50 * DOT)     // 5000ms

#include <xmc_common.h>
#include <xmc_gpio.h>
#include <stdio.h>
#include "MORSE_ALPHABET.h"

uint32_t GetTime();
void Delay_ms (uint32_t sleep_duration_ms);
void SendLetterFromMorseWord (const char* letter);
void ConvertWordToMorseWord(const char* word); 

#endif // MORSE_ENCODER