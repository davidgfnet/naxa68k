#!/bin/env python

# ASCII/text optimized LZSS-like compression algorithm
# Copyright 2017 David Guillen Fandos <david@davidgf.net>

# Format, each byte is
#
# 00000000          -> EOF (stream end)
# 011111LL          -> Emit raw caracter LL+1 chars
# 0DDDDDLL          -> Emit D,L  num chars is L+2, back is D+len
# 1DDDDDDD DDDDLLLL -> Same as before

BG_L_BITS =  4    # Bits used for LLLL
SM_L_BITS =  2    # Bits used for LL
BG_D_BITS = 11    # Bits used for DDDD
SM_D_BITS =  5    # Bits used for DD

assert(BG_D_BITS + BG_L_BITS == 15)
assert(SM_D_BITS + SM_L_BITS ==  7)

MIN_LEN = 1                            # Minimum length we copy
MAX_BACK = (2**BG_D_BITS)              # Max D bits (using any code), this defined the search back window
MAX_LEN = (2**BG_L_BITS-1 + MIN_LEN)   # This is the max len, 4 bits used for LLLL + MIN_LEN

SM_MAX_LEN   = (2**SM_L_BITS-1)        # Max encodable LEN in Small (not including minimum)
SM_MAX_BACK  = (2**SM_D_BITS-1-1)      # Extra -1 because 1111 combination is overriden for raw output

MAX_RAW_CHARS = 4                      # Num of chars in each raw output

def encodedl(d, l):
	d -= l
	l -= MIN_LEN
	assert d >= 0
	# Can't have 0,0 otherwise is EOF, prevented by bestcost
	assert d > 0 or l > 0
	if d > SM_MAX_BACK or l > SM_MAX_LEN:
		return [ 128 | (d // 16), l | ((d&15) * 16) ]

	return [ (d * 4) | l ]

def codecost(d, l):
	# Outputting a code with (d,l) would cost us (can be negative!)
	if d-l > SM_MAX_BACK or l > SM_MAX_LEN + MIN_LEN:
		return 2 - l
	return 1 - l

def prefmatch(buf, i, j):
	r = 0
	for x in range(min(len(buf) - i, len(buf) - j, abs(i-j))):
		if buf[i+x] == buf[j+x]:
			r += 1
		else:
			break
	return r

def prefsearch(buf, cur):
	# Search in the buf[0:cur] buffer for some prefix of buf[cur:]
	best = None
	bestcost = 2 # We can always output 1 char at cost 2 (well really its 1-2)
	for i in range(max(0, cur - MAX_BACK), cur):
		n = min(prefmatch(buf, i, cur), MAX_LEN)
		if n > MIN_LEN:
			d = cur - i
			c = codecost(d, n)
			if c < bestcost:
				bestcost = c
				best = (d, n)

	return best

def emitraw(nf):
	ret = []
	while len(nf) > 0:
		t = nf[:MAX_RAW_CHARS]
		ret += [ 0x7C | (len(t) - 1) ]
		ret += t
		nf = nf[MAX_RAW_CHARS:]
	return ret

def compress(inbuf):
	# Brain dead algorithm
	# Try every possible output sequence and see which one is the best
	ret,nf = [],[]

	i = 0
	while i < len(inbuf):
		# Backwards search
		dl = prefsearch(inbuf, i)
		if dl:
			# Emit buffered chars!
			ret += emitraw(nf)
			nf = []
			
			enc = encodedl(*dl)

			ret += enc 
			i += dl[1]
			continue

		# Nothing found or not worth it, output one char to a tmp buff
		nf.append(inbuf[i])
		i += 1

	# Emit buffered chars!
	ret += emitraw(nf)

	ret.append(0)
	return ret

if __name__ == "__main__":
	import sys
	inp = open(sys.argv[1], "rb").read()
	inbuf = [ ord(x) for x in inp ]
	
	outbuf = compress(inbuf)
	outp = "".join([chr(x) for x in outbuf])
	
	open(sys.argv[2], "wb").write(outp)
	
	
	
