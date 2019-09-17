#include <genesis.h>
#include "config.h"
#include "util.h"
#include "levels.h"

#include "resources.h"
#include "assets/menu/tv_planea1.h"
#include "assets/menu/tv_planea2.h"
#include "assets/menu/tv_planea3.h"
#include "assets/menu/tv_planeb.h"
#include "assets/menu/menu.h"
#include "assets/menu/gameover.h"
#include "assets/menu/chars.h"

// Initial menu with small menu intro.

// Tile layout, this fits exactly (+4 transparent tiles :D)
#define TV_BGPB_TB  (                                        0)
#define TV_BGA1_TB  (                  sprites_tv_planeb_count)
#define TV_BGA2_TB  (TV_BGA1_TB +     sprites_tv_planea1_count)
#define TV_BGA3_TB  (TV_BGA2_TB +     sprites_tv_planea2_count)
#define SPCHARS_TB  (TV_BGA3_TB +     sprites_tv_planea3_count)
#define MENUTTL_TB  (SPCHARS_TB +          sprites_chars_count)

//sprites_chars_count
#define FINALTL_TB  (MENUTTL_TB +     sprites_titles_count)

#define EDGEUP(k, pk, mask)   ( !((pk) & (mask)) && ((k) & (mask)) )

// Reuse BGA2 once it is not used any more, show actual menu.

_Static_assert(FINALTL_TB <= GAME_TRANS_TILE, "Tile count sanity");

// Waves of enemies are described in the following structure.
// In general enemies come in waves, that move toghether of follow a trajectory.

// Set character to play and returns level to load (usually 1)
unsigned menulvl() {
	// Reset to a known state
	Z80_init();
	VDP_resetSprites();

	loadDecompressTiles(tiles_compr_tv_planeb, TV_BGPB_TB, DMA);
	VDP_loadTileData(tiles_tv_planea1,     TV_BGA1_TB, sprites_tv_planea1_count, DMA);
	VDP_loadTileData(tiles_tv_planea2,     TV_BGA2_TB, sprites_tv_planea2_count, DMA);
	VDP_loadTileData(tiles_tv_planea3,     TV_BGA3_TB, sprites_tv_planea3_count, DMA);
	VDP_loadTileData(sprites_chars,        SPCHARS_TB, sprites_chars_count, DMA);
	VDP_loadTileData(sprites_titles,       MENUTTL_TB, sprites_titles_count, DMA);
	DMA_flushQueue();

	XGM_setPCM(128, menu_select_snd, sizeof(menu_select_snd));

	// Viewport and level loading
	VDP_setVerticalScroll(PLAN_B, 0);
	VDP_setVerticalScroll(PLAN_A, 0);
	VDP_setHorizontalScroll(PLAN_B, 0);
	VDP_setHorizontalScroll(PLAN_A, 0);

	// Load background and noise planes (use scroll to switch, so load at offset 32)
	VDP_fillTileMap(VDP_PLAN_A, TILE_ATTR_FULL(PAL0, 0, 0, 0, GAME_TRANS_TILE), 0, 64*64);
	VDP_fillTileMap(VDP_PLAN_B, TILE_ATTR_FULL(PAL0, 0, 0, 0, GAME_TRANS_TILE), 0, 64*64);
	for (unsigned x = 0; x < 40; x++) {
		for (unsigned y = 0; y < 30; y++) {
			// This is a hack to prevent PlaneA (when scrolled horz) to show around the edges
			// of the screen. Just making PlaneB high prio in those 2-3 tile columns is enough.
			unsigned p = (x < 4 || x > 36) ? 1 : 0;
			VDP_setTileMapXY(PLAN_B, TILE_ATTR_FULL(PAL0, p, 0, 0, TV_BGPB_TB + tilemap_tv_planeb [x + y*40]), x, y);
			VDP_setTileMapXY(PLAN_A, TILE_ATTR_FULL(PAL1, 0, 0, 0, TV_BGA1_TB + tilemap_tv_planea1[x + y*40]), x, y);
			VDP_setTileMapXY(PLAN_A, TILE_ATTR_FULL(PAL2, 0, 0, 0, TV_BGA2_TB + tilemap_tv_planea2[x + y*40]), x, y+32);
		}
	}

	// Play initial noise
	XGM_setMusicTempo(50);
	XGM_setLoopNumber(0);
	XGM_startPlay(music_noise);

	uint16_t refpal[4][16];
	memcpy(refpal[0], tv_planeb_palette, 32);
	memcpy(refpal[1], tv_planea1_palette, 32);
	memcpy(refpal[2], tv_planea2_palette, 32);
	memcpy(refpal[3], ps_palette, 32);

	VDP_setEnable(1);

	unsigned magic_fsm = 0;
	unsigned levelstatus = LEVEL_STARTING;
	unsigned fsm = 0;
	unsigned fcounter = 0;
	uint16_t pkey = 0;
	bool enable_selector = FALSE;
	while (fsm < 7) {
		// Rotate palette for gaussian blur
		for (unsigned n = 1; n < 3; n++) {
			int p1 = (randomu32() % 7) + 1;
			int p2 = (randomu32() % 7) + 1;
			uint16_t tmp = refpal[n][p1];
			refpal[n][p1] = refpal[n][p2];
			refpal[n][p2] = tmp;
		}

		uint16_t workpal[4][16];
		switch (levelstatus) {
		case LEVEL_STARTING:
			if (fcounter >= 32)
				levelstatus = LEVEL_PLAY;
			unsigned nf = fcounter >> 2;
			for (unsigned i = 0; i < 4; i++)
				fade_dark_pal(workpal[i], &refpal[i][0], nf, 3);
			break;
		case LEVEL_PLAY:
			memcpy(workpal, refpal, sizeof(workpal));
			break;
		};

		// Check magic combination
		uint16_t k = JOY_readJoypad(JOY_1);
		if      (magic_fsm == 0 && EDGEUP(k, pkey, BUTTON_UP))     magic_fsm++;
		else if (magic_fsm == 1 && EDGEUP(k, pkey, BUTTON_DOWN))   magic_fsm++;
		else if (magic_fsm == 2 && EDGEUP(k, pkey, BUTTON_LEFT))   magic_fsm++;
		else if (magic_fsm == 3 && EDGEUP(k, pkey, BUTTON_RIGHT))  magic_fsm++;
		else if (EDGEUP(k, pkey, (BUTTON_UP|BUTTON_DOWN|BUTTON_LEFT|BUTTON_RIGHT)))
			magic_fsm = 0;

		if (magic_fsm == 4 && (k & BUTTON_A) && (k & BUTTON_B) && (k & BUTTON_C))
			magic_fsm++;
		pkey = k;

		VDP_waitVSync();
		for (unsigned i = 0; i < 4; i++)
			VDP_setPalette(PAL0+i, workpal[i]);

		bool advanced_state = (fcounter > (100 >> CLAMPMAX((fsm >> 1), 3)) && fsm != 4 && fsm < 7);

		if (advanced_state) {
			switch (fsm) {
			case 0:
				// Show the other noise
				VDP_setVerticalScroll(PLAN_A, 32*8);
				break;
			case 1:
				// Remove all noise
				VDP_setHorizontalScroll(PLAN_A, 32*8);
				VDP_fillTileMap(VDP_PLAN_A, TILE_ATTR_FULL(PAL0, 0, 0, 0, GAME_TRANS_TILE), 64*32, 64*32);
				break;
			case 2:
				// Start playing song!
				XGM_stopPlay();
				XGM_setMusicTempo(60);  // From defle
				XGM_startPlay(music_menu);
				break;
			case 3:
				// Show press start button
				VDP_loadTileData(sprites_pressstart, TV_BGA2_TB, sprites_pressstart_count, CPU);
				break;
			case 5:
				// Show noise briefly
				VDP_setVerticalScroll(PLAN_A, 0);
				VDP_setHorizontalScroll(PLAN_A, 0);
				VDP_resetSprites();

				// Remove the transp noise in favour of next thingy
				for (unsigned x = 0; x < 40; x++)
					for (unsigned y = 0; y < 30; y++)
						VDP_setTileMapXY(PLAN_A, TILE_ATTR_FULL(PAL3, 0, 0, 0,
							TV_BGA3_TB + tilemap_tv_planea3[x + y*40]), x, y+32);
				break;
			case 6:
				// Transition to the chroma and menu
				VDP_setPalette(PAL3, tv_planea3_palette);
				memcpy(refpal[3], tv_planea3_palette, 32);
				VDP_setVerticalScroll(PLAN_A, 32*8);
				VDP_setHorizontalScroll(PLAN_A, 0);
				break;
			};
		}

		if (fsm == 4) {
			// 80, 164 starts
			if (fcounter & 32) {
				for (unsigned i = 0; i < 5; i++)
					VDP_setSpriteFull(i, 80 + (i << 5), 164, SPRITE_SIZE(4, 2),
						TILE_ATTR_FULL(PAL3, 1, 0, 0, TV_BGA2_TB + (i << 3)), i+1);
				VDP_setSpriteLink(4, 0);
			}
			else
				VDP_resetSprites();

			// TODO add some magic to enable level selection here!
			if (JOY_readJoypad(JOY_1) & BUTTON_START) {
				if (magic_fsm == 5)
					enable_selector = TRUE;
				advanced_state = true;
				XGM_startPlayPCM(128, 0, 1);
			}
		}

		if (advanced_state) {
			fcounter = 0;
			fsm++;
		}

		VDP_updateSprites(16, FALSE);
		fcounter++;
	}

	// Show the menu itself.
	VDP_setPalette(PAL1, char_palette);
	VDP_setPalette(PAL2, ps_palette);
	memcpy(refpal[1], char_palette, 32);
	memcpy(refpal[2], ps_palette, 32);
	unsigned prvopt = 1, levelsel = 0;
	pkey = 0;
	while (1) {
		uint16_t k = JOY_readJoypad(JOY_1);
		if (k & BUTTON_LEFT)
			playerchar = 0;
		else if (k & BUTTON_RIGHT)
			playerchar = 1;
		else if (!pkey && (k & BUTTON_UP) && enable_selector) {
			levelsel++;
			XGM_startPlayPCM(128, 0, 1);   // Noise to indicate level selection (hidden)
		}
		else if (!pkey && (k & BUTTON_DOWN) && enable_selector) {
			levelsel--;
			XGM_startPlayPCM(128, 0, 1);
		}
		else if (k & BUTTON_START) {
			// Selected option can be terminating
			break;
		}
		pkey = k;
		if (playerchar != prvopt)
			XGM_startPlayPCM(128, 0, 1);
		prvopt = playerchar;

		VDP_waitVSync();

		VDP_setSpriteFull(0,  80, 80, SPRITE_SIZE(2, 4), TILE_ATTR_FULL(PAL1, 1, 0, 0, SPCHARS_TB), 1);
		VDP_setSpriteFull(1, 224, 80, SPRITE_SIZE(2, 4), TILE_ATTR_FULL(PAL1, 1, 0, 0, SPCHARS_TB + 8), 2);
		if (!playerchar) {
			VDP_setSpriteFull(2, 104, 96, SPRITE_SIZE(4, 2), TILE_ATTR_FULL(PAL2, 1, 0, 0, MENUTTL_TB + 30), 3);
			VDP_setSpriteFull(3, 136, 96, SPRITE_SIZE(4, 2), TILE_ATTR_FULL(PAL2, 1, 0, 0, MENUTTL_TB + 38), 4);
			VDP_setSpriteFull(4, 168, 96, SPRITE_SIZE(1, 2), TILE_ATTR_FULL(PAL2, 1, 0, 0, MENUTTL_TB + 46), 0);
		}
		else {
			VDP_setSpriteFull(2, 104, 96, SPRITE_SIZE(4, 2), TILE_ATTR_FULL(PAL2, 1, 0, 0, MENUTTL_TB +  0), 3);
			VDP_setSpriteFull(3, 136, 96, SPRITE_SIZE(4, 2), TILE_ATTR_FULL(PAL2, 1, 0, 0, MENUTTL_TB +  8), 4);
			VDP_setSpriteFull(4, 168, 96, SPRITE_SIZE(4, 2), TILE_ATTR_FULL(PAL2, 1, 0, 0, MENUTTL_TB + 16), 5);
			VDP_setSpriteFull(5, 200, 96, SPRITE_SIZE(3, 2), TILE_ATTR_FULL(PAL2, 1, 0, 0, MENUTTL_TB + 24), 0);
		}
		VDP_updateSprites(16, FALSE);
	}

	// Do fade to black here
	for (unsigned i = 0; i < 32; i++) {
		uint16_t workpal[4][16];
		for (unsigned j = 0; j < 4; j++)
			fade_dark_pal(workpal[j], &refpal[j][0], 8 - (i >> 2), 3);
		VDP_waitVSync();
		for (unsigned j = 0; j < 4; j++)
			VDP_setPalette(PAL0+j, workpal[j]);
	}

	XGM_stopPlay();

	return levelsel;
}

void gameover() {
	VDP_resetSprites();
	VDP_updateSprites(32, FALSE);

	loadDecompressTiles(tiles_compr_gameover, 0, DMA);
	DMA_flushQueue();

	VDP_fillTileMap(VDP_PLAN_A, TILE_ATTR_FULL(PAL0, 0, 0, 0, GAME_TRANS_TILE), 0, 64*64);
	VDP_fillTileMap(VDP_PLAN_B, TILE_ATTR_FULL(PAL0, 0, 0, 0, GAME_TRANS_TILE), 0, 64*64);
	for (unsigned x = 0; x < tilemap_gameover_width; x++)
		for (unsigned y = 0; y < tilemap_gameover_height; y++)
			VDP_setTileMapXY(PLAN_B, TILE_ATTR_FULL(PAL0, 0, 0, 0, 
				tilemap_gameover[x + y*tilemap_gameover_width]), x, y);

	VDP_setVerticalScroll(PLAN_B, -56);
	VDP_setHorizontalScroll(PLAN_B, 64);
	uint16_t workpal[16] = {0};

	VDP_setEnable(1);

	unsigned fcounter = 0;
	while (fcounter < 64) {
		if (fcounter <= 32)
			fade_dark_pal(workpal, gameover_palette, (fcounter >> 2), 3);
		else
			fade_dark_pal(workpal, gameover_palette, 8 - ((fcounter - 32) >> 2), 3);

		VDP_waitVSync();
		VDP_setPalette(PAL0, workpal);
		if (fcounter != 32)
			fcounter++;
		else if (JOY_readJoypad(JOY_1) & BUTTON_START)
			fcounter++;
	}
}


