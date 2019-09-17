
// ASCII/text optimized LZSS-like compression algorithm
// Copyright 2017 David Guillen Fandos <david@davidgf.net>

// Format, each byte is
//
// 00000000          -> EOF (stream end)
// 011111LL          -> Emit raw caracter LL+1 chars
// 0DDDDDLL          -> Emit D,L  num chars is L+2, back is D+len
// 1DDDDDDD DDDDLLLL -> Same as before

#include "decomp.h"
#define uintptr_t uint32_t

// Decompresses into outbuf up to maxsize, make sure maxsize is at least 17 bytes
// to guarantee forward progress. It will return the amount of bytes decompressed
unsigned decompress(const uint8_t *inbuf, struct t_decomp_state *state, uint8_t *outbuf, unsigned maxsize) {
	uint8_t *original_outbuf = outbuf;
	while ((uintptr_t)outbuf - (uintptr_t)original_outbuf < maxsize - 16) {
		if (decomp_eof(state, inbuf))
			break;
		else if ((inbuf[state->inptr] & 0xFC) == 0x7C) {
			// Copy n bytes from input buffer
			unsigned n = (inbuf[state->inptr] & 3) + 1;
			const uint8_t *iptr = &inbuf[state->inptr+1];
			state->inptr += n+1;

			while (n--) {
				*outbuf++ = state->window[state->ptr] = *iptr++;
				state->ptr = (state->ptr + 1) & MIN_WIN_MASK;
			}
		}
		else  {
			// 1-2byte back
			unsigned d, l;
			if ((inbuf[state->inptr] & 0x80) == 0) {
				// 1 byte back
				d = (inbuf[state->inptr] >> 2);
				l = (inbuf[state->inptr] & 3) + 1;
				state->inptr++;
			} else {
				// 2 byte back
				d = ((inbuf[state->inptr] & 0x7f) << 4U) | (inbuf[state->inptr+1] >> 4);
				l = (inbuf[state->inptr+1] & 15) + 1;
				state->inptr += 2;
			}

			unsigned back = l + d;
			while (l--) {
				unsigned wptr = (state->ptr - back) & MIN_WIN_MASK;				
				*outbuf++ = state->window[state->ptr] = state->window[wptr];
				state->ptr = (state->ptr + 1) & MIN_WIN_MASK;
			}
		}
	}

	return (uintptr_t)outbuf - (uintptr_t)original_outbuf;
}


