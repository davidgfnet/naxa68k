#!/usr/bin/env pypy3

# Note! This compresses stuff, use pypy3 otherwise you will die waiting :)

# Filename  : tiler.py
# Authors   : David Guillen Fandos
# Version   : 0.1
# Date      : July 11, 2019
# Copyright : Public Domain

# Tool that processes an image and produces a tileset+tilemap out of it.
# The output is meant to be used directly by the game.

from PIL import Image
import os, sys, hashlib, math, argparse
from comp import compress
from common import *

parser = argparse.ArgumentParser(prog='tiler')
parser.add_argument('images', metavar='img.png', type=str, nargs='+', help='PNG image files')
parser.add_argument('--outfile', dest='outfile', required=True, help='Output file .h format')
parser.add_argument('--palname', dest='palname', type=str, required=True, help='Palette name')
parser.add_argument('--palette', dest='palette', type=str, default="", help='Palette to use (ie. sprites:444,28A,...)')
parser.add_argument('--notilemap', dest='notilemap', action='store_true', help='Do not use a tilemap')
parser.add_argument('--comptilem', dest='comptilem', action='store_true', help='Compress tilemap for sparse maps')
parser.add_argument('--u8tilemap', dest='u8tilemap', action='store_true', help='Use uint8_t tilemaps')
parser.add_argument('--compress', dest='compress', action='store_true', help='Compress sprites using custom compr.')
parser.add_argument('--debugfile', dest='debugfile', default="", help='Output an output file')
parser.add_argument('--notmopts', dest='notmopts', action='store_true', help='Do not use tilemap optimizations')

args = parser.parse_args()

# Read image and divide it in blocks of 8x8
N = 8
BL = 8  # 8x8 blocks for compression too

# Palette can be calculated or read
if args.palette:
	pcols = args.palette.split(",")
	assert len(pcols) < 16
	pcols = [hexto9b(x) for x in pcols]
	#pal = [0x200] + sorted(pcols, reverse=True)
	pal = [0x200] + pcols
else:
	palette = set([0x200])
	for image in args.images:
		im = Image.open(image).convert("RGBA")
		assert im.size[0] % N == 0 and im.size[1] % N == 0
		palette |= set(image9b(im))  # Get all possible colors
	pal = sorted(palette, reverse=True)
print("Palete is %d colors big (incl. transp.)" % len(pal))

# Loads & opts all imagez at once
allimages = { image: Image.open(image).convert("RGBA") for image in args.images }
for im in allimages.values():
	optimizeimage(im)

# Optimize the tilemap and tileset, optimization only present if:
# User is using u16 for tilemaps, the notilemap flag is disabled and comptilem
# optimization technique is disabled also!
canopttm = not args.u8tilemap and not args.notilemap and not args.comptilem and not args.notmopts

# Use unified tileset
tileset = []       # just a list of images
tilemapping = {}   # a map of hash -> position in the previous list
tilemaps = {}      # a map of image file -> (image hash, flipv, fliph)
outf = open(args.outfile, "w")
outf.write("#include \"level_functions.h\"\n\n")
optedaway = set()
for image, im in allimages.items():
	# Create tileset
	print("Image ", im.size)
	tilemap = [[0 for _ in range(0, im.size[0], N)] for _ in range(0, im.size[1], N)]
	for c in range(0, im.size[0], N):
		for r in range(0, im.size[1], N):
			# Take tile and hash it, ensure to color convert it first!
			tile = im.crop((c, r, c+N, r+N))
			h = hashlib.sha1(bytes(map2pal(image9b(tile), pal))).hexdigest()
			# 3 variants if optimizable
			if canopttm:
				hflipim = hashlib.sha1(bytes(map2pal(image9b(tile.transpose(Image.FLIP_LEFT_RIGHT)), pal))).hexdigest()
				vflipim = hashlib.sha1(bytes(map2pal(image9b(tile.transpose(Image.FLIP_TOP_BOTTOM)), pal))).hexdigest()
				aflipim = hashlib.sha1(bytes(map2pal(image9b(tile.transpose(
					Image.FLIP_TOP_BOTTOM).transpose(Image.FLIP_LEFT_RIGHT)), pal))).hexdigest()

			p = None
			if not args.notilemap:
				if h in tilemapping:
					p, hf, vf = tilemapping[h], 0, 0
				elif canopttm:
					if hflipim in tilemapping:
						p, hf, vf = tilemapping[hflipim], 1, 0
						optedaway.add(hflipim)
					elif vflipim in tilemapping:
						p, hf, vf = tilemapping[vflipim], 0, 1
						optedaway.add(vflipim)
					elif aflipim in tilemapping:
						p, hf, vf = tilemapping[aflipim], 1, 1
						optedaway.add(aflipim)

			# Not found or not allowed to lookup
			if p is None:
				p, hf, vf = len(tileset), 0, 0
				tilemapping[h] = p
				tileset.append(tile)

			tilemap[r//N][c//N] = (p, hf, vf)
	tilemaps[image] = tilemap

print("Generated", len(tileset), "tiles, optimized", len(optedaway), "tiles")

for image, im in allimages.items():
	print("Generated", len(tileset), "tiles, for a map", len(tilemap), "x", len(tilemap[0]))

	basename = os.path.basename(image).split(".")[0]
	tmtype = "uint16_t"
	if args.u8tilemap:
		assert len(tileset) < 256
		tmtype = "uint8_t"

	tilemap = tilemaps[image]
	if not args.notilemap:
		if args.comptilem:
			# Divide the tilemap in blocks that are 8x8, if the block has only
			# one type of tile, output it inline (mark it with a flag)
			# Otherwise the flag value is a "pointer" to the real map storage.
			# TODO: davidgfnet to implement this! Saves ~90% space for cave level.
			ptrs = []
			cntr = 0
			outf.write("static const %s sub_tilemap_%s[] = {\n" % (tmtype, basename))
			for blr in range(0, im.size[1]//N, BL):
				for blc in range(0, im.size[0]//N, BL):
					ref = tilemap[blr][blc]
					eq = True
					vals = []
					for r in range(BL):
						for c in range(BL):
							if blr+r < len(tilemap) and blc+c < len(tilemap[blr+r]):
								vals.append(tilemap[blr+r][blc+c])
								if tilemap[blr+r][blc+c] != ref:
									eq = False
							else:
								vals.append((0, 0, 0))    # Dummy, for edges
					if eq:
						# Emit a single thing, note there's no flag!
						ptrs.append(ref[0])
					else:
						# Emit a regular 8x8 sub-tilemap with the content, and a ptr to this
						outf.write(",".join(str(x[0]) for x in vals))
						outf.write(",\n")
						ptrs.append(cntr | (1<<15));
						cntr += 1
			outf.write("\n};\n")
			outf.write("static const uint16_t packed_tilemap_%s[] = {\n" % (basename))
			outf.write(",".join(str(x) for x in ptrs))
			outf.write("\n};\n")

			# Also produce a struct to easily pass it around
			stride = (im.size[0]//N + BL-1)//BL
			outf.write("static const struct t_packed_tm16 packed_usrptr_%s = {\n" % basename)
			outf.write("  sub_tilemap_%s, packed_tilemap_%s, %d, %d, %d};\n" % (
				basename, basename, stride, im.size[0]//N, im.size[1]//N))
		else:
			outf.write("static const %s tilemap_%s[] = {\n" % (tmtype, basename))
			for r in tilemap:
				outf.write("  " + ",".join([str(x[0] | (x[2]<<12) | (x[1]<<11)) for x in r]) + ",\n")
			outf.write("\n};\n")
	outf.write("#define tilemap_%s_width (%d)\n" % (basename, im.size[0]//N))
	outf.write("#define tilemap_%s_height (%d)\n\n" % (basename, im.size[1]//N))

# Output tileset
if args.compress:
	data = []
	for sp in tileset:
		spr = map2pal(image9b(sp), pal)
		assert len(spr) == 64
		for i in range(0, 64, 2):
			data.append((spr[i+0] << 4) | spr[i+1])
	print("Original size: %d bytes" % len(data))
	data = compress(data)
	print("Compressed size: %d bytes" % len(data))
	outf.write("static const uint8_t tiles_compr_%s[] = {\n" % args.palname)
	outf.write(",".join(["0x%x" % x for x in data]))
	outf.write("\n};\n")
else:
	outf.write("static const uint32_t tiles_%s[] = {\n" % args.palname)
	for sp in tileset:
		spr = map2pal(image9b(sp), pal)
		assert len(spr) == 64
		for i in range(0, 64, 8):
			outf.write("  0x%x%x%x%x%x%x%x%x,\n" % (
					   spr[i+0], spr[i+1], spr[i+2], spr[i+3],
					   spr[i+4], spr[i+5], spr[i+6], spr[i+7]))
	outf.write("\n};\n")
outf.write("#define sprites_%s_count (%d)\n\n" % (args.palname, len(tileset)))

outf.write("static const uint16_t %s_palette[] = {\n" % args.palname)
for c in pal:
	outf.write("0x%x, " % ((((c & 0x1C0)) << 3) | ((c & 0x38) << 2) | ((c & 0x7) << 1)))
outf.write("\n};\n\n")

# Debug
if args.debugfile:
	d = math.ceil(math.sqrt(len(tileset)))
	print("Will generate tileset of size", d);

	imtileset = Image.new("RGBA", (N*d, N*d))
	for i in range(len(tileset)):
		r, c = int(i / d), i % d
		box = (c*N, r*N, (c+1)*N, (r+1)*N)
		imtileset.paste(tileset[i].convert("RGBA"), box)
	imtileset.save(args.debugfile)


