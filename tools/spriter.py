#!/usr/bin/env pypy3

# Filename  : spriter.py
# Authors   : David Guillen Fandos
# Version   : 0.1
# Date      : June 1, 2019
# Copyright : Public Domain

# Processes sprites in images to create datasets in headers that can be
# included in the game. Assumes most sprites are actually animations.

from PIL import Image
import os, sys, hashlib, math, argparse, re
from common import *
from comp import compress

parser = argparse.ArgumentParser(prog='spriter')
parser.add_argument('images', metavar='img.png', type=str, nargs='+', help='PNG image files')
parser.add_argument('--outfile', dest='outfile', required=True, help='Output file (.c/.h format)')
parser.add_argument('--nsprites', dest='nsprites', type=int, default=0, help='Number of sprites')
parser.add_argument('--palette', dest='palette', required=True, help='Palette to use (ie. sprites:444,28A,...)')
parser.add_argument('--compressanim', dest='compressanim', action='store_true', help='Compress animations')
parser.add_argument('--comprsprites', dest='comprsprites', action='store_true', help='Compress sprites')
args = parser.parse_args()

# Parse palette, it should be 1-15 colors in 3 hex chars
# they also must be even (not odd)
if ":" in args.palette:
	pname, pdef = args.palette.split(":")
else:
	pname, pdef = None, args.palette

pcols = pdef.split(",")
assert len(pcols) < 16
pcols = [hexto9b(x) for x in pcols]
pal = [0x200] + sorted(pcols, reverse=True)

outf = open(args.outfile, "w")

for imagef in args.images:
	# Read image and divide it in blocks of NxN
	im = Image.open(imagef).convert("RGBA")
	imagename = imagef
	optimizeimage(im)

	if args.nsprites:
		# We are told the sprite count
		assert im.size[0] % args.nsprites == 0
		N = im.size[0] // args.nsprites
	else:
		# Try to guess from filename?
		m = re.match(r"(.*)_([0-9]+)x([0-9]+)\.png$", imagef)
		if m:
			N = int(m.group(2))
			imagename = m.group(1)
		else:
			N = im.size[1]
	assert im.size[0] % N == 0
	assert im.size[0] % 8 == 0 and im.size[1] % 8 == 0

	# Use filename as variable name
	basename = os.path.basename(imagename).split(".")[0]
	# TODO: Validate file name as a variable!

	# Create tileset
	print("Image", imagef, "size:", im.size)
	spriteset = {}
	anim = []
	for c in range(0, im.size[0], N):
		# Take tile and hash it
		sprite = im.crop((c, 0, c+N, im.size[1]))
		raw = b"".join([bytes(x) for x in (list(sprite.getdata()))])
		h = hashlib.sha1(raw).hexdigest()
		spriteset[h] = sprite
		anim.append(h)

	# Do not output animation data if there's no good reason for it
	if len(spriteset) == len(anim) or not args.compressanim:
		ssprites = anim
	else:
		# Output the sprite database in hash order for consistency
		ssprites = sorted(spriteset.keys())

	tcount = 0
	if args.comprsprites:
		data = []
		for h in ssprites:
			# Break them down in 8x8 tiles
			bigsp = spriteset[h]
			for c in range(0, bigsp.size[0], 8):
				for r in range(0, bigsp.size[1], 8):
					tcount += 1
					tile8 = bigsp.crop((c, r, c+8, r+8))
					spr = map2pal(image9b(tile8), pal)
					assert len(spr) == 64
					for i in range(0, 64, 2):
						data.append((spr[i+0] << 4) | spr[i+1])

		print("Original size: %d bytes" % len(data))
		data = compress(data)
		print("Compressed size: %d bytes" % len(data))
		outf.write("static const uint8_t sprites_compr_%s[] = {\n" % basename)
		outf.write(",".join(["0x%x" % x for x in data]))
		outf.write("\n};\n")
	else:
		outf.write("static const uint32_t sprites_%s[] = {\n" % basename)
		for h in ssprites:
			# Break them down in 8x8 tiles
			bigsp = spriteset[h]
			for c in range(0, bigsp.size[0], 8):
				for r in range(0, bigsp.size[1], 8):
					tcount += 1
					tile8 = bigsp.crop((c, r, c+8, r+8))
					spr = map2pal(image9b(tile8), pal)
					assert len(spr) == 64
					for i in range(0, 64, 8):
						outf.write("  0x%x%x%x%x%x%x%x%x,\n" % (
								   spr[i+0], spr[i+1], spr[i+2], spr[i+3],
								   spr[i+4], spr[i+5], spr[i+6], spr[i+7]))
		outf.write("\n};\n")

	outf.write("#define sprites_%s_count (%d)\n\n" % (basename, tcount))

	# Now output the animation tables where we copy info from
	if len(ssprites) != len(anim):
		outf.write("const uint8_t anim_frames_%s[] = {\n" % basename)
		for f in anim:
			tileoffset = ssprites.index(f)
			outf.write("%d, " % tileoffset)
		outf.write("\n};\n\n")
	outf.write("#define anim_frames_%s_count %s\n" % (basename, len(anim)))

# Output the palette
if pname:
	outf.write("static const uint16_t %s_palette[16] = {\n" % pname)
	outf.write(",".join(["0x%x" % c for c in pal2gen(pal)]))
	outf.write("\n};\n\n")


