
#ifndef __UTIL_HH__
#define __UTIL_HH__

#include <genesis.h>

#define ANY_BUTTON (BUTTON_A|BUTTON_B|BUTTON_C)

// Random generation
void random_reseed(uint32_t seed);
uint32_t randomu32();

// Linear color interpolation to black and white (for fades)
uint16_t colinterpB(uint16_t col, unsigned mult, unsigned shift);
uint16_t colinterpW(uint16_t col, unsigned mult, unsigned shift);
void fade_dark_pal(uint16_t *workpal, const uint16_t *refpal, unsigned amount, unsigned sa);
void fade_white_pal(uint16_t *workpal, const uint16_t *refpal, unsigned amount, unsigned sa);

// Font related stuff
void setup_font16(unsigned basetile);
void load_font16(void *cpuram);
void print_font16(const char *text, VDPPlan plane, unsigned pal, unsigned basetile, unsigned x, unsigned y);

// Resume codes
unsigned get_resume_code(unsigned code5);
int verify_resume_code(unsigned code13);

// Compression related stuff for graphics
void loadDecompressTiles(const uint8_t *data, unsigned basetile, TransferMethod tm);

// Sprite output routine
// This performs some visibility checks :)
#define SET_SPRITE_PLUS(spnum, x, y, s, attr) { \
	if ((signed)(x) >= -48 && (signed)(x) <= _SCREEN_WIDTH+48 && \
		(signed)(y) >= -48 && (signed)(y) <= _SCREEN_HEIGHT+48) { \
		SET_SPRITE_PLUS_NVC(spnum, x, y, s, attr); \
	} \
}

// This does not perform any checks at all
#define SET_SPRITE_PLUS_NVC(spnum, x, y, s, attr) { \
	VDP_setSpriteFull((spnum), (x), (y), s, attr, (spnum)+1); \
	(spnum)++; \
}

// Some misc macros
#define DEF_TSET(name, prev, prevsize) \
	const uint32_t name ## _TILEBASE = (prev ## _TILEBASE + prevsize);

// Max, min, clamp, etc
#define CLAMPMAX(x, maxv) ((x) < (maxv) ? (x) : (maxv))
#define CLAMPMIN(x, minv) ((x) > (minv) ? (x) : (minv))

#define INTABS(x)  ( (x) >= 0 ? (x) : (-(x)) )

// Level states (common at least)
#define LEVEL_STARTING       0
#define LEVEL_INTRO          1
#define LEVEL_PLAY           2
#define LEVEL_RESTART        3
#define LEVEL_RESTARTING     4
#define LEVEL_SPECIAL_ENDING 5
#define LEVEL_ENDING_WIN     6
#define LEVEL_SWITCH         7

#endif

