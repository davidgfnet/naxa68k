
from PIL import Image

# Just divides the reference image into a checkpattern
# outputing blue and red intensities that we later override.

im = Image.open("tv_planea1.png").convert("RGBA")
pixmap = im.load()
for c in range(im.size[0]):
	for r in range(im.size[1]):
		color = pixmap[c, r]
		if (c & 1) ^ (r & 1):
			pixmap[c, r] = (color[0], color[1], color[2], 0)
im.save("tv_planea2.png")
