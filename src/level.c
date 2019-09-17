
// Copyright 2019 David Guillen Fandos (david@davidgf.net)
// This code is released under the GPL2 license.

// Contains all the functions used for plane scrolling.
// Assuming we use 64x64 tilemaps [1] and that it is possible
// for the viewport to move faster (at a times) than the max
// speed we can possible refill the tilemap.
//
// [1]: Need to reserve 8KB for the planes in order to allow for
//      64x64 maps.

#include <genesis.h>
#include "config.h"

unsigned vp_x, vp_y;
struct {
	unsigned load_vpx, load_vpy;
} lvlscrl[2];

// Centers viewport at coordinates given
void levelSetViewport(int x, int y) {
	vp_x = x;
	vp_y = y;
}

// Tells the viewport to move (if needed) so char can be seen.
// Margins are defined so that VP doesnt move unnecessarily.
void levelSetViewportArea(int x, int y, int w, int h) {
	unsigned relposx = x - (vp_x - _SCREEN_WIDTH/2);
	unsigned relposy = y - (vp_y - _SCREEN_HEIGHT/2);
	unsigned marginx = (_SCREEN_WIDTH  - w) >> 1;
	unsigned marginy = (_SCREEN_HEIGHT - h) >> 1;

	if (relposx < marginx)
		vp_x = x - marginx + _SCREEN_WIDTH/2;
	else if (relposx > _SCREEN_WIDTH - marginx)
		vp_x = x + marginx - _SCREEN_WIDTH/2;

	if (relposy < marginy)
		vp_y = y - marginy + _SCREEN_HEIGHT/2;
	else if (relposy > _SCREEN_HEIGHT - marginy)
		vp_y = y + marginy - _SCREEN_HEIGHT/2;
}


