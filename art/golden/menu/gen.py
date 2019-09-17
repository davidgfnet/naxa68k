
from PIL import Image
import random

# Generate a big image and a collection of small ones (say 32)
chunks = []
for i in range(32):
	im = Image.new("RGBA", (8,8))
	p = im.load()
	for x in range(8):
		for y in range(8):
			n = int(random.random() * 7) + 1
			p[x, y] = (0x20 * n, 0x20 * n, 0x20 * n, 255)
	chunks.append(im)

im = Image.new("RGBA", (512,512))
for i in range(64):
	for j in range(64):
		# Pick cand
		p = chunks[int(random.random() * len(chunks))]
		im.paste(p, (i*8, j*8, (i+1)*8, (j+1)*8))

im.save("basenoise.png")
