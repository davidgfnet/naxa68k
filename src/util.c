
// Copyright 2019 David Guillen Fandos (david@davidgf.net)
// This code is released under the GPL2 license.

// Utility library that contains:
//
// * Lightweight random number generation.

#include <genesis.h>
#include <string.h>

#include "util.h"
#include "decomp.h"
#include "assets/sprites/font16x16.h"

static uint32_t rnd_seed = 0xdeadbeed;

void random_reseed(uint32_t seed) {
	rnd_seed = seed;
}

uint32_t randomu32() {
	rnd_seed = (rnd_seed * 16807) & 0x7FFFFFFF;
	return (rnd_seed ^ (rnd_seed >> 16));
}

// Returns a 13 bit num
unsigned get_resume_code(unsigned code5) {
	uint32_t crc = (code5 * 0x5DEECE66DU + 11U) & 0xFF;
	uint32_t code = (crc << 5) | code5;
	// Reverse the pair bits, some obfuscation is always nice to have :)
	code = ((code & 0xAAA) >> 1) | ((code & 0x555) << 1) | (code & 0x1000);
	return code;
}

int verify_resume_code(unsigned code13) {
	// A number between [0 and 8192)
	code13 = ((code13 & 0xAAA) >> 1) | ((code13 & 0x555) << 1) | (code13 & 0x1000);
	uint32_t crc = code13 >> 5;
	uint32_t data = code13 & 0x1F;
	uint32_t expcrc = (data * 0x5DEECE66DU + 11U) & 0xFF;
	if (crc == expcrc)
		return data;
	return -1;
}

uint16_t colinterpB(uint16_t col, unsigned mult, unsigned shift) {
	unsigned b = ((col >> 8) * mult) >> shift;
	unsigned g = (((col >> 4) & 15) * mult) >> shift;
	unsigned r = ((col & 15) * mult) >> shift;
	return (b << 8) | (g << 4) | r;
}

uint16_t colinterpW(uint16_t col, unsigned mult, unsigned shift) {
	unsigned b = ((0xe - (col >> 8)) * mult) >> shift;
	unsigned g = ((0xe - ((col >> 4) & 15)) * mult) >> shift;
	unsigned r = ((0xe - (col & 15)) * mult) >> shift;
	return ((0xe - b) << 8) | ((0xe - g) << 4) | (0xe - r);
}

void fade_dark_pal(uint16_t *workpal, const uint16_t *refpal, unsigned amount, unsigned sa) {
	for (unsigned i = 1; i < 16; i++)
		workpal[i] = colinterpB(refpal[i], amount, sa);
}

void fade_white_pal(uint16_t *workpal, const uint16_t *refpal, unsigned amount, unsigned sa) {
	for (unsigned i = 1; i < 16; i++)
		workpal[i] = colinterpW(refpal[i], amount, sa);
}

// Loads 16x16 font tileset in VRAM at basetile
void setup_font16(unsigned basetile) {
	loadDecompressTiles(sprites_compr_font16x16, basetile, CPU);
}

// Just uncompresses font data to CPU RAM for other uses (like sprite creation)
// Ensure there's enough RAM! Takes ~12KB as it is now.
void load_font16(void *cpuram) {
	struct t_decomp_state st;
	decomp_init(&st);
	decompress(sprites_compr_font16x16, &st, cpuram, 64*1024);
}

void print_font16(const char *text, VDPPlan plane, unsigned pal, unsigned basetile, unsigned x, unsigned y) {
	if (!text)
		return;

	while (*text) {
		unsigned tilen = ((*text++ - 32) << 2) + basetile;
		VDP_setTileMapXY(plane, TILE_ATTR_FULL(pal, 0, 0, 0, tilen  ), x,   y);
		VDP_setTileMapXY(plane, TILE_ATTR_FULL(pal, 0, 0, 0, tilen+1), x,   y+1);
		VDP_setTileMapXY(plane, TILE_ATTR_FULL(pal, 0, 0, 0, tilen+2), x+1, y);
		VDP_setTileMapXY(plane, TILE_ATTR_FULL(pal, 0, 0, 0, tilen+3), x+1, y+1);
		x += 2;
	}
}

// Decompression helper routines
void loadDecompressTiles(const uint8_t *data, unsigned basetile, TransferMethod tm) {
	struct t_decomp_state st;
	decomp_init(&st);
	uint8_t buffer[4*1024];
	unsigned wptr = 0;
	do {
		// Decompress more data!
		unsigned r = decompress(data, &st, &buffer[wptr], sizeof(buffer) - wptr);
		wptr += r;
		// Load tiles (in chunks of 32 bytes)
		unsigned ntl = wptr >> 5;
		if (ntl)
			VDP_loadTileData((uint32_t*)buffer, basetile, ntl, tm);
		basetile += ntl;
		// Move buffer, not very efficient
		unsigned rem = wptr - (ntl << 5);
		if (rem)
		for (unsigned i = 0; i < rem; i++)
			buffer[i] = buffer[wptr - rem + i];
		wptr = rem;
	} while (!decomp_eof(&st, data));
}

