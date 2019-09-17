#include "level_functions.h"

static const uint16_t tilemap_gameover[] = {
  0,16,32,48,57,71,86,101,116,131,146,161,176,191,206,213,223,239,249,264,279,293,307,321,
  1,17,33,49,58,72,87,102,117,132,147,162,177,192,207,214,224,240,250,265,280,294,308,322,
  2,18,34,50,59,73,88,103,118,133,148,163,178,193,208,215,225,241,251,266,281,295,309,323,
  3,19,35,51,52,74,89,104,119,134,149,164,179,194,52,216,226,241,252,267,282,296,310,324,
  4,20,36,52,60,75,90,105,120,135,150,165,180,195,52,52,227,242,253,268,283,297,311,325,
  5,21,37,52,61,76,91,106,121,136,151,166,181,196,52,52,228,243,254,269,284,298,312,326,
  6,22,38,52,62,77,92,107,122,137,152,167,182,197,52,52,229,241,255,270,285,299,313,327,
  7,23,39,52,52,78,93,108,123,138,153,168,183,198,52,52,230,241,256,271,286,300,314,328,
  8,24,40,52,63,79,94,109,124,139,154,169,184,199,52,52,231,241,257,272,287,301,315,329,
  9,25,41,52,64,80,95,110,125,140,155,170,185,200,52,52,232,244,258,273,288,302,316,330,
  10,26,42,52,65,81,96,111,126,141,156,171,186,201,52,217,233,241,259,274,289,303,317,331,
  11,27,43,52,66,82,97,112,127,142,157,172,187,202,52,218,234,241,260,275,275,275,275,332,
  12,28,44,53,67,52,52,52,52,52,52,52,52,52,209,219,235,245,4356,4371,4371,4371,4371,333,
  13,29,45,54,68,83,98,113,128,143,158,173,188,203,210,220,236,246,261,276,290,304,318,334,
  14,30,46,55,69,84,99,114,129,144,159,174,189,204,211,221,237,247,262,277,291,305,319,335,
  15,31,47,56,70,85,100,115,130,145,160,175,190,205,212,222,238,248,263,278,292,306,320,336,

};
#define tilemap_gameover_width (24)
#define tilemap_gameover_height (16)

static const uint8_t tiles_compr_gameover[] = {
0x7f,0x65,0x66,0x55,0x66,0x7f,0x68,0x87,0x77,0x77,0x3,0x19,0x7d,0x88,0x88,0x3,0x80,0x7,0x59,0x7c,0x78,0x80,0x96,0x76,0x7e,0x86,0x78,0x87,0x76,0x7c,0x56,0x76,0x7c,0x36,0x76,0x7c,0x37,0x31,0x7d,0x88,0x47,0x6,0x7c,0x46,0x76,0x7d,0x46,0x58,0x3,0x36,0x3,0x26,0x7c,0x57,0x33,0x56,0x7d,0x47,0x68,0x84,0xd1,0x9,0x82,0x92,0x3,0x80,0x74,0x36,0x7c,0x58,0x3,0x80,0x7,0x79,0x7c,0x37,0x36,0x7c,0x27,0x80,0x16,0x82,0x3,0x85,0x61,0x80,0xa5,0x7d,0x27,0x48,0x3,0x80,0x7,0x80,0xd6,0x82,0x56,0x80,0xb,0x81,0x25,0x7c,0x77,0x79,0x8b,0x92,0x9,0x16,0x88,0x92,0x79,0x8b,0x51,0x83,0xf4,0x46,0x7c,0x57,0x84,0xd2,0x86,0x21,0x8b,0x12,0x3,0x7c,0x48,0x80,0x16,0x80,0x56,0x7c,0x45,0x83,0x52,0x7c,0x45,0x83,0x12,0x8c,0x3,0x7c,0x57,0x6,0x90,0x21,0x39,0x8e,0x3,0x3,0x7c,0x65,0x3,0x56,0x7c,0x74,0x3,0x13,0x8e,0xd2,0x7c,0x73,0x3,0x16,0x7d,0x83,0x77,0x79,0x7c,0x84,0x36,0x7c,0x82,0x82,0xc1,0x3,0x19,0x36,0x7d,0x81,0x67,0x88,0x21,0x9,0x16,0x23,0x33,0x80,0x7,0x95,0x81,0x69,0x92,0x92,0x80,0x8f,0x7d,0x81,0x87,0x76,0x83,0xd2,0x3,0x80,0x7,0x80,0xf,0x81,0x96,0x7c,0x78,0x80,0x7,0x80,0x56,0x80,0x7,0x80,0xf,0x7d,0x78,0x83,0x76,0x9b,0xf1,0x6,0x9a,0x72,0x16,0x89,0x1,0x19,0x89,0x32,0x3,0x7c,0x88,0x80,0x16,0x7d,0x87,0x57,0x80,0x96,0x7c,0x57,0x79,0x8e,0xd1,0x9d,0xb1,0x9e,0xd2,0x7f,0x66,0x66,0x56,0x55,0x7c,0x54,0x3e,0x31,0x97,0x72,0x4e,0x80,0x4,0x80,0x9,0x84,0xe4,0x81,0xf1,0x7f,0x46,0x56,0x44,0x45,0x7c,0x54,0x83,0x13,0x82,0x25,0x82,0x73,0x7c,0x44,0x8d,0x72,0x7d,0x33,0x46,0x6,0x7f,0x34,0x66,0x88,0x43,0x7d,0x32,0x35,0x9,0x7d,0x31,0x23,0x6,0x7c,0x12,0x49,0x3,0x7d,0x31,0x11,0x3,0x80,0x25,0x7c,0x21,0x49,0x7c,0x43,0x3,0x80,0x34,0x7c,0x22,0x26,0x43,0x79,0x7c,0x32,0x56,0x63,0x73,0x33,0x80,0x38,0x84,0x3,0x81,0x56,0x82,0x3,0x82,0xc3,0x7c,0x44,0x66,0x7c,0x44,0x56,0x85,0xc3,0x85,0xf4,0x76,0x7c,0x34,0x36,0x9,0x84,0xe1,0x7c,0x35,0x85,0x3,0x7c,0x22,0x19,0x7c,0x32,0x6,0x81,0x87,0x82,0x25,0x80,0x96,0x86,0xd2,0x7c,0x42,0x56,0x7d,0x52,0x32,0x19,0x7c,0x53,0x88,0x92,0x9,0x16,0x7c,0x32,0x13,0x23,0x79,0x7d,0x62,0x22,0x79,0x7c,0x62,0x56,0x7c,0x63,0x3,0x8b,0x52,0x19,0x6,0x80,0x16,0x59,0x69,0x7c,0x73,0x76,0x7c,0x72,0x56,0x7c,0x73,0x73,0x83,0x52,0x7c,0x62,0x66,0x7c,0x62,0x13,0x84,0xd2,0x29,0x80,0xa5,0x7d,0x72,0x22,0x80,0x87,0x82,0xe5,0x43,0x7c,0x61,0x59,0x7d,0x78,0x61,0x49,0x3,0x80,0x25,0x59,0x80,0x69,0x87,0x21,0x81,0x74,0x7c,0x71,0x3,0x83,0xa1,0x7c,0x78,0x83,0xe1,0x79,0x6,0x7c,0x68,0x86,0x92,0x7c,0x78,0x6,0x13,0x23,0x7c,0x68,0x80,0xf4,0x83,0x52,0x6,0x80,0x16,0x7c,0x12,0x36,0x7f,0x22,0x68,0x74,0x12,0x7c,0x33,0x79,0x7c,0x23,0x9,0x7e,0x63,0x43,0x44,0xb4,0xd2,0xac,0x71,0xa7,0xb1,0x7e,0x77,0x11,0x11,0x95,0x52,0x9b,0x7a,0x7c,0x67,0x9d,0xe3,0x9d,0x34,0x9a,0xc2,0x7c,0x78,0x1d,0x29,0x21,0x7d,0x77,0x76,0x6,0x4a,0x7c,0x85,0x9d,0x43,0x9e,0x76,0x7d,0x55,0x44,0xbf,0xe1,0x1f,0x9e,0xef,0xa0,0x1b,0x7e,0x66,0x64,0x54,0x82,0x41,0x7c,0x55,0x1,0xa0,0xb5,0x82,0x19,0x17,0x85,0x52,0x7d,0x77,0x66,0x87,0xd2,0x7d,0x45,0x56,0xa4,0x81,0x7c,0x34,0xa,0x7c,0x33,0x1a,0x7c,0x23,0x85,0x21,0x7c,0x57,0x89,0xc1,0x89,0x41,0x9,0x7f,0x36,0x88,0x23,0x21,0xbc,0x81,0x99,0x91,0xc4,0x61,0x7c,0x23,0x6,0x7d,0x32,0x13,0xb5,0xe1,0x7d,0x33,0x16,0x6,0x7c,0x17,0x19,0x7d,0x22,0x27,0x26,0x7c,0x37,0x16,0x9,0x7e,0x77,0x31,0x28,0x9,0x7d,0x22,0x38,0x16,0xc1,0xb1,0x7d,0x77,0x21,0xc3,0x31,0x9,0xc6,0x91,0x39,0x3,0x16,0xa5,0x31,0x29,0x83,0x42,0x3,0x80,0x7,0x7d,0x77,0x14,0x46,0x7c,0x15,0x56,0x7c,0x26,0x66,0x84,0x51,0xcb,0xa1,0x3,0x83,0xc2,0x3,0x80,0x34,0x84,0xc2,0x3,0x80,0x7,0x80,0xa5,0x8f,0x72,0x80,0x8f,0x81,0x8f,0x82,0x8f,0x84,0x7,0x85,0xc7,0x13,0x8b,0xc2,0x87,0xb4,0xae,0x71,0x3,0x8a,0x52,0x80,0x7,0x56,0x7c,0x32,0x80,0x96,0x8b,0x74,0xd5,0x22,0x7c,0x32,0x8d,0x12,0x7c,0x32,0x8d,0x92,0x7c,0x23,0x8d,0xc3,0x16,0x7c,0x33,0x29,0x7d,0x87,0x23,0x8f,0x92,0x8f,0xf4,0x90,0x83,0x7c,0x21,0x36,0x7c,0x31,0xd7,0x21,0x7d,0x23,0x42,0x6,0x7c,0x31,0xd2,0xc1,0x7d,0x23,0x41,0x6d,0x7d,0x23,0x34,0x6d,0x7e,0x22,0x35,0x41,0x85,0xe1,0x7f,0x34,0x54,0x33,0x22,0x94,0xf1,0x7c,0x43,0x19,0x97,0x81,0x19,0x9e,0x61,0x7f,0x22,0x44,0x34,0x34,0x7e,0x34,0x44,0x44,0x1,0x9,0x7e,0x34,0x33,0x33,0x1d,0x3,0x2f,0xb8,0xe5,0x11,0x7d,0x11,0x22,0xbc,0x51,0xbb,0x1f,0xbc,0x1f,0xcd,0xc1,0x9b,0xa2,0xbe,0x15,0xbe,0x48,0x7d,0x54,0x55,0xbf,0xe1,0xbe,0x9f,0xbf,0x9f,0x86,0xa2,0x7c,0x33,0x9f,0x78,0xc1,0xd,0x9f,0xb3,0x9f,0xe3,0xc4,0x51,0x2,0x80,0x5,0xc4,0x45,0xc4,0x94,0x72,0x81,0xa7,0x82,0x26,0x80,0xd,0x80,0xcf,0x81,0xcf,0x82,0xde,0xac,0x13,0xad,0x5,0x21,0xad,0xc3,0x92,0x1,0x7c,0x47,0xda,0x71,0x7d,0x54,0x45,0x5,0x8c,0x31,0xa9,0xf1,0x91,0xa1,0x7c,0x45,0x91,0x85,0x91,0xf5,0xaa,0xc3,0xb1,0xc1,0x93,0x71,0xcc,0xd7,0x55,0x7c,0x45,0xae,0x34,0xcd,0x5f,0xce,0xd7,0x85,0x13,0x8a,0xf8,0xd0,0xa,0xaf,0x7,0x73,0xd1,0x1f,0xd1,0xb7,0xc7,0x82,0x97,0xc1,0x91,0xf,0xb1,0x56,0x6,0x7c,0x76,0x91,0x1a,0x83,0xd9,0x90,0xe9,0xb9,0x41,0x55,0x7c,0x73,0x32,0x7c,0x72,0x42,0x9f,0x71,0x59,0x7c,0x31,0x3,0x80,0x7,0x80,0x96,0x7c,0x21,0x3,0x80,0x7,0x96,0xf5,0x7d,0x63,0x11,0x82,0xc2,0x6,0x82,0xf,0x7d,0x77,0x51,0x84,0x96,0x3,0x96,0xef,0x3f,0x81,0x47,0x81,0xc7,0x7c,0x62,0x7a,0x7c,0x52,0x83,0x16,0x87,0xc3,0x3,0x80,0x7,0x33,0x9d,0xd6,0xdd,0x21,0x8a,0x42,0x86,0xc3,0x83,0xf,0x7c,0x74,0x9d,0x3f,0x9e,0x3f,0x9f,0x3f,0x7b,0xa3,0x63,0xa3,0xb3,0x7c,0x44,0xe4,0x11,0x7d,0x77,0x55,0x9b,0x81,0xc8,0x51,0xe8,0x42,0x29,0x9b,0x53,0xe6,0x71,0x9c,0x3,0xc6,0xc3,0x12,0x9b,0x78,0xc5,0xd1,0xce,0x22,0x94,0x92,0xe9,0x5f,0xea,0x9c,0xfb,0x92,0xeb,0x5f,0xcb,0x61,0xa1,0xf1,0xec,0x9f,0x99,0x6e,0x7d,0x78,0x22,0xe9,0x11,0x7c,0x11,0xad,0x3c,0xad,0x42,0x9b,0x87,0xad,0x54,0x7c,0x65,0xe,0x87,0xe2,0xae,0x6f,0xac,0xbf,0xad,0xec,0x7f,0x51,0x11,0x14,0x62,0xd9,0x31,0xd9,0x32,0xa,0x7d,0x11,0x34,0x21,0x7c,0x15,0x96,0x61,0x6,0x93,0xf1,0x7c,0x16,0x6,0x7c,0x47,0xda,0xd2,0x3,0xf8,0xd1,0x3,0x80,0x7,0x49,0x7c,0x67,0x83,0x31,0x49,0x7d,0x72,0x11,0xf3,0xc1,0x7d,0x57,0x31,0x82,0xd1,0x99,0x21,0x82,0x91,0x9d,0xe2,0x73,0x82,0xe1,0x7f,0x76,0x11,0x36,0x77,0x7c,0x75,0x83,0x78,0x82,0x71,0x59,0x85,0x92,0xf6,0x41,0x21,0x2a,0xa0,0x52,0x91,0x75,0xb5,0xbf,0xa7,0x82,0x7c,0x55,0x9b,0x42,0x81,0xf4,0x82,0x83,0x25,0x7d,0x13,0x65,0x31,0x84,0x71,0x83,0xd2,0x3,0x16,0x87,0xf1,0x3,0x7d,0x11,0x12,0x36,0x7c,0x13,0x3,0x80,0x7,0x80,0x96,0x56,0x7c,0x52,0x7a,0xfc,0xc1,0x7c,0x14,0xd8,0x21,0x75,0x88,0x21,0x8b,0x43,0x82,0xc7,0x83,0x74,0x79,0xfc,0xc1,0x89,0x71,0x73,0x88,0x38,0x1a,0xe8,0xc1,0x7d,0x64,0x33,0xbe,0xbb,0x7c,0x54,0x8e,0x52,0x7c,0x45,0xe3,0xe2,0xe6,0x91,0x7c,0x35,0x9e,0x62,0xba,0x34,0xe4,0xb3,0x39,0x7d,0x45,0x45,0x99,0x38,0xc4,0x5,0x7d,0x65,0x65,0xe7,0x7f,0xe8,0x8f,0xe9,0xbf,0x9d,0xd2,0xcf,0xf4,0xea,0xff,0x9d,0x46,0xee,0xc3,0xf1,0x92,0x9d,0xc3,0xe9,0xe1,0xbe,0x38,0x4e,0xd0,0x91,0xcb,0x9,0x87,0x41,0x71,0xc5,0xb4,0xb8,0xaf,0xca,0xcf,0xcb,0xde,0x91,0x41,0x9b,0xd6,0x9a,0xa2,0x3,0x94,0x43,0x2,0xce,0x2f,0xcf,0x98,0x92,0xc7,0xa1,0x42,0x7c,0x57,0x9f,0xc7,0xfd,0xc1,0x49,0x9,0x97,0x96,0x84,0xe2,0x84,0x74,0x9f,0x62,0x7d,0x11,0x26,0xa1,0xb4,0x7c,0x76,0x32,0xbb,0x8f,0x84,0x6d,0x82,0x83,0x9c,0xb4,0xa5,0xf1,0xac,0x51,0x2d,0xc1,0x1,0x9c,0x71,0xa1,0xe2,0x7c,0x57,0xa2,0x13,0x80,0x16,0x23,0x7d,0x47,0x74,0x9f,0x62,0xa3,0xf1,0x6,0x9a,0xe2,0x6,0x7c,0x26,0x3,0x7c,0x76,0x82,0xd3,0xa,0xa4,0xe3,0x75,0xb8,0x71,0xbe,0x1,0xa9,0xd3,0x82,0xb8,0x84,0x34,0x9f,0xa5,0x59,0xab,0xd3,0xa3,0x34,0xa9,0x62,0xbb,0x2,0xdc,0x6f,0x9e,0x21,0xff,0xa1,0x9,0xd8,0x32,0xeb,0x51,0x7c,0x43,0x5,0x15,0xe5,0x51,0xeb,0x51,0x9d,0xc1,0xe2,0x9a,0xd0,0x33,0x75,0xeb,0x81,0xe9,0x4f,0xbb,0xbf,0xbc,0x18,0xec,0x66,0x7c,0x33,0xef,0x91,0x9d,0xf,0xec,0x35,0x3,0x7c,0x87,0xf6,0x62,0x9d,0x96,0xe7,0xe3,0xd4,0xbf,0xd7,0x83,0x9d,0x91,0x9d,0xa2,0xeb,0xa2,0xee,0x97,0xe7,0xff,0xe8,0xff,0xea,0x4a,0xda,0xb2,0xce,0xc3,0xbc,0x41,0xd6,0x25,0xcf,0xc3,0x3,0x16,0xb8,0x81,0x7c,0x12,0x62,0x3,0x7c,0x13,0x16,0xae,0xa1,0xbb,0x11,0x7c,0x43,0xd8,0x82,0x7c,0x53,0x72,0x7c,0x64,0x72,0xbd,0x32,0x1d,0xbf,0xa1,0x7d,0x47,0x75,0x45,0x7d,0x58,0x72,0xbe,0xe2,0x7c,0x88,0xba,0x3,0xbd,0x41,0xd,0xbd,0xb2,0xbd,0x92,0xbf,0xb1,0x81,0xf3,0x19,0xb3,0x83,0xb3,0xe5,0xf1,0x3f,0xda,0xc,0xc0,0xf2,0x9b,0x12,0x82,0xd3,0xb6,0x53,0xc3,0xc1,0x9a,0x13,0x13,0xb9,0x71,0xc4,0x63,0xc4,0xe3,0xb9,0xb1,0xc0,0xf2,0x3,0xd8,0x81,0x9c,0x92,0x89,0xcb,0xe1,0xd2,0xc4,0x52,0x7c,0x62,0x88,0xd3,0x8a,0x74,0xc4,0x41,0xdb,0xd6,0xa0,0xc1,0xe9,0x42,0xe5,0x3,0xe6,0xb,0xe7,0x83,0xe0,0x43,0xe2,0x8f,0xfb,0x98,0x9c,0xe2,0x7c,0x65,0x98,0xe1,0x98,0xe2,0xf5,0x62,0xf7,0x1,0xbc,0xc2,0xbd,0x2,0xf5,0x87,0xff,0xc3,0xf0,0x51,0xd4,0x71,0xf0,0xce,0xf1,0xba,0xf5,0xb2,0x9a,0xd2,0x9d,0x65,0xf5,0x79,0xfb,0x52,0xd4,0xf8,0x9d,0x38,0xe1,0xa2,0x2,0xb9,0xb7,0xb6,0x61,0x7c,0x15,0xd9,0xf1,0x7d,0x66,0x68,0xf4,0x51,0xf2,0xbf,0x7c,0x67,0xf5,0x62,0xfe,0x52,0x7c,0x54,0x9e,0xcf,0xee,0x8f,0xef,0x8f,0x83,0x74,0x7a,0xd6,0x12,0x7c,0x77,0xd6,0x12,0x80,0x34,0x9c,0x71,0xd7,0xc3,0xd8,0x7,0xd9,0x56,0xb8,0x3,0xb8,0x87,0x13,0xbe,0xe2,0xba,0x83,0x7c,0x58,0xd9,0xe2,0xb9,0xb4,0x56,0x25,0x7c,0x37,0xb1,0x56,0x2d,0x13,0x98,0xd5,0xd4,0xf1,0xb6,0x92,0xbd,0x35,0xa0,0xb4,0xf7,0xaf,0xf8,0xeb,0xa4,0xcb,0xb8,0x65,0x9c,0x96,0x9d,0x43,0x7d,0x15,0x67,0xe4,0x31,0x7d,0x16,0x57,0x9c,0x42,0xb8,0x13,0x7c,0x67,0x9,0x7c,0x46,0x6,0x7c,0x57,0x26,0x7d,0x67,0x45,0xc3,0x62,0x91,0x42,0x7e,0x67,0x11,0x24,0xe6,0x61,0x7d,0x12,0x42,0xc2,0xc1,0xc7,0xf4,0x8c,0x22,0x7c,0x13,0xba,0x74,0xde,0x3,0xc6,0x47,0xde,0x16,0xdd,0xc3,0xc5,0x56,0xfa,0xf,0xfd,0xc7,0xf2,0x92,0x7d,0x55,0x33,0xba,0x21,0xbb,0xf1,0x7c,0x43,0xba,0x61,0xf8,0x72,0x2,0xb7,0x83,0xf0,0x65,0xf0,0x43,0xec,0x73,0xf7,0xf,0xf7,0xd3,0xf7,0x4,0x9d,0xd2,0xf3,0x22,0xf7,0xf2,0xf7,0xe4,0xf8,0xd9,0x7e,0x43,0x32,0x22,0xf6,0x1d,0x9c,0xe8,0xd9,0x74,0xd9,0x87,0x9c,0x92,0xf5,0x78,0xb7,0x88,0xf7,0xa2,0xe2,0x12,0x7c,0x44,0xf7,0xa3,0xbc,0xcf,0xf7,0x3f,0xf8,0x3f,0xfb,0x63,0xf2,0xf2,0xb1,0x12,0x94,0x78,0x95,0xb,0x80,0x6d,0xf7,0x11,0xb0,0xa1,0x3,0xfb,0x21,0x19,0x7e,0x61,0x11,0x23,0xf7,0x82,0x7c,0x11,0xd2,0xe1,0x7d,0x14,0x32,0x55,0x7c,0x25,0xb1,0x21,0xf8,0xb1,0xf4,0x3,0x9c,0x43,0x3,0x9c,0xa5,0x3,0xdb,0x62,0xd3,0xd2,0xb9,0xa1,0xf4,0x13,0xff,0xf,0xf8,0x2a,0x7c,0x17,0xff,0x42,0xfe,0xd1,0x9,0xfe,0x91,0x19,0xfa,0x31,0x80,0x25,0x80,0x47,0xc5,0x65,0x80,0x7,0x80,0xf,0x73,0xfe,0xd1,0xfd,0x62,0xdf,0xe1,0xa1,0x43,0xc3,0xb1,0xe6,0xc2,0xc8,0x43,0x82,0x8f,0x83,0x8f,0x73,0xeb,0x3f,0x9d,0x87,0x1,0x9d,0x91,0x7d,0x32,0x23,0x99,0x11,0x7d,0x33,0x42,0xd8,0x81,0x7c,0x34,0xd5,0x91,0xa,0xef,0x7,0x9d,0xf,0xbb,0x7f,0xf4,0x91,0xa0,0x35,0xb9,0x94,0xbc,0xca,0xdd,0x1,0x7c,0x23,0xdd,0x21,0xbd,0xca,0xf2,0xdf,0x33,0x7c,0x12,0xd0,0xc1,0x7c,0x22,0xf5,0x5a,0x7d,0x88,0x76,0xed,0x51,0x5,0x9d,0x87,0xc4,0xb2,0xf7,0xc2,0x7c,0x54,0xda,0xcf,0xf7,0x5f,0xf8,0x5f,0xdd,0x83,0xf6,0x82,0x7c,0x31,0xf2,0x2,0x7c,0x21,0x19,0xef,0x1,0xf4,0xd1,0x9,0xf3,0x11,0xd0,0xb5,0xf3,0xd1,0xdb,0x21,0x19,0xf0,0x81,0xf9,0xe1,0xed,0x1,0xf6,0xd1,0x6,0xec,0x1,0x88,0x11,0x7d,0x66,0x21,0xb1,0x91,0x7c,0x54,0xf6,0x41,0xb2,0xd2,0x7d,0x15,0x33,0x12,0x7f,0x12,0x22,0x12,0x32,0x7e,0x12,0x21,0x14,0x6d,0x61,0x7c,0x55,0xf8,0x21,0x7d,0x11,0x65,0x82,0x72,0x7c,0x56,0x2a,0xb6,0x2,0xfd,0x41,0xa0,0x11,0x84,0x5,0x84,0x67,0xff,0xf,0xfc,0x6d,0xf3,0x83,0x80,0x5,0x7c,0x14,0xff,0x92,0x3,0x80,0x7,0x80,0xf,0x73,0x7d,0x13,0x56,0xae,0x61,0xff,0xd2,0x7c,0x15,0x3,0x16,0x91,0x91,0xff,0xd2,0xdd,0x71,0xce,0x16,0x3,0x83,0x4f,0x84,0xd6,0x85,0xcb,0xf6,0xff,0xbb,0x69,0x7d,0x56,0x55,0xd8,0x21,0x1,0x7d,0x22,0x33,0x8d,0xb1,0x7f,0x43,0x23,0x23,0x32,0x9e,0x1,0xf6,0xe1,0xd1,0xf3,0xd9,0x96,0xdf,0xb2,0xbb,0xf,0xf6,0xf5,0xd9,0x23,0xb8,0xc2,0xbb,0xd2,0xd,0x25,0xbe,0xa4,0xf7,0x78,0x84,0xa1,0x9d,0xf1,0xf6,0x51,0xd8,0xc3,0xf9,0xb3,0x6e,0xfe,0xcf,0xd4,0x72,0xbe,0x1,0xfa,0x41,0xf6,0xf4,0xfa,0xf6,0xfe,0x81,0x99,0x73,0xbb,0xb4,0x7e,0x77,0x44,0x54,0xbb,0xe1,0xe2,0xe1,0xff,0x52,0xbe,0xb2,0xf7,0xf,0xf7,0xcf,0xf6,0xee,0xd8,0xa5,0x3,0xd9,0x7,0x3,0xf3,0xd1,0x59,0xd7,0x11,0xd9,0x65,0xd9,0x7,0xd9,0xc7,0xda,0x16,0xcf,0xf4,0x7c,0x46,0xee,0xb2,0xf8,0x81,0xfa,0x81,0xdd,0x62,0xdb,0x3,0xf6,0x83,0xf4,0x15,0xf7,0x65,0xbb,0xfb,0xff,0xf,0xe1,0xad,0xff,0xa1,0x3,0x13,0xb7,0xa5,0x80,0x7,0x80,0xf,0x81,0xf,0x83,0x43,0xfa,0x31,0xf4,0xd6,0xf6,0xa1,0xa8,0xf3,0xc8,0x83,0x83,0x8f,0x82,0x38,0x85,0xcb,0xf7,0x2f,0xd9,0x87,0xd4,0xa2,0x9d,0xd3,0xab,0xb1,0x7c,0x22,0x9d,0xc1,0x97,0x32,0xbc,0x21,0xf8,0x3,0x9d,0xb4,0xd2,0x93,0xd9,0xf,0xf7,0xc5,0xf4,0xc2,0xd9,0x34,0xdc,0x51,0xda,0x64,0x98,0x43,0x9c,0xe2,0xfb,0x7,0x7d,0x23,0x24,0xc0,0xf1,0x9d,0x67,0x73,0xf7,0x53,0xf1,0x8d,0x9,0xbe,0x3,0xb6,0xc3,0xdd,0xf2,0xfb,0x44,0xa4,0x83,0xb7,0xc5,0x15,0xc5,0x3,0xa6,0xd1,0x9f,0xd2,0xf9,0xd1,0x7c,0x53,0xf9,0x56,0x83,0x5c,0xf8,0xf,0xd9,0x4a,0x7c,0x66,0xee,0x82,0x7c,0x11,0x3,0xf2,0xd2,0x7c,0x33,0xf3,0x25,0xf3,0x87,0x80,0xb,0xf4,0xc7,0x82,0x47,0xf1,0xb4,0x7c,0x15,0xbd,0x1,0xed,0x3,0xd1,0xe3,0x82,0x8f,0x82,0xf,0xb2,0xf,0xbb,0xf,0xfc,0x92,0xbb,0x82,0xed,0x83,0x7c,0x15,0xf6,0xa1,0x9,0x7c,0x78,0xf6,0x62,0x3,0x7c,0x78,0xe0,0x21,0x29,0xc4,0x52,0x3,0x80,0x74,0x81,0x16,0x82,0x12,0xbe,0xd6,0x7c,0x36,0xcd,0x61,0xf5,0x31,0xbb,0xe3,0x7d,0x11,0x22,0xfa,0xa1,0x71,0xff,0xe1,0x7c,0x32,0xf7,0xe2,0x7c,0x15,0xf8,0xa1,0x7d,0x14,0x15,0xfc,0x12,0x3,0x7c,0x15,0xe1,0xe2,0xf9,0x51,0xfb,0x62,0xdb,0x52,0x8d,0x49,0xfe,0x31,0xf7,0x2f,0xf8,0xd4,0x7e,0x75,0x54,0x44,0xbb,0xa1,0x84,0x82,0x7c,0x12,0xd2,0xc1,0xbc,0x12,0x7c,0x21,0xd5,0x41,0xd5,0x71,0xda,0x1,0x9d,0xd2,0xf7,0xf4,0x9d,0xc3,0xf2,0x25,0xd8,0xaf,0xd8,0xf4,0xf8,0x22,0xf7,0x53,0x16,0xf5,0x92,0x3f,0x9d,0x69,0xd9,0xc2,0x9b,0x83,0xb7,0x65,0x6f,0xf1,0x6f,0xd4,0x52,0xc2,0x42,0xc2,0x43,0x7c,0x66,0xfe,0x47,0xbb,0xe1,0xb7,0xa3,0xe2,0xc7,0xd9,0xc1,0xe2,0x71,0x7d,0x43,0x44,0xa7,0x53,0xdf,0x72,0x7c,0x76,0xfe,0x53,0xf7,0x4f,0xf8,0x4f,0xfb,0x74,0xcb,0x3,0xb8,0xd6,0x7c,0x33,0xcd,0x97,0xfa,0xdf,0xd3,0xcb,0xbd,0x3,0xbd,0x16,0xfd,0x4f,0xfe,0x4f,0xd7,0x34,0xbe,0x87,0xd8,0xff,0xf7,0xdf,0xf9,0x66,0xfa,0x13,0xc2,0x82,0x7c,0x77,0xdc,0xa3,0x13,0x23,0xd6,0xc3,0x80,0x47,0x81,0x7,0xc5,0xc2,0x82,0x74,0x82,0xcb,0xfd,0xf,0x73,0xcd,0xd3,0xdb,0x43,0x84,0x47,0xc9,0xb4,0xda,0x43,0xc7,0x61,0xf6,0xff,0x37,0x7c,0x73,0xd0,0x61,0xe8,0x92,0xde,0xd3,0x9e,0x12,0xfc,0xc1,0xda,0x2,0xd9,0x92,0xef,0x92,0x2,0xd1,0x92,0xf3,0x16,0xf7,0xf,0x9c,0xd4,0xd3,0x43,0x2,0xd5,0xa3,0xb9,0x93,0x97,0x84,0x3d,0xf7,0x98,0x9b,0x83,0x9d,0x78,0xf1,0x15,0xd8,0xf3,0xf6,0x2,0xd9,0x73,0x23,0xbb,0xc3,0xe0,0x25,0xbd,0x73,0xf9,0xe3,0x62,0xbb,0xa5,0xe3,0x3,0xf7,0xb1,0xf8,0x3,0xa7,0x92,0xc5,0x11,0x7c,0x53,0xa,0xbf,0xc2,0x7c,0x64,0x66,0x9d,0x4f,0xfc,0x7f,0xaa,0xd7,0x7c,0x55,0x8d,0x74,0xa1,0xc1,0xef,0xbb,0xab,0xd2,0x7c,0x21,0xe8,0x11,0xab,0xe2,0xdf,0x42,0xa5,0x73,0x8d,0x5a,0xef,0x51,0xe6,0xde,0xe5,0x91,0xaa,0x83,0x11,0x12,0xe7,0x72,0xac,0x11,0x2d,0x25,0xe8,0x62,0xe2,0x65,0xab,0xb4,0x7e,0x22,0x27,0x63,0x88,0xb3,0xe3,0x9,0x8d,0x31,0xc7,0x51,0xa5,0xa3,0x8e,0x76,0x21,0x7d,0x67,0x43,0xd0,0x51,0xd0,0x52,0x9,0xf9,0xbd,0xaa,0x25,0xad,0x43,0x7f,0x44,0x45,0x45,0x54,0x7c,0x23,0xd2,0xd4,0xa,0xf1,0xe2,0x5d,0x7c,0x76,0xcf,0x12,0xee,0x21,0xf8,0x81,0xec,0xc3,0x7c,0x67,0x90,0xe3,0xad,0x3,0x7c,0x68,0x84,0xb7,0xb8,0xa3,0xfe,0x5f,0x81,0x16,0x63,0x80,0x7,0x80,0xf,0xea,0xf3,0x3,0x80,0x7,0x80,0x5a,0xf0,0xc3,0x3,0x16,0x7d,0x75,0x66,0xf4,0xc1,0x97,0x11,0x7c,0x78,0xfa,0xa1,0x45,0x9,0x7f,0x76,0x64,0x34,0x55,0xa0,0xe7,0xa1,0x95,0x26,0x2d,0xd6,0xd1,0xa1,0xc3,0x7c,0x32,0x93,0xa4,0xfc,0x61,0xa1,0xb4,0xf4,0xd3,0xfb,0xf,0xfb,0x32,0xfa,0x34,0xff,0xa5,0xfb,0xa2,0xfa,0x32,0xa2,0xd4,0xf6,0x56,0x7b,0xf7,0x91,0xf9,0xc1,0xf4,0xaa,0xbe,0xb3,0x93,0x14,0x88,0xd3,0x22,0xba,0x51,0x91,0xc3,0xfa,0x1,0xc7,0xb1,0xc6,0xb1,0xc2,0x34,0xde,0xe6,0xc5,0x24,0xb,0xf8,0x52,0x7d,0x88,0x45,0x8b,0x11,0x7e,0x88,0x34,0x45,0x92,0x61,0x7d,0x34,0x46,0x13,0x26,0x7c,0x56,0x3,0x49,0x7c,0x75,0x16,0x7c,0x87,0x26,0x7c,0x88,0x3,0x82,0x12,0x3,0xff,0xb1,0x46,0x7c,0x86,0x46,0x3,0x16,0x7c,0x78,0x3,0x46,0x7c,0x68,0x76,0x7d,0x78,0x67,0x9e,0xa3,0x46,0xe8,0x72,0x80,0x5,0xe4,0xf2,0x3,0x80,0x16,0xc5,0xb1,0x3,0x80,0xc7,0xed,0x12,0x3,0x80,0x7,0x80,0x87,0x85,0xd5,0x3,0x80,0x7,0x80,0xf,0x81,0xf,0x82,0xf,0x83,0x78,0x8a,0xb7,0xcf,0x62,0x3,0x80,0x34,0xa0,0xc1,0x3,0x13,0x6d,0x3,0x80,0x7,0x80,0xf,0x7c,0x68,0x3,0x80,0x16,0xd7,0x41,0x3,0x8e,0x56,0x80,0x7,0x46,0xad,0xa3,0xf7,0x93,0x3,0xf3,0x64,0x8f,0xd2,0x3,0x7c,0x86,0x16,0xd1,0xe1,0x79,0x7c,0x56,0x36,0x7c,0x45,0x94,0x92,0x7c,0x44,0x94,0x96,0x7c,0x22,0x26,0x7c,0x23,0x3,0x96,0x92,0xfd,0xa1,0x96,0xb1,0xfd,0xd2,0x7c,0x56,0xb3,0x5a,0x7c,0x78,0xfd,0x1e,0xfa,0x54,0xfa,0xa4,0x7c,0x86,0xfd,0xb3,0x7c,0x74,0xf7,0x92,0x7c,0x72,0xf7,0xc3,0x7c,0x23,0xb3,0x4a,0x85,0x21,0xae,0xf2,0xad,0x81,0xfe,0xe2,0xc0,0xa1,0xdd,0x13,0x9e,0xd1,0xb2,0xd2,0xc1,0xb3,0x80,0x34,0xc1,0xd2,0xb1,0xa5,0x11,0xdd,0xe1,0x7c,0x15,0x61,0x7d,0x88,0x14,0x9a,0x91,0x7d,0x88,0x16,0xfe,0x2,0x7c,0x17,0x3,0x80,0x7,0x80,0xf,0x81,0xf,0x82,0x2d,0x7c,0x78,0x82,0xcf,0x83,0xcf,0x85,0x38,0xbf,0xc2,0x86,0x25,0xeb,0xe1,0x86,0xf,0x87,0xf,0x88,0xf,0x73,0x7c,0x18,0x89,0x8f,0x8a,0xf8,0xf0,0xe1,0x63,0x73,0x80,0x7,0x39,0xf2,0x61,0x49,0xfe,0xa1,0x7c,0x38,0xc7,0x5,0xc7,0x53,0xa8,0x52,0xc2,0x1c,0xf4,0x71,0x7c,0x76,0x49,0x7c,0x84,0xc4,0x3,0xd3,0xe2,0xf3,0x69,0x7f,0x32,0x76,0x13,0x23,0xf5,0xc3,0xc6,0x59,0x93,0x43,0xb3,0x42,0xc8,0x33,0xb3,0xa2,0xc8,0x92,0x7c,0x66,0xf2,0x1,0x7d,0x55,0x54,0xcf,0x13,0x93,0x14,0xe6,0x3,0xef,0x92,0x7d,0x16,0x66,0xfb,0xf1,0x7f,0x16,0x65,0x35,0x44,0x7d,0x17,0x65,0xf3,0x81,0x7d,0x17,0x75,0x6d,0x7c,0x16,0x3,0x7c,0x75,0x23,0x36,0x7c,0x74,0x3,0x53,0x7d,0x32,0x23,0x26,0x7c,0x24,0x36,0x7e,0x35,0x17,0x64,0x9e,0xf1,0x3,0x69,0xb5,0xc1,0x59,0xa1,0x1,0x3,0x80,0x16,0x7c,0x45,0x39,0xf7,0xe1,0x59,0xa0,0xb1,0x83,0x52,0x82,0xc3,0x3,0x83,0xd6,0x7d,0x36,0x66,0x86,0xe1,0xd2,0x1,0x7c,0x17,0xf6,0x92,0x87,0x43,0x3,0x80,0x7,0x59,0xe0,0x61,0x3,0x80,0x29,0x73,0x84,0x12,0x7f,0x75,0x35,0x66,0x16,0x7e,0x74,0x42,0x12,0x9,0x7c,0x32,0x8a,0xe1,0x19,0xfa,0x11,0x7c,0x73,0xfd,0x81,0x3,0x19,0xfd,0xf1,0x7c,0x15,0x3,0x7c,0x63,0x8a,0x92,0x7c,0x63,0xdb,0xe1,0x9,0x16,0xe0,0xf1,0x7e,0x21,0x16,0x62,0x36,0x7c,0x52,0x66,0x26,0x7d,0x15,0x64,0x86,0xe1,0x7c,0x15,0x8d,0x52,0xa2,0x72,0xf4,0xa1,0xab,0xb1,0x9,0x7c,0x35,0xd7,0x41,0x7c,0x18,0xed,0x32,0x96,0x94,0xd8,0x1,0x7c,0x66,0x80,0x74,0xe6,0x22,0x7c,0x18,0xcb,0x81,0x7c,0x44,0x80,0xf8,0x73,0x80,0xb,0x80,0x8f,0x81,0x4f,0x82,0xf,0x63,0xf2,0xf1,0x83,0xcf,0x85,0x25,0xeb,0x22,0x7c,0x18,0x9d,0x2,0x87,0xf4,0x98,0x52,0xf3,0xf,0xee,0xd2,0xb1,0xf4,0xec,0xc3,0x7c,0x72,0x6,0x7d,0x87,0x65,0xf3,0xa4,0xb9,0x65,0x9d,0x87,0xef,0x62,0xf3,0x38,0xdc,0x1,0xf2,0xb2,0x9c,0xd1,0xd1,0x22,0xe3,0x52,0x26,0xe6,0x52,0xe3,0xc3,0x7c,0x54,0xf2,0x22,0xfb,0x23,0xfa,0x7,0xf5,0x73,0x9b,0x81,0xd9,0xe1,0xfb,0xe1,0x7c,0x43,0xe5,0x11,0xda,0xd1,0xfd,0x11,0xfc,0xd1,0x7c,0x25,0xf3,0xc2,0x7c,0x74,0x5,0x7f,0x67,0x42,0x22,0x36,0x7f,0x84,0x24,0x23,0x68,0x7f,0x53,0x24,0x35,0x88,0x7e,0x42,0x42,0x47,0xbd,0x81,0x7c,0x54,0xb7,0x91,0x7d,0x34,0x68,0xed,0x91,0x7c,0x57,0xed,0xc2,0xcf,0xa2,0x7d,0x87,0x55,0xfe,0x82,0x7d,0x54,0x46,0xd,0x7d,0x63,0x53,0xbe,0xb2,0x7c,0x26,0x3d,0x7d,0x85,0x15,0xc4,0x61,0x7d,0x86,0x42,0xc7,0x12,0x7c,0x74,0xd4,0x73,0x7c,0x45,0x2d,0xd5,0x74,0x7c,0x33,0xd6,0xa1,0xbc,0x31,0xe1,0x71,0xeb,0x73,0x7c,0x66,0xf9,0x52,0xdb,0xa5,0xf2,0x42,0xf8,0x8f,0xf9,0x4f,0xa2,0x41,0xf0,0x71,0xf0,0x61,0x7c,0x55,0x9c,0x1,0xf7,0xd1,0x83,0x33,0xa7,0x21,0xff,0x22,0x7c,0x21,0xe5,0x91,0x1,0x39,0x8a,0x12,0x7d,0x26,0x65,0x29,0xf9,0x92,0xf9,0xc3,0x9f,0xc1,0xfe,0xe1,0x7c,0x21,0x82,0x22,0x26,0x7c,0x25,0xff,0x83,0xe7,0x42,0x96,0x11,0x7d,0x12,0x21,0xf2,0x83,0xfb,0x41,0xf4,0x25,0xf5,0xd3,0x33,0xe6,0xd6,0xf2,0xa1,0x80,0x56,0x63,0xf4,0xc1,0x7d,0x43,0x43,0xe8,0x47,0xff,0x17,0x80,0xb,0x53,0xec,0xa7,0x81,0x4f,0x82,0xf,0x83,0xf,0x84,0xcb,0x99,0x47,0x7b,0xb6,0x42,0x9d,0xf,0xef,0x34,0x7,0xea,0x54,0xfe,0xe3,0xf6,0x32,0xeb,0x14,0xff,0x72,0x9d,0xb7,0x93,0xc3,0x1b,0xfd,0x22,0x7c,0x57,0xba,0xa2,0x7c,0x67,0xfd,0x71,0x9d,0x22,0x6,0xcf,0x62,0xef,0x91,0xbb,0xa2,0xd1,0x51,0x15,0x93,0x12,0x7c,0x55,0xf7,0x41,0x9b,0x91,0xf4,0xc5,0xfc,0x54,0x7c,0x76,0xa1,0x52,0xb0,0x31,0xc0,0xc1,0x7d,0x33,0x25,0x6d,0x7d,0x52,0x54,0x25,0x91,0xb1,0x7d,0x31,0x13,0xbe,0x11,0x7d,0x42,0x24,0x6,0x89,0x52,0x7d,0x21,0x32,0x89,0x92,0x45,0x7d,0x67,0x31,0x29,0xbd,0x91,0x7d,0x77,0x42,0xf8,0x41,0x7c,0x63,0x9d,0x12,0xde,0xb2,0xf2,0x11,0x86,0x31,0x7c,0x65,0xfe,0x91,0x7d,0x54,0x67,0xfd,0x41,0xf5,0x21,0xc1,0x51,0x7d,0x24,0x46,0xa1,0xf2,0xfc,0x71,0x7f,0x43,0x13,0x45,0x33,0x7d,0x65,0x53,0xa2,0xd2,0xf2,0xa1,0xa4,0x71,0xe0,0xa6,0x7c,0x64,0xa5,0x32,0x7c,0x22,0x3,0x98,0x13,0xf9,0x12,0xd9,0x83,0xaa,0x48,0x9d,0x4f,0xfb,0x87,0xfa,0x33,0x7c,0x56,0xa0,0xd6,0xa7,0x52,0x83,0xd2,0x26,0xe1,0x31,0x9d,0x32,0xd,0x85,0x91,0x56,0x97,0x22,0xb2,0x82,0x7d,0x11,0x34,0x87,0x22,0xb1,0x82,0x1f,0x7c,0x11,0xfd,0x92,0x7c,0x33,0x81,0xc6,0x7c,0x11,0xde,0x74,0xab,0x22,0xfe,0x43,0x7c,0x74,0xa,0x8b,0x41,0xb3,0x3,0x7d,0x23,0x44,0xb0,0xa3,0x9d,0x4b,0xa1,0x2,0x7c,0x44,0x9d,0xf,0x9d,0xcf,0x9b,0xb,0x7a,0x7c,0x21,0x9f,0xcf,0x73,0xab,0x83,0xef,0xb3,0x87,0x47,0xeb,0x3c,0x7e,0x45,0x57,0x53,0xfd,0xf4,0x9c,0x4,0x12,0x97,0x63,0xf7,0xc1,0xf4,0x72,0x12,0xed,0xaa,0xbe,0x2,0xef,0xc3,0xee,0x74,0xd6,0x72,0xf3,0x51,0x95,0xe1,0x9b,0x44,0xd7,0x82,0x1a,0x39,0xd7,0xd2,0xd7,0xc1,0x2,0x87,0xc7,0xbc,0x25,0x71,0x87,0x84,0x7c,0x23,0x65,0xce,0x41,0xba,0x1,0x7c,0x43,0xc3,0xe1,0x1,0x7c,0x51,0x8f,0xe7,0x8a,0xbe,0xb1,0x2,0x7c,0x75,0xc5,0xd2,0x7f,0x64,0x47,0x61,0x11,0x7c,0x87,0xd6,0x61,0x7d,0x12,0x77,0xbb,0xc1,0x7e,0x52,0x65,0x31,0x3d,0x7f,0x55,0x21,0x13,0x31,0x7c,0x44,0x83,0x52,0x7e,0x23,0x31,0x64,0xd0,0xb1,0x7e,0x25,0x53,0x11,0xbc,0xf1,0x7f,0x32,0x12,0x46,0x52,0x59,0xd1,0xc2,0x39,0xb5,0xc3,0xb5,0x32,0xfb,0xa1,0xb9,0x78,0xc0,0xf3,0xb8,0xbf,0xb9,0xbf,0x9b,0xd2,0xb9,0xb4,0x7c,0x55,0xb9,0x85,0x11,0xb5,0xa5,0x9a,0x43,0xd6,0xe1,0x29,0xd6,0x71,0xae,0xe3,0x3,0xff,0xa1,0x7c,0x72,0x85,0xc4,0x9d,0x82,0xa8,0x11,0x98,0x65,0xbb,0x33,0x7c,0x12,0xb9,0xa5,0x71,0xeb,0x7,0x7f,0x56,0x65,0x65,0x56,0x9e,0xc3,0xa4,0xf2,0xfc,0x43,0x7c,0x65,0xb9,0xa5,0xfe,0x91,0x9e,0x3,0xd0,0x74,0xce,0x52,0xb9,0xf,0xba,0xda,0xa1,0xb4,0x9a,0x34,0x21,0xb5,0xf8,0x9b,0xf,0xf2,0xc3,0x9b,0xf,0xf1,0x65,0x7c,0x86,0xb4,0x91,0xf3,0xa9,0xb8,0x24,0xb8,0x95,0x46,0xd5,0x59,0xcc,0xa1,0xd5,0xc3,0x7c,0x87,0xfe,0x1,0xb7,0x42,0xf2,0xb1,0xab,0x93,0xd4,0xd5,0x80,0x54,0x37,0x7c,0x75,0xce,0x42,0xf3,0xc1,0x7c,0x45,0x9b,0xc3,0xd5,0x87,0x7c,0x78,0x7a,0x7c,0x77,0xb5,0xc2,0x7c,0x57,0xf5,0x51,0x7d,0x51,0x37,0xb9,0x52,0x7c,0x37,0x69,0x7d,0x42,0x27,0x6,0x13,0x26,0x7c,0x31,0x26,0x7c,0x32,0x36,0x7c,0x33,0x69,0x6,0x66,0x7c,0x47,0x36,0x3,0x7c,0x48,0x80,0x7,0x76,0x56,0x7c,0x41,0x36,0x7d,0x42,0x38,0x83,0x16,0x23,0x33,0x80,0x25,0xbe,0xa1,0x63,0x7c,0x48,0xfb,0xc1,0x59,0xe4,0x2,0x7c,0x78,0xfe,0xf1,0xd9,0xa1,0xd5,0x56,0x80,0x7,0x80,0xf,0x81,0xb4,0xde,0xd1,0x7d,0x34,0x78,0xbd,0x71,0x7d,0x33,0x68,0xfe,0x91,0x7d,0x62,0x58,0xd4,0x91,0x7d,0x62,0x48,0xf3,0xc1,0x9,0x7d,0x31,0x12,0x19,0xfe,0x1,0x3,0x80,0x25,0x7e,0x31,0x22,0x61,0x80,0x1a,0x7d,0x62,0x57,0x9c,0x81,0x82,0x92,0x7e,0x22,0x72,0x57,0xff,0xc1,0x7c,0x72,0xe4,0x92,0x7d,0x63,0x68,0x9b,0xd2,0x8d,0xd2,0x86,0xb4,0x7c,0x68,0xe5,0x41,0xab,0x34,0xd5,0x12,0xe1,0x72,0x9a,0xd6,0xdc,0x34,0xbb,0xc2,0xec,0x56,0x7c,0x77,0xd5,0x1e,0xc8,0x31,0xd2,0xcf,0xa1,0x75,0x7c,0x12,0xd7,0xcf,0x9b,0x69,0x83,0xe5,0xec,0x52,0x7c,0x68,0xf1,0x5a,0x7d,0x87,0x76,0xf3,0x61,0xd3,0x34,0xd0,0x73,0xe5,0x44,0xf1,0x21,0xf2,0xb1,0xd4,0x43,0xb5,0xc2,0x55,0xc9,0xd1,0xd4,0xf4,0xd3,0xc3,0xee,0x62,0x1a,0xf5,0xc1,0xf3,0x11,0x7c,0x64,0xf0,0x51,0x7d,0x68,0x74,0x9c,0x21,0x7c,0x78,0xea,0xa1,0x9,0xcb,0xe1,0x7c,0x65,0x19,0xd3,0x71,0x16,0x26,0xc9,0x21,0x39,0x7d,0x11,0x46,0x49,0x7f,0x86,0x35,0x78,0x65,0x9,0x8d,0x51,0x7d,0x86,0x45,0x16,0x3,0x49,0x3,0x80,0x25,0xed,0xb1,0x36,0x66,0xee,0x71,0x73,0x82,0xa1,0x81,0x47,0x36,0x7c,0x85,0x84,0x61,0x80,0x25,0x63,0x29,0x7c,0x55,0x33,0x23,0x7c,0x85,0x76,0x7c,0x85,0x84,0x92,0x73,0x80,0x34,0x23,0x46,0x7c,0x84,0x46,0x7c,0x84,0x76,0x80,0x34,0x88,0x12,0x6,0x80,0xa5,0x46,0x7c,0x83,0x53,0x87,0x52,0x7c,0x83,0x46,0x3,0x80,0x34,0x81,0x16,0x80,0x7,0x63,0x7c,0x82,0x8b,0x92,0x7c,0x82,0x8d,0x31,0x7c,0x75,0x6,0x7c,0x66,0x16,0x39,0x23,0x8d,0x21,0x7d,0x75,0x86,0x80,0x74,0xe2,0xd1,0x43,0x7c,0x81,0x80,0x65,0x7c,0x75,0x19,0x7c,0x88,0x26,0x3,0xd2,0x51,0x3,0x36,0x43,0x83,0x92,0xe5,0x51,0x82,0xe1,0x76,0xfb,0xf1,0x7c,0x76,0x80,0xd6,0x81,0x87,0x82,0xc3,0x80,0xe5,0x7c,0x56,0x3,0x76,0x3,0x80,0x65,0x80,0xc7,0x7c,0x74,0x79,0x7c,0x87,0x86,0x12,0x7c,0x87,0x19,0x7c,0x56,0x6,0xcf,0x91,0x86,0x52,0xe1,0x61,0x7d,0x81,0x57,0x3,0x16,0xea,0x1,0x81,0x25,0x63,0x80,0x7,0x82,0x65,0x7c,0x73,0x80,0x5a,0x83,0x3,0x7c,0x67,0x56,0x89,0x61,0x53,0xd3,0xd1,0x69,0x83,0x87,0x29,0x79,0xf1,0x1,0x29,0x3,0x19,0x80,0x74,0xfb,0xb1,0x49,0x9e,0x62,0x7c,0x64,0x6,0x7c,0x65,0x3,0x7c,0x76,0x29,0x7c,0x75,0xaf,0x81,0x9,0x46,0x59,0xfe,0x11,0x3,0x59,0x19,0x45,0x29,0x7c,0x75,0xff,0x2,0x7c,0x73,0xea,0x82,0x7c,0x73,0x0
};
#define sprites_gameover_count (337)

static const uint16_t gameover_palette[] = {
0x0, 0xeee, 0xccc, 0xaaa, 0x888, 0x666, 0x444, 0x222, 0x0, 
};
