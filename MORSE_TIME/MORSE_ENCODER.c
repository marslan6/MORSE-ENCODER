#include <xmc_common.h>
#include <stdio.h>

#include "MORSE_ENCODER.h"
#include "MORSE_ALPHABET.h"

void SendLetterFromMorseWord (const char* letter)
{
  uint32_t len = strlen(letter);
  for (uint32_t i = 0; i < len; i++)
  {
    bool is_next_char_eof = (i == len - 1);
    
    if (letter[i] == '.')
    {
      // ON 100ms (DOT)
      printf("on 1u, ");
    }
    else if (letter[i] == '-')
    {
      // ON 300ms (DASH)
      printf("on 3u, ");
    }

    if (is_next_char_eof == false)
    {
      // OFF 100ms (INTRA_SYMBOL_GAP)
      printf("off 1u, ");
    }
    else 
    {
      printf("return, ");
    }
  }
}

// C (-.-.) => ON 3u, OFF 1u, ON 1u, OFF 1u, ON 3u, OFF 1u, ON 1u, then OFF 3u (letter gap)
// A (.-)   => ON 1u, OFF 1u, ON 3u, then OFF 3u (letter gap)
// N (-.)   => ON 3u, OFF 1u, ON 1u, then OFF 7u (word gap)
void ConvertWordToMorseWord(const char* word) 
{
  // Word = I CAN MORSE
  for (uint32_t i = 0; i <= strlen(word); i++)
  {
    if (i != strlen(word))
    {
      const char* character_morse_equivalent = NULL;
      uint32_t letter_ascii_id = (unsigned char)word[i];
      bool is_next_letter_space = (word[i+1] == ' ');
      bool is_next_letter_eof = (i == strlen(word) - 1);
      bool is_letter_ascii_id_valid = (
                                        (letter_ascii_id >= 65 && letter_ascii_id <= 90) || // A-Z
                                        (letter_ascii_id >= 48 && letter_ascii_id <= 57) || // 0-9
                                        (letter_ascii_id == 32)                             // "0"
                                      );

      if (is_letter_ascii_id_valid)
      {
        character_morse_equivalent = MORSE_ALPHABET[letter_ascii_id];
        SendLetterFromMorseWord(character_morse_equivalent);

        if (is_next_letter_space)
        {
          i++;
          // OFF LIGHT 700ms (WORD_GAP)
          printf("off 7u, ");
        }        
        else if (is_next_letter_eof == false)
        {
          // OFF LIGHT 300ms (LETTER_GAP)
          printf("off 3u, ");
        }
      }
    }
    else 
    {
      // OFF LIGHT 5000ms (SENTENCE_GAP)
      printf("off 50u, ");
    }
  }
} 