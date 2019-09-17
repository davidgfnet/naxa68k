
// ASCII/text optimized LZSS-like compression algorithm
// Copyright 2017 David Guillen Fandos <david@davidgf.net>

#ifndef __LZSS_COMP_DECOMPRESS_H__
#define __LZSS_COMP_DECOMPRESS_H__

#include <genesis.h>

#define MIN_WIN_SIZE  (2048)
#define MIN_WIN_MASK  (2047)

struct t_decomp_state {
	unsigned ptr, inptr;
	uint8_t window[MIN_WIN_SIZE];
};

// Decompresses into outbuf up to maxsize, make sure maxsize is at least 18 bytes
// to guarantee forward progress. It will return the amount of bytes decompressed
unsigned decompress(const uint8_t *inbuf, struct t_decomp_state *state, uint8_t *outbuf, unsigned maxsize);

// Check for EOF
#define decomp_eof(state, inbuf) (inbuf[(state)->inptr] == 0)

// Initialize state
inline static void decomp_init(struct t_decomp_state *state) {
	state->ptr = 0;
	state->inptr = 0;
}

#endif

