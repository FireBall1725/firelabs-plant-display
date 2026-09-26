/*******************************************************************************
 * Size: 9 px
 * Bpp: 4
 * Opts: --font Roboto-700.ttf -r 0x20-0x7E -r 0xB0,0xB5,0xB7,0x2013,0x2212,0x2026,0x2019 --size 9 --bpp 4 --format lvgl --no-compress --lv-include lvgl.h --lv-font-name font_b9 -o src/fonts/font_b9.c
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl.h"
#endif

#ifndef FONT_B9
#define FONT_B9 1
#endif

#if FONT_B9

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */

    /* U+0021 "!" */
    0x6e, 0x6e, 0x5d, 0x5d, 0x27, 0x13, 0x5c,

    /* U+0022 "\"" */
    0xb7, 0x9b, 0x57, 0x52, 0x30,

    /* U+0023 "#" */
    0x0, 0xc5, 0x60, 0x2, 0x98, 0x40, 0x4f, 0xff,
    0xe1, 0x8, 0x4c, 0x0, 0x8f, 0xef, 0x90, 0xc,
    0x2a, 0x0, 0xc, 0x57, 0x0,

    /* U+0024 "$" */
    0x0, 0x91, 0x0, 0x9f, 0xd2, 0x4f, 0x2b, 0x93,
    0xf4, 0x12, 0x7, 0xfb, 0x11, 0x21, 0xc9, 0x7d,
    0x1a, 0xa0, 0xbf, 0xc2, 0x0, 0x90, 0x0,

    /* U+0025 "%" */
    0x4c, 0xb0, 0x10, 0x8, 0x4b, 0x2b, 0x0, 0x3b,
    0xa7, 0x50, 0x0, 0x1, 0xb0, 0x0, 0x0, 0x95,
    0xba, 0x0, 0x39, 0x75, 0x93, 0x2, 0x13, 0xcc,
    0x10,

    /* U+0026 "&" */
    0x8, 0xea, 0x0, 0x1f, 0x2e, 0x20, 0xf, 0xcd,
    0x0, 0xf, 0xf3, 0x21, 0x8c, 0xac, 0xd5, 0x9b,
    0xf, 0xf1, 0x1c, 0xfd, 0xf6,

    /* U+0027 "'" */
    0xb3, 0xb2, 0x50,

    /* U+0028 "(" */
    0x0, 0x10, 0x3b, 0xd, 0x23, 0xc0, 0x5a, 0x6,
    0x90, 0x5b, 0x2, 0xe0, 0xb, 0x40, 0x1a, 0x0,
    0x0,

    /* U+0029 ")" */
    0x10, 0x9, 0x60, 0x1e, 0x10, 0xb6, 0x8, 0x90,
    0x7a, 0x8, 0x80, 0xc5, 0x2d, 0x9, 0x30, 0x0,
    0x0,

    /* U+002A "*" */
    0x4, 0x70, 0x9, 0xab, 0xb0, 0xc, 0xd1, 0x3,
    0x76, 0x40,

    /* U+002B "+" */
    0x2, 0xd0, 0x0, 0x2f, 0x0, 0xbf, 0xff, 0x91,
    0x5f, 0x31, 0x2, 0xf0, 0x0, 0x2, 0x0,

    /* U+002C "," */
    0x6a, 0x78, 0x71,

    /* U+002D "-" */
    0x7e, 0xe0, 0x11,

    /* U+002E "." */
    0x3, 0x5d,

    /* U+002F "/" */
    0x0, 0xd, 0x10, 0x3, 0xb0, 0x0, 0x86, 0x0,
    0xd, 0x10, 0x3, 0xb0, 0x0, 0x96, 0x0, 0xd,
    0x10, 0x0,

    /* U+0030 "0" */
    0xa, 0xfc, 0x15, 0xe1, 0xc8, 0x8b, 0x8, 0xb9,
    0xb0, 0x8c, 0x8b, 0x8, 0xb5, 0xe1, 0xc8, 0xa,
    0xfb, 0x10,

    /* U+0031 "1" */
    0x4, 0xa8, 0x4d, 0xe8, 0x0, 0xb8, 0x0, 0xb8,
    0x0, 0xb8, 0x0, 0xb8, 0x0, 0xb8,

    /* U+0032 "2" */
    0xb, 0xfb, 0x18, 0xc1, 0xd7, 0x22, 0xc, 0x80,
    0x5, 0xf1, 0x3, 0xf4, 0x1, 0xe7, 0x0, 0x8f,
    0xff, 0xd0,

    /* U+0033 "3" */
    0x1b, 0xfb, 0x17, 0xb1, 0xc8, 0x0, 0xc, 0x70,
    0x5f, 0xf1, 0x0, 0xb, 0x99, 0xb1, 0xba, 0x2c,
    0xfc, 0x20,

    /* U+0034 "4" */
    0x0, 0x7f, 0x30, 0x1e, 0xf3, 0xa, 0x8f, 0x33,
    0xd1, 0xf3, 0xbf, 0xff, 0xf1, 0x12, 0xf4, 0x0,
    0x1f, 0x30,

    /* U+0035 "5" */
    0x1f, 0xff, 0x92, 0xf1, 0x10, 0x3e, 0x0, 0x5,
    0xfe, 0xd3, 0x3, 0xa, 0xb5, 0xa1, 0xab, 0x1b,
    0xfd, 0x20,

    /* U+0036 "6" */
    0x2, 0xbd, 0x0, 0xe8, 0x10, 0x5f, 0xcc, 0x28,
    0xf2, 0xba, 0x8c, 0x7, 0xd4, 0xe1, 0xba, 0x9,
    0xfc, 0x10,

    /* U+0037 "7" */
    0xbf, 0xff, 0xc0, 0x0, 0xc7, 0x0, 0x2f, 0x10,
    0x9, 0xa0, 0x0, 0xf4, 0x0, 0x6e, 0x0, 0xd,
    0x80, 0x0,

    /* U+0038 "8" */
    0xa, 0xfc, 0x16, 0xe1, 0xc8, 0x5e, 0x1c, 0x70,
    0xef, 0xf1, 0x7d, 0x1b, 0x98, 0xd1, 0xba, 0x1b,
    0xfc, 0x20,

    /* U+0039 "9" */
    0xa, 0xfa, 0x6, 0xe1, 0xd7, 0x9a, 0x9, 0xa7,
    0xd0, 0xfa, 0x1b, 0xef, 0x80, 0x15, 0xf2, 0xa,
    0xc4, 0x0,

    /* U+003A ":" */
    0x4c, 0x13, 0x0, 0x3, 0x5c,

    /* U+003B ";" */
    0x6b, 0x13, 0x0, 0x1, 0x4c, 0x69, 0x41,

    /* U+003C "<" */
    0x0, 0x5c, 0x7, 0xec, 0x60, 0xff, 0x71, 0x0,
    0x3a, 0xf0, 0x0, 0x2, 0x0,

    /* U+003D "=" */
    0x5e, 0xee, 0x80, 0x11, 0x10, 0x5e, 0xee, 0x80,
    0x11, 0x10,

    /* U+003E ">" */
    0x69, 0x20, 0x2, 0x9e, 0xb3, 0x3, 0xaf, 0x67,
    0xe7, 0x10, 0x10, 0x0, 0x0,

    /* U+003F "?" */
    0x3d, 0xe8, 0x9, 0x94, 0xf1, 0x0, 0x5f, 0x0,
    0x2f, 0x50, 0x6, 0xa0, 0x0, 0x12, 0x0, 0x7,
    0xa0, 0x0,

    /* U+0040 "@" */
    0x0, 0x7b, 0xbb, 0x30, 0x9, 0x70, 0x1, 0xc1,
    0x2b, 0x9, 0xd9, 0x57, 0x76, 0x59, 0x4a, 0x2a,
    0x93, 0xa4, 0x58, 0x1a, 0x94, 0xb4, 0xc7, 0x57,
    0x67, 0x5c, 0x9c, 0xa0, 0x1d, 0x30, 0x0, 0x0,
    0x2, 0xbc, 0xc5, 0x0,

    /* U+0041 "A" */
    0x0, 0xff, 0x0, 0x0, 0x1f, 0xf2, 0x0, 0x7,
    0xcb, 0x70, 0x0, 0xc7, 0x6d, 0x0, 0x1f, 0xff,
    0xf2, 0x7, 0xf2, 0x2e, 0x80, 0xc9, 0x0, 0x8d,
    0x0,

    /* U+0042 "B" */
    0x6f, 0xfe, 0x70, 0x6e, 0x16, 0xf1, 0x6e, 0x4,
    0xf1, 0x6f, 0xff, 0x90, 0x6e, 0x2, 0xf3, 0x6e,
    0x14, 0xf3, 0x6f, 0xfe, 0x90,

    /* U+0043 "C" */
    0x6, 0xef, 0x90, 0x3f, 0x43, 0xf5, 0x8c, 0x0,
    0x43, 0x9b, 0x0, 0x0, 0x8c, 0x0, 0x32, 0x3f,
    0x43, 0xf6, 0x7, 0xef, 0x80,

    /* U+0044 "D" */
    0x6f, 0xfc, 0x30, 0x6f, 0x18, 0xe1, 0x6e, 0x0,
    0xf5, 0x6e, 0x0, 0xe7, 0x6e, 0x0, 0xf5, 0x6f,
    0x18, 0xe1, 0x6f, 0xfc, 0x30,

    /* U+0045 "E" */
    0x6f, 0xff, 0xd6, 0xe1, 0x11, 0x6e, 0x0, 0x6,
    0xfe, 0xe6, 0x6f, 0x11, 0x6, 0xe1, 0x10, 0x6f,
    0xff, 0xd0,

    /* U+0046 "F" */
    0x6f, 0xff, 0xc6, 0xe1, 0x10, 0x6e, 0x0, 0x6,
    0xff, 0xf6, 0x6f, 0x22, 0x6, 0xe0, 0x0, 0x6e,
    0x0, 0x0,

    /* U+0047 "G" */
    0x6, 0xef, 0xa0, 0x2f, 0x52, 0xd7, 0x7d, 0x0,
    0x0, 0x9c, 0xd, 0xfa, 0x7d, 0x0, 0xaa, 0x3f,
    0x51, 0xca, 0x6, 0xdf, 0xc3,

    /* U+0048 "H" */
    0x6e, 0x0, 0x8c, 0x6e, 0x0, 0x8c, 0x6e, 0x0,
    0x8c, 0x6f, 0xff, 0xfc, 0x6f, 0x22, 0xac, 0x6e,
    0x0, 0x8c, 0x6e, 0x0, 0x8c,

    /* U+0049 "I" */
    0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f,

    /* U+004A "J" */
    0x0, 0xd, 0x70, 0x0, 0xd7, 0x0, 0xd, 0x70,
    0x0, 0xd7, 0x10, 0xd, 0x7b, 0xa2, 0xf5, 0x3d,
    0xf9, 0x0,

    /* U+004B "K" */
    0x6e, 0x4, 0xf5, 0x6e, 0x1e, 0x90, 0x6e, 0xad,
    0x0, 0x6f, 0xfd, 0x0, 0x6f, 0x8f, 0x40, 0x6e,
    0xa, 0xd0, 0x6e, 0x1, 0xf7,

    /* U+004C "L" */
    0x6e, 0x0, 0x6, 0xe0, 0x0, 0x6e, 0x0, 0x6,
    0xe0, 0x0, 0x6e, 0x0, 0x6, 0xe1, 0x10, 0x6f,
    0xff, 0xb0,

    /* U+004D "M" */
    0x6f, 0x70, 0x9, 0xf4, 0x6f, 0xc0, 0xe, 0xf4,
    0x6d, 0xf1, 0x3d, 0xe4, 0x6d, 0xa6, 0x88, 0xf4,
    0x6d, 0x5b, 0xd3, 0xf4, 0x6e, 0xf, 0xd0, 0xf4,
    0x6e, 0xa, 0x80, 0xf4,

    /* U+004E "N" */
    0x6f, 0x20, 0x8c, 0x6f, 0xb0, 0x8c, 0x6e, 0xe4,
    0x8c, 0x6e, 0x7c, 0x8c, 0x6e, 0xe, 0xdc, 0x6e,
    0x5, 0xfc, 0x6e, 0x0, 0xcc,

    /* U+004F "O" */
    0x5, 0xde, 0x80, 0x2f, 0x53, 0xe6, 0x8c, 0x0,
    0x9b, 0x9b, 0x0, 0x8c, 0x8c, 0x0, 0x9b, 0x3f,
    0x53, 0xe6, 0x6, 0xde, 0x80,

    /* U+0050 "P" */
    0x6f, 0xfe, 0x70, 0x6e, 0x14, 0xf3, 0x6e, 0x0,
    0xe7, 0x6e, 0x2, 0xf5, 0x6f, 0xff, 0xa0, 0x6f,
    0x21, 0x0, 0x6e, 0x0, 0x0,

    /* U+0051 "Q" */
    0x6, 0xde, 0x80, 0x3f, 0x53, 0xe6, 0x8c, 0x0,
    0x9b, 0x9b, 0x0, 0x8c, 0x8c, 0x0, 0x9b, 0x3f,
    0x53, 0xe6, 0x6, 0xdf, 0xf1, 0x0, 0x0, 0xb7,
    0x0, 0x0, 0x0,

    /* U+0052 "R" */
    0x6f, 0xfe, 0x80, 0x6e, 0x14, 0xf3, 0x6e, 0x2,
    0xf3, 0x6f, 0xff, 0x90, 0x6f, 0x2f, 0x70, 0x6e,
    0x7, 0xe0, 0x6e, 0x1, 0xf5,

    /* U+0053 "S" */
    0xa, 0xfe, 0x50, 0x6e, 0x26, 0xf1, 0x5f, 0x50,
    0x10, 0x8, 0xfe, 0x50, 0x22, 0x8, 0xf1, 0x8d,
    0x14, 0xf2, 0xa, 0xfe, 0x70,

    /* U+0054 "T" */
    0xdf, 0xff, 0xf6, 0x11, 0xf8, 0x10, 0x0, 0xe7,
    0x0, 0x0, 0xe7, 0x0, 0x0, 0xe7, 0x0, 0x0,
    0xe7, 0x0, 0x0, 0xe7, 0x0,

    /* U+0055 "U" */
    0x7d, 0x0, 0xe6, 0x7d, 0x0, 0xe6, 0x7d, 0x0,
    0xe6, 0x7d, 0x0, 0xe6, 0x7d, 0x0, 0xe6, 0x5f,
    0x34, 0xf4, 0x8, 0xfe, 0x70,

    /* U+0056 "V" */
    0xd9, 0x0, 0xbb, 0x7e, 0x0, 0xf6, 0x2f, 0x24,
    0xf1, 0xd, 0x79, 0xb0, 0x8, 0xbd, 0x60, 0x3,
    0xff, 0x10, 0x0, 0xfe, 0x0,

    /* U+0057 "W" */
    0xb8, 0xf, 0xc0, 0xaa, 0x8b, 0xf, 0xf0, 0xd6,
    0x5e, 0x2f, 0xf1, 0xf3, 0x2f, 0x7b, 0xd7, 0xf0,
    0xe, 0xd7, 0x9d, 0xd0, 0xb, 0xf3, 0x5f, 0x90,
    0x8, 0xf0, 0x2f, 0x60,

    /* U+0058 "X" */
    0x9e, 0x3, 0xf4, 0x1f, 0x6a, 0xc0, 0x8, 0xef,
    0x30, 0x2, 0xfd, 0x0, 0x8, 0xef, 0x40, 0x1f,
    0x5a, 0xc0, 0xad, 0x2, 0xf5,

    /* U+0059 "Y" */
    0xca, 0x1, 0xf5, 0x4f, 0x18, 0xd0, 0xc, 0x8e,
    0x60, 0x5, 0xfe, 0x0, 0x0, 0xe8, 0x0, 0x0,
    0xe7, 0x0, 0x0, 0xe7, 0x0,

    /* U+005A "Z" */
    0xaf, 0xff, 0xf1, 0x1, 0x1c, 0xb0, 0x0, 0x5f,
    0x10, 0x0, 0xe6, 0x0, 0x9, 0xc0, 0x0, 0x3f,
    0x41, 0x10, 0xff, 0xff, 0xf3,

    /* U+005B "[" */
    0x7f, 0x67, 0xc0, 0x7c, 0x7, 0xc0, 0x7c, 0x7,
    0xc0, 0x7c, 0x7, 0xc0, 0x7f, 0x60,

    /* U+005C "\\" */
    0xc7, 0x0, 0x6d, 0x0, 0x1f, 0x40, 0xa, 0xa0,
    0x4, 0xf0, 0x0, 0xe6, 0x0, 0x8c,

    /* U+005D "]" */
    0xef, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f,
    0xef,

    /* U+005E "^" */
    0x6, 0x50, 0xe, 0xd0, 0x3b, 0xc2, 0x96, 0x68,

    /* U+005F "_" */
    0xff, 0xff,

    /* U+0060 "`" */
    0x11, 0x3, 0xe1, 0x3, 0x10,

    /* U+0061 "a" */
    0x2c, 0xea, 0x5, 0x70, 0xf4, 0x2b, 0xbf, 0x5a,
    0xa0, 0xf5, 0x4e, 0xcf, 0x60,

    /* U+0062 "b" */
    0x8c, 0x0, 0x8, 0xc0, 0x0, 0x8f, 0xed, 0x28,
    0xf1, 0xb9, 0x8f, 0x8, 0xb8, 0xf0, 0xa9, 0x8e,
    0xed, 0x20,

    /* U+0063 "c" */
    0x1b, 0xfa, 0x8, 0xc1, 0xc5, 0xa9, 0x0, 0x8,
    0xc1, 0xa4, 0x1b, 0xfa, 0x0,

    /* U+0064 "d" */
    0x0, 0xb, 0x90, 0x0, 0xb9, 0x1c, 0xee, 0x98,
    0xc1, 0xf9, 0xa9, 0xf, 0x98, 0xc0, 0xf9, 0x1c,
    0xee, 0x90,

    /* U+0065 "e" */
    0xb, 0xfb, 0x7, 0xc1, 0xc7, 0xcf, 0xee, 0x98,
    0xd1, 0x41, 0xa, 0xfd, 0x30,

    /* U+0066 "f" */
    0x9, 0xf3, 0x1f, 0x30, 0xff, 0xf0, 0x2f, 0x10,
    0x2f, 0x10, 0x2f, 0x10, 0x2f, 0x10,

    /* U+0067 "g" */
    0x1c, 0xee, 0xa8, 0xc0, 0xfa, 0xa9, 0xf, 0xb8,
    0xd1, 0xfa, 0x1c, 0xee, 0xa0, 0x50, 0xd8, 0x2d,
    0xfb, 0x10,

    /* U+0068 "h" */
    0x8b, 0x0, 0x8, 0xb0, 0x0, 0x8e, 0xde, 0x28,
    0xf1, 0xd8, 0x8d, 0xb, 0x98, 0xb0, 0xb9, 0x8b,
    0xb, 0x90,

    /* U+0069 "i" */
    0x5b, 0x12, 0x7d, 0x7d, 0x7d, 0x7d, 0x7d,

    /* U+006A "j" */
    0x6, 0xb0, 0x12, 0x7, 0xd0, 0x7d, 0x7, 0xd0,
    0x7d, 0x7, 0xd0, 0x8c, 0x6e, 0x50,

    /* U+006B "k" */
    0x8c, 0x0, 0x8, 0xc0, 0x0, 0x8c, 0x3f, 0x58,
    0xde, 0x70, 0x8f, 0xf4, 0x8, 0xf7, 0xe0, 0x8c,
    0xd, 0x90,

    /* U+006C "l" */
    0x7d, 0x7d, 0x7d, 0x7d, 0x7d, 0x7d, 0x7d,

    /* U+006D "m" */
    0x8e, 0xde, 0x8e, 0xc0, 0x8f, 0xd, 0xf0, 0xf4,
    0x8d, 0xb, 0x90, 0xf5, 0x8c, 0xb, 0x80, 0xf5,
    0x8c, 0xb, 0x80, 0xf5,

    /* U+006E "n" */
    0x8d, 0xde, 0x38, 0xf0, 0xc8, 0x8d, 0xb, 0x98,
    0xb0, 0xb9, 0x8b, 0xb, 0x90,

    /* U+006F "o" */
    0x1b, 0xfb, 0x18, 0xc1, 0xb9, 0xa9, 0x7, 0xc8,
    0xc1, 0xb9, 0x1b, 0xfb, 0x10,

    /* U+0070 "p" */
    0x8e, 0xed, 0x28, 0xf0, 0xb9, 0x8f, 0x8, 0xb8,
    0xf1, 0xb9, 0x8f, 0xed, 0x28, 0xc0, 0x0, 0x8c,
    0x0, 0x0,

    /* U+0071 "q" */
    0x1c, 0xee, 0x99, 0xc0, 0xf9, 0xa9, 0xf, 0x98,
    0xc1, 0xf9, 0x1c, 0xef, 0x90, 0x0, 0xb9, 0x0,
    0xb, 0x90,

    /* U+0072 "r" */
    0x0, 0x0, 0x8d, 0xe2, 0x8f, 0x30, 0x8d, 0x0,
    0x8c, 0x0, 0x8c, 0x0,

    /* U+0073 "s" */
    0x3d, 0xea, 0x8, 0xb1, 0x81, 0x2b, 0xe9, 0x6,
    0x51, 0xf3, 0x3c, 0xea, 0x0,

    /* U+0074 "t" */
    0x3, 0x4, 0xf0, 0xff, 0xc4, 0xf0, 0x4f, 0x3,
    0xf1, 0xd, 0xd0,

    /* U+0075 "u" */
    0x8b, 0xb, 0x98, 0xb0, 0xb9, 0x8b, 0xc, 0x98,
    0xc0, 0xf9, 0x2e, 0xee, 0x90,

    /* U+0076 "v" */
    0xc8, 0xf, 0x57, 0xc3, 0xf0, 0x2f, 0x8a, 0x0,
    0xdf, 0x50, 0x8, 0xf1, 0x0,

    /* U+0077 "w" */
    0xc7, 0x4f, 0xd, 0x58, 0xa9, 0xf3, 0xf1, 0x4d,
    0xd9, 0xad, 0x0, 0xfd, 0x3f, 0xa0, 0xf, 0xa0,
    0xf6, 0x0,

    /* U+0078 "x" */
    0x9c, 0x2f, 0x21, 0xfd, 0x90, 0xa, 0xf3, 0x1,
    0xfc, 0xb0, 0xab, 0x1f, 0x30,

    /* U+0079 "y" */
    0xd8, 0xf, 0x57, 0xc4, 0xf0, 0x2f, 0xaa, 0x0,
    0xdf, 0x50, 0x9, 0xf0, 0x0, 0x9b, 0x0, 0x7e,
    0x30, 0x0,

    /* U+007A "z" */
    0x9f, 0xff, 0x20, 0xb, 0xa0, 0x7, 0xe1, 0x2,
    0xf4, 0x0, 0xff, 0xff, 0x40,

    /* U+007B "{" */
    0x4, 0x90, 0xe4, 0xf, 0x22, 0xf0, 0xea, 0x0,
    0xf0, 0xf, 0x20, 0xc5, 0x2, 0x70,

    /* U+007C "|" */
    0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x36,

    /* U+007D "}" */
    0x94, 0x4, 0xd0, 0x2f, 0x1, 0xf1, 0xa, 0xf1,
    0xf1, 0x2f, 0x4, 0xd0, 0x94, 0x0,

    /* U+007E "~" */
    0x1c, 0xc3, 0xa4, 0x78, 0x4f, 0xd0, 0x0, 0x0,
    0x0,

    /* U+00B0 "°" */
    0x1a, 0x66, 0x4a, 0x1a, 0x60,

    /* U+00B5 "µ" */
    0x6e, 0x6, 0xd6, 0xe0, 0x6d, 0x6f, 0x7, 0xe6,
    0xf0, 0xdf, 0x6f, 0xef, 0xd6, 0xe0, 0x0, 0x6e,
    0x0, 0x0,

    /* U+00B7 "·" */
    0x0, 0x4, 0xe0, 0x2, 0x0,

    /* U+2013 "–" */
    0x5e, 0xee, 0xe3, 0x1, 0x11, 0x10,

    /* U+2019 "’" */
    0x5a, 0x86, 0x20,

    /* U+2026 "…" */
    0x3, 0x3, 0x4, 0x4, 0xd3, 0xe3, 0xe0,

    /* U+2212 "−" */
    0x1d, 0xdd, 0x90, 0x22, 0x21
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 36, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 0, .adv_w = 39, .box_w = 2, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 7, .adv_w = 46, .box_w = 3, .box_h = 3, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 12, .adv_w = 85, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 33, .adv_w = 83, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 56, .adv_w = 106, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 81, .adv_w = 95, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 102, .adv_w = 23, .box_w = 2, .box_h = 3, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 105, .adv_w = 50, .box_w = 3, .box_h = 11, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 122, .adv_w = 51, .box_w = 3, .box_h = 11, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 139, .adv_w = 65, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 149, .adv_w = 78, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 164, .adv_w = 35, .box_w = 2, .box_h = 3, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 167, .adv_w = 57, .box_w = 3, .box_h = 2, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 170, .adv_w = 42, .box_w = 2, .box_h = 2, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 172, .adv_w = 53, .box_w = 5, .box_h = 7, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 190, .adv_w = 83, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 208, .adv_w = 83, .box_w = 4, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 222, .adv_w = 83, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 240, .adv_w = 83, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 258, .adv_w = 83, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 276, .adv_w = 83, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 294, .adv_w = 83, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 312, .adv_w = 83, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 330, .adv_w = 83, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 348, .adv_w = 83, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 366, .adv_w = 41, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 371, .adv_w = 38, .box_w = 2, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 378, .adv_w = 73, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 391, .adv_w = 83, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 401, .adv_w = 74, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 414, .adv_w = 72, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 432, .adv_w = 129, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 468, .adv_w = 97, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 493, .adv_w = 92, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 514, .adv_w = 94, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 535, .adv_w = 94, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 556, .adv_w = 81, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 574, .adv_w = 79, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 592, .adv_w = 98, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 613, .adv_w = 102, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 634, .adv_w = 42, .box_w = 2, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 641, .adv_w = 80, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 659, .adv_w = 92, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 680, .adv_w = 78, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 698, .adv_w = 126, .box_w = 8, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 726, .adv_w = 102, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 747, .adv_w = 99, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 768, .adv_w = 93, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 789, .adv_w = 99, .box_w = 6, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 816, .adv_w = 92, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 837, .adv_w = 89, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 858, .adv_w = 89, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 879, .adv_w = 95, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 900, .adv_w = 94, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 921, .adv_w = 126, .box_w = 8, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 949, .adv_w = 91, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 970, .adv_w = 89, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 991, .adv_w = 87, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1012, .adv_w = 40, .box_w = 3, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1026, .adv_w = 61, .box_w = 4, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1040, .adv_w = 40, .box_w = 2, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1049, .adv_w = 63, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 1057, .adv_w = 64, .box_w = 4, .box_h = 1, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1059, .adv_w = 48, .box_w = 3, .box_h = 3, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 1064, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1077, .adv_w = 81, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1095, .adv_w = 75, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1108, .adv_w = 81, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1126, .adv_w = 78, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1139, .adv_w = 52, .box_w = 4, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1153, .adv_w = 82, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1171, .adv_w = 81, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1189, .adv_w = 38, .box_w = 2, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1196, .adv_w = 37, .box_w = 3, .box_h = 9, .ofs_x = -1, .ofs_y = -2},
    {.bitmap_index = 1210, .adv_w = 77, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1228, .adv_w = 38, .box_w = 2, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1235, .adv_w = 125, .box_w = 8, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1255, .adv_w = 81, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1268, .adv_w = 81, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1281, .adv_w = 81, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1299, .adv_w = 81, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1317, .adv_w = 53, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1329, .adv_w = 74, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1342, .adv_w = 49, .box_w = 3, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1353, .adv_w = 81, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1366, .adv_w = 73, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1379, .adv_w = 106, .box_w = 7, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1397, .adv_w = 73, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1410, .adv_w = 72, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1428, .adv_w = 73, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1441, .adv_w = 47, .box_w = 3, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1455, .adv_w = 36, .box_w = 2, .box_h = 8, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1463, .adv_w = 47, .box_w = 3, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1477, .adv_w = 93, .box_w = 6, .box_h = 3, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 1486, .adv_w = 56, .box_w = 3, .box_h = 3, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 1491, .adv_w = 89, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1509, .adv_w = 43, .box_w = 3, .box_h = 3, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 1514, .adv_w = 91, .box_w = 6, .box_h = 2, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 1520, .adv_w = 33, .box_w = 2, .box_h = 3, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 1523, .adv_w = 107, .box_w = 7, .box_h = 2, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1530, .adv_w = 80, .box_w = 5, .box_h = 2, .ofs_x = 0, .ofs_y = 2}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_1[] = {
    0x0, 0x5, 0x7, 0x1f63, 0x1f69, 0x1f76, 0x2162
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 176, .range_length = 8547, .glyph_id_start = 96,
        .unicode_list = unicode_list_1, .glyph_id_ofs_list = NULL, .list_length = 7, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    }
};

/*-----------------
 *    KERNING
 *----------------*/


/*Pair left and right glyphs for kerning*/
static const uint8_t kern_pair_glyph_ids[] =
{
    1, 53,
    3, 3,
    3, 8,
    3, 100,
    8, 3,
    8, 8,
    8, 100,
    9, 55,
    9, 56,
    9, 58,
    13, 3,
    13, 8,
    13, 100,
    15, 3,
    15, 8,
    15, 100,
    16, 16,
    34, 3,
    34, 8,
    34, 32,
    34, 36,
    34, 40,
    34, 48,
    34, 50,
    34, 53,
    34, 54,
    34, 55,
    34, 56,
    34, 58,
    34, 78,
    34, 79,
    34, 80,
    34, 81,
    34, 85,
    34, 86,
    34, 87,
    34, 88,
    34, 90,
    34, 91,
    34, 100,
    35, 53,
    35, 55,
    35, 58,
    36, 10,
    36, 53,
    36, 62,
    36, 94,
    37, 13,
    37, 15,
    37, 34,
    37, 53,
    37, 55,
    37, 57,
    37, 58,
    37, 59,
    37, 101,
    38, 53,
    38, 68,
    38, 69,
    38, 70,
    38, 71,
    38, 72,
    38, 80,
    38, 82,
    38, 86,
    38, 87,
    38, 88,
    38, 90,
    39, 13,
    39, 15,
    39, 34,
    39, 43,
    39, 53,
    39, 66,
    39, 68,
    39, 69,
    39, 70,
    39, 72,
    39, 80,
    39, 82,
    39, 83,
    39, 86,
    39, 87,
    39, 90,
    39, 101,
    41, 34,
    41, 53,
    41, 57,
    41, 58,
    42, 34,
    42, 53,
    42, 57,
    42, 58,
    43, 34,
    44, 14,
    44, 36,
    44, 40,
    44, 48,
    44, 50,
    44, 68,
    44, 69,
    44, 70,
    44, 72,
    44, 78,
    44, 79,
    44, 80,
    44, 81,
    44, 82,
    44, 86,
    44, 87,
    44, 88,
    44, 90,
    44, 99,
    45, 34,
    45, 36,
    45, 40,
    45, 48,
    45, 50,
    45, 53,
    45, 54,
    45, 55,
    45, 56,
    45, 58,
    45, 86,
    45, 87,
    45, 88,
    45, 90,
    46, 34,
    46, 53,
    46, 57,
    46, 58,
    47, 34,
    47, 53,
    47, 57,
    47, 58,
    48, 13,
    48, 15,
    48, 34,
    48, 53,
    48, 55,
    48, 57,
    48, 58,
    48, 59,
    48, 101,
    49, 13,
    49, 15,
    49, 34,
    49, 43,
    49, 57,
    49, 59,
    49, 66,
    49, 68,
    49, 69,
    49, 70,
    49, 72,
    49, 80,
    49, 82,
    49, 85,
    49, 87,
    49, 90,
    49, 101,
    50, 53,
    50, 55,
    50, 56,
    50, 58,
    51, 53,
    51, 55,
    51, 58,
    53, 1,
    53, 13,
    53, 14,
    53, 15,
    53, 34,
    53, 36,
    53, 40,
    53, 43,
    53, 48,
    53, 50,
    53, 52,
    53, 53,
    53, 55,
    53, 56,
    53, 58,
    53, 66,
    53, 68,
    53, 69,
    53, 70,
    53, 72,
    53, 78,
    53, 79,
    53, 80,
    53, 81,
    53, 82,
    53, 83,
    53, 84,
    53, 86,
    53, 87,
    53, 88,
    53, 89,
    53, 90,
    53, 91,
    53, 99,
    53, 101,
    54, 34,
    55, 10,
    55, 13,
    55, 14,
    55, 15,
    55, 34,
    55, 36,
    55, 40,
    55, 48,
    55, 50,
    55, 62,
    55, 66,
    55, 68,
    55, 69,
    55, 70,
    55, 72,
    55, 80,
    55, 82,
    55, 83,
    55, 86,
    55, 87,
    55, 90,
    55, 94,
    55, 99,
    55, 101,
    56, 10,
    56, 13,
    56, 14,
    56, 15,
    56, 34,
    56, 53,
    56, 62,
    56, 66,
    56, 68,
    56, 69,
    56, 70,
    56, 72,
    56, 80,
    56, 82,
    56, 83,
    56, 86,
    56, 94,
    56, 99,
    56, 101,
    57, 14,
    57, 36,
    57, 40,
    57, 48,
    57, 50,
    57, 55,
    57, 68,
    57, 69,
    57, 70,
    57, 72,
    57, 80,
    57, 82,
    57, 86,
    57, 87,
    57, 90,
    57, 99,
    58, 7,
    58, 10,
    58, 11,
    58, 13,
    58, 14,
    58, 15,
    58, 34,
    58, 36,
    58, 40,
    58, 43,
    58, 48,
    58, 50,
    58, 52,
    58, 53,
    58, 54,
    58, 55,
    58, 56,
    58, 57,
    58, 58,
    58, 62,
    58, 66,
    58, 68,
    58, 69,
    58, 70,
    58, 71,
    58, 72,
    58, 78,
    58, 79,
    58, 80,
    58, 81,
    58, 82,
    58, 83,
    58, 84,
    58, 85,
    58, 86,
    58, 87,
    58, 89,
    58, 90,
    58, 91,
    58, 94,
    58, 99,
    58, 101,
    59, 34,
    59, 36,
    59, 40,
    59, 48,
    59, 50,
    59, 68,
    59, 69,
    59, 70,
    59, 72,
    59, 80,
    59, 82,
    59, 86,
    59, 87,
    59, 88,
    59, 90,
    60, 43,
    60, 54,
    66, 3,
    66, 8,
    66, 87,
    66, 90,
    66, 100,
    67, 3,
    67, 8,
    67, 87,
    67, 89,
    67, 90,
    67, 91,
    67, 100,
    68, 3,
    68, 8,
    68, 100,
    70, 3,
    70, 8,
    70, 87,
    70, 90,
    70, 100,
    71, 3,
    71, 8,
    71, 10,
    71, 62,
    71, 68,
    71, 69,
    71, 70,
    71, 72,
    71, 82,
    71, 94,
    71, 100,
    73, 3,
    73, 8,
    73, 100,
    76, 68,
    76, 69,
    76, 70,
    76, 72,
    76, 82,
    78, 3,
    78, 8,
    78, 100,
    79, 3,
    79, 8,
    79, 100,
    80, 3,
    80, 8,
    80, 87,
    80, 89,
    80, 90,
    80, 91,
    80, 100,
    81, 3,
    81, 8,
    81, 87,
    81, 89,
    81, 90,
    81, 91,
    81, 100,
    83, 3,
    83, 8,
    83, 13,
    83, 15,
    83, 66,
    83, 68,
    83, 69,
    83, 70,
    83, 71,
    83, 72,
    83, 80,
    83, 82,
    83, 85,
    83, 87,
    83, 88,
    83, 90,
    83, 100,
    83, 101,
    85, 80,
    85, 100,
    87, 3,
    87, 8,
    87, 13,
    87, 15,
    87, 66,
    87, 68,
    87, 69,
    87, 70,
    87, 71,
    87, 72,
    87, 80,
    87, 82,
    87, 100,
    87, 101,
    88, 13,
    88, 15,
    88, 101,
    89, 68,
    89, 69,
    89, 70,
    89, 72,
    89, 80,
    89, 82,
    90, 3,
    90, 8,
    90, 13,
    90, 15,
    90, 66,
    90, 68,
    90, 69,
    90, 70,
    90, 71,
    90, 72,
    90, 80,
    90, 82,
    90, 100,
    90, 101,
    91, 68,
    91, 69,
    91, 70,
    91, 72,
    91, 80,
    91, 82,
    92, 43,
    92, 54,
    100, 3,
    100, 8,
    100, 100,
    101, 3,
    101, 8,
    101, 100
};

/* Kerning between the respective left and right glyphs
 * 4.4 format which needs to scaled with `kern_scale`*/
static const int8_t kern_pair_values[] =
{
    -4, -4, -4, -4, -4, -4, -4, 1,
    2, 2, -17, -17, -17, -17, -17, -17,
    -17, -8, -8, -5, -1, -1, -1, -1,
    -9, -1, -6, -3, -9, -2, -2, -1,
    -2, -1, -1, -4, -2, -4, 1, -8,
    -2, -2, -4, -2, -2, -1, -1, -8,
    -8, -1, -5, -2, -2, -3, -2, -8,
    1, -1, -1, -1, -1, -1, -1, -1,
    -1, -2, -2, -2, -18, -18, -13, -16,
    1, -2, -1, -1, -1, -1, -1, -1,
    -2, -2, -2, -2, -18, 1, -2, 1,
    -2, 1, -2, 1, -2, -2, -9, -2,
    -2, -2, -2, -2, -2, -2, -2, 0,
    0, -2, 0, -2, -2, -3, -4, -3,
    -9, 1, -4, -4, -4, -4, -16, -2,
    -14, -8, -19, -2, -9, -5, -9, 1,
    -2, 1, -2, 1, -2, 1, -2, -8,
    -8, -1, -5, -2, -2, -3, -2, -8,
    -27, -27, -12, -13, -3, -2, -1, -1,
    -1, -1, -1, -1, -1, 1, 1, 1,
    -27, -3, -2, -1, -2, -4, -1, -3,
    -4, -17, -18, -17, -8, -2, -2, -16,
    -2, -2, -1, 1, 1, 1, 1, -11,
    -6, -6, -6, -6, -7, -7, -12, -7,
    -6, -5, -6, -5, -6, -4, -5, -6,
    -4, -18, -17, -2, 1, -15, -8, -15,
    -5, -1, -1, -1, -1, 1, -3, -3,
    -3, -3, -3, -3, -3, -2, -2, -1,
    -1, 1, -8, -15, 1, -10, -4, -10,
    -3, 1, 1, -2, -2, -2, -2, -2,
    -2, -2, -1, -1, 1, -4, -10, -9,
    -2, -2, -2, -2, 1, -2, -2, -2,
    -2, -1, -2, -1, -2, -2, -9, -2,
    1, -3, -16, -8, -16, -9, -2, -2,
    -7, -2, -2, -1, 1, -7, 1, 1,
    1, 1, 1, -5, -5, -5, -5, -2,
    -5, -3, -3, -5, -3, -5, -3, -4,
    -2, -3, -1, -2, -1, -2, 1, -8,
    -16, 1, -2, -2, -2, -2, -1, -1,
    -1, -1, -1, -1, -1, -2, -2, -2,
    -1, -1, -2, -2, -1, -1, -2, -2,
    -2, -1, -1, -1, -1, -2, -1, -1,
    -1, -1, -1, -1, -1, -5, 1, 1,
    1, 1, -2, -2, -2, -2, -2, 1,
    1, -6, -6, -7, -1, -1, -1, -1,
    -1, -6, -6, -8, -6, -6, -8, -7,
    -7, -1, -1, -1, -1, -7, -2, -2,
    -1, -1, -1, -1, -2, 1, 1, -11,
    -11, -2, -1, -1, -1, 1, -1, -2,
    -1, 4, 1, 1, 1, 1, -11, -2,
    1, 1, 1, -10, -10, -1, -1, -1,
    -1, 1, -1, -1, -1, 1, -10, -9,
    -9, -9, -1, -1, -1, -1, -2, -1,
    1, 1, -10, -10, -1, -1, -1, -1,
    1, -1, -1, -1, 1, -10, -1, -1,
    -1, -1, -1, -1, -1, -1, -4, -4,
    -4, -17, -17, -17
};

/*Collect the kern pair's data in one place*/
static const lv_font_fmt_txt_kern_pair_t kern_pairs =
{
    .glyph_ids = kern_pair_glyph_ids,
    .values = kern_pair_values,
    .pair_cnt = 452,
    .glyph_ids_size = 0
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = &kern_pairs,
    .kern_scale = 16,
    .cmap_num = 2,
    .bpp = 4,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t font_b9 = {
#else
lv_font_t font_b9 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 11,          /*The maximum line height required by the font*/
    .base_line = 3,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 0,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if FONT_B9*/

