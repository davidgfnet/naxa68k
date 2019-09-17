#include <genesis.h>
#include "config.h"
#include "util.h"
#include "level_functions.h"
#include "levels.h"
#include "collision.h"

#include "resources.h"
#include "assets/level1/bg.h"
#include "assets/level1/level.h"
#include "assets/level1/aux.h"
#include "assets/level1/enemies.h"
#include "assets/sprites/mainchar24F.h"
#include "assets/sprites/mainchar24M.h"
#include "assets/sprites/portal.h"

// Arcade game inspired on pacman

#define MAINCHAR_W  12
#define MAINCHAR_H  20
#define ENEMY_W 16
#define ENEMY_H 16
#define ENEMY_BB_W 15
#define ENEMY_BB_H 15
#define ENEMY_SPEED 12
#define DISP_MAINCHAR_W  16
#define DISP_MAINCHAR_H  24
#define MAX_FALL_SPEED   (1 << 7)
#define MAX_RUN_SPEED    12
#define FALL_SPEED_STEP   3

#define SND_JUMP        128
#define SND_HIT_DAMAGE  129
#define SND_PORTAL_OPN  130
#define SND_PORTAL_CL   131
#define SND_PORTAL      132

#define SND_CH_EVENTS   1
#define SND_CH_DAMAGE   2
#define SND_CH_JUMP     3

// Tile layout:
#define BACKGND_TB  (                                        0)  // Background tiles   ~64
#define LEVELPA_TB  (                         sprites_bg_count)  // Foreground tiles  ~160
#define MAINCHR_TB  (LEVELPA_TB +          sprites_level_count)  // Main char tiles    ~48
#define ENEMIES_TB  (MAINCHR_TB +    sprites_mainchar24M_count)  // Enemy tiles        ~80
#define SPORTAL_TB  (ENEMIES_TB +        sprites_enemies_count)  // Portal tiles      ~528
#define FINALTX_TB  (SPORTAL_TB +         sprites_portal_count)  // Final text        ~120

#define MAX_SPRITES_ONSCREEN 64

static void render_text(uint8_t *font, const char *t) {
	// Calculate the char position to fill
	for (unsigned cnum = 0; cnum < 30; cnum++) {
		unsigned addr = (t[cnum] - 32) << 7;
		VDP_loadTileData((uint32_t*)&font[addr], FINALTX_TB + cnum * 4, 4, DMA);
	}
}

levelForceReload_TEMPLATE(8A,
	uint8_t, tilemap_level_width, tilemap_level_height, PLAN_A, PAL1, LEVELPA_TB, NULL)
levelVblankWork_TEMPLATE(8A,
	uint8_t, tilemap_level_width, tilemap_level_height, PLAN_A, PAL1, LEVELPA_TB, NULL)

 _Static_assert(FINALTX_TB + 120 < 1200, "Tile count sanity");

unsigned level1() {
	// Reset to a known state
	Z80_init();
	VDP_resetSprites();
	loadDecompressTiles(sprites_compr_bg, BACKGND_TB, DMA);
	loadDecompressTiles(tiles_compr_level, LEVELPA_TB, DMA);
	loadDecompressTiles(playerchar ? sprites_compr_mainchar24F : sprites_compr_mainchar24M, MAINCHR_TB, DMA);
	loadDecompressTiles(sprites_compr_enemies, ENEMIES_TB, DMA);
	loadDecompressTiles(sprites_compr_portal, SPORTAL_TB, DMA);
	VDP_fillTileData(0, FINALTX_TB, 120, FALSE);
	DMA_flushQueue();

	VDP_fillTileMap(VDP_PLAN_B, TILE_ATTR_FULL(PAL0, 0, 0, 0, GAME_TRANS_TILE), 0, 64*64);
	VDP_fillTileMap(VDP_PLAN_A, TILE_ATTR_FULL(PAL1, 0, 0, 0, GAME_TRANS_TILE), 0, 64*64);

	XGM_setPCM(SND_JUMP, jump_snd, sizeof(jump_snd));
	XGM_setPCM(SND_HIT_DAMAGE, arcade_hit, sizeof(arcade_hit));
	XGM_setPCM(SND_PORTAL_OPN, portal_opens, sizeof(portal_opens));
	XGM_setPCM(SND_PORTAL_CL, portal_closes, sizeof(portal_closes));
	XGM_setPCM(SND_PORTAL, portal, sizeof(portal));

	// Create background using random tiles in BG
	uint8_t pattern[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
	for (unsigned i = 0; i < 64; i++) {
		unsigned p1 = i & 15, p2 = randomu32() & 15;
		uint8_t t = pattern[p1];
		pattern[p1] = pattern[p2];
		pattern[p2] = t;
	}
	VDP_setVerticalScroll(PLAN_B, 0);
	VDP_setVerticalScroll(PLAN_A, 0);
	for (unsigned y = 0; y < 21; y++) {
		for (unsigned x = 0; x < 21; x++) {
			unsigned bt = (pattern[((x + ((y >> 2) << 1)) & 3) | ((y & 3) << 2)] << 2) + BACKGND_TB;
			VDP_setTileMapXY(PLAN_B, TILE_ATTR_FULL(PAL0, 0, 0, 0, bt  ), 3*x  , 3*y  );
			VDP_setTileMapXY(PLAN_B, TILE_ATTR_FULL(PAL0, 0, 0, 0, bt+1), 3*x  , 3*y+1);
			VDP_setTileMapXY(PLAN_B, TILE_ATTR_FULL(PAL0, 0, 0, 0, bt+2), 3*x+1, 3*y  );
			VDP_setTileMapXY(PLAN_B, TILE_ATTR_FULL(PAL0, 0, 0, 0, bt+3), 3*x+1, 3*y+1);
		}
	}

	// Create final title, discard font after doing so.
	{
		uint8_t localfont[400 * 32];
		load_font16(localfont);
		render_text(localfont, "Danger!        Corrupted level");
	}


	// Init char level
	char_w = MAINCHAR_W;
	char_h = MAINCHAR_H;
	char_x = (60) << 4;  // 900<<4
	char_y = (344) << 4; // 440<< 4;

	levelSetViewport(char_x >> 4, char_y >> 4);
	levelForceReload8A(&tilemap_level[0], NULL);

	uint16_t refpal[4][16], workpal[4][16];
	memcpy(refpal[0], bg_palette, 32);
	memcpy(refpal[1], level_palette, 32);
	memcpy(refpal[2], portal_palette, 32);
	memcpy(refpal[3], playerchar ? mainchar24F_palette : mainchar24M_palette, 32);
	memset(workpal, 0, sizeof(workpal));

	VDP_setEnable(1);

	// Do the intro, just render the portal and make the character jump through it.
	// Everything goes into 32 frame cycles:
	// 2 cycles: fade in
	// 1 cycles: open portal
	// 2 cycles: portal is open
	// 1 cycles: jump!
	// 1 cycle: portal is open
	// 1 cycle: portal closes
	// 4 cycles wait
	for (unsigned i = 0; i < 32*12; i++) {
		VDP_setHorizontalScroll(PLAN_B, -((3*vp_x) >> 4));
		VDP_setVerticalScroll(PLAN_B, vp_y >> 3);
		unsigned cycle = i >> 5;

		unsigned pframe = ((i >> 2) & 7) + (cycle < 3 ? 0 : cycle >= 7 ? 16 : 8);
		pframe = CLAMPMAX(pframe, 21);

		// Initial: 0, 330
		// Player higher point: 24, 320
		// Final: 48, 330
		// Eq: +0.01736*x^2 - 0.8333x + 10 + 320
		int x0 = i - 5*32;
		int x = CLAMPMAX(8 + x0 + (x0 >> 2), 48);
		int y = ((x * x * 568) >> 16) - ((27305 * x) >> 16) + 329;

		int scxoff = vp_x - _SCREEN_WIDTH/2;
		int scyoff = vp_y - _SCREEN_HEIGHT/2;

		// A bit of hacky frame-almost-accurate music :)
		if (i == 2*32)
			XGM_startPlayPCM(SND_PORTAL_OPN, 0, SND_CH_EVENTS);
		else if (i > 2*32+15 && i < 7*32 && !XGM_isPlayingPCM(1))
			XGM_startPlayPCM(SND_PORTAL, 0, SND_CH_EVENTS);
		else if (i == 7*32)
			XGM_startPlayPCM(SND_PORTAL_CL, 0, SND_CH_EVENTS);

		// Draw the two parts in two chunks (total 4 bits) using different priorities.
		unsigned nextsprite = 0;
		if (i <= 32) {
			for (unsigned j = 0; j < 4; j++)
				fade_dark_pal(workpal[j], &refpal[j][0], (i >> 2), 3);
		}
		if (cycle >= 2 && cycle <= 7) {
			SET_SPRITE_PLUS(nextsprite, 4 - scxoff, 310 - scyoff,
				SPRITE_SIZE(2, 3), TILE_ATTR_FULL(PAL2, 1, 0, 0, SPORTAL_TB + 24*pframe));
			SET_SPRITE_PLUS(nextsprite, 4 - scxoff, 334 - scyoff,
				SPRITE_SIZE(2, 3), TILE_ATTR_FULL(PAL2, 1, 0, 0, SPORTAL_TB + 12 + 24*pframe));
		}
		if (cycle >= 5) {
			unsigned orh = cycle == 9 ? 1 : 0;
			SET_SPRITE_PLUS(nextsprite, x - scxoff, y - scyoff,
				SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PAL3, 0, 0, orh, MAINCHR_TB + 9 * (x >= 48 ? 7 : 1)))
		}
		if (cycle >= 2 && cycle <= 7) {
			SET_SPRITE_PLUS(nextsprite, 20 - scxoff, 310 - scyoff,
				SPRITE_SIZE(2, 3), TILE_ATTR_FULL(PAL2, 0, 0, 0, SPORTAL_TB +  6 + 24*pframe));
			SET_SPRITE_PLUS(nextsprite, 20 - scxoff, 334 - scyoff,
				SPRITE_SIZE(2, 3), TILE_ATTR_FULL(PAL2, 0, 0, 0, SPORTAL_TB + 18 + 24*pframe));
		}
		VDP_setSpriteLink(nextsprite-1, 0);

		VDP_waitVSync();
		for (unsigned i = 0; i < 4; i++)
			VDP_setPalette(PAL0 + i, workpal[i]);
		XGM_set68KBUSProtection(TRUE);
		VDP_updateSprites(32, FALSE);
		XGM_set68KBUSProtection(FALSE);
	}

	// Switch to enemies palette
	memcpy(refpal[2], enemies_palette, 32);
	memcpy(workpal, refpal, sizeof(workpal));

	// Play music on repeat
	XGM_setMusicTempo(60);
	XGM_setLoopNumber(-1);
	XGM_startPlay(music_arcade);

	// Enemies & init
	struct {
		uint16_t x, y; // Fixed point .4
		uint16_t btile;
		int8_t movx, movy;
	} enemies[ENEMIES_COUNT];
	for (unsigned i = 0; i < ENEMIES_COUNT; i++) {
		enemies[i].x = enemies_info[i].x << 6;
		enemies[i].y = enemies_info[i].y << 6;
		enemies[i].btile = ENEMIES_TB + (enemies_info[i].flags & 7) * 8;
		enemies[i].movx = (enemies_info[i].flags >> 3) ? 0 : 1;
		enemies[i].movy = 1 - enemies[i].movx;
	}

	onfloor = TRUE;
	bool prevjump = FALSE, onhold = FALSE;
	unsigned fcounter = 0, charanim = 0, orientation = 0, lvlcnter = 0, levelfsm = LEVEL_PLAY;
	int retcode = -1;
	while (retcode < 0) {
		VDP_setHorizontalScroll(PLAN_B, -((3*vp_x) >> 4));
		VDP_setVerticalScroll(PLAN_B, vp_y >> 3);

		// Process input
		uint16_t k1 = onhold ? 0 : JOY_readJoypad(JOY_1);

		// Left/right speed depends on whether on floor or air
		if (k1 & BUTTON_LEFT) {
			char_hspeed -= (onfloor ? 2 : 1);
			orientation = 1;
		}
		else if (k1 & BUTTON_RIGHT) {
			char_hspeed += (onfloor ? 2 : 1);
			orientation = 0;
		}
		else
			char_hspeed /= 2;   // Doesn't immediately stop tho.

		// TODO: make jumps sensitive to button press time?
		// Idea: give some initial speed, and add some extra after N frames
		// if the button is still pressed.
		if ((k1 & BUTTON_A) && onfloor && !prevjump) {
			char_vspeed = 0x40;
			XGM_startPlayPCM(SND_JUMP, 0, SND_CH_JUMP);
		}
		prevjump = (k1 & BUTTON_A);

		// If the character is jumping, we lock the frame to "jumping"
		unsigned cframe = 7;
		if (!onhold) {
			if (char_hspeed && onfloor)
				cframe = (charanim++ >> 2) & 7;
			else if (!onfloor)
				cframe = 1;    // Jump frame
			else
				charanim = 0;  // Reset animation

			// Process gravity and collision, twice to avoid issues with big speeds
			for (int i = 0; i < 2; i++) {
				char_y -= (char_vspeed >> 1);
				char_x += char_hspeed;
				collisionGravityProcess(&colmap[0][0], colmap_width8);
			}
			char_vspeed -= FALL_SPEED_STEP;
			if (char_vspeed < -MAX_FALL_SPEED)
				char_vspeed = -MAX_FALL_SPEED;
			if (char_hspeed > MAX_RUN_SPEED)
				char_hspeed = MAX_RUN_SPEED;
			else if (char_hspeed < -MAX_RUN_SPEED)
				char_hspeed = -MAX_RUN_SPEED;
		}

		// Move enemies, change direction if they hit a wall tho.
		for (unsigned i = 0; i < ENEMIES_COUNT; i++) {
			unsigned nx = enemies[i].x + enemies[i].movx * ENEMY_SPEED;
			unsigned ny = enemies[i].y + enemies[i].movy * ENEMY_SPEED;
			unsigned cx = nx + enemies[i].movx * 7 * 16;
			unsigned cy = ny + enemies[i].movy * 7 * 16;
			if (colmap[cy >> 7][cx >> 10] & (1 << ((cx >> 7) & 7))) {
				enemies[i].movx = -enemies[i].movx;
				enemies[i].movy = -enemies[i].movy;
			} else {
				enemies[i].x = nx;
				enemies[i].y = ny;
			}
		}

		// Col det for mainchar-ghost
		bool enemyhit = FALSE;
		int cright = char_x + MAINCHAR_W/2*16;
		int cleft  = char_x - MAINCHAR_W/2*16;
		int cdown  = char_y + (MAINCHAR_H/2 - 1)*16;
		int cup    = char_y - (MAINCHAR_H/2 - 1)*16;
		for (unsigned i = 0; i < ENEMIES_COUNT; i++) {
			// Use old school bounding boxes
			int eleft  = enemies[i].x - ENEMY_BB_W/2*16;
			int eright = enemies[i].x + ENEMY_BB_W/2*16;
			int eup    = enemies[i].y - ENEMY_BB_H/2*16;
			int edown  = enemies[i].y + ENEMY_BB_H/2*16;
			if (cright > eleft && cleft < eright && cup < edown && cdown > eup)
				enemyhit = TRUE;
		}

		// Check whether viewport needs to be repositioned.
		int scxoff = vp_x - _SCREEN_WIDTH/2;
		int scyoff = vp_y - _SCREEN_HEIGHT/2;

		int charpx = char_x >> 4;
		int charpy = char_y >> 4;
		// Only follow char with camera during play time :)
		if (levelfsm == LEVEL_PLAY)
			levelSetViewportArea(charpx, charpy, 100, 80);

		// Meta level FSM
		switch (levelfsm) {
		case LEVEL_PLAY:
			lvlcnter = 0;
			onhold = FALSE;
			// Die transition
			if (enemyhit) {
				levelfsm = LEVEL_RESTART;
				XGM_startPlayPCM(SND_HIT_DAMAGE, 0, SND_CH_DAMAGE);
			}
			// Win transition
			if (charpx > 940 && charpy > 460)
				levelfsm = LEVEL_SPECIAL_ENDING;
			break;
		case LEVEL_RESTART:
			// Just wait a bit while we play dead animation.
			if (lvlcnter++ > 70) {
				lvlcnter = 0;
				levelfsm = LEVEL_RESTARTING;
			} else {
				// Make it fall to the right direction
				char_x += orientation ? -3 : 3;
				char_y += (lvlcnter-16) * 4;
			}
			onhold = TRUE;
			break;
		case LEVEL_RESTARTING:
			// Palette setup here for fadeout
			for (unsigned i = 0; i < 4; i++)
				fade_dark_pal(workpal[i], &refpal[i][0], 8 - (lvlcnter >> 2), 3);

			if (lvlcnter++ >= 32)
				retcode = LEVEL_FAILED;

			onhold = TRUE;
			break;
		case LEVEL_SPECIAL_ENDING:
			if (lvlcnter++ > 100) {
				lvlcnter = 0;
				levelfsm = LEVEL_ENDING_WIN;
			}
			onhold = TRUE;
			break;
		case LEVEL_ENDING_WIN:
			for (unsigned i = 0; i < 4; i++)
				fade_dark_pal(workpal[i], &refpal[i][0], 8 - (lvlcnter >> 2), 3);

			if (lvlcnter++ >= 32)
				retcode = LEVEL_ACHIEVED;

			break;
		}

		unsigned nextsprite = 0;
		if (levelfsm == LEVEL_SPECIAL_ENDING)
			for (unsigned j = 0; j < 2; j++)
				for (unsigned i = 0; i < 8; i++)
					SET_SPRITE_PLUS(nextsprite, 40 + 32*i, 190 + 20 * j, SPRITE_SIZE(i < 7 ? 4 : 2, 2),
						TILE_ATTR_FULL(PAL1, 1, 0, 0, FINALTX_TB + 60 * j + i*8));

		if (levelfsm != LEVEL_RESTART || (lvlcnter & 4))
			SET_SPRITE_PLUS(nextsprite, charpx - scxoff - DISP_MAINCHAR_H/2, charpy - scyoff - DISP_MAINCHAR_H/2 - 1,
				SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PAL3, 0, 0, orientation, MAINCHR_TB + 9 * cframe));

		for (unsigned i = 0; i < ENEMIES_COUNT; i++) {
			SET_SPRITE_PLUS(nextsprite, (enemies[i].x >> 4) - scxoff -ENEMY_W/2, (enemies[i].y >> 4) - scyoff -ENEMY_H/2,
				SPRITE_SIZE(2, 2), TILE_ATTR_FULL(PAL2, 0, 0, enemies[i].movx > 0 ? 0 : 1,
				enemies[i].btile + 4 * ((fcounter >> 3) & 1)));
		}
		VDP_setSpriteLink(nextsprite-1, 0);

		VDP_waitVSync();
		for (unsigned i = 0; i < 4; i++)
			VDP_setPalette(PAL0 + i, workpal[i]);
		VDP_updateSprites(32, FALSE);
		levelVblankWork8A(&tilemap_level[0], NULL);
		fcounter++;
	}

	XGM_stopPlay();

	return retcode;
}


