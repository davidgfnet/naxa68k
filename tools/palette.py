#!/usr/bin/env python3

# Filename  : palette.py
# Authors   : David Guillen Fandos
# Version   : 0.1
# Date      : June 25, 2019
# Copyright : Public Domain

# Prints palette for a bunch of images (unifies it!)

from PIL import Image
import os, sys, hashlib, math, argparse
from common import *

parser = argparse.ArgumentParser(prog='palette')
parser.add_argument('images', metavar='img.png', type=str, nargs='+', help='PNG image files')
args = parser.parse_args()

pals = set()
for imagef in args.images:
	# Read image and divide it in blocks of NxN
	im = Image.open(imagef).convert("RGBA")

	# Extract palette from the image
	pal = set([0x200])
	for c in image9b(im):
		if c not in pal:
			pal.add(c)
	eprint("Image '%s' has %d colors" % (imagef, len(pal)))
	pals |= pal

eprint("Uberpalette has %d colors (including transparent!)" % len(pals))
pal = sorted(pals, reverse=True)
pal.remove(0x200) # Remove transparent!

pal = [t9btohex(x) for x in pal]
print(",".join(pal))

