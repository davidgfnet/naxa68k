#include "level_functions.h"

static const uint8_t tilemap_tv_planea3[] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,1,6,8,10,14,11,11,17,17,17,17,19,19,19,19,18,18,18,18,20,21,22,22,27,29,31,33,0,0,0,0,0,0,
  0,0,0,0,0,0,0,2,2,2,11,11,11,11,17,17,17,17,19,19,19,19,18,18,18,18,20,20,20,20,3,3,3,34,0,0,0,0,0,0,
  0,0,0,0,0,0,0,2,2,2,11,11,11,11,17,17,17,17,19,19,19,19,18,18,18,18,20,20,20,20,3,3,3,35,0,0,0,0,0,0,
  0,0,0,0,0,0,0,2,2,2,11,11,11,11,17,17,17,17,19,19,19,19,18,18,18,18,20,20,20,20,3,3,3,35,0,0,0,0,0,0,
  0,0,0,0,0,0,0,2,2,2,11,11,11,11,17,17,17,17,19,19,19,19,18,18,18,18,20,20,20,20,3,3,3,36,0,0,0,0,0,0,
  0,0,0,0,0,0,0,2,2,2,11,11,11,11,17,17,17,17,19,19,19,19,18,18,18,18,20,20,20,20,3,3,3,37,0,0,0,0,0,0,
  0,0,0,0,0,0,0,2,2,2,11,11,11,11,17,17,17,17,19,19,19,19,18,18,18,18,20,20,20,20,3,3,3,38,0,0,0,0,0,0,
  0,0,0,0,0,0,0,2,2,2,11,11,11,11,17,17,17,17,19,19,19,19,18,18,18,18,20,20,20,20,3,3,3,38,0,0,0,0,0,0,
  0,0,0,0,0,0,0,2,2,2,11,11,11,11,17,17,17,17,19,19,19,19,18,18,18,18,20,20,20,20,3,3,3,38,0,0,0,0,0,0,
  0,0,0,0,0,0,0,2,2,2,11,11,11,11,17,17,17,17,19,19,19,19,18,18,18,18,20,20,20,20,3,3,3,38,0,0,0,0,0,0,
  0,0,0,0,0,0,0,2,2,2,11,11,11,11,17,17,17,17,19,19,19,19,18,18,18,18,20,20,20,20,3,3,3,38,0,0,0,0,0,0,
  0,0,0,0,0,0,0,2,2,2,11,11,11,11,17,17,17,17,19,19,19,19,18,18,18,18,20,20,20,20,3,3,3,39,0,0,0,0,0,0,
  0,0,0,0,0,0,0,2,2,2,11,11,11,11,17,17,17,17,19,19,19,19,18,18,18,18,20,20,20,20,3,3,3,35,0,0,0,0,0,0,
  0,0,0,0,0,0,0,3,3,3,12,12,12,12,18,18,18,18,12,12,12,12,17,17,17,17,12,12,12,12,2,2,2,40,0,0,0,0,0,0,
  0,0,0,0,0,0,0,3,3,3,12,12,12,12,18,18,18,18,12,12,12,12,17,17,17,17,12,12,12,12,2,2,2,41,0,0,0,0,0,0,
  0,0,0,0,0,0,0,4,4,4,4,15,15,15,15,15,4,4,4,4,4,12,12,12,12,12,12,12,23,25,12,12,12,42,0,0,0,0,0,0,
  0,0,0,0,0,0,0,4,4,4,4,15,15,15,15,15,4,4,4,4,4,12,12,12,12,12,12,12,23,25,12,12,12,43,0,0,0,0,0,0,
  0,0,0,0,0,0,0,4,4,4,4,15,15,15,15,15,4,4,4,4,4,12,12,12,12,12,12,12,23,25,12,12,12,44,0,0,0,0,0,0,
  0,0,0,0,0,0,0,4,4,4,4,15,15,15,15,15,4,4,4,4,4,12,12,12,12,12,12,12,23,25,12,12,12,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,5,7,9,13,16,15,15,15,15,4,4,4,4,4,12,12,12,12,12,12,12,24,26,28,30,32,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

};
#define tilemap_tv_planea3_width (40)
#define tilemap_tv_planea3_height (30)

static const uint32_t tiles_tv_planea3[] = {
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00050555,
  0x00555555,
  0x05555555,
  0x05555555,
  0x55555555,
  0x55555555,
  0x55555555,
  0x55555555,
  0x55555555,
  0x55555555,
  0x55555555,
  0x55555555,
  0x55555555,
  0x55555555,
  0x44444444,
  0x44444444,
  0x44444444,
  0x44444444,
  0x44444444,
  0x44444444,
  0x44444444,
  0x44444444,
  0x66666666,
  0x66666666,
  0x66666666,
  0x66666666,
  0x66666666,
  0x66666666,
  0x66666666,
  0x66666666,
  0x00666666,
  0x00606666,
  0x00000666,
  0x00000006,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x05555555,
  0x55555555,
  0x55555555,
  0x55555555,
  0x55555555,
  0x55555555,
  0x55555555,
  0x66666666,
  0x66666666,
  0x66666666,
  0x66666666,
  0x00666666,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x55555555,
  0x55555555,
  0x55555555,
  0x55555555,
  0x55555555,
  0x55555555,
  0x55555555,
  0x66666666,
  0x66666666,
  0x66666666,
  0x66666666,
  0x66666666,
  0x00066666,
  0x00000000,
  0x00000000,
  0x00000000,
  0x88888888,
  0x88888888,
  0x88888888,
  0x88888888,
  0x88888888,
  0x88888888,
  0x88888888,
  0x88888888,
  0x88888888,
  0x88888888,
  0x88888888,
  0x88888888,
  0x88888888,
  0x88888888,
  0x88888888,
  0xbbbbbbbb,
  0xbbbbbbbb,
  0xbbbbbbbb,
  0xbbbbbbbb,
  0xbbbbbbbb,
  0xbbbbbbbb,
  0xbbbbbbbb,
  0xbbbbbbbb,
  0x66666666,
  0x66666666,
  0x66666666,
  0x66666666,
  0x66666666,
  0x66666666,
  0x00000666,
  0x00000000,
  0x00000088,
  0x88888888,
  0x88888888,
  0x88888888,
  0x88888888,
  0x88888888,
  0x88888888,
  0x88888888,
  0x11111111,
  0x11111111,
  0x11111111,
  0x11111111,
  0x11111111,
  0x11111111,
  0x11111111,
  0x11111111,
  0x11111111,
  0x11111111,
  0x11111111,
  0x11111111,
  0x11111111,
  0x11111111,
  0x11111111,
  0x00000000,
  0x22222222,
  0x22222222,
  0x22222222,
  0x22222222,
  0x22222222,
  0x22222222,
  0x22222222,
  0x22222222,
  0x33333333,
  0x33333333,
  0x33333333,
  0x33333333,
  0x33333333,
  0x33333333,
  0x33333333,
  0x33333333,
  0x99999999,
  0x99999999,
  0x99999999,
  0x99999999,
  0x99999999,
  0x99999999,
  0x99999999,
  0x99999999,
  0xaaaaaaaa,
  0xaaaaaaaa,
  0xaaaaaaaa,
  0xaaaaaaaa,
  0xaaaaaaaa,
  0xaaaaaaaa,
  0xaaaaaaaa,
  0xaaaaaaaa,
  0xaaa00000,
  0xaaaaaaaa,
  0xaaaaaaaa,
  0xaaaaaaaa,
  0xaaaaaaaa,
  0xaaaaaaaa,
  0xaaaaaaaa,
  0xaaaaaaaa,
  0x00000000,
  0xaaaaaaaa,
  0xaaaaaaaa,
  0xaaaaaaaa,
  0xaaaaaaaa,
  0xaaaaaaaa,
  0xaaaaaaaa,
  0xaaaaaaaa,
  0xbbbb7777,
  0xbbbb7777,
  0xbbbb7777,
  0xbbbb7777,
  0xbbbb7777,
  0xbbbb7777,
  0xbbbb7777,
  0xbbbb7777,
  0xbbbb7777,
  0xbbbb7777,
  0xbbbb7777,
  0xbbbb7777,
  0xbbbb7777,
  0xbbbb7777,
  0xbbbb7777,
  0xbbbb0000,
  0x77777777,
  0x77777777,
  0x77777777,
  0x77777777,
  0x77777777,
  0x77777777,
  0x77777777,
  0x77777777,
  0x77777777,
  0x77777777,
  0x77777777,
  0x77777777,
  0x77777777,
  0x77777777,
  0x77777777,
  0x00000000,
  0x00000000,
  0x44440000,
  0x44444444,
  0x44444444,
  0x44444444,
  0x44444444,
  0x44444444,
  0x44444444,
  0xbbbbbbbb,
  0xbbbbbbbb,
  0xbbbbbbbb,
  0xbbbbbbbb,
  0xbbbbbbbb,
  0xbbbbbbbb,
  0xbbbbbbbb,
  0x00000000,
  0x00000000,
  0x00000000,
  0x44444444,
  0x44444444,
  0x44444444,
  0x44444444,
  0x44444444,
  0x44444444,
  0xbbbbbbbb,
  0xbbbbbbbb,
  0xbbbbbbbb,
  0xbbbbbbbb,
  0xbbbbbbbb,
  0xbbbbbbbb,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x44444000,
  0x44444440,
  0x44444444,
  0x44444444,
  0x44444444,
  0x44444444,
  0xbbbbbbbb,
  0xbbbbbbbb,
  0xbbbbbbb0,
  0xbbbbbbb0,
  0xbbbbb000,
  0xbbbb0000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x44000000,
  0x44400000,
  0x44400000,
  0x44000000,
  0x44400000,
  0x44000000,
  0x44000000,
  0x44000000,
  0x44000000,
  0x44000000,
  0x44000000,
  0x44000000,
  0x44000000,
  0x44000000,
  0x44000000,
  0x44000000,
  0x44000000,
  0x44000000,
  0x44000000,
  0x44000000,
  0x44000000,
  0x44000000,
  0x44000000,
  0x44400000,
  0x44000000,
  0x44400000,
  0x44000000,
  0x44400000,
  0x44000000,
  0x44000000,
  0x44400000,
  0x44400000,
  0x44400000,
  0x44000000,
  0x44400000,
  0x44400000,
  0x44400000,
  0x44400000,
  0x44400000,
  0x44400000,
  0x44400000,
  0x44400000,
  0x44400000,
  0x44400000,
  0x44400000,
  0x44400000,
  0x44400000,
  0x44400000,
  0x44400000,
  0x44400000,
  0x44000000,
  0x55000000,
  0x55000000,
  0x55000000,
  0x55000000,
  0x55000000,
  0x55000000,
  0x55000000,
  0x55000000,
  0x50000000,
  0x50000000,
  0x55000000,
  0x50000000,
  0x55000000,
  0x50000000,
  0x50000000,
  0x50000000,
  0xb0000000,
  0xb0000000,
  0x00000000,
  0xb0000000,
  0x00000000,
  0xb0000000,
  0x00000000,
  0xb0000000,
  0xb0000000,
  0xb0000000,
  0xb0000000,
  0xb0000000,
  0xb0000000,
  0x00000000,
  0xb0000000,
  0xb0000000,
  0x00000000,
  0xb0000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,

};
#define sprites_tv_planea3_count (45)

static const uint16_t tv_planea3_palette[] = {
0x0, 0xccc, 0xcc0, 0xc0c, 0xc00, 0xaaa, 0xa00, 0x888, 0xcc, 0xc0, 0xc, 0x0, 
};

