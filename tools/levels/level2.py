#!/usr/bin/env python3

# Filename  : level2.py
# Authors   : David Guillen Fandos
# Version   : 0.1
# Date      : June 25, 2019
# Copyright : Public Domain

import os, sys, argparse
sys.path.append(os.path.realpath(os.path.join(os.path.dirname(__file__), "..")))
from common import *

parser = argparse.ArgumentParser(prog='level2')
parser.add_argument('--level', dest='levelfile', required=True, help='Level file (TMX format)')
parser.add_argument('--outfile', dest='outfile', required=True, help='Output file (.c/.h format)')
args = parser.parse_args()

# Procudes level2 specifc code to be used to place enemies and objects in grid
tmx = TMXMap(args.levelfile)

# Produce a collision map for the 16x16 tilemap.
outc = open(args.outfile, "w")
outc.write("// Autogenerated from %s by level2.py\n\n" % args.levelfile)
outc.write("//#include <stdint.h>\n\n")
outc.write(tmx.collLayer(tmx._layers["obstacles"], "bombmap"))

# Output the enemies and their attributes
etypes = sorted(set([x[5]["type"].lower() for x in tmx._objgrps["Enemies"]]))
outc.write("\n".join("#define ENTYPE_%s %d" % (x.upper(), i) for i, x in enumerate(etypes)))
outc.write("\n#define ENTYPE_MAX %d\n" % (len(etypes)))
outc.write("\nconst struct { uint8_t x, y, t; } enemies_info[] = {\n")
for name, x, y, w, h, attrs in tmx._objgrps["Enemies"]:
	outc.write("  {%d, %d, %d},\n" % (x//w, y//h, etypes.index(attrs["type"])))
outc.write("};\n")
outc.write("#define ENEMIES_COUNT %d\n\n" % (len(tmx._objgrps["Enemies"])))

# Also output placeholder variables for any object that needs a tile on top.
for name, x, y, w, h, attrs in tmx._objgrps["Placeholders"]:
	outc.write("#define OBJ_%s_X  %d\n" % (name.upper(), x))
	outc.write("#define OBJ_%s_Y  %d\n" % (name.upper(), y))
	outc.write("#define OBJ_%s_W  %d\n" % (name.upper(), w))
	outc.write("#define OBJ_%s_H  %d\n" % (name.upper(), h))

outc.close()

