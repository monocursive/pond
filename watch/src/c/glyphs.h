#pragma once
#include <stdint.h>
/* Original Pond pixel glyphs, from design/pixel-glyphs.json. */
static const uint8_t *pond_glyph(char c) {
  static const uint8_t blank[7] = {0};
  switch (c) {
  case '0': {
    static const uint8_t rows[7] = {14, 27, 27, 27, 27, 27, 14};
    return rows;
  }
  case '1': {
    static const uint8_t rows[7] = {6, 14, 6, 6, 6, 6, 15};
    return rows;
  }
  case '2': {
    static const uint8_t rows[7] = {14, 27, 3, 6, 12, 24, 31};
    return rows;
  }
  case '3': {
    static const uint8_t rows[7] = {30, 3, 3, 14, 3, 3, 30};
    return rows;
  }
  case '4': {
    static const uint8_t rows[7] = {3, 7, 15, 27, 31, 3, 3};
    return rows;
  }
  case '5': {
    static const uint8_t rows[7] = {31, 24, 24, 30, 3, 27, 14};
    return rows;
  }
  case '6': {
    static const uint8_t rows[7] = {6, 12, 24, 30, 27, 27, 14};
    return rows;
  }
  case '7': {
    static const uint8_t rows[7] = {31, 3, 6, 6, 12, 12, 12};
    return rows;
  }
  case '8': {
    static const uint8_t rows[7] = {14, 27, 27, 14, 27, 27, 14};
    return rows;
  }
  case '9': {
    static const uint8_t rows[7] = {14, 27, 27, 15, 3, 6, 12};
    return rows;
  }
  case ':': {
    static const uint8_t rows[7] = {0, 1, 1, 0, 1, 1, 0};
    return rows;
  }
  case 'A': {
    static const uint8_t rows[7] = {14, 17, 17, 31, 17, 17, 17};
    return rows;
  }
  case 'B': {
    static const uint8_t rows[7] = {30, 17, 17, 30, 17, 17, 30};
    return rows;
  }
  case 'C': {
    static const uint8_t rows[7] = {15, 16, 16, 16, 16, 16, 15};
    return rows;
  }
  case 'D': {
    static const uint8_t rows[7] = {30, 17, 17, 17, 17, 17, 30};
    return rows;
  }
  case 'E': {
    static const uint8_t rows[7] = {31, 16, 16, 30, 16, 16, 31};
    return rows;
  }
  case 'F': {
    static const uint8_t rows[7] = {31, 16, 16, 30, 16, 16, 16};
    return rows;
  }
  case 'G': {
    static const uint8_t rows[7] = {15, 16, 16, 23, 17, 17, 14};
    return rows;
  }
  case 'H': {
    static const uint8_t rows[7] = {17, 17, 17, 31, 17, 17, 17};
    return rows;
  }
  case 'I': {
    static const uint8_t rows[7] = {7, 2, 2, 2, 2, 2, 7};
    return rows;
  }
  case 'J': {
    static const uint8_t rows[7] = {7, 2, 2, 2, 2, 18, 12};
    return rows;
  }
  case 'K': {
    static const uint8_t rows[7] = {17, 18, 20, 24, 20, 18, 17};
    return rows;
  }
  case 'L': {
    static const uint8_t rows[7] = {16, 16, 16, 16, 16, 16, 31};
    return rows;
  }
  case 'M': {
    static const uint8_t rows[7] = {17, 27, 21, 21, 17, 17, 17};
    return rows;
  }
  case 'N': {
    static const uint8_t rows[7] = {17, 25, 25, 21, 19, 19, 17};
    return rows;
  }
  case 'O': {
    static const uint8_t rows[7] = {14, 17, 17, 17, 17, 17, 14};
    return rows;
  }
  case 'P': {
    static const uint8_t rows[7] = {30, 17, 17, 30, 16, 16, 16};
    return rows;
  }
  case 'Q': {
    static const uint8_t rows[7] = {14, 17, 17, 17, 21, 18, 13};
    return rows;
  }
  case 'R': {
    static const uint8_t rows[7] = {30, 17, 17, 30, 20, 18, 17};
    return rows;
  }
  case 'S': {
    static const uint8_t rows[7] = {15, 16, 16, 14, 1, 1, 30};
    return rows;
  }
  case 'T': {
    static const uint8_t rows[7] = {31, 4, 4, 4, 4, 4, 4};
    return rows;
  }
  case 'U': {
    static const uint8_t rows[7] = {17, 17, 17, 17, 17, 17, 14};
    return rows;
  }
  case 'V': {
    static const uint8_t rows[7] = {17, 17, 17, 17, 17, 10, 4};
    return rows;
  }
  case 'W': {
    static const uint8_t rows[7] = {17, 17, 17, 21, 21, 27, 17};
    return rows;
  }
  case 'X': {
    static const uint8_t rows[7] = {17, 17, 10, 4, 10, 17, 17};
    return rows;
  }
  case 'Y': {
    static const uint8_t rows[7] = {17, 17, 10, 4, 4, 4, 4};
    return rows;
  }
  case 'Z': {
    static const uint8_t rows[7] = {31, 1, 2, 4, 8, 16, 31};
    return rows;
  }
  case ' ': {
    static const uint8_t rows[7] = {0, 0, 0, 0, 0, 0, 0};
    return rows;
  }
  case '-': {
    static const uint8_t rows[7] = {0, 0, 0, 7, 0, 0, 0};
    return rows;
  }
  case '?': {
    static const uint8_t rows[7] = {14, 17, 1, 2, 4, 0, 4};
    return rows;
  }
  default:
    return blank;
  }
}
