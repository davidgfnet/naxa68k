#include <genesis.h>
#include "config.h"
#include "util.h"
#include "levels.h"
#include "level_functions.h"
#include "collision.h"

#include "resources.h"
#include "assets/sprites/maincharC24M.h"
#include "assets/sprites/maincharC24F.h"
#include "assets/level4/collectable.h"
#include "assets/level4/door.h"
#include "assets/level4/enemy.h"
#include "assets/level4/caveL.h"
#include "assets/level4/caveH.h"
#include "assets/level4/cave1_aux.h"
#include "assets/level4/cave2_aux.h"
#include "assets/level4/cave3_aux.h"

unsigned sublevel4(unsigned cavenum, int *lifepts);

#define SND_THRW_CH  1
#define SND_DAMG_CH  2
#define SND_EVEN_CH  3

#define SND_THROW    128
#define SND_COLLIDE  129
#define SND_DAMAGE   130
#define SND_DIE      131
#define SND_LIFE     132

#define MAINCHAR_W  16
#define MAINCHAR_H  16
#define MAX_ENEMIES 16
#define MAX_BOXES   12
#define MAX_KNIVES  12
#define MAX_SHOOTERS 9
#define ATTACK_RADIUS  64
#define BASE_RADIUS    64
#define ENSPEED     9
#define CHARSPEED  14

// First define common assets to all dungeons, then later come stuff specific to each one of them.
#define MAINCHR_TB  (                                        0)  // Main character      ~70
#define PLANE_B_TB  (               sprites_maincharC24M_count)  // Level PlaneB tiles ~255
#define PLANE_A_TB  (PLANE_B_TB +         sprites_planeb_count)  // Level PlaneA tiles ~ 20
#define COLLECT_TB  (PLANE_A_TB +        sprites_planebH_count)  // Collectable stuff   ~12
#define TRAPSAN_TB  (COLLECT_TB +    sprites_collectable_count)  // Trap tiles          ~16
#define ENEMIES_TB  (TRAPSAN_TB +           sprites_trap_count)  // Enemy tiles         ~64
#define WEAPONS_TB  (ENEMIES_TB +        sprites_enemies_count)  // Weapon tiles        ~12
#define DOORCLD_TB  (WEAPONS_TB +         sprites_weapon_count)  // Weapon tiles        ~16
#define KEYSCOL_TB  (DOORCLD_TB +           sprites_door_count)  // Keys tiles          ~64
#define HEARTTL_TB  (KEYSCOL_TB +           sprites_keys_count)  // Heart

_Static_assert(HEARTTL_TB < 1300, "Tile count sanity");

uint16_t sparseMapLookup(const void *ptr, int x, int y);

levelForceReload_TEMPLATE(Cave1L, uint8_t, tilemap_cave1L_width, tilemap_cave1L_height, PLAN_B, PAL0, PLANE_B_TB, NULL)
levelVblankWork_TEMPLATE( Cave1L, uint8_t, tilemap_cave1L_width, tilemap_cave1L_height, PLAN_B, PAL0, PLANE_B_TB, NULL)
levelForceReload_TEMPLATE(Cave2L, uint8_t, tilemap_cave2L_width, tilemap_cave2L_height, PLAN_B, PAL0, PLANE_B_TB, NULL)
levelVblankWork_TEMPLATE( Cave2L, uint8_t, tilemap_cave2L_width, tilemap_cave2L_height, PLAN_B, PAL0, PLANE_B_TB, NULL)
levelForceReload_TEMPLATE(Cave3L, uint8_t, tilemap_cave3L_width, tilemap_cave3L_height, PLAN_B, PAL0, PLANE_B_TB, NULL)
levelVblankWork_TEMPLATE( Cave3L, uint8_t, tilemap_cave3L_width, tilemap_cave3L_height, PLAN_B, PAL0, PLANE_B_TB, NULL)

levelForceReload_TEMPLATE(Cave1H, uint16_t, tilemap_cave1H_width, tilemap_cave1H_height, PLAN_A, PAL0, 0, sparseMapLookup)
levelVblankWork_TEMPLATE( Cave1H, uint16_t, tilemap_cave1H_width, tilemap_cave1H_height, PLAN_A, PAL0, 0, sparseMapLookup)
levelForceReload_TEMPLATE(Cave2H, uint16_t, tilemap_cave2H_width, tilemap_cave2H_height, PLAN_A, PAL0, 0, sparseMapLookup)
levelVblankWork_TEMPLATE( Cave2H, uint16_t, tilemap_cave2H_width, tilemap_cave2H_height, PLAN_A, PAL0, 0, sparseMapLookup)
levelForceReload_TEMPLATE(Cave3H, uint16_t, tilemap_cave3H_width, tilemap_cave3H_height, PLAN_A, PAL0, 0, sparseMapLookup)
levelVblankWork_TEMPLATE( Cave3H, uint16_t, tilemap_cave3H_width, tilemap_cave3H_height, PLAN_A, PAL0, 0, sparseMapLookup)

const int8_t knife_offs[4][2] = {{0, -3}, {0, 3}, {3, 0}, {-3, 0}};

uint16_t sparseMapLookup(const void *ptr, int x, int y) {
	// User pointer contains the first and second level tables.
	const struct t_packed_tm16 *pd = (struct t_packed_tm16*)ptr;
	// Bound check!
	if (x < 0 || y < 0 || x >= pd->w || y >= pd->h)
		return GAME_TRANS_TILE;

	// Lookup first level, divide the coordinates
	unsigned flx = x >> 3, fly = y >> 3;
	uint16_t iptr = pd->firstlvl[flx + fly * pd->firstlvl_stride];
	if (!(iptr & (1 << 15)))
		return TILE_ATTR_PRIORITY_MASK | (PLANE_A_TB + iptr);  // No need for 2nd level

	unsigned subx = x & 7, suby = y & 7;
	iptr = (iptr & 0x7FFF) << 6;
	return TILE_ATTR_PRIORITY_MASK | (PLANE_A_TB + pd->secondlvl[iptr + (suby << 3) + subx]);
}

// Info block that describes the current level.
static const struct {
	uint8_t startx, starty;
	const uint8_t *collmap, *deathmap;
	unsigned width8;
	const uint8_t *tilemapL;
	const struct t_packed_tm16 *packedtmH;
	void (*reloadL)(const uint8_t *, const void *);
	void (*updateL)(const uint8_t *, const void *);
	void (*reloadH)(const uint16_t *, const void *);
	void (*updateH)(const uint16_t *, const void *);
	// Stuff on the level
	const uint8_t *traps, *enemies, *boxes, *doors, *shooters;
	uint8_t trapcnt, enemycnt, boxcnt, doorcnt, shootercnt;
	// Exit areas (to next level or win)
	int eminx, emaxx, eminy, emaxy;
} subl [] = {
	{ 4, 88, &colmap1[0][0], &deathmap1[0][0], colmap1_width8, tilemap_cave1L, &packed_usrptr_cave1H,
	  levelForceReloadCave1L, levelVblankWorkCave1L, levelForceReloadCave1H, levelVblankWorkCave1H,
	  &traps1_info[0][0], &enemies1_info[0][0], &boxes1_info[0][0], &door1_info[0][0], &shooters1_info[0][0],
	  TRAPS1_COUNT, ENEMIES1_COUNT, BOXES1_COUNT, DOORS1_COUNT, SHOOTERS1_COUNT,
	  EXIT1_MINX, EXIT1_MAXX, EXIT1_MINY, EXIT1_MAXY, },
	{ 8, 8, &colmap2[0][0], &deathmap2[0][0], colmap2_width8, tilemap_cave2L, &packed_usrptr_cave2H,
	  levelForceReloadCave2L, levelVblankWorkCave2L, levelForceReloadCave2H, levelVblankWorkCave2H,
	  &traps2_info[0][0], &enemies2_info[0][0], &boxes2_info[0][0], &door2_info[0][0], &shooters2_info[0][0],
	  TRAPS2_COUNT, ENEMIES2_COUNT, BOXES2_COUNT, DOORS2_COUNT, SHOOTERS2_COUNT,
	  EXIT2_MINX, EXIT2_MAXX, EXIT2_MINY, EXIT2_MAXY, },
	{30,  6, &colmap3[0][0], &deathmap3[0][0], colmap3_width8, tilemap_cave3L, &packed_usrptr_cave3H,
	  levelForceReloadCave3L, levelVblankWorkCave3L, levelForceReloadCave3H, levelVblankWorkCave3H,
	  &traps3_info[0][0], &enemies3_info[0][0], &boxes3_info[0][0], &door3_info[0][0], &shooters3_info[0][0],
	  TRAPS3_COUNT, ENEMIES3_COUNT, BOXES3_COUNT, DOORS3_COUNT, SHOOTERS3_COUNT,
	  EXIT3_MINX, EXIT3_MAXX, EXIT3_MINY, EXIT3_MAXY, },
};
#define NUM_ROOMS  (sizeof(subl)/sizeof(subl[0]))

unsigned level4() {
	// Load all the shared assets and stuff
	Z80_init();

	// Load sprites and all what the dungeon level needs.
	loadDecompressTiles(playerchar ? sprites_compr_maincharC24F : sprites_compr_maincharC24M, MAINCHR_TB, DMA);
	loadDecompressTiles(tiles_compr_planeb,        PLANE_B_TB, DMA);
	loadDecompressTiles(tiles_compr_planebH,       PLANE_A_TB, DMA);
	loadDecompressTiles(sprites_compr_collectable, COLLECT_TB, DMA);
	loadDecompressTiles(sprites_compr_trap,        TRAPSAN_TB, DMA);
	loadDecompressTiles(sprites_compr_enemies,     ENEMIES_TB, DMA);
	loadDecompressTiles(sprites_compr_weapon,      WEAPONS_TB, DMA);
	loadDecompressTiles(sprites_compr_door,        DOORCLD_TB, DMA);
	loadDecompressTiles(sprites_compr_keys,        KEYSCOL_TB, DMA);
	loadDecompressTiles(sprites_compr_heart,       HEARTTL_TB, DMA);

	DMA_flushQueue();

	XGM_setPCM(SND_THROW,   dungeon_throw, sizeof(dungeon_throw));
	XGM_setPCM(SND_COLLIDE, dungeon_collide, sizeof(dungeon_collide));
	XGM_setPCM(SND_DAMAGE,  dungeon_damage, sizeof(dungeon_damage));
	XGM_setPCM(SND_DIE,     arcade_hit, sizeof(arcade_hit));
	XGM_setPCM(SND_LIFE,    dungeon_life, sizeof(dungeon_life));

	// Start the music already
	XGM_setMusicTempo(50);
	XGM_setLoopNumber(-1);
	XGM_startPlay(music_cave);

	// Palettes are black by default
	VDP_setEnable(1);

	// Now jump to a sublevel, only tilemap needs to be reloaded
	int retcode = -1, currroom = 0, lifepts = 3;
	while (retcode < 0) {
		retcode = sublevel4(currroom, &lifepts);
		currroom++;
		if (currroom >= NUM_ROOMS && retcode < 0)
			retcode = LEVEL_ACHIEVED;
	}

	XGM_stopPlay();

	return retcode;
}

bool moveallowed(const uint8_t *m, unsigned mw8, int x, int y) {
	for (int dx = -7; dx <= 7; dx += 14) {
		for (int dy = -7; dy <= 7; dy += 14) {
			unsigned tx = (x + dx) >> 3, ty = (y + dy) >> 3;
			if (m[ty * mw8 + (tx >> 3)] & (1 << (tx & 7)))
				return FALSE;
		}
	}
	return TRUE;
}

unsigned sublevel4(unsigned cavenum, int *lifepts) {
	// Character tracking, initialize
	char_w = MAINCHAR_W;
	char_h = MAINCHAR_H;
	char_x = subl[cavenum].startx << (3 + 4);
	char_y = subl[cavenum].starty << (3 + 4);
	VDP_resetSprites();
	levelSetViewport(char_x >> 4, char_y >> 4);
	subl[cavenum].reloadL(subl[cavenum].tilemapL, NULL);
	subl[cavenum].reloadH(NULL, subl[cavenum].packedtmH);

	// Initialize stuff in the level.
	struct {
		int ox, oy;
		int x, y;
		int mx, my;
		uint8_t type, fsm;
		int8_t pts;
	} enemies[MAX_ENEMIES];
	for (unsigned i = 0; i < MAX_ENEMIES; i++)
		enemies[i].type = 255;
	for (unsigned i = 0; i < subl[cavenum].enemycnt; i++) {
		enemies[i].x = enemies[i].ox = subl[cavenum].enemies[2*i+0] << (3+4);
		enemies[i].y = enemies[i].oy = subl[cavenum].enemies[2*i+1] << (3+4);
		enemies[i].mx = enemies[i].my = 0;
		enemies[i].type = randomu32() & 1;
		enemies[i].fsm = 1;
		enemies[i].pts = 3;
	}
	uint8_t box_fsm[MAX_BOXES] = {0};
	struct {
		int x, y;
		int dir;  // up right down left
		uint8_t steps, flags;
	} knives[MAX_KNIVES];
	for (unsigned i = 0; i < MAX_KNIVES; i++)
		knives[i].dir = -1;
	struct {
		int x, y;
		uint8_t fsm;
	} shooters[MAX_SHOOTERS];
	for (unsigned i = 0; i < subl[cavenum].shootercnt; i++) {
		shooters[i].x = subl[cavenum].shooters[2*i+0] << 3;
		shooters[i].y = subl[cavenum].shooters[2*i+1] << 3;
		enemies[i].fsm = 0;
	}

	// Working palettes
	uint16_t workpal[4][16], refpal[4][16];
	memcpy(&refpal[0][0], planeb_palette,       32);
	memcpy(&refpal[1][0], collectable_palette,  32);
	memcpy(&refpal[2][0], enemy_palette,        32);
	memcpy(&refpal[3][0], playerchar ? maincharC24F_palette : maincharC24M_palette, 32);
	memset(workpal, 0, sizeof(workpal));

	// Editable collision map!
	uint8_t wcolmap[128 * 24];
	memcpy(wcolmap, subl[cavenum].collmap, sizeof(wcolmap));  // FIXME copy exact size!

	uint8_t levelfsm = LEVEL_STARTING, levelcntr = 0, flipv = 0;;
	uint16_t prevkey = 0, trap_fsm = 0, box_loading_fsm = 0;
	unsigned orientation = 2, framec = 0, invincible = 0, door_state = 0, reload = 0;
	bool onhold = FALSE;
	while (1) {
		// Process input
		uint16_t key1 = onhold ? 0 : JOY_readJoypad(JOY_1);
		bool moved = (key1 & (BUTTON_LEFT|BUTTON_RIGHT|BUTTON_UP|BUTTON_DOWN));
		if (key1 & BUTTON_LEFT) {
			char_x -= CHARSPEED;
			orientation = 3;
			flipv = 1;
		}
		if (key1 & BUTTON_RIGHT) {
			char_x += CHARSPEED;
			orientation = 2;
			flipv = 0;
		}
		if (key1 & BUTTON_UP) {
			char_y -= CHARSPEED;
			orientation = 0;
		}
		if (key1 & BUTTON_DOWN) {
			char_y += CHARSPEED;
			orientation = 1;
		}
		if ((key1 & (ANY_BUTTON)) && !(prevkey & (ANY_BUTTON)) && !reload) {
			for (unsigned i = 0; i < MAX_KNIVES; i++)
				if (knives[i].dir < 0) {
					knives[i].steps = 0;
					knives[i].flags = 0;   // User created, does not hurt us!
					knives[i].dir = orientation;
					knives[i].x = (char_x >> 4) + (knife_offs[orientation][0] << 2);
					knives[i].y = (char_y >> 4) + (knife_offs[orientation][1] << 2);
					XGM_startPlayPCM(SND_THROW, 0, SND_THRW_CH);
					reload = 10;
					break;
				}
		}
		prevkey = key1;
		if (reload) reload--;

		// Metalevel FSM
		switch (levelfsm) {
		case LEVEL_STARTING:
			for (unsigned i = 0; i < 4; i++)
				fade_dark_pal(workpal[i], &refpal[i][0], (levelcntr >> 2), 3);

			if (levelcntr++ >= 32)
				levelfsm = LEVEL_PLAY;
			break;
		case LEVEL_PLAY:
			levelcntr = 0;
			if (*lifepts <= 0)
				levelfsm = LEVEL_RESTART;
			onhold = FALSE;
			break;
		case LEVEL_RESTART:
			if (++levelcntr >= 32) {
				levelfsm = LEVEL_RESTARTING;
				levelcntr = 0;
			}
			onhold = TRUE;
			break;
		case LEVEL_RESTARTING:
			for (unsigned i = 0; i < 4; i++)
				fade_dark_pal(workpal[i], &refpal[i][0], 8 - (levelcntr >> 2), 3);

			if (levelcntr++ >= 32)
				return LEVEL_FAILED;

			onhold = TRUE;
			break;
		case LEVEL_SWITCH:
			for (unsigned i = 0; i < 4; i++)
				fade_dark_pal(workpal[i], &refpal[i][0], 8 - (levelcntr >> 2), 3);

			if (levelcntr++ >= 32)
				return -1;

			onhold = TRUE;
			break;
		};

		if ((char_x >> 4) >= subl[cavenum].eminx && (char_x >> 4) <= subl[cavenum].emaxx &&
		    (char_y >> 4) >= subl[cavenum].eminy && (char_y >> 4) <= subl[cavenum].emaxy) {
			if (levelfsm == LEVEL_PLAY)
				levelfsm = LEVEL_SWITCH;
		}

		// Process collision routines for the main character.
		collisionMapProcess16(wcolmap, subl[cavenum].width8);
		int posx = char_x >> 4, posy = char_y >> 4;

		// Process main char death by falling
		if (levelfsm == LEVEL_PLAY) {
			unsigned cxp = posx >> 3, cyp = (posy + 6) >> 3;
			uint8_t v = subl[cavenum].deathmap[cyp * subl[cavenum].width8 + (cxp >> 3)];
			if (v & (1 << (cxp & 7))) {
				// Play falling animation
				levelfsm = LEVEL_RESTART;
			}
		}

		// Check whether viewport needs to be repositioned.
		int scxoff = vp_x - _SCREEN_WIDTH/2;
		int scyoff = vp_y - _SCREEN_HEIGHT/2;

		levelSetViewportArea(posx, posy, 200, 100);

		// Visual effects :P
		workpal[0][1] = workpal[0][2] = workpal[0][3] = 0xA84;   // BGR!
		workpal[0][3 - ((framec >> 2) % 3)] = 0xCC8;

		int nextsprite = 0;
		// Dummy sprite to ensure we can hide others
		SET_SPRITE_PLUS_NVC(nextsprite, -64, -64, SPRITE_SIZE(1, 1), TILE_ATTR_FULL(PAL0, 0, 0, 0, GAME_TRANS_TILE));
		for (int i = 0; i < *lifepts; i++)
			SET_SPRITE_PLUS_NVC(nextsprite, 10 + i * 10, 10, SPRITE_SIZE(1, 1),
				TILE_ATTR_FULL(PAL2, 1, 0, 0, HEARTTL_TB));

		// Render doors before, due to priority
		for (unsigned i = 0; i < subl[cavenum].doorcnt; i++) {
			if (door_state & (1 << i))
				SET_SPRITE_PLUS(nextsprite, 290 - i * 20, 10,
					SPRITE_SIZE(2, 2), TILE_ATTR_FULL(PAL2, 1, 0, 0,
					KEYSCOL_TB + (48 * i) + 4 * ((framec >> 2) % 12)))
			else
				SET_SPRITE_PLUS(nextsprite, 8*subl[cavenum].doors[3*i+0] - scxoff,
					8*subl[cavenum].doors[3*i+1] - scyoff + 4,
					SPRITE_SIZE(4, 4), TILE_ATTR_FULL(PAL0, 1, 0, 0, DOORCLD_TB))
		}
		// Same for knives
		for (unsigned i = 0; i < MAX_KNIVES; i++) {
			if (knives[i].dir < 0)
				continue;

			if (!invincible && knives[i].flags && levelfsm == LEVEL_PLAY) {
				if (knives[i].x + 6 > posx - 6 && knives[i].x - 6 < posx + 6 && 
					knives[i].y + 6 > posy - 6 && knives[i].y - 6 < posy + 6) {
					(*lifepts)--;
					invincible = 70;
					if (*lifepts)
						XGM_startPlayPCM(SND_DAMAGE, 1, SND_DAMG_CH);
					else
						XGM_startPlayPCM(SND_DIE, 1, SND_DAMG_CH);
					knives[i].dir = -1;
					continue;
				}
			}

			knives[i].x += knife_offs[knives[i].dir][0];
			knives[i].y += knife_offs[knives[i].dir][1];
			SET_SPRITE_PLUS(nextsprite, knives[i].x - 8 - scxoff, knives[i].y - 8 - scyoff,
				SPRITE_SIZE(2, 2), TILE_ATTR_FULL(PAL2, 0, knives[i].dir & 1, knives[i].dir & 1,
				WEAPONS_TB + (knives[i].dir >> 1) * 4));

			// Make them expire!
			if (knives[i].steps++ > 20)
				knives[i].dir = -1;
			// Collision as well
			unsigned txp = knives[i].x >> 3, typ = knives[i].y >> 3;
			uint8_t v = wcolmap[typ * subl[cavenum].width8 + (txp >> 3)];
			if (v & (1 << (txp & 7))) {
				knives[i].dir = -1;
				XGM_startPlayPCM(SND_COLLIDE, 1, SND_THRW_CH);
			}
		}

		// Animate main char using a blink animation, first fast then slow.
		const uint8_t mcframe[4] = {1*4, 2*4, 0*4, 0*4};
		unsigned cframe = 6 * (((framec >> 3) & 3) + mcframe[orientation]);
		if (!moved)
			cframe = 6 * 12;  // Do nothing, idle
		if (levelfsm == LEVEL_RESTART && *lifepts > 0)
			cframe = 6 * (12 + (levelcntr >> 3));  // Die animation
		if (!invincible || ((invincible > 64) && (framec & 4)) || ((invincible <= 64) && (framec & 8)))
			if (levelfsm != LEVEL_RESTARTING)
				SET_SPRITE_PLUS(nextsprite, posx - scxoff - MAINCHAR_W/2, posy - scyoff - MAINCHAR_H/2 - 8,
					SPRITE_SIZE(2, 3), TILE_ATTR_FULL(PAL3, 0, 0, flipv, MAINCHR_TB + cframe));

		// Render shooters, quite static
		for (unsigned i = 0; i < subl[cavenum].shootercnt; i++) {
			if (!(shooters[i].x - scxoff - 8 > -16 && shooters[i].x - scxoff - 8 < _SCREEN_WIDTH+16 &&
			      shooters[i].y - scyoff - 8 > -16 && shooters[i].y - scyoff - 8 < _SCREEN_HEIGHT+16))
				continue;

			int vecx = (posx - shooters[i].x);
			int vecy =-(posy - shooters[i].y);
			bool lookup = ( vecy >  INTABS(vecx));
			bool lookdw = (-vecy >  INTABS(vecx));
			unsigned bt = ENEMIES_TB + 16*4 + (lookup ? 4 : 0);
			SET_SPRITE_PLUS(nextsprite, shooters[i].x - 8 - scxoff, shooters[i].y - 8 - scyoff,
				SPRITE_SIZE(2, 2), TILE_ATTR_FULL(PAL2, 0, 0, (vecx > 0 ? 0 : 1), bt));

			unsigned or = lookup ? 0 : lookdw ? 1 : vecx > 0 ? 2 : 3;
			// Maybe create a knife!
			if (shooters[i].fsm)
				shooters[i].fsm--;

			if (!shooters[i].fsm && (!(randomu32() & 15))) {
				for (unsigned k = 0; k < MAX_KNIVES; k++)
					if (knives[k].dir < 0) {
						shooters[i].fsm = (randomu32() & 127) | 64;
						knives[k].steps = 0;
						knives[k].flags = 1;   // Enemy created, does hurt us!
						knives[k].dir = or;
						knives[k].x = shooters[i].x + (knife_offs[or][0] << 2);
						knives[k].y = shooters[i].y + (knife_offs[or][1] << 2);
						XGM_startPlayPCM(SND_THROW, 0, SND_THRW_CH);
						break;
					}
			}
		}

		// Render enemies and knives, move enemies too
		for (unsigned i = 0; i < subl[cavenum].enemycnt; i++) {
			if (enemies[i].pts <= 0)
				enemies[i].type = 255;

			if (enemies[i].type != 255) {
				// Perform visibility checks manually to make it more efficient.
				int ex = enemies[i].x >> 4, ey = enemies[i].y >> 4;
				if (ex - scxoff - 8 > -16 && ex - scxoff - 8 < _SCREEN_WIDTH+16 &&
				    ey - scyoff - 8 > -16 && ey - scyoff - 8 < _SCREEN_HEIGHT+16) {

					// Char collision
					if (!invincible && levelfsm == LEVEL_PLAY) {
						if (ex + 6 > posx - 6 && ex - 6 < posx + 6 && 
							ey + 6 > posy - 6 && ey - 6 < posy + 6) {
							(*lifepts)--;
							invincible = 70;
							if (*lifepts)
								XGM_startPlayPCM(SND_DAMAGE, 1, SND_DAMG_CH);
							else
								XGM_startPlayPCM(SND_DIE, 1, SND_DAMG_CH);
						}
					}

					// Collision with knives, it's a bit slow O(N*M)
					bool hit = FALSE;
					for (unsigned j = 0; j < MAX_KNIVES; j++) {
						if (knives[j].dir < 0)
							continue;
						if (knives[j].x > ex - 8 && knives[j].x < ex + 8 &&
						    knives[j].y > ey - 8 && knives[j].y < ey + 8) {
							knives[j].dir = -1;
							enemies[i].pts--;
							if (!hit)
								XGM_startPlayPCM(SND_COLLIDE, 1, SND_THRW_CH);
							hit = TRUE;
						}
					}

					if (!hit) {
						bool moves = enemies[i].mx || enemies[i].my;
						unsigned bt = (enemies[i].type << 3) + (moves ? ((framec >> 3) & 3) : 0);
						SET_SPRITE_PLUS_NVC(nextsprite, ex - 8 - scxoff, ey - 8 - scyoff,
							SPRITE_SIZE(2, 2), TILE_ATTR_FULL(PAL2, 0, 0, enemies[i].mx < 0 ? 1 : 0,
							ENEMIES_TB + (bt << 2)));
					}

					bool mpnear = (INTABS(enemies[i].x - char_x) < ATTACK_RADIUS*16 &&
							       INTABS(enemies[i].y - char_y) < ATTACK_RADIUS*16);
					bool nearbs = (INTABS(enemies[i].x - enemies[i].ox) < BASE_RADIUS*16 &&
							       INTABS(enemies[i].y - enemies[i].oy) < BASE_RADIUS*16);

					if (!--enemies[i].fsm || (mpnear && !enemies[i].mx && !enemies[i].my)) {
						// Decide whether move or not, must be near and not far from base
						bool followplayer = mpnear && nearbs;
						int targetx = followplayer ? char_x : enemies[i].ox;
						int targety = followplayer ? char_y : enemies[i].oy;
						enemies[i].mx = enemies[i].x > targetx ? -ENSPEED : enemies[i].x < targetx ? ENSPEED : 0;
						enemies[i].my = enemies[i].y > targety ? -ENSPEED : enemies[i].y < targety ? ENSPEED : 0;
						enemies[i].fsm = (randomu32() & 31) | 16;
					}

					// Now record next move
					int nx = enemies[i].x + enemies[i].mx, ny = enemies[i].y;
					// Only do the move if it's legal
					if (moveallowed(wcolmap, subl[cavenum].width8, nx >> 4, ny >> 4))
						enemies[i].x = nx;
					nx = enemies[i].x; ny = enemies[i].y + enemies[i].my;
					if (moveallowed(wcolmap, subl[cavenum].width8, nx >> 4, ny >> 4))
						enemies[i].y = ny;
				}
			}
		}

		// Process box opening
		for (unsigned i = 0; i < subl[cavenum].boxcnt; i++) {
			// Only update tiles that are visible
			int tx = subl[cavenum].boxes[3*i];
			int ty = subl[cavenum].boxes[3*i+1];
			int fsm = box_fsm[i];

			// Check whether the char is close to it.
			if (fsm == 22) {
				// TODO: Award content! Message?
				box_fsm[i]++;
				int a = subl[cavenum].boxes[3*i+2];
				if (a & 128) {
					// Key for a door!
					// TODO: Add the key symbol somehow!
					unsigned dn = a & 127;
					door_state |= (1 << dn);   // Do not render it anymore
					// Clear up colmap
					unsigned bx = subl[cavenum].doors[3*dn+0];
					unsigned by = subl[cavenum].doors[3*dn+1];
					for (unsigned x = bx; x < bx + 4; x++)
						for (unsigned y = by; y < by + 5; y++)
							wcolmap[y * subl[cavenum].width8 + (x >> 3)] &= ~(1 << (x & 7));
				}
				else if (a) {
					(*lifepts)++;
					XGM_startPlayPCM(SND_LIFE, 1, SND_EVEN_CH);
				}
			}
			else if (fsm < 23) {
				if (posx > 8*tx - 12 && posx < 8*tx + 28 &&   // Coords are not centered
				    posy > 8*ty - 12 && posy < 8*ty + 28) {
					box_fsm[i]++;
				}
			}
		}

		// Process trap hits
		// Process traps animation by changing tiles
		if (trap_fsm > 192 && trap_fsm < 256 && !invincible) {
			for (unsigned i = 0; i < subl[cavenum].trapcnt; i++) {
				// Only update tiles that are visible
				int tx = subl[cavenum].traps[2*i] << 3, ty = subl[cavenum].traps[2*i+1] << 3;
				if (posx+3 > tx && posx-3 < tx+16 && posy+6 > ty && posy+6 < ty+16) {
					// Take trap damage!
					invincible = 80;
					(*lifepts)--;
				}
			}
		}

		VDP_setSpriteLink(nextsprite-1, 0);
		VDP_waitVSync();
		for (unsigned i = 0; i < 4; i++)
			VDP_setPalette(PAL0 + i, workpal[i]);
		VDP_updateSprites(48, FALSE);  // Just to be on the safe side
		subl[cavenum].updateL(subl[cavenum].tilemapL, NULL);
		subl[cavenum].updateH(NULL, subl[cavenum].packedtmH);

		// Process traps animation by changing tiles
		int epx = vp_x >> 3, epy = vp_y >> 3;
		uint16_t trapbt = (trap_fsm < 128) ? 0 :
		                  (trap_fsm < 192) ? ((trap_fsm - 128) >> 4) :   // Deploy trap
		                  (trap_fsm < 256) ? 3 :                         // Deployed
		                  3 - ((trap_fsm - 256) >> 4);                   // Going back
		if (++trap_fsm >= 320)
			trap_fsm = 0;
		for (unsigned i = 0; i < subl[cavenum].trapcnt; i++) {
			// Only update tiles that are visible
			int tx = subl[cavenum].traps[2*i], ty = subl[cavenum].traps[2*i+1];
			if (tx > epx - 24 && tx < epx + 24 && ty > epy - 24 && ty < epy + 24) {
				for (unsigned k = 0; k < 4; k++)
					VDP_setTileMapXY(PLAN_B, TILE_ATTR_FULL(PAL1, 0, 0, 0, TRAPSAN_TB + trapbt * 4 + k),
					                 (tx+(k>>1)) & 63, (ty+(k&1)) & 63);
			}
		}

		// Update tiles to add boxes, to avoid a lot of CPU usage, just do 4 per frame.
		for (unsigned i = 0; i < 4; i++) {
			if (++box_loading_fsm >= subl[cavenum].boxcnt)
				box_loading_fsm = 0;

			// Only update tiles that are visible
			int tx = subl[cavenum].boxes[3*box_loading_fsm];
			int ty = subl[cavenum].boxes[3*box_loading_fsm+1];
			int fsm = box_fsm[box_loading_fsm];
			if (tx > epx - 24 && tx < epx + 24 && ty > epy - 24 && ty < epy + 24) {
				for (unsigned k = 0; k < 4; k++)
					VDP_setTileMapXY(PLAN_A, TILE_ATTR_FULL(PAL1, 0, 0, 0, COLLECT_TB + (fsm >> 3) * 4 + k),
					                 (tx+(k>>1)) & 63, (ty+(k&1)) & 63);
			}
		}

		framec++;
		if (invincible)
			invincible--;
	}
}


