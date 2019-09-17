#include <genesis.h>
#include "config.h"
#include "util.h"
#include "level_functions.h"
#include "collision.h"
#include "levels.h"

#include "resources.h"
#include "assets/sprites/mainchar16M.h"
#include "assets/sprites/mainchar16F.h"
#include "assets/level2/level.h"
#include "assets/level2/aux.h"
#include "assets/level2/flame_anim.h"
#include "assets/level2/collectable.h"
#include "assets/level2/enemies.h"
#include "assets/level2/bomb.h"
#include "assets/level2/planea.h"
#include "assets/level2/door_anim.h"
#include "assets/level2/flames.h"
#include "assets/level2/heart.h"

// Bomberman-like game playground.

#define MAINCHAR_W  16
#define MAINCHAR_H  16
#define ENEMYCHR_W  16
#define ENEMYCHR_H  16
#define MIN_RADIUS   2    // 2 contiguous cells (8+1  flames)
#define MAX_RADIUS   4    // 5 contiguous cells (16+1 flames)
#define MIN_BOMBS    1    // 1 bomb at least
#define MAX_BOMBS    3    // 3 bombs tops

#define DOOR_VP_X  720
#define DOOR_VP_Y   80

// At most we can have 51 flames in screen, less than 80 limit.
// On a given line we have 320 pixels, so up to 20 flames, under the scanline limit too.

// Define functions to use for this level.
uint16_t PAlookup(const void *ptr, int x, int y);

levelForceReload_TEMPLATE(16B,
	uint16_t, level2_tilemap_planeb_low_width, level2_tilemap_planeb_low_height, PLAN_B, PAL0, 0, NULL)
levelVblankWork_TEMPLATE(16B,
	uint16_t, level2_tilemap_planeb_low_width, level2_tilemap_planeb_low_height, PLAN_B, PAL0, 0, NULL)
levelForceReload_TEMPLATE(16A,
	uint16_t, level2_tilemap_planeb_low_width, level2_tilemap_planeb_low_height, PLAN_A, 0, 0, PAlookup)
levelVblankWork_TEMPLATE(16A,
	uint16_t, level2_tilemap_planeb_low_width, level2_tilemap_planeb_low_height, PLAN_A, 0, 0, PAlookup)

// Tile layout:
#define PLANE_B_TB  (                                        0)  // Level PlaneB tiles ~500
#define PLANE_A_TB  (                       level2_tiles_count)  // Level PlaneA tiles ~60
#define DOORANI_TB  (PLANE_A_TB +    sprites_destroyable_count)  // Door animation     ~130
#define TORCHAN_TB  (DOORANI_TB +      sprites_door_anim_count)  // Fire animation     ~30
#define BOMBANI_TB  (TORCHAN_TB +     sprites_flame_anim_count)  // Bomb animation     ~30
#define EPLXANI_TB  (BOMBANI_TB +           sprites_bomb_count)  // Expl animation     ~30
#define BURNANI_TB  (EPLXANI_TB +      sprites_explosion_count)  // Flame burn anim    ~30
#define MAINCHR_TB  (BURNANI_TB +         sprites_flames_count)  // Mainchar tiles     ~50
#define ENEMBAT_TB  (MAINCHR_TB +    sprites_mainchar16M_count)  // Enemy bat          ~50
#define ENEMDEV_TB  (ENEMBAT_TB +      sprites_enemy_bat_count)  // Enemy devil        ~50
#define ENEMFLA_TB  (ENEMDEV_TB +    sprites_enemy_devil_count)  // Enemy flame        ~50
#define ENEMWOR_TB  (ENEMFLA_TB +    sprites_enemy_flame_count)  // Enemy worm         ~50
#define HEARTSP_TB  (ENEMWOR_TB +     sprites_enemy_worm_count)  // Heart              ~25
#define KEYRANI_TB  (HEARTSP_TB +          sprites_heart_count)  // Key rot anim       ~48
#define COLLECT_TB  (KEYRANI_TB +       sprites_key_anim_count)  // Collectable items  ~12
#define LAST_TILE   (COLLECT_TB +    sprites_collectable_count)
// Sound! PCM channel 1 used for explosions, 2 for damage and 3 for events
#define SND_EXPL_CH  1
#define SND_DAMG_CH  2
#define SND_EVEN_CH  3

#define SND_EXPLODE  128
#define SND_PICKUP   129
#define SND_UPGRADE  130
#define SND_DEAD     131
#define SND_PLANT    132

// Total ~ 1.1k tiles (max ~1400)

_Static_assert(LAST_TILE < 1300, "Tile count sanity");

#define HOR_OFFSET   (24/8)
#define VER_OFFSET   (48/8)

#define ATTR_FREE     0   // Must be free to walk
#define ATTR_OBSTACLE 1   // Has an obstacle (wall, stone)
#define ATTR_MAYDESTR 2   // May place a bombable object
#define ATTR_BOX      3   // There's a box with something in it.

// Uses a bitmap:
// Bit 7: Is a collision tile?
// Bit 6: Can burn?

#define MAP_FREE        (  0 | 0)   // Free to walk
#define MAP_OBSTACLE    (128 | 0)   // Can't walk, wall.
#define MAP_DESTRUCT1   (192 | 1)   // Can be blown up
#define MAP_DESTRUCT2   (192 | 2)
#define MAP_DESTRUCT3   (192 | 3)
#define MAP_BOX         (192 | 4)
#define MAP_FLAMEUP     ( 64 | 5)   // Powerups!
#define MAP_BOMBUP      ( 64 | 6)
#define MAP_HEALTH      ( 64 | 7)
#define MAP_BOMBTICK1   (192 | 8)   // Bomb ticking animation (4 frames)
#define MAP_BOMBTICK4   (192 |11)
#define MAP_BOMBEXPL1   (192 |12)   // Bomb explode animation (4 frames)
#define MAP_BOMBEXPL4   (192 |15)
#define MAP_KEYASSET    (  0 |16)   // Key (non) rotating

#define ISCOLL(x)  ((x) & 128)
#define ISDEST(x)  ((x) &  64)

// Map from things to actual tile content (just the base)
#define tp(x) ((x) << TILE_ATTR_PALETTE_SFT)
static const uint16_t attilemap[] = {
	tp(PAL0) | (GAME_TRANS_TILE),   // Do not draw anything
	tp(PAL1) | (PLANE_A_TB),        // The three destroyable items (2x2)
	tp(PAL1) | (PLANE_A_TB + 4),
	tp(PAL1) | (PLANE_A_TB + 8),
	tp(PAL1) | (PLANE_A_TB + 12),   // Box

	tp(PAL2) | (COLLECT_TB),        // Flame upgrade
	tp(PAL2) | (COLLECT_TB + 4),    // Bomb upgrade
	tp(PAL2) | (COLLECT_TB + 8),    // Life points

	tp(PAL0) | (BOMBANI_TB),        // Tick 1
	tp(PAL0) | (BOMBANI_TB + 4),
	tp(PAL0) | (BOMBANI_TB + 8),
	tp(PAL0) | (BOMBANI_TB + 12),
	tp(PAL0) | (BOMBANI_TB + 16),   // Expl 1
	tp(PAL0) | (BOMBANI_TB + 20),
	tp(PAL0) | (BOMBANI_TB + 24),
	tp(PAL0) | (BOMBANI_TB + 28),
	tp(PAL1) | (KEYRANI_TB),
};

enum BombState { bombUnused = 0, bombTicking = 1, bombExploding = 2, bombBlast = 3 };
enum EnemyState { enemyIdle = 0, enemyIdling, enemyStartMove, enemyMove, enemyDie, enemyDead };

struct enem_data {
	uint8_t  speed, fpsh;
	struct {
		uint16_t base_tile;
		uint8_t  frames;
		uint8_t  flip;
	} a[6];
};

// Sprite map as follows:
// - 0     Main char
// - 1     Heart indicator
// - 2-3   Flames 1 and 2
// - 4-5   Key count indication
// - 6     Opening door?
// - ?+    Burn/Expl animation (up to MAX_BOMBS -> 3)
// - ?+    Flames burning (up to MAX_BOMBS * 4 * MAX_RADIUS -> 48)
// - ?+    Enemies (onscreen only!)
// Max sprites: 3+3+48+14 -> 70 sprites on screen!
#define MAX_SPRITES_ONSCREEN   70

// Function that renders plane A from an attribute map to an actual tile number
uint16_t PAlookup(const void *ptr, int x, int y) {
	const uint8_t *atmap = (uint8_t*)ptr;
	x -= HOR_OFFSET;
	y -= VER_OFFSET;
	int x16 = x >> 1;
	int y16 = y >> 1;

	if (x16 < 0 || y16 < 0 || x16 >= bombmap_width || y16 >= bombmap_height)
		return GAME_TRANS_TILE;

	// Ignore upper bits, only useful for collision really.
	uint8_t tileid = atmap[bombmap_width * y16 + x16] & 0x3F;
	uint16_t tnum = attilemap[tileid];

	// Now need to map this 16x16 into something at 8x8, adding 0..3 to tilenum
	return tnum + ((x & 1) << 1) + (y & 1);
}

_Static_assert(collision_width8 * collision_height == sizeof(collision), "size check");

#define set_planeA_tile(attrid, x, y) \
	{ \
		int rx = ((x) << 1) + HOR_OFFSET; \
		int ry = ((y) << 1) + VER_OFFSET; \
		uint16_t tnum = attilemap[(attrid) & 0x3F]; \
		atmap[(y)][(x)] = attrid; \
		VDP_setTileMapXY(PLAN_A, TILE_ATTR_FULL(0, 0, 0, 0, (tnum+0)), (rx+0) & 63, (ry+0) & 63); \
		VDP_setTileMapXY(PLAN_A, TILE_ATTR_FULL(0, 0, 0, 0, (tnum+1)), (rx+0) & 63, (ry+1) & 63); \
		VDP_setTileMapXY(PLAN_A, TILE_ATTR_FULL(0, 0, 0, 0, (tnum+2)), (rx+1) & 63, (ry+0) & 63); \
		VDP_setTileMapXY(PLAN_A, TILE_ATTR_FULL(0, 0, 0, 0, (tnum+3)), (rx+1) & 63, (ry+1) & 63); \
	}

#define set_colbits8(x, y, val) \
	if (val) \
		colarr[(y)][(x) >> 3] |= (1 << ((x) & 7)); \
	else \
		colarr[(y)][(x) >> 3] &= ~(1 << ((x) & 7));

#define get_colbits8(x, y) \
	colarr[(y)][(x) >> 3] & (1 << ((x) & 7));

#define set_colbits16(x, y, val) \
	{ \
		int x8 = ((x) << 1) + HOR_OFFSET; \
		int y8 = ((y) << 1) + VER_OFFSET; \
		set_colbits8(x8+0, y8+0, val); \
		set_colbits8(x8+1, y8+0, val); \
		set_colbits8(x8+0, y8+1, val); \
		set_colbits8(x8+1, y8+1, val); \
	}


unsigned level2() {
	Z80_init();
	// Setup collision in RAM to allow modifications.
	uint8_t colarr[collision_height][collision_width8];
	memcpy(colarr, collision, sizeof(colarr));

	// Setup bomberman level, read the attr map and generate a map for this game instance.
	uint8_t keystocollect = 0;
	uint8_t atmap[bombmap_height][bombmap_width];
	// Initialize it using the map table above.
	for (int y = 0; y < bombmap_height; y++) {
		for (int x = 0; x < bombmap_width; x++) {
			// Lookup the map, which is 2 bits per attr.
			uint8_t attr = (bombmap[y][x >> 2] >> ((x & 3) << 1)) & 3;
			switch (attr) {
			case ATTR_FREE:
				atmap[y][x] = MAP_FREE;
				break;
			case ATTR_OBSTACLE:
				atmap[y][x] = MAP_OBSTACLE;
				break;
			case ATTR_MAYDESTR:
				{
					uint8_t rnum = randomu32() & 3;
					if (rnum == 3)
						atmap[y][x] = MAP_FREE;
					else
						atmap[y][x] = rnum + MAP_DESTRUCT1;
				}
				break;
			case ATTR_BOX:
				atmap[y][x] = MAP_BOX;
				keystocollect++;
				break;
			};

			// Propagate to collision array
			if (ISCOLL(atmap[y][x]))
				set_colbits16(x, y, 1);
		}
	}

	// Load level tiles, main character, enemies.
	VDP_loadTileData(level2_tiles,           PLANE_B_TB, level2_tiles_count, DMA);
	loadDecompressTiles(sprites_compr_destroyable, PLANE_A_TB, DMA);
	loadDecompressTiles(sprites_compr_door_anim,   DOORANI_TB, DMA);
	loadDecompressTiles(sprites_compr_flame_anim,  TORCHAN_TB, DMA);
	loadDecompressTiles(sprites_compr_bomb,        BOMBANI_TB, DMA);
	loadDecompressTiles(sprites_compr_explosion,   EPLXANI_TB, DMA);
	loadDecompressTiles(sprites_compr_flames,      BURNANI_TB, DMA);
	loadDecompressTiles(sprites_compr_heart,       HEARTSP_TB, DMA);
	loadDecompressTiles(sprites_compr_key_anim,    KEYRANI_TB, DMA);
	loadDecompressTiles(sprites_compr_collectable, COLLECT_TB, DMA);
	loadDecompressTiles(playerchar ? sprites_compr_mainchar16F : sprites_compr_mainchar16M, MAINCHR_TB, DMA);
	loadDecompressTiles(sprites_compr_enemy_bat,   ENEMBAT_TB, DMA);
	loadDecompressTiles(sprites_compr_enemy_devil, ENEMDEV_TB, DMA);
	loadDecompressTiles(sprites_compr_enemy_flame, ENEMFLA_TB, DMA);
	loadDecompressTiles(sprites_compr_enemy_worm,  ENEMWOR_TB, DMA);
	DMA_flushQueue();

	XGM_setPCM(SND_EXPLODE, bomberman_exp, sizeof(bomberman_exp));
	XGM_setPCM(SND_PICKUP,  bomberman_pickup, sizeof(bomberman_pickup));
	XGM_setPCM(SND_UPGRADE, bomberman_upgrade, sizeof(bomberman_upgrade));
	XGM_setPCM(SND_DEAD,    negative_hit, sizeof(negative_hit));
	XGM_setPCM(SND_PLANT,   bomberman_plant, sizeof(bomberman_plant));

	// Load palettes in the working area, generate fades
	uint16_t refpal[4][16], workpal[4][16];
	memcpy(refpal[0], palette_0[0], 32);
	memcpy(refpal[1], planea_palette, 32);
	memcpy(refpal[2], enemies_palette, 32);
	memcpy(refpal[3], playerchar ? mainchar16F_palette : mainchar16M_palette, 32);
	memset(workpal, 0, sizeof(workpal));

	// Give it a try to sprites
	VDP_resetSprites();

	// Character tracking, initialize
	char_w = MAINCHAR_W;
	char_h = MAINCHAR_H;
	char_x = 120; //650; //120;
	char_y = 268; //40; // 268;

	// Viewport and level loading
	levelSetViewport(char_x, char_y);
	levelForceReload16B(&level2_tilemap_planeb_low[0][0], NULL);
	levelForceReload16A(NULL, atmap);

	// Definition of game assets:
	struct {
		uint16_t x, y;
		enum BombState state;
		uint8_t fcount, scount;
	} bombs[MAX_BOMBS] = {0};

	struct {
		uint16_t x, y;
		enum EnemyState state;
		uint8_t t, fsm;
	} enemies[ENEMIES_COUNT];
	for (int i = 0; i < ENEMIES_COUNT; i++) {
		enemies[i].x = enemies_info[i].x << (4+4);   // 12.4 fixed point
		enemies[i].y = enemies_info[i].y << (4+4);   // 16 grid -> 12.4
		enemies[i].t = enemies_info[i].t;
		enemies[i].fsm = 0;
		enemies[i].state = enemyIdle;
	}

	XGM_setMusicTempo(50);
	XGM_setLoopNumber(-1);
	XGM_startPlay(music_bomberman);

	// Start level, now that it is totally loaded! Enable VDP!
	VDP_setEnable(1);

	// Game logic init
	uint8_t availbombs = MIN_BOMBS, currradius = MIN_RADIUS;
	int8_t lifepts = 3;
	uint8_t invincible = 0, collectedkeys = 0;
	bool onhold = TRUE;
	uint8_t levelfsm = LEVEL_STARTING;
	int orientation = 2; // left, up, down, right
	int fcounter = 0, lvlcnter = 0, doorframe = 0;
	uint16_t prevkey1 = 0;
	int retcode = -1;
	int ovpx, ovpy;
	while(retcode < 0) {
		// Process input
		int move = 0;
		uint16_t key1 = (onhold || invincible > 80) ? 0 : JOY_readJoypad(JOY_1);
		if (key1 & BUTTON_LEFT) {
			char_x -= 1;
			orientation = 0;
			move = 1;
		}
		if (key1 & BUTTON_RIGHT) {
			char_x += 1;
			orientation = 3;
			move = 1;
		}
		if (key1 & BUTTON_UP) {
			char_y -= 1;
			orientation = 1;
			move = 1;
		}
		if (key1 & BUTTON_DOWN) {
			char_y += 1;
			orientation = 2;
			move = 1;
		}
		uint16_t posx = ((char_x >> 3) - HOR_OFFSET) >> 1;
		uint16_t posy = ((char_y >> 3) - VER_OFFSET) >> 1;
		if ((key1 & BUTTON_A) && !(prevkey1 & BUTTON_A) && availbombs) {
			// We can plant a bomb provided that:
			// 1. The current position is free.
			// 2. There's some bombs to be planted
			if (atmap[posy][posx] == MAP_FREE) {
				// Plant the bomb!
				int free_pos = -1;
				for (unsigned i = 0; i < MAX_BOMBS; i++)
					if (bombs[i].state == bombUnused)
						free_pos = i;
				if (free_pos >= 0) {
					XGM_startPlayPCM(SND_PLANT, 0, SND_EVEN_CH);
					bombs[free_pos].state = bombTicking;
					bombs[free_pos].x = posx;
					bombs[free_pos].y = posy;
					bombs[free_pos].fcount = 0;
					bombs[free_pos].scount = 3;
					availbombs--;
				}
			}
		}
		prevkey1 = key1;

		// Process collision routines for the main character.
		collisionMapProcess(&colarr[0][0], collision_width8);

		// Check whether viewport needs to be repositioned.
		levelSetViewportArea(char_x, char_y, 200, 100);


		// Meta level FSM
		switch (levelfsm) {
		case LEVEL_STARTING:
			// Palette setup here for fadein
			for (unsigned i = 0; i < 4; i++)
				fade_dark_pal(workpal[i], &refpal[i][0], (lvlcnter >> 2), 3);

			if (lvlcnter++ >= 32)
				levelfsm = LEVEL_PLAY;
			break;
		case LEVEL_ENDING_WIN:
			// Palette setup here for fadein
			for (unsigned i = 0; i < 4; i++)
				fade_dark_pal(workpal[i], &refpal[i][0], 8 - (lvlcnter >> 2), 3);

			if (lvlcnter++ >= 32)
				retcode = LEVEL_ACHIEVED;
			break;
		case LEVEL_PLAY:
			lvlcnter = 0;
			onhold = FALSE;
			// Die transition
			if (lifepts <= 0)
				levelfsm = LEVEL_RESTART;
			// Collect all keys!
			if (keystocollect == collectedkeys && !doorframe)
				levelfsm = LEVEL_SPECIAL_ENDING;
			// End! Can't do this unless the door is open :)
			if (char_y < 40)
				levelfsm = LEVEL_ENDING_WIN;
			break;
		case LEVEL_SPECIAL_ENDING:
			onhold = TRUE;
			// Move camera to 712, 16 approx
			if (!lvlcnter) {
				ovpx = vp_x; ovpy = vp_y;
			}
			else if (lvlcnter < 128) {
				int newx = (((DOOR_VP_X - ovpx) * lvlcnter) >> 7) + ovpx;
				int newy = (((DOOR_VP_Y - ovpy) * lvlcnter) >> 7) + ovpy;
				levelSetViewport(newx, newy);
			}
			else if (lvlcnter < 256) {
				doorframe = (lvlcnter - 128) >> 4;
				levelSetViewport(DOOR_VP_X, DOOR_VP_Y);
				// Mark the door as non-colliding
			}
			else if (lvlcnter < 384) {
				int newx = (((ovpx - DOOR_VP_X) * (lvlcnter-256)) >> 7) + DOOR_VP_X;
				int newy = (((ovpy - DOOR_VP_Y) * (lvlcnter-256)) >> 7) + DOOR_VP_Y;
				levelSetViewport(newx, newy);
			}
			else {
				// Mark the door as non-colliding
				for (unsigned i = 89; i < 93; i++)
					for (unsigned j = 3; j < 6; j++)
						set_colbits8(i, j, 0);
				levelfsm = LEVEL_PLAY;
			}
			lvlcnter++;
			// davidgf
			break;
		case LEVEL_RESTART:
			if (lvlcnter++ > 70) {
				lvlcnter = 0;
				levelfsm = LEVEL_RESTARTING;
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
		}

		// Process collectable items
		switch (atmap[posy][posx]) {
		case MAP_KEYASSET:
			// Grab the key TODO
			set_planeA_tile(MAP_FREE, posx, posy);
			XGM_startPlayPCM(SND_PICKUP, 0, SND_EVEN_CH);
			collectedkeys++;
			break;
		case MAP_FLAMEUP:
			currradius = CLAMPMAX(currradius + 1, MAX_RADIUS);
			set_planeA_tile(MAP_FREE, posx, posy);
			XGM_startPlayPCM(SND_UPGRADE, 0, SND_EVEN_CH);
			break;
		case MAP_BOMBUP:
			availbombs++;   // No need to clamp, there's only MAX_BOMB slots :D
			set_planeA_tile(MAP_FREE, posx, posy);
			XGM_startPlayPCM(SND_UPGRADE, 0, SND_EVEN_CH);
			break;
		case MAP_HEALTH:
			lifepts = CLAMPMAX(lifepts + 1, 3);
			set_planeA_tile(MAP_FREE, posx, posy);
			XGM_startPlayPCM(SND_PICKUP, 0, SND_EVEN_CH);
			break;
		default: break;
		}

		int scxoff = vp_x - _SCREEN_WIDTH/2;
		int scyoff = vp_y - _SCREEN_HEIGHT/2;

		// Main char animation calculations
		int charfc = move ? fcounter : 0;
		int btchar = (4 * orientation + ((charfc >> 2) & 3));
		int flipmain = (btchar >= 12) ? 1 : 0;
		if (flipmain)
			btchar -= 12;
		if (invincible > 80)
			btchar = 14;
		else if (lifepts <= 0)
			btchar = 15;
		int spframe = btchar << 2; // 2x2 tiles

		// Position sprite according to vp coords and char coords!
		int nextsprite = 0;
		// Animate main char using a blink animation, first fast then slow.
		if (!invincible || ((invincible > 64) && (fcounter & 4)) || ((invincible <= 64) && (fcounter & 8)))
			SET_SPRITE_PLUS(nextsprite, char_x - scxoff - MAINCHAR_W/2, char_y - scyoff - MAINCHAR_H/2,
				SPRITE_SIZE(2, 2), TILE_ATTR_FULL(PAL3, 0, 0, flipmain, MAINCHR_TB + spframe));

		// OSD on top, priority=1
		SET_SPRITE_PLUS(nextsprite, 16, 16, SPRITE_SIZE(2, 2),
			TILE_ATTR_FULL(PAL2, 1, 0, 0, HEARTSP_TB + ((3-CLAMPMIN(lifepts, 1)) << 2)));
		for (int i = 0; i < (keystocollect - collectedkeys); i++)
			SET_SPRITE_PLUS(nextsprite, 288 - 8*i, 16, SPRITE_SIZE(2, 2),
				TILE_ATTR_FULL(PAL1, 1, 0, 0, KEYRANI_TB +
					(((fcounter >> 3) % anim_frames_key_anim_count) << 2)));

		// Flames, as animations, are really sprites
		SET_SPRITE_PLUS(nextsprite, OBJ_FLAME1_X - scxoff, OBJ_FLAME1_Y - scyoff, SPRITE_SIZE(1, 2),
			TILE_ATTR_FULL(PAL2, 0, 0, 0, TORCHAN_TB + (((fcounter >> 3) % anim_frames_flames_count) << 1)));
		SET_SPRITE_PLUS(nextsprite, OBJ_FLAME2_X - scxoff, OBJ_FLAME2_Y - scyoff, SPRITE_SIZE(1, 2),
			TILE_ATTR_FULL(PAL2, 0, 0, 0, TORCHAN_TB + (((fcounter >> 3) % anim_frames_flames_count) << 1)));

		// Display the door as mandated!
		SET_SPRITE_PLUS(nextsprite, 712 - scxoff, 16 - scyoff, SPRITE_SIZE(4, 4),
			TILE_ATTR_FULL(PAL0, 1, 0, 0, DOORANI_TB + (doorframe << 4)));

		// Bomb processing
		const int8_t offs[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};
		for (unsigned i = 0; i < MAX_BOMBS; i++) {
			switch (bombs[i].state) {
				case bombBlast:
					// Add flames on the burnable objects
					for (unsigned j = 0; j < 4; j++) {
						int lx = bombs[i].x, ly = bombs[i].y;
						for (unsigned c = 0; c < currradius; c++) {
							lx += offs[j][0];
							ly += offs[j][1];
							if (lx < 0 || ly < 0 || lx >= bombmap_width || ly >= bombmap_height)
								break;
							if (ISCOLL(atmap[ly][lx]) && !ISDEST(atmap[ly][lx]))
								break;

							int ex = (lx << 4) + HOR_OFFSET*8 - scxoff;
							int ey = (ly << 4) + VER_OFFSET*8 - scyoff - 8;
							SET_SPRITE_PLUS(nextsprite, ex, ey, SPRITE_SIZE(2, 3),
							                TILE_ATTR_FULL(PAL2, 1, 0, 0, BURNANI_TB + 6*((bombs[i].fcount>>1)&7)));

							// Receive damage check (prevents double damage)
							if (!invincible && posx == lx && posy == ly) {
								invincible = 100;
								lifepts--;
								XGM_startPlayPCM(SND_DEAD, 0, SND_DAMG_CH);
							}

							// Also process enemies walking by
							for (unsigned n = 0; n < ENEMIES_COUNT; n++) {
								if (enemies[n].state != enemyDead &&
								    (enemies[n].x >> 8) == lx && (enemies[n].y >> 8) == ly) {
									enemies[n].state = enemyDie;
									enemies[n].fsm &= 1;
								}
							}
						}
					}
					// Corner case, bomb position also damages us!
					if (!invincible && posx == bombs[i].x && posy == bombs[i].y) {
						invincible = 100;
						lifepts--;
						XGM_startPlayPCM(SND_DEAD, 0, SND_DAMG_CH);
					}

					// Blast animation
					int bx = (bombs[i].x << 4) + HOR_OFFSET*8 - scxoff - 4;
					int by = (bombs[i].y << 4) + VER_OFFSET*8 - scyoff - 4;
					SET_SPRITE_PLUS(nextsprite, bx, by,
						SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PAL0, 0, 0, 0, EPLXANI_TB + 9*((bombs[i].fcount) >> 2)));

					/* fall through */
				case bombTicking:
					// Add collision if the user is not on top of the bomb.
					{
						int x8 = (bombs[i].x << 1) + HOR_OFFSET;
						int y8 = (bombs[i].y << 1) + VER_OFFSET;
						int xcoord = x8<<3, ycoord = y8<<3;
						if (!(char_x-MAINCHAR_W/2 <= xcoord+13 && char_x+MAINCHAR_W/2 >= xcoord+3 &&
						      char_y-MAINCHAR_H/2 <= ycoord+13 && char_y+MAINCHAR_H/2 >= ycoord+3)) {

							set_colbits16(bombs[i].x, bombs[i].y, 1);
						}
					}
					break;
				default: break;
			}
		}

		// Process enemies
		for (unsigned i = 0; i < ENEMIES_COUNT; i++) {
			if (enemies[i].state == enemyDead)
				continue;

			uint16_t tilen = GAME_TRANS_TILE;
			uint8_t flipv = 0;
			uint8_t x16 = enemies[i].x >> 8;
			uint8_t y16 = enemies[i].y >> 8;
			bool diravail[4] = {
				x16 < bombmap_width-1  && !(ISCOLL(atmap[y16][x16+1])), // right
				x16 > 0                && !(ISCOLL(atmap[y16][x16-1])), // left
				y16 < bombmap_height-1 && !(ISCOLL(atmap[y16+1][x16])), // down
				y16 > 0                && !(ISCOLL(atmap[y16-1][x16])), // up
			};

			// Character damage on enemy collision! Ignore if dying.
			if (enemies[i].state != enemyDie && !invincible && posx == x16 && posy == y16) {
				invincible = 100;
				lifepts--;
				XGM_startPlayPCM(SND_DEAD, 0, SND_DAMG_CH);
			}

			const struct enem_data enem_anims[] = {
				{ 7, 2, { {ENEMBAT_TB, 7, 0}, {ENEMBAT_TB, 7, 1},  // Bat
				          {ENEMBAT_TB, 7, 0}, {ENEMBAT_TB + 8*4, 7, 0},
				          {ENEMBAT_TB, 7, 0}, {ENEMBAT_TB + 16*4, 4, 0}}},
				{ 6, 3, { {ENEMDEV_TB + 4, 7, 0}, {ENEMDEV_TB + 4, 7, 1},  // Devil
				          {ENEMDEV_TB + 4, 7, 0}, {ENEMDEV_TB + 4, 7, 0},
				          {ENEMDEV_TB, 0, 0}, {ENEMDEV_TB + 4*9, 1, 0}}},
				{ 6, 3, { {ENEMFLA_TB +  4*4, 7, 0}, {ENEMFLA_TB +  4*4, 7, 1},  // Flame
				          {ENEMFLA_TB + 12*4, 3, 0}, {ENEMFLA_TB + 16*4, 3, 0},
				          {ENEMFLA_TB, 3, 0}, {ENEMFLA_TB + 20*4, 6, 0}}},
				{ 5, 3, { {ENEMWOR_TB + 2*4, 7, 0}, {ENEMWOR_TB + 2*4, 7, 1},  // Worm
				          {ENEMWOR_TB + 2*4, 7, 0}, {ENEMWOR_TB + 2*4, 7, 0},
				          {ENEMWOR_TB + 2*4, 7, 0}, {ENEMWOR_TB + 2*4, 7, 0}}},
			};
			const struct enem_data *ed = &enem_anims[enemies[i].t];

			// Enemy shared logic
			switch (enemies[i].state) {
			case enemyStartMove:
				if (enemies[i].fsm > 240)
					enemies[i].state = enemyMove;
				else
					enemies[i].fsm += 4;
				/* fall through */
			case enemyMove: {
				// Can only stop at crosses if it's not starting the move
				bool stopcross = (enemies[i].state == enemyMove) && !(randomu32() & 7);
				bool edgex = !((enemies[i].x >> 4) & 15);
				bool edgey = !((enemies[i].y >> 4) & 15);
				uint8_t movd = enemies[i].fsm & 3;
				tilen = (ed->a[movd].base_tile +
				        (((fcounter >> ed->fpsh) & ed->a[movd].frames) << 2));
				flipv = ed->a[movd].flip;
				switch (movd) {
				case 0:  // Go right
					if (edgex && (!diravail[0] || ((diravail[2] || diravail[3]) && stopcross)))
						enemies[i].state = enemyIdle;
					else
						enemies[i].x += ed->speed;
					break;
				case 1:  // Go left
					if (edgex && (!diravail[1] || ((diravail[2] || diravail[3]) && stopcross)))
						enemies[i].state = enemyIdle;
					else
						enemies[i].x -= ed->speed;
					break;
				case 2:  // Go down
					if (edgey && (!diravail[2] || ((diravail[0] || diravail[1]) && stopcross)))
						enemies[i].state = enemyIdle;
					else
						enemies[i].y += ed->speed;
					break;
				case 3:  // Go up
					if (edgey && (!diravail[3] || ((diravail[0] || diravail[1]) && stopcross)))
						enemies[i].state = enemyIdle;
					else
						enemies[i].y -= ed->speed;
					break;
				};
				} break;
			case enemyIdle:
				// Pick some initial value (count up to 240, so 1-2 sec or so)
				enemies[i].fsm = (randomu32() & 0x7C) | (enemies[i].fsm & 3);
				enemies[i].state = enemyIdling;
				/* fall through */
			case enemyIdling:
				// Idle, with animation, for ~1-2 sec, then pick direction & move.
				enemies[i].fsm += 2;
				if (enemies[i].fsm >= 240) {
					// Check possible directions and pick one.
					uint8_t dir = randomu32();
					for (unsigned j = 0; j < 4; j++)
						if (diravail[dir & 3])
							break;
						else
							dir++;

					enemies[i].state = enemyStartMove;
					enemies[i].fsm = dir & 3;
				}
				// LSB determines the idling orientation
				tilen = (ed->a[4].base_tile +
				        (((fcounter >> ed->fpsh) & ed->a[4].frames) << 2));
				flipv = enemies[i].fsm & 1;
				break;
			case enemyDie:
				// FSM last bit encodes flip. Animation only played once
				tilen = (ed->a[5].base_tile + ((enemies[i].fsm >> 4) << 2));
				flipv = enemies[i].fsm & 1;
				enemies[i].fsm += 2;
				if ((enemies[i].fsm >> 4) == ed->a[5].frames)
					enemies[i].state = enemyDead;
				break;
			default: break;
			};

			int enemx = (enemies[i].x >> 4) + HOR_OFFSET*8 - scxoff; // - ENEMYCHR_W/2;
			int enemy = (enemies[i].y >> 4) + VER_OFFSET*8 - scyoff; // - ENEMYCHR_H/2;
			SET_SPRITE_PLUS(nextsprite, enemx, enemy,
			                SPRITE_SIZE(2, 2), TILE_ATTR_FULL(PAL2, 0, 0, flipv, tilen));
		}

		// Close sprite list
		VDP_setSpriteLink(nextsprite-1, 0);

		VDP_waitVSync();

		for (unsigned i = 0; i < 4; i++)
			VDP_setPalette(PAL0 + i, workpal[i]);

		// Flush sprites to VRAM, also load tilemaps to plane A and B
		VDP_updateSprites(MAX_SPRITES_ONSCREEN, TRUE);
		levelVblankWork16B(&level2_tilemap_planeb_low[0][0], NULL);
		levelVblankWork16A(NULL, atmap);

		// Flush any changes in plane A due to bomb states.
		for (unsigned i = 0; i < MAX_BOMBS; i++) {
			switch (bombs[i].state) {
				case bombTicking:
					bombs[i].fcount++;
					set_planeA_tile(MAP_BOMBTICK1 + ((bombs[i].fcount >> bombs[i].scount) & 3),
					                bombs[i].x, bombs[i].y);
					if (bombs[i].fcount > 50) {
						bombs[i].fcount = 0;
						if (!--bombs[i].scount)
							bombs[i].state = bombExploding;
					}
					break;
				case bombExploding:
					set_planeA_tile(MAP_BOMBEXPL1 + ((bombs[i].fcount >> 3) & 3),
					                bombs[i].x, bombs[i].y);
					if (++bombs[i].fcount == 32) {
						set_planeA_tile(MAP_FREE, bombs[i].x, bombs[i].y);
						bombs[i].fcount = 0;
						bombs[i].state = bombBlast;
						XGM_startPlayPCM(SND_EXPLODE, 0, SND_EXPL_CH);
					}
					break;
				case bombBlast:
					if (++bombs[i].fcount == 16) {
						// Delete enemy setting type to ENTYPE_MAX
						for (unsigned j = 0; j < 4; j++) {
							int lx = bombs[i].x, ly = bombs[i].y;
							for (unsigned c = 0; c < currradius; c++) {
								lx += offs[j][0];
								ly += offs[j][1];
								if (lx < 0 || ly < 0 || lx >= bombmap_width || ly >= bombmap_height)
									break;
								if (ISCOLL(atmap[ly][lx]) && !ISDEST(atmap[ly][lx]))
									break;
								if (!ISDEST(atmap[ly][lx]))
									continue;   // No interaction

								// Clear out the object, maybe?
								uint8_t mapattr = MAP_FREE;
								if (atmap[ly][lx] == MAP_BOX)
									mapattr = MAP_KEYASSET;
								else if (atmap[ly][lx] >= MAP_DESTRUCT1 && atmap[ly][lx] <= MAP_DESTRUCT3) {
									if (!(randomu32() & 3) && !(randomu32() & 3)) {
										unsigned e = randomu32() & 3;
										mapattr = MAP_FLAMEUP + CLAMPMAX(e, 2);
									}
								}

								set_colbits16(lx, ly, 0);
								set_planeA_tile(mapattr, lx, ly);
							}
						}

						// Bomb has completely exploded, cleanup!
						availbombs++;
						bombs[i].state = bombUnused;
						set_colbits16(bombs[i].x, bombs[i].y, 0);
					}
					break;
				default: break;
			};
		}

		// Next frame
		fcounter++;
		if (invincible)
			invincible--;
	}

	XGM_stopPlay();

	return retcode;
}


