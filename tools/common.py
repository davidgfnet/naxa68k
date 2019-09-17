
from __future__ import print_function
import sys, os, xmltodict
from PIL import Image

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

# Makes images/tiles more optimal by making all their transparent pixels same
# (that is completely transparent and black) and leaving other pixels the way they are
# This reduces tile map sizes due to some issues with transparent tiles when they are exported.
def optimizeimage(t):
	# Makes all transparent tiles to be black
	pixmap = t.load()
	for c in range(t.size[0]):
		for r in range(t.size[1]):
			col = pixmap[c, r]
			if len(col) > 3:
				col = col if col[3] == 255 else (0,0,0,0)
			pixmap[c, r] = col


# Converts an image (PIL) into an array of colors, each color
# being a 9 bit integer , except for transparencies which are
# exactly 0x200 (that is really 10 bits).
def image9b(im):
	ret = []
	for d in im.getdata():
		if len(d) > 3:
			r,g,b,a = d
		else:
			r,g,b,a = (*d, 255)   # Solid color
		color = ((b >> 5) << 6) | ((g >> 5) << 3) | (r >> 5)
		if a == 255:
			ret.append(color)
		elif a == 0:
			ret.append(1 << 9)
		else:
			print("Error semi transparent color!")
	return ret

# Given an image (an array of colors really) and a palette,
# map it to the palette. No checks!
def map2pal(im9, pal):
	return [pal.index(x) for x in im9]

# Given a palette, a bunch of colors in a list, convert it to the native
# genesis format. The palette is in the 9 bit color format of course!
def pal2gen(pal):
	# from BGR to BGR with some weird padding!
	pal += [0] * (16 - len(pal))
	return [((x & 0x7) << 1) | ((x & 0x38) << 2) | ((x & 0x1C0) << 3) for x in pal]

# Transform hex ABC colour to 9b format
def hexto9b(col):
	v = int(col, 16)
	assert (v & 1 == 0) and (v & 16 == 0) and (v & 256 == 0)
	return ((v >> 1) & 7) | ((v >> 2) & 0x38) | ((v >> 3) & 0x1C0)

# Inverse as above
def t9btohex(x):
	x = ((x & 0x7) << 1) | ((x & 0x38) << 2) | ((x & 0x1C0) << 3)
	return hex(x)[2:]

# Returns a forced list, issues with the xmltodict library.
def getlist(x):
	if type(x) is not list:
		return [x]
	return x

# Fades out a color by some amount (zero is black, 1 is the color itself)
def col9fade(c, amount):
	c = c & 0x1FF
	r, g, b = c >> 6, (c >> 3) & 7, c & 7
	r = int(r * amount)
	g = int(g * amount)
	b = int(b * amount)
	return (r << 6) | (g << 3) | b

# Class to manipulate TMX files
class TMXMap(object):
	def __init__(self, fname):
		self._fname = fname
		self._xml = xmltodict.parse(open(fname, "r").read())
		lmap = self._xml["map"]
		assert int(lmap["@tilewidth"]) == int(lmap["@tileheight"])
		self._N = int(lmap["@tilewidth"])
		self._w, self._h = int(lmap["@width"]), int(lmap["@height"])

		#print("Loaded a map with %dx%d tiles and %d layers and %d tilemaps" % (w, h, len(lmap["layer"]), len(lmap["tileset"])))

		self._layers = [TMXMap.parseLayer(x) for x in getlist(lmap["layer"])]
		self._layers = {x[0].lower(): x[1:] for x in self._layers}

		# Load tilesets, assuming well-formed ones (non-overlapping tile counts)
		self._tilesets = sorted([self.loadTS(x) for x in getlist(lmap["tileset"])])

		# Load objects
		if "objectgroup" in lmap:
			self._objgrps = dict([self.loadObjs(x) for x in getlist(lmap["objectgroup"])])

	@staticmethod
	def parseLayer(l):
		name, w, h = l["@name"], int(l["@width"]), int(l["@height"])
		assert l["data"]["@encoding"] == "csv"
		# Read properties too
		props = {}
		if "properties" in l:
			props = getlist(l["properties"]["property"])
			props = { x["@name"]: x["@value"] for x in props }
		data = l["data"]["#text"].replace("\n", "").replace(" ", "").strip(",")
		# Note: any missing tile is mapped to the first tile in the tileset!
		tiles = [max(0, int(x)-1) for x in data.split(",")]
		tmap = [tiles[r*w: (r+1)*w] for r in range(h)]
		return name, w, h, tmap, props

	def loadObjs(self, t):
		ret = []
		for obj in t["object"]:
			n, w, h = obj["@name"], int(obj["@width"]), int(obj["@height"])
			x, y = int(obj["@x"]), int(obj["@y"])
			props = {}
			if "properties" in obj:
				props = getlist(obj["properties"]["property"])
				props = { x["@name"]: x["@value"] for x in props }
			ret.append((n, x, y, w, h, props))
		return t["@name"], ret

	def loadTS(self, t):
		baset = int(t["@firstgid"])-1
		if "@source" in t:
			sourcef = t["@source"]
			tsc = xmltodict.parse(open(os.path.join(os.path.dirname(self._fname), sourcef)).read())
			tile_set = tsc["tileset"]
		else:
			tile_set = t

		tcount = int(tile_set["@tilecount"])
		image = tile_set["image"]["@source"]
		im = Image.open(os.path.join(os.path.dirname(self._fname), image))
		N = self._N
		tileset = {}
		for c in range(int(im.size[0]/N)):
			for r in range(int(im.size[1]/N)):
				tileset[c+r*int(im.size[1]/N)+baset] = im.crop((c*N, r*N, (c+1)*N, (r+1)*N))

		# Tileset for tile num: [baset, baset+tcount)
		return baset, tcount, image, tileset

	def baseTile(self, tn):
		cand = None
		for ts in self._tilesets:
			if tn >= ts[0]:
				cand = ts
		return cand

	def collLayer(self, clayer, vname, mapper=lambda x: x):
		"""Processes a layer assuming we only care about relative tile index."""
		# The collision layer must have a "collision-type" property
		coltype = {
			"boolean": (1, 3),
			"quad":    (2, 2),
			"nibble":  (4, 1),
			"full":    (8, 0),
		}
		nbits, sa = coltype[clayer[3]["collision-type"]]
		# Round up if needed!
		colcount = (clayer[0]+(8//nbits-1)) >> sa

		ret = "static const uint8_t %s[%d][%d] = {\n" % (vname, clayer[1], colcount)
		for row in clayer[2]:
			nums, steps = [], 8//nbits
			for i in range(0, len(row), 8//nbits):
				block = [mapper(x-self.baseTile(x)[0]) for x in row[i:i+8//nbits]]
				assert all((x >= 0 and x < 2**nbits) for x in block)
				nums.append(sum(x << (j*nbits) for j, x in enumerate(block)))
			ret += "{" + (",".join([str(x) for x in nums])) + "},\n"
		ret += "};\n\n"
		ret += "#define %s_height %d\n" % (vname, clayer[1])
		ret += "#define %s_width  %d\n" % (vname, clayer[0])
		ret += "#define %s_width8 %d\n" % (vname, colcount)

		return ret


