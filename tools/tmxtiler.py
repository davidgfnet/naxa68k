#!/usr/bin/env python3

# Filename  : tmxtiler.py
# Authors   : David Guillen Fandos
# Version   : 0.1
# Date      : June 1, 2019
# Copyright : Public Domain

# Tool that outputs an almost-ready TMX out of an image.
# This is an import step so that artists can easily convert images from GIMP
# to TMX editable files to later define collisions and sprites.

from PIL import Image
import os, sys, hashlib, math, argparse
from common import *

parser = argparse.ArgumentParser(prog='tmxtiler')
parser.add_argument('--image', dest='image', required=True, type=str, help='PNG image files')
parser.add_argument('--size', dest='size', default=8, type=int, help='Tile size in pixels (ie. 8 or 16)')
parser.add_argument('--outfile', dest='outfile', required=True, help='Output file .tmx format')
args = parser.parse_args()

# Read image and divide it in blocks of NxN
N = args.size
im = Image.open(args.image).convert("RGBA")
optimizeimage(im)
assert im.size[0] % N == 0 and im.size[1] % N == 0

# Create tileset
print("Image ", im.size)
tileset = []
tilemapping = {}
tilemap = [[0 for _ in range(0, im.size[0], N)] for _ in range(0, im.size[1], N)]
for c in range(0, im.size[0], N):
	for r in range(0, im.size[1], N):
		# Take tile and hash it
		tile = im.crop((c, r, c+N, r+N))
		raw = tile.tobytes()
		h = hashlib.sha1(raw).hexdigest()
		if h not in tilemapping:
			p = tilemapping[h] = len(tileset)
			tileset.append(tile)
		else:
			p = tilemapping[h]
		tilemap[int(r/N)][int(c/N)] = p

print("Generated", len(tileset), "tiles, for a map", len(tilemap), "x", len(tilemap[0]))
d = math.ceil(math.sqrt(len(tileset)))
print("Will generate tileset of size", d);

imtileset = Image.new("RGBA", (N*d, N*d))
for i in range(len(tileset)):
	r, c = int(i / d), i % d
	box = (c*N, r*N, (c+1)*N, (r+1)*N)
	imtileset.paste(tileset[i].convert("RGBA"), box)
imtileset.save(args.outfile + ".tilemap.png")

csv = []
for r in tilemap:
	csv.append(",".join([str(x+1) for x in r]))
csv = ",\n".join(csv)

# Produce TMX file
tmx_file = (
"""<?xml version="1.0" encoding="UTF-8"?>
<map version="1.0" tiledversion="1.1.6" orientation="orthogonal" renderorder="right-down" width="%(twidth)d" height="%(theight)d" tilewidth="%(tsize)d" tileheight="%(tsize)d" infinite="0">
 <tileset firstgid="1" name="tileset" tilewidth="%(tsize)d" tileheight="%(tsize)d" tilecount="%(tcount)d" columns="%(tcountcols)d">
  <image source="%(imgfile)s.tilemap.png" width="%(twidth)d" height="%(theight)d"/>
 </tileset>
 <tileset firstgid="%(tcount)d" name="bincoll" tilewidth="%(tsize)d" tileheight="%(tsize)d" tilecount="2" columns="2">
  <image source="bincoll%(tsize)d.png" width="%(tsize2)d" height="%(tsize)d"/>
 </tileset>
 <layer name="Default" width="%(twidth)d" height="%(theight)d" locked="1">
  <properties>
   <property name="pal_slot" type="int" value="9"/>
  </properties>
  <data encoding="csv">
   %(csv)s
  </data>
 </layer>
</map>
""" % {
	"imgfile": os.path.basename(args.outfile),
	"tcount": d*d,
	"tcountcols": d,
	"twidth": im.size[0] / N,
	"theight": im.size[1] / N,
	"tsize": N,
	"tsize2": N*2,
	"csv": csv,
})

open(args.outfile + ".tmx", "w").write(tmx_file)

