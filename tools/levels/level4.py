#!/usr/bin/env python3

# Filename  : level4.py
# Authors   : David Guillen Fandos
# Version   : 0.1
# Date      : August 6, 2019
# Copyright : Public Domain

import os, sys, argparse, hashlib
sys.path.append(os.path.realpath(os.path.join(os.path.dirname(__file__), "..")))
from common import *

parser = argparse.ArgumentParser(prog='level4')
parser.add_argument('--level', dest='levelfile', required=True, help='Level file (TMX format)')
parser.add_argument('--outfile', dest='outfile', required=True, help='Output file (.c/.h format)')
parser.add_argument('--basenum', dest='basenum', required=True, help='Number of sublevel')
args = parser.parse_args()

# Produces level1 specifc code to be used to place enemies and objects in grid
tmx = TMXMap(args.levelfile)

# Produce a collision map for the 16x16 tilemap.
outc = open(args.outfile, "w")
outc.write("// Autogenerated from %s by level4.py\n\n" % args.levelfile)
outc.write("//#include <stdint.h>\n\n")
# We use 3 types of tiles: free, solid, and die (void)
outc.write(tmx.collLayer(tmx._layers["attribute"], "colmap%s" % args.basenum, mapper=lambda x: x&1))
# Here we write the death map, which essentially represents the character falling
outc.write(tmx.collLayer(tmx._layers["attribute"], "deathmap%s" % args.basenum, mapper=lambda x: x >> 1))

# Output the enemies and their attributes
#colors = ["red", "blue", "orange", "pink"]
#moves = ["horizontal", "vertical"]
#def pickcol(name, attrs):
#	if "color" in attrs:
#		return colors.index(attrs["color"])
#	# Pick a random color based on name, to make it deterministic
#	return hashlib.sha1(name.encode("ascii")).digest()[0] & 3

enemies, traps, shooters = [], [], []
for name, x, y, w, h, attrs in tmx._objgrps["Enemies"]:
	if attrs["type"] == "trap":
		traps.append("  {%d, %d}," % (x//8, y//8))
	elif attrs["type"] == "walker":
		enemies.append("  {%d, %d}," % (x//8 + w//16, y//8 + h//16))
	elif attrs["type"] == "shooter":
		shooters.append("  {%d, %d}," % (x//8 + w//16, y//8 + h//16))

outc.write("// Divided by 8!\nstatic const uint8_t traps%s_info[%d][2] = {\n" % (args.basenum, len(traps)))
outc.write("\n".join(traps))
outc.write("};\n")
outc.write("#define TRAPS%s_COUNT %d\n\n" % (args.basenum, len(traps)))

outc.write("// Divided by 8!\nstatic const uint8_t enemies%s_info[%d][2] = {\n" % (args.basenum, len(enemies)))
outc.write("\n".join(enemies))
outc.write("};\n")
outc.write("#define ENEMIES%s_COUNT %d\n\n" % (args.basenum, len(enemies)))

outc.write("// Divided by 8!\nstatic const uint8_t shooters%s_info[%d][2] = {\n" % (args.basenum, len(shooters)))
outc.write("\n".join(shooters))
outc.write("};\n")
outc.write("#define SHOOTERS%s_COUNT %d\n\n" % (args.basenum, len(shooters)))

boxtypes = {"nothing": 0, "life": 1, "key0": 128, "key1": 129}
boxes, doors = [], []
for name, x, y, w, h, attrs in tmx._objgrps["Objects"]:
	if attrs["type"] == "door":
		doors.append("  {%d, %d, %d}," % (x//8, y//8, 0))
	elif attrs["type"] == "exit":
		outc.write("#define EXIT%s_MINX %d\n" % (args.basenum, x))
		outc.write("#define EXIT%s_MAXX %d\n" % (args.basenum, x+w))
		outc.write("#define EXIT%s_MINY %d\n" % (args.basenum, y))
		outc.write("#define EXIT%s_MAXY %d\n" % (args.basenum, y+h))
	else:
		t = boxtypes[attrs["type"]]
		boxes.append("  {%d, %d, %d}," % (x//8, y//8, t))

outc.write("// Divided by 8!\nstatic const uint8_t boxes%s_info[%d][3] = {\n" % (args.basenum, len(boxes)))
outc.write("\n".join(boxes))
outc.write("};\n")
outc.write("#define BOXES%s_COUNT %d\n\n" % (args.basenum, len(boxes)))

outc.write("// Divided by 8!\nstatic const uint8_t door%s_info[%d][3] = {\n" % (args.basenum, len(doors)))
outc.write("\n".join(doors))
outc.write("};\n")
outc.write("#define DOORS%s_COUNT %d\n\n" % (args.basenum, len(doors)))

outc.close()

