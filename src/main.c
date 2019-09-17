#include <genesis.h>
#include "config.h"
#include "level_functions.h"
#include "levels.h"
#include "collision.h"
#include "util.h"

#define MAX_LIFES   4    // You can play up to 5 times

enum UberState {
	eMenu = 0,
	eIntro = 1,        // Intro story to the game
	eArcade = 2,       // Arcade like level right after intro
	eMetalevel0 = 3,   // Intro in metalevel
	// Display checkpoint code!
	eBomberman = 4,    // Bomberman game
	eMetalevel1 = 5,   // Cartridge unblock level
	// Display checkpoint code!
	eDungeon = 6,      // Sent to dungeon!
	eMetalevel2 = 7,   // Got part and now exit
	// Display checkpoint code!
	eShooter = 8,      // Travel very fast to get out
	eMetalevel3 = 9,   // Return home!
	// Display checkpoint code for credits!
	eCredits = 10,     // Credits
	eGameOver = 11,    // Game Over screen!
};

int main() {
	// Init VDP basics to bring this to a good known state
	VDP_setEnable(0);
	VDP_resetScreen();           // Reset stuff to a known state
	VDP_setScreenHeight240();    // Be compatible with PAL and NTSC, also easier for scrolling
	VDP_setScreenWidth320();

	// Use 8KB for each plane, to have a 64x64 tile plane
	VDP_setPlanSize(64, 64);
	VDP_setHScrollTableAddress(GAME_HSCROLL_ADDR);
	VDP_setSpriteListAddress(GAME_SPRITES_ADDR);
	VDP_setBPlanAddress(GAME_BPLANES_ADDR);
	VDP_setAPlanAddress(GAME_APLANES_ADDR);

	// Reset scrolling to plane scrolling and reset it.
	VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
	VDP_setHorizontalScroll(PLAN_A, 0);
	VDP_setVerticalScroll(PLAN_A, 0);

	// Fill transparent tiles, set palettes to black.
	VDP_fillTileData(0, GAME_TRANS_TILE, 4, TRUE);
	for (uint16_t idx = 0; idx < 64; idx++)
		VDP_setPaletteColor(idx, 0);

	// Start the random num generator, not sure how really random it is tho.
	random_reseed(GET_HVCOUNTER ^ 0xdeadbeef);

	// Select player 1 by default, testing...
	playerchar = 1;

	// Meta level tracking, here transitions between levels, menus, etc happen
	enum UberState ustate = eMenu;
	int level_lives = MAX_LIFES;
	while (1) {
		int rl = LEVEL_ACHIEVED;
		switch (ustate) {
		case eMenu:
			level_lives = MAX_LIFES;   // Reset lives
			int sel = menulvl();
			ustate += sel;         // Level skip (+1 implicit)
			break;
		case eIntro:
			dointro();
			break;
		case eArcade:
			rl = level1();
			break;
		case eMetalevel0:
		case eMetalevel1:
		case eMetalevel2:
		case eMetalevel3:
			metalevel((ustate - eMetalevel0) >> 1);
			break;
		case eBomberman:
			rl = level2();
			break;
		case eDungeon:
			rl = level4();
			break;
		case eShooter:
			rl = level3();
			break;
		case eCredits:
			docredits();
			break;
		case eGameOver:
			gameover();
			break;
		};

		// Level finished, stop VDP before calling out a new level
		VDP_setEnable(0);

		// Clear to black for fadein/out
		for (uint16_t idx = 0; idx < 64; idx++)
			VDP_setPaletteColor(idx, 0);
		VDP_resetSprites();
		VDP_updateSprites(128, FALSE);
		VDP_fillTileMap(VDP_PLAN_A, TILE_ATTR_FULL(PAL0, 0, 0, 0, GAME_TRANS_TILE), 0, 64*64);
		VDP_fillTileMap(VDP_PLAN_B, TILE_ATTR_FULL(PAL0, 0, 0, 0, GAME_TRANS_TILE), 0, 64*64);

		// Check level success status now
		if (rl == LEVEL_ACHIEVED) {
			ustate++;
			if (ustate > eCredits)
				ustate = eMenu;
		}
		else {
			// Just remove a life for now...
			level_lives--;
			if (level_lives < 0)
				ustate = eGameOver;
		}
	}
}

