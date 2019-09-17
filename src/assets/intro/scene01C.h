#include "level_functions.h"

#define tilemap_scene01C_width (12)
#define tilemap_scene01C_height (5)

static const uint8_t tiles_compr_scene01C[] = {
0x7d,0x0,0x0,0x1,0x3,0x80,0x7,0x80,0xf,0x81,0xf,0x82,0xf,0x83,0xf,0x84,0xf,0x85,0xf,0x86,0xf,0x87,0x2d,0x7f,0x2,0x22,0x0,0x22,0x7f,0x22,0x32,0x2,0x23,0x7f,0x23,0x33,0x22,0x23,0x7c,0x32,0x6,0x7d,0x33,0x33,0x89,0xf,0x8a,0xf,0x83,0x13,0x3,0x7a,0x7c,0x12,0x3,0x80,0x7,0x80,0xa5,0x7c,0x1,0x84,0xe1,0x3,0x75,0x80,0x56,0x7c,0x12,0x3,0x7c,0x32,0x79,0x86,0x31,0x79,0x7d,0x33,0x23,0x80,0x25,0x86,0xa1,0x79,0x7c,0x3,0x26,0x84,0x34,0x8f,0xff,0x43,0x89,0x21,0x89,0x51,0x89,0x41,0x7c,0x22,0x88,0xa1,0x2,0xd,0x15,0x7c,0x34,0x89,0x42,0xf,0x7c,0x44,0x21,0x7d,0x33,0x43,0x93,0x69,0x7d,0x3,0x45,0x29,0x7d,0x35,0x66,0x15,0x7f,0x56,0x67,0x0,0x34,0x7f,0x66,0x66,0x3,0x55,0x7e,0x56,0x66,0x24,0x6,0x7d,0x35,0x55,0x29,0x7c,0x45,0x25,0x9,0x39,0x7e,0x76,0x44,0x55,0x2d,0x7c,0x34,0x6,0x7c,0x44,0x2d,0x7d,0x65,0x34,0x3a,0x7f,0x33,0x54,0x56,0x56,0x7f,0x23,0x64,0x45,0x66,0x7f,0x23,0x54,0x44,0x66,0x7d,0x13,0x54,0x82,0x41,0x7f,0x23,0x63,0x44,0x56,0x7d,0x24,0x52,0x82,0x41,0x7d,0x25,0x43,0x19,0x7d,0x23,0x22,0x29,0x7d,0x32,0x22,0x82,0xc1,0x87,0x52,0x7c,0x44,0x6,0x7c,0x43,0x91,0x42,0x91,0x21,0x2d,0x7c,0x23,0x92,0x1,0x7f,0x34,0x34,0x22,0x45,0x7f,0x43,0x43,0x36,0x43,0x59,0x87,0xd2,0x88,0xf2,0x82,0xb1,0x89,0x2,0x12,0x16,0x55,0x7c,0x43,0x71,0x79,0x61,0x7c,0x34,0x1e,0xd,0x7c,0x44,0x65,0x89,0x78,0x7d,0x55,0x44,0x21,0x89,0x41,0x7e,0x65,0x0,0x67,0xd,0x7d,0x60,0x77,0xd,0x7c,0x76,0x19,0x7c,0x67,0xe,0x80,0x34,0x55,0x19,0x7d,0x66,0x77,0x2a,0x69,0xe,0x66,0x52,0x3e,0x55,0x8c,0x1,0x5a,0x7c,0x67,0x80,0x64,0x55,0x16,0x82,0x74,0x83,0xa2,0x1a,0x79,0x13,0x82,0x82,0x8c,0x81,0x8e,0x71,0x89,0x1,0x7d,0x45,0x50,0x91,0x82,0x7c,0x30,0x9a,0xc2,0x8b,0x41,0x91,0x41,0x7c,0x32,0x9,0x87,0x81,0x49,0x21,0x88,0x72,0x16,0x88,0x63,0x80,0x25,0x1e,0x5d,0x8a,0x2,0x5a,0x6e,0x8a,0x22,0xe,0xa7,0xf,0x33,0x7c,0x50,0x4a,0x94,0x71,0x61,0x8a,0xc1,0x6,0x7c,0x60,0x26,0x6,0x7c,0x65,0x2a,0x8a,0xd1,0x6,0x4a,0x7c,0x77,0x23,0x6a,0x53,0x7c,0x76,0x81,0x96,0x80,0x34,0x83,0x62,0x83,0x43,0x52,0x3,0xac,0xbf,0xae,0x28,0x7c,0x20,0x76,0x7c,0x30,0x3,0x7a,0xa5,0xd3,0x94,0x91,0x16,0x8b,0x51,0x9b,0x51,0xa,0x94,0xe4,0x94,0xf2,0x94,0x71,0x95,0xd3,0x96,0x61,0xb1,0xf,0xb2,0xf,0xb3,0xf,0xb4,0xf,0xb5,0xf,0xb6,0xf,0xb7,0xf,0xb8,0xf,0xb9,0xf,0xab,0xa2,0xab,0x81,0x8b,0xa2,0x96,0x41,0x8c,0x41,0xa8,0xc2,0xad,0xd2,0xbb,0x2f,0xbc,0x2f,0xbd,0x2f,0xbe,0x2f,0xbf,0x2f,0xc0,0x2f,0xc1,0x2f,0xc2,0x5c,0x7c,0x11,0xb6,0x12,0x7c,0x21,0xbb,0x52,0x16,0xb6,0x3,0x7c,0x42,0x36,0x7c,0x43,0x36,0x7c,0x43,0xb6,0x12,0x95,0x23,0x3,0xc5,0x2f,0xc6,0x2f,0xc7,0x2f,0x7d,0x0,0x4,0x3,0x7a,0x7c,0x5,0x7a,0x7c,0x6,0x3,0x7a,0x7c,0x35,0x7a,0x7c,0x56,0x3,0x72,0x16,0x7c,0x5,0x26,0xb3,0x61,0x22,0x49,0xb3,0x11,0x23,0x66,0xb6,0x71,0x43,0x7f,0x5,0x53,0x46,0x66,0x7c,0x2,0xb8,0x51,0x7e,0x66,0x12,0x23,0xb7,0xe1,0x7d,0x11,0x12,0x29,0x7d,0x21,0x22,0xb8,0xe1,0x19,0xae,0x71,0x7d,0x11,0x22,0xae,0x1,0x7d,0x21,0x12,0xac,0xb1,0x39,0x7f,0x67,0x77,0x12,0x12,0x7d,0x57,0x87,0xc6,0x51,0x7d,0x57,0x77,0xb5,0xe1,0xb9,0x21,0xcf,0x5a,0x86,0xd2,0x7c,0x6,0xa7,0x42,0xb0,0x91,0x85,0xc2,0x9,0xbb,0x61,0x19,0xbb,0x52,0x41,0x82,0xe1,0xb0,0x92,0x7c,0x67,0xb0,0x92,0x7c,0x77,0x35,0xb2,0x83,0x19,0x7,0xb3,0x94,0xb3,0x41,0xb3,0xf2,0xa,0x2d,0xb3,0xc2,0x56,0xb3,0xf3,0xb2,0x83,0xb3,0xd5,0x3e,0x3,0xb,0xa,0x82,0x12,0xb4,0x12,0x7e,0x86,0x77,0x77,0x1,0x9,0x62,0x81,0x74,0xb4,0x43,0x85,0xa3,0x84,0x85,0xb5,0x53,0xb7,0xe3,0x7c,0x87,0x42,0x7c,0x78,0xae,0x31,0x51,0x7c,0x87,0x8d,0x3,0xb0,0xa9,0xba,0xd1,0xb0,0xb1,0x83,0x92,0x19,0xba,0x1,0xbc,0x21,0xba,0x14,0x87,0x33,0x88,0x92,0x85,0xb2,0x13,0x23,0x88,0xc4,0x4b,0x89,0x35,0xbd,0xf3,0x88,0x24,0x8a,0x26,0x73,0xbc,0xf2,0x6b,0x8b,0x84,0xc0,0x22,0xc0,0x33,0x12,0x13,0xbc,0xf3,0x88,0x35,0x2f,0xcb,0x81,0x6d,0x7c,0x65,0x3e,0x3,0xcb,0xb1,0x16,0xc1,0x62,0x9,0x46,0x8c,0x92,0xc0,0x33,0x22,0x7c,0x57,0x89,0x78,0xe3,0x8f,0xe5,0x7,0xb8,0x47,0x13,0xbd,0x3,0x3,0x80,0x7,0x80,0x78,0xbb,0xda,0x23,0xbc,0x34,0xbc,0x56,0x82,0xc7,0xbd,0x3,0xc0,0xc3,0x7c,0x55,0x3,0x7c,0x42,0x16,0x7f,0x52,0x21,0x10,0x54,0x7d,0x52,0x11,0x9,0xa7,0x11,0x7e,0x11,0x44,0x31,0x9,0x35,0x19,0x7d,0x42,0x22,0x16,0x7c,0x12,0x39,0x7c,0x63,0x9,0x7d,0x12,0x64,0x39,0x7c,0x13,0xd3,0x71,0x7c,0x11,0xe3,0xf,0xed,0xff,0xee,0xff,0xef,0xff,0xf0,0xff,0xf1,0xff,0xf2,0xff,0xf3,0xff,0x7d,0x0,0x10,0xb2,0x13,0xa,0x3,0xa8,0xe1,0x26,0xe6,0xe3,0x6,0xe7,0xe3,0xdd,0xe1,0x79,0x0
};
#define sprites_scene01C_count (60)

static const uint16_t scene01C_palette[] = {
0x0, 0xeee, 0xccc, 0xaaa, 0x888, 0x666, 0x444, 0x222, 0x0, 
};
