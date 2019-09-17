// Copyright 2019 David Guillen Fandos (david@davidgf.net)
// This code is released under the GPL2 license.

// Contains all the functions used for plane scrolling.

#ifndef __LEVEL__H__
#define __LEVEL__H__

#include <stdbool.h>
#include "config.h"

extern unsigned vp_x, vp_y;
extern struct {
	unsigned load_vpx, load_vpy;
} lvlscrl[2];

// Set viewport coordinates
void levelSetViewport(int x, int y);
void levelSetViewportArea(int x, int y, int w, int h);

struct t_packed_tm16 {
	const uint8_t *secondlvl;        // Actual data
	const uint16_t *firstlvl;        // Pointer table
	unsigned firstlvl_stride;  // Stride for the first level table
	uint16_t w, h;             // Bounds checking
};

// WARNING! This assumes that the last tile is set to transparent!
// That is, GAME_TRANS_TILE, is a transparent tile. Used for overflowing
// the tilemap.

// Forces a level reload around the viewport address.
// This will load the tilemap and block until it's loaded. Can be used
// at level start for instance. This might take more than 1 vblank period,
// therefore might be a good idea to set all palettes to a solid color to hide it.
// Also can work with a callback: uint16_t (*callback)(int x, int y)
#define levelForceReload_TEMPLATE(suffix, type, width, height, plane, palette, basetn, callback) \
	static void levelForceReload##suffix(const type *tilemap, const void *ptr) { \
		uint16_t (*callback_fn)(const void *ptr, int x, int y) = callback;  \
		/* Reset this, since we are going to load them all! */ \
		lvlscrl[CONST_##plane].load_vpx = vp_x >> 3; \
		lvlscrl[CONST_##plane].load_vpy = vp_y >> 3; \
		 \
		/* The plane is a 64x64 tile (or 512x512 pixel area) therefore we make it */  \
		/* wrap around the seams. */ \
		VDP_setHorizontalScroll(plane, -(vp_x - _SCREEN_WIDTH/2) & 511); \
		VDP_setVerticalScroll(plane, (vp_y - _SCREEN_HEIGHT/2) & 511); \
		 \
		/* Load the tiles now. */ \
		for (int dx = -32; dx < 32; dx++) { \
			for (int dy = -32; dy < 32; dy++) { \
				int ycoord = lvlscrl[CONST_##plane].load_vpy + dy; \
				int xcoord = lvlscrl[CONST_##plane].load_vpx + dx; \
				bool outcoord = ycoord < 0 || ycoord >= height || xcoord < 0 || xcoord >= width; \
				uint16_t tilenum = callback_fn ? callback_fn(ptr, xcoord, ycoord) : \
				                   outcoord ? GAME_TRANS_TILE : \
				                   tilemap[width * ycoord + xcoord] + basetn; \
				VDP_setTileMapXY(plane, TILE_ATTR_FULL(palette, 0, 0, 0, tilenum), \
					             (xcoord) & 63, (ycoord) & 63);                    \
			} \
		} \
	}


// This should be called every vblank period. It will update the scrolling params
// so that changes in viewport coords are reflected in the screen. This will also
//  check whether new tiles need to be loaded and load a row or a column (perhaps
// both?) to keep up with plane scrolling.
#define levelVblankWork_TEMPLATE(suffix, type, width, height, plane, palette, basetn, callback) \
static void levelVblankWork##suffix(const type *tilemap, const void *ptr) { \
	uint16_t (*callback_fn)(const void *ptr, int x, int y) = callback;  \
	/* Keep track of the latest valid loaded viewport position.                */ \
	/* We move this position in increments of 8 "chasing" the real viewport    */ \
	/* And moving it once per vblank. This allows some fast viewport movements */ \
	/* as long as we can later catch up with the scrolling.                    */ \
	uint8_t epvx = vp_x >> 3; \
	uint8_t epvy = vp_y >> 3; \
	 \
	/* Check if we need to move along horizontal axis */ \
	if (epvx > lvlscrl[CONST_##plane].load_vpx) { \
		/* Need to load a column worth of tiles to the right. */ \
		int xcoord = lvlscrl[CONST_##plane].load_vpx+32; \
		for (int dy = -32; dy < 32; dy++) { \
			int ycoord = lvlscrl[CONST_##plane].load_vpy + dy; \
			bool outcoord = ycoord < 0 || ycoord >= height || xcoord < 0 || xcoord >= width; \
			uint16_t tilenum = callback_fn ? callback_fn(ptr, xcoord, ycoord) : \
			                   outcoord ? GAME_TRANS_TILE : \
			                   tilemap[width * ycoord + xcoord] + basetn; \
			VDP_setTileMapXY(plane, TILE_ATTR_FULL(palette, 0, 0, 0, tilenum), \
			                 xcoord & 63, ycoord & 63); \
		} \
		 \
		lvlscrl[CONST_##plane].load_vpx++;   /* Move right 1 column */  \
	} \
	else if (epvx < lvlscrl[CONST_##plane].load_vpx) { \
		lvlscrl[CONST_##plane].load_vpx--;   /* Move left 1 column */  \
		int xcoord = lvlscrl[CONST_##plane].load_vpx-32; \
		/* Need to load a column worth of tiles to the right. */ \
		for (int dy = -32; dy < 32; dy++) { \
			int ycoord = lvlscrl[CONST_##plane].load_vpy + dy; \
			bool outcoord = ycoord < 0 || ycoord >= height || xcoord < 0 || xcoord >= width; \
			uint16_t tilenum = callback_fn ? callback_fn(ptr, xcoord, ycoord) : \
			                   outcoord ? GAME_TRANS_TILE : \
			                   tilemap[width * ycoord + xcoord] + basetn; \
			VDP_setTileMapXY(plane, TILE_ATTR_FULL(palette, 0, 0, 0, tilenum), \
			                 xcoord & 63, ycoord & 63); \
		} \
	} \
	 \
	/* Check if we need to move along vertical axis */  \
	if (epvy > lvlscrl[CONST_##plane].load_vpy) { \
		/* Need to load a column worth of tiles to the right. */  \
		int ycoord = lvlscrl[CONST_##plane].load_vpy+32; \
		for (int dx = -32; dx < 32; dx++) { \
			int xcoord = lvlscrl[CONST_##plane].load_vpx + dx; \
			bool outcoord = ycoord < 0 || ycoord >= height || xcoord < 0 || xcoord >= width; \
			uint16_t tilenum = callback_fn ? callback_fn(ptr, xcoord, ycoord) : \
			                   outcoord ? GAME_TRANS_TILE : \
			                   tilemap[width * ycoord + xcoord] + basetn; \
			VDP_setTileMapXY(plane, TILE_ATTR_FULL(palette, 0, 0, 0, tilenum), \
			                 xcoord & 63, ycoord & 63); \
		} \
		 \
		lvlscrl[CONST_##plane].load_vpy++;   /* Move down 1 row */  \
	} \
	else if (epvy < lvlscrl[CONST_##plane].load_vpy) { \
		lvlscrl[CONST_##plane].load_vpy--;   /* Move up 1 row */  \
		int ycoord = lvlscrl[CONST_##plane].load_vpy-32; \
		/* Need to load a column worth of tiles to the right. */ \
		for (int dx = -32; dx < 32; dx++) { \
			int xcoord = lvlscrl[CONST_##plane].load_vpx + dx; \
			bool outcoord = ycoord < 0 || ycoord >= height || xcoord < 0 || xcoord >= width; \
			uint16_t tilenum = callback_fn ? callback_fn(ptr, xcoord, ycoord) : \
			                   outcoord ? GAME_TRANS_TILE : \
			                   tilemap[width * ycoord + xcoord] + basetn; \
			VDP_setTileMapXY(plane, TILE_ATTR_FULL(palette, 0, 0, 0, tilenum), \
			                 xcoord & 63, ycoord & 63); \
		} \
	} \
	 \
	VDP_setHorizontalScroll(plane, -(vp_x - _SCREEN_WIDTH/2) & 511); \
	VDP_setVerticalScroll(plane, (vp_y - _SCREEN_HEIGHT/2) & 511); \
}


// Forces a reload of the background at some scrolling offset.
// Similar to levelForceReload_TEMPLATE, see docs there.
#define background_H_ForceReload_TEMPLATE(suffix, type, width, height, plane, palette, basetn) \
	static void background_H_ForceReload##suffix(const type *tilemap) { \
		/* The plane is a 64x64 tile, we wrap only around horizontal seams. */ \
		/*VDP_setHorizontalScroll(plane, -(vp_x - _SCREEN_WIDTH/2) & 511);*/ \
		/*VDP_setVerticalScroll(plane, (vp_y - _SCREEN_HEIGHT/2) & 511);*/ \
		VDP_setHorizontalScroll(plane, 0); VDP_setVerticalScroll(plane, 0); \
		 \
		/* Load the tiles now. */ \
		for (int dx = 0; dx < 64; dx++) { \
			for (int dy = 0; dy < 64; dy++) { \
				int xcoord = dx % width; \
				int ycoord = (dy - 32 + height/2 + height*16) % height; \
				uint16_t tilenum = basetn + tilemap[width * ycoord + xcoord]; \
				VDP_setTileMapXY(plane, TILE_ATTR_FULL(palette, 0, 0, 0, tilenum), dx, dy);  \
			} \
		} \
	}


#endif

