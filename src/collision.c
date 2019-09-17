
#include <genesis.h>
#include "collision.h"

// Character coords tracking.
// For precision reasons there's 4 LSB bits used as fixed point.
int char_w, char_h, char_x, char_y;
int char_vspeed, char_hspeed;
bool onfloor;


// Process character collision and dynamics assuming there's gravity that pulls
// it to the ground (by changing its speed).
void collisionGravityProcess(const uint8_t *collmap, unsigned width) {
	// Now check whether the character is into some wall and push it back
	// Add -1 to ensure we don't read the tile just below the wall
	int charpx = char_x >> 4;
	int charpy = char_y >> 4;

	for (int y = charpy + char_h/4; y > charpy - char_h/4; y -= 4) {
		unsigned tiley = y >> 3;
		// First right
		unsigned rightx = (charpx + char_w/2) >> 3;
		uint8_t colld1 = collmap[(rightx >> 3) + tiley * width];   // It's a bitmap! Hence tilex/8
		if (colld1 & (1 << (rightx & 7))) {
			// Push left a bit :)
			charpx = (rightx << 3) - char_w/2;
			char_x = charpx << 4;
			char_hspeed = 0;
		}
		// Noe left right
		unsigned leftx = (charpx - char_w/2) >> 3;
		uint8_t colld2 = collmap[(leftx >> 3) + tiley * width];   // It's a bitmap! Hence tilex/8
		if (colld2 & (1 << (leftx & 7))) {
			// Push right a bit :)
			charpx = (leftx << 3) + 8 + char_w/2;
			char_x = charpx << 4;
			char_hspeed = 0;
		}
	}


	// First check the bottom bits of the character and we "float" it
	// until it reaches the floor line. We do that until center coord.
	onfloor = FALSE;
	int xcheck[3] = { charpx - char_w/2 + 4, charpx, charpx + char_w/2 - 4 };

	for (int i = 0; i < 3; i++) {
		int x = xcheck[i];
		unsigned charb = (charpy + char_h/2);
		unsigned tilex = x >> 3;
		unsigned tiley = charb >> 3;
		uint8_t colld = collmap[(tilex>>3) + tiley * width];   // It's a bitmap! Hence tilex/8
		if (colld & (1 << (tilex & 7))) {
			// There's something under us, check how many pixels we need to "up" it
			charpy = (tiley << 3) - char_h/2;
			char_y = charpy << 4;
			onfloor = TRUE;
			// Stop any movement trying to go down
			if (char_vspeed < 0)
				char_vspeed = 0;
		}
	}

	for (int i = 0; i < 3; i++) {
		int x = xcheck[i];
		unsigned charb = (charpy - char_h/2);
		unsigned tilex = x >> 3;
		unsigned tiley = charb >> 3;
		uint8_t colld = collmap[(tilex>>3) + tiley * width];   // It's a bitmap! Hence tilex/8
		if (colld & (1 << (tilex & 7))) {
			// There's something under us, check how many pixels we need to "up" it
			charpy = (tiley << 3) + 8 + char_h/2;
			char_y = charpy << 4;
			if (char_vspeed > 0)
				char_vspeed = 0;
		}
	}
}

void collisionMapProcess(const uint8_t *collmap, unsigned width) {
	// Just check character and push it outside of collision areas.
	// This is used in map-like games with tiled obstacles (like Pokemon/Bomberman).

	// Collision right:
	unsigned centery = char_y >> 3;
	unsigned rightx  = (char_x + char_w/2);
	unsigned rightx3 = rightx >> 3;  // Divide by 8 to get to tile coord
	uint8_t coll1 = collmap[(rightx3 >> 3) + centery * width];

	if (coll1 & (1 << (rightx3 & 7)))  // Push the char left
		char_x = (rightx3 << 3) - char_w/2;

	// Collision left:
	unsigned leftx  = (char_x - char_w/2);
	unsigned leftx3 = leftx >> 3;
	uint8_t coll2 = collmap[(leftx3 >> 3) + centery * width];

	if (coll2 & (1 << (leftx3 & 7)))  // Push the char right
		char_x = (leftx3 << 3) + 8 + char_w/2;

	// Collision top:
	unsigned centerx = char_x >> 3;
	unsigned topy  = (char_y - char_h/2);
	unsigned topy3 = topy >> 3;
	uint8_t coll3 = collmap[topy3 * width + (centerx >> 3)];

	if (coll3 & (1 << (centerx & 7)))  // Push the char down
		char_y = (topy3 << 3) + 8 + char_h/2;

	// Collision bottom:
	unsigned bottomy  = (char_y + char_h/2);
	unsigned bottomy3 = bottomy >> 3;
	uint8_t coll4 = collmap[bottomy3 * width + (centerx >> 3)];

	if (coll4 & (1 << (centerx & 7)))  // Push the char up
		char_y = (bottomy3 << 3) - char_h/2;
}

void collisionMapProcess16(const uint8_t *collmap, unsigned width) {
	// Just check character and push it outside of collision areas.
	// This is used in map-like games with tiled obstacles (like Pokemon/Bomberman).
	// same as above but coords are .4 fixed

	// Collision right:
	unsigned centery = char_y >> 7;
	unsigned rightx  = ((char_x >> 4)  + char_w/2);
	unsigned rightx3 = rightx >> 3;  // Divide by 8 to get to tile coord
	uint8_t coll1 = collmap[(rightx3 >> 3) + centery * width];

	if (coll1 & (1 << (rightx3 & 7)))  // Push the char left
		char_x = ((rightx3 << 3) - char_w/2) << 4;

	// Collision left:
	unsigned leftx  = ((char_x >> 4) - char_w/2);
	unsigned leftx3 = leftx >> 3;
	uint8_t coll2 = collmap[(leftx3 >> 3) + centery * width];

	if (coll2 & (1 << (leftx3 & 7)))  // Push the char right
		char_x = ((leftx3 << 3) + 8 + char_w/2) << 4;

	// Collision top:
	unsigned centerx = char_x >> 7;
	unsigned topy  = ((char_y >> 4) - char_h/2);
	unsigned topy3 = topy >> 3;
	uint8_t coll3 = collmap[topy3 * width + (centerx >> 3)];

	if (coll3 & (1 << (centerx & 7)))  // Push the char down
		char_y = ((topy3 << 3) + 8 + char_h/2) << 4;

	// Collision bottom:
	unsigned bottomy  = ((char_y >> 4) + char_h/2);
	unsigned bottomy3 = bottomy >> 3;
	uint8_t coll4 = collmap[bottomy3 * width + (centerx >> 3)];

	if (coll4 & (1 << (centerx & 7)))  // Push the char up
		char_y = ((bottomy3 << 3) - char_h/2) << 4;
}

