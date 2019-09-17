#include <genesis.h>
#include "config.h"
#include "util.h"
#include "level_functions.h"
#include "collision.h"
#include "levels.h"

#include "resources.h"
#include "assets/level3/landscape_a.h"
#include "assets/level3/landscape_b.h"
#include "assets/level3/enemies.h"
#include "assets/level3/player.h"
#include "assets/level3/effects.h"

// Horizontal shooter game

#define PLAYER_W  32
#define PLAYER_H  32

// Tile layout:
#define PLAYERS_TB  (                                        0)  // Player ship        ~48
#define ENEMYPL_TB  (                     sprites_player_count)  // Orange enemy       ~32
#define ENEMYML_TB  (ENEMYPL_TB +    sprites_enemy_plane_count)  // Misile enemy       ~32
#define ENEMYAL_TB  (ENEMYML_TB +   sprites_enemy_misile_count)  // Alien enemy        ~32
#define ENEMYJT_TB  (ENEMYAL_TB +    sprites_enemy_alien_count)  // Big plane enemy    ~80
#define BIGEXPL_TB  (ENEMYJT_TB +     sprites_enemy_ship_count)  // Big explosion      ~96
#define SMLEXPL_TB  (BIGEXPL_TB +      sprites_explosion_count)  // Small explosion    ~20
#define FLBOOST_TB  (SMLEXPL_TB + sprites_smallexplosion_count)  // Flame boost        ~32
#define FIREWPN_TB  (FLBOOST_TB +    sprites_flame_boost_count)  // Fire weapon        ~24
#define FIREBLL_TB  (FIREWPN_TB +     sprites_fireweapon_count)  // Fire ball          ~32
#define FIRESHT_TB  (FIREBLL_TB +       sprites_fireball_count)  // Fire shot          ~8
#define BARTILS_TB  (FIRESHT_TB +           sprites_shot_count)  // Bar                ~3


#define LANDS_B_TB  (BARTILS_TB +            sprites_bar_count)  // Level PlaneB tiles ~240
#define LANDS_A_TB  (LANDS_B_TB +    sprites_landscape_b_count)  // Level PlaneA tiles ~160

#define MAX_SPRITES_ONSCREEN 64

#define SND_SHOT_CH  1
#define SND_DAMG_CH  2
#define SND_EXPL_CH  3

#define SND_DAMAGE   128
#define SND_SHOOT    129
#define SND_EXP      130

background_H_ForceReload_TEMPLATE(8B,
	uint8_t, tilemap_landscape_b_width, tilemap_landscape_b_height, PLAN_B, PAL0, LANDS_B_TB)
background_H_ForceReload_TEMPLATE(8A,
	uint8_t, tilemap_landscape_a_width, tilemap_landscape_a_height, PLAN_A, PAL0, LANDS_A_TB)

_Static_assert(LANDS_A_TB < 1200, "Tile count sanity");

const int16_t sint[128] = {
	0,13,25,38,50,62,74,86,98,109,121,132,142,152,162,172,181,190,198,
	206,213,220,226,231,237,241,245,248,251,253,255,256,256,256,255,253,
	251,248,245,241,237,231,226,220,213,206,198,190,181,172,162,152,142,
	132,121,109,98,86,74,62,50,38,25,13,0,-13,-25,-38,-50,-62,-74,-86,
	-98,-109,-121,-132,-142,-152,-162,-172,-181,-190,-198,-206,-213,-220,
	-226,-231,-237,-241,-245,-248,-251,-253,-255,-256,-256,-256,-255,
	-253,-251,-248,-245,-241,-237,-231,-226,-220,-213,-206,-198,-190,-181,
	-172,-162,-152,-142,-132,-121,-109,-98,-86,-74,-62,-50,-38,-25,-13
};

// Waves of enemies are described in the following structure.
// In general enemies come in waves, that move toghether of follow a trajectory.

struct t_enemy_desc {
	uint32_t x, y;     // These are center coordinates in the "tunnel view space"
	uint8_t etype;     // Enemy sprite is 0..1, shooting type 2..3, 4..5 mov type
	uint8_t extra;     // For move processing
};

// Frame calculation for sprite rendering
typedef uint16_t (*t_fcalc_fn)(unsigned fn);
uint16_t fc_eplane_orange(unsigned fn) {
	unsigned frame = (fn & 1) ? ((fn >> 3) & 3) + 1 : 0;
	return ENEMYPL_TB + (frame << 3);
}
uint16_t fc_eplane_purple(unsigned fn) {
	unsigned frame = (fn & 1) ? ((fn >> 3) & 3) + 1 : 0;
	return ENEMYPL_TB + ((frame + 5) << 3);
}
uint16_t fc_alien(unsigned fn) {
	return ENEMYAL_TB;
}
uint16_t fc_jetship(unsigned fn) {
	unsigned frame = (fn & 1) ? ((fn >> 3) & 3) + 1 : 0;
	return ENEMYJT_TB + (frame << 4);
}
const t_fcalc_fn rendtable[4] = {
	fc_eplane_orange, fc_eplane_purple, fc_alien, fc_jetship
};
const uint8_t sizetable[4] = { 0x42, 0x42, 0x33, 0x44 };
const uint8_t ptstable [4] = { 2, 2, 4, 8 };

// Movement functions, return a pair tuple
struct coord2d { int x, y; };
typedef struct coord2d (*t_mov_fn)(uint32_t fcounter, int16_t x, int16_t y, uint8_t e);
struct coord2d mov_identity(uint32_t fcounter, int16_t x, int16_t y, uint8_t e) {
	return (struct coord2d){x, y};
}
struct coord2d mov_swing(uint32_t fcounter, int16_t x, int16_t y, uint8_t e) {
	int ny = (int)y + (int)sint[(fcounter + e) & 127];
	return (struct coord2d){x, ny};
}
struct coord2d mov_circle(uint32_t fcounter, int16_t x, int16_t y, uint8_t e) {
	int ny = (int)y + sint[(e * (fcounter >> 1)) & 127];
	int nx = (int)x + sint[((e * (fcounter >> 1)) + 32) & 127]; // poor's man cos()
	return (struct coord2d){nx, ny};
}
const t_mov_fn movtable[4] = {
	mov_identity, mov_swing, mov_circle,
};

uint16_t fc_orangewide(unsigned fn) {
	return FIREWPN_TB + (((fn >> 2) % 3) * 6);
}
uint16_t fc_bluewide(unsigned fn) {
	return FIREWPN_TB + (((fn >> 2) % 3) * 6) + 18;
}
uint16_t fc_orangenarrow(unsigned fn) {
	return FIREBLL_TB + (((fn >> 2) & 3) * 6);
}
uint16_t fc_bluenarrow(unsigned fn) {
	return FIREBLL_TB + (((fn >> 2) & 3) * 6) + 24;
}
const t_fcalc_fn shotrendtable[4] = {
	fc_bluewide, fc_orangewide, fc_bluenarrow, fc_orangenarrow
};
const uint8_t shotsize[4] = { 0x23, 0x23, 0x32, 0x32 };

struct t_enemy {
	const struct t_enemy_desc *desc;
	int x, y;
	uint8_t w, h;
	int8_t pts;
	uint8_t shotf;
	t_fcalc_fn framecalc;
	t_mov_fn movefn;
};
#define MAX_OS_ENEMIES   24
#define MAX_USR_SHOTS    20
#define MAX_ENM_SHOTS    20
#define MAX_EXPLOSIONS    8

#define ENEMY_ORANGE_PLANE  0x00
#define ENEMY_PURPLE_PLANE  0x01
#define ENEMY_ALIEN_SHIP    0x02
#define ENEMY_BIG_JT        0x03

#define SHOOT_BLUE_WIDE     0x00
#define SHOOT_ORAN_WIDE     0x04
#define SHOOT_BLUE_NARR     0x08
#define SHOOT_ORAN_NARR     0x0C

#define MOVE_IDENTITY       0x00
#define MOVE_SWING          0x10
#define MOVE_CIRCLE         0x20

#include "shooter_defs.h"
#define ENEMY_TABLE_COUNT  (sizeof(enemy_table)/sizeof(enemy_table[0]))

#define allocexp(__x, __y, __t) \
	for (unsigned h = 0; h < MAX_EXPLOSIONS; h++) { \
		if (explosions[h].type > 3) { \
			explosions[h].x = (__x);      \
			explosions[h].y = (__y);      \
			explosions[h].frame = 0;  \
			explosions[h].type = (__t);   \
			break;  \
		} }

unsigned level3() {
	// Reset to a known state
	VDP_resetSprites();
	Z80_init();

	levelSetViewport(0, 0);

	// Load all common sprites
	loadDecompressTiles(sprites_compr_player,         PLAYERS_TB, DMA);
	loadDecompressTiles(sprites_compr_enemy_plane,    ENEMYPL_TB, DMA);
	loadDecompressTiles(sprites_compr_enemy_misile,   ENEMYML_TB, DMA);
	loadDecompressTiles(sprites_compr_enemy_alien,    ENEMYAL_TB, DMA);
	loadDecompressTiles(sprites_compr_enemy_ship,     ENEMYJT_TB, DMA);
	loadDecompressTiles(sprites_compr_explosion,      BIGEXPL_TB, DMA);
	loadDecompressTiles(sprites_compr_smallexplosion, SMLEXPL_TB, DMA);
	loadDecompressTiles(sprites_compr_flame_boost,    FLBOOST_TB, DMA);
	loadDecompressTiles(sprites_compr_fireweapon,     FIREWPN_TB, DMA);
	loadDecompressTiles(sprites_compr_fireball,       FIREBLL_TB, DMA);
	loadDecompressTiles(sprites_compr_shot,           FIRESHT_TB, DMA);
	loadDecompressTiles(sprites_compr_bar,            BARTILS_TB, DMA);
	DMA_flushQueue();

	loadDecompressTiles(tiles_compr_landscape_b, LANDS_B_TB, DMA);
	loadDecompressTiles(tiles_compr_landscape_a, LANDS_A_TB, DMA);
	background_H_ForceReload8A(tilemap_landscape_a);
	background_H_ForceReload8B(tilemap_landscape_b);
	VDP_setVerticalScroll(PLAN_B, 144);
	VDP_setVerticalScroll(PLAN_A, 144);

	uint16_t refpal[4][16], workpal[4][16];
	memcpy(refpal[0], landscape_b_palette, 32);
	memcpy(refpal[1], enemies_palette, 32);
	memcpy(refpal[2], effects_palette, 32);
	memcpy(refpal[3], player_palette, 32);
	memset(workpal, 0, sizeof(workpal));

	XGM_setPCM(SND_DAMAGE,   shooter_damage, sizeof(shooter_damage));
	XGM_setPCM(SND_SHOOT,   shooter_shoot, sizeof(shooter_shoot));
	XGM_setPCM(SND_EXP,   shooter_exp, sizeof(shooter_exp));

	VDP_setEnable(1);

	XGM_setMusicTempo(60);
	XGM_setLoopNumber(-1);
	XGM_startPlay(music_shooter);

	int char_x = 64 << 4, char_y = 140 << 4;

	// Objects on screen
	struct t_enemy enemies[MAX_OS_ENEMIES] = {0};
	struct {
		int16_t x, y;
	} usershots[MAX_USR_SHOTS];
	struct {
		int16_t x, y;
		t_fcalc_fn rend;
		uint8_t w, h;
	} enemyshots[MAX_ENM_SHOTS];
	struct {
		int16_t x, y;
		uint8_t frame, type;
	} explosions[MAX_EXPLOSIONS];
	memset(usershots,  ~0, sizeof(usershots));
	memset(enemyshots,  0, sizeof(enemyshots));
	memset(explosions, ~0, sizeof(explosions));

	unsigned fcounter = 0, invincible = 0;
	uint32_t scrollx = 0, escroll = 0, currenemy = 0;
	bool prevshot = FALSE;
	unsigned levelfsm = LEVEL_STARTING, lvlcnter = 0;
	int retcode = -1, lifepts = 32;
	while (retcode < 0) {
		// Process input, keep player within the limits
		uint16_t key1 = JOY_readJoypad(JOY_1);
		if (key1 & BUTTON_LEFT)
			char_x = CLAMPMIN(char_x - 24, 16*16);
		if (key1 & BUTTON_RIGHT)
			char_x = CLAMPMAX(char_x + 24, (_SCREEN_WIDTH-16)*16);
		if (key1 & BUTTON_UP)
			char_y = CLAMPMIN(char_y - 24, 16*16);
		if (key1 & BUTTON_DOWN)
			char_y = CLAMPMAX(char_y + 24, (_SCREEN_HEIGHT-16)*16);
		if (lifepts > 0 && key1 & (ANY_BUTTON)) {
			if (!prevshot)
				for (unsigned i = 0; i < MAX_USR_SHOTS; i++)
					if (usershots[i].x < 0) {
						prevshot = TRUE;
						usershots[i].x = (char_x >> 4) + 15;
						usershots[i].y = (char_y >> 4) + 8;
						XGM_startPlayPCM(SND_SHOOT, 0, SND_SHOT_CH);
						break;
					}
		}
		else
			prevshot = FALSE;

		// Meta level FSM
		switch (levelfsm) {
		case LEVEL_PLAY:
			lvlcnter = 0;
			if (lifepts <= 0) {
				levelfsm = LEVEL_RESTART;
				XGM_startPlayPCM(SND_EXP, 0, SND_EXPL_CH);
			}
			break;
		case LEVEL_RESTART:
			if (lvlcnter++ >= 24) {
				levelfsm = LEVEL_RESTARTING;
				lvlcnter = 0;
			}
			break;
		case LEVEL_RESTARTING:
			for (unsigned i = 0; i < 4; i++)
				fade_dark_pal(workpal[i], &refpal[i][0], 8 - (lvlcnter >> 2), 3);
			if (lvlcnter++ >= 32)
				retcode = LEVEL_FAILED;
			break;
		case LEVEL_STARTING:
			// Palette setup here for fadeout
			for (unsigned i = 0; i < 4; i++)
				fade_dark_pal(workpal[i], &refpal[i][0], (lvlcnter >> 2), 3);
			if (lvlcnter++ >= 32)
				levelfsm = LEVEL_PLAY;
			break;
		case LEVEL_ENDING_WIN:
			for (unsigned i = 0; i < 4; i++)
				fade_dark_pal(workpal[i], &refpal[i][0], 8 - (lvlcnter >> 2), 3);
			if (lvlcnter++ >= 32)
				retcode = LEVEL_ACHIEVED;
			break;
		};

		// Scroll background relatively slow.
		VDP_setHorizontalScroll(PLAN_B, (scrollx >> 2));
		VDP_setHorizontalScroll(PLAN_A, ((scrollx * 3) >> 2));
		scrollx -= 3;
		escroll += 34;

		// Take damage and similar logic
		int posx = char_x >> 4, posy = char_y >> 4;
		for (unsigned i = 0; i < MAX_ENM_SHOTS && !invincible; i++)
			if (enemyshots[i].x >= posx - PLAYER_W/2 && enemyshots[i].x <= posx + PLAYER_W/2 &&
			    enemyshots[i].y >= posy - PLAYER_H/2 && enemyshots[i].y <= posy + PLAYER_H/2) {
				// Shots do not award invisibilty, only collisions.
				// invincible = 100;
				// Remove the bullet also and add explosion
				allocexp(enemyshots[i].x, enemyshots[i].y, 0)
				enemyshots[i].x = -32;
				lifepts--;
				XGM_startPlayPCM(SND_DAMAGE, 0, SND_DAMG_CH);
			}

		for (unsigned i = 0; i < MAX_OS_ENEMIES && !invincible; i++)
			if (enemies[i].x >= char_x - PLAYER_W/2*16 && enemies[i].x <= char_x + PLAYER_W/2*16 &&
			    enemies[i].y >= char_y - PLAYER_H/2*16 && enemies[i].y <= char_y + PLAYER_H/2*16) {
				// Colliding takes extra damage but gives invincibility for a bit
				invincible = 80;
				// Remove the bullet also and add explosion
				lifepts -= 3;
			}

		// Draw shots from user and enemies
		for (unsigned i = 0; i < MAX_USR_SHOTS; i++)
			if (usershots[i].x >= 0) {
				for (unsigned j = 0; j < MAX_OS_ENEMIES; j++) {
					if (enemies[j].desc) {
						if (usershots[i].x > (enemies[j].x >> 4) - (enemies[j].w << 2) &&
						    usershots[i].x < (enemies[j].x >> 4) + (enemies[j].w << 2) &&
						    usershots[i].y > (enemies[j].y >> 4) - (enemies[j].h << 2) &&
						    usershots[i].y < (enemies[j].y >> 4) + (enemies[j].h << 2)) {
							// Take some damage
							// FIXME for now instakill
							if (--enemies[i].pts <= 0) {
								enemies[j].desc = NULL;
								allocexp(enemies[j].x >> 4, enemies[j].y >> 4, randomu32() & 1);
								XGM_startPlayPCM(SND_EXP, 0, SND_EXPL_CH);
							}
							else {
								allocexp(usershots[i].x, usershots[i].y, 0);
								XGM_startPlayPCM(SND_DAMAGE, 1, SND_DAMG_CH);
							}

							usershots[i].x = 512;
							break;
						}
					}
				}

				usershots[i].x += 3;
				if (usershots[i].x > _SCREEN_WIDTH + 16)
					usershots[i].x = -32;
			}
		for (unsigned i = 0; i < MAX_ENM_SHOTS; i++)
			if (enemyshots[i].x > -32)
				enemyshots[i].x -= 5;

		// Process enemies to clean up any ones that are "gone"
		for (unsigned i = 0; i < MAX_OS_ENEMIES; i++)
			if (enemies[i].desc && enemies[i].x < -64*16)
				memset(&enemies[i], 0, sizeof(struct t_enemy));

		// Process enemy table, and add enemies as needed
		while (enemy_table[currenemy].x > 0 &&
		       enemy_table[currenemy].x < (escroll >> 4) + 64 + _SCREEN_WIDTH) {

			// Look for an enemy entry
			for (unsigned i = 0; i < MAX_OS_ENEMIES; i++)
				if (!enemies[i].desc) {
					enemies[i].desc = &enemy_table[currenemy];
					enemies[i].framecalc = rendtable[enemy_table[currenemy].etype & 3];
					enemies[i].movefn = movtable[(enemy_table[currenemy].etype >> 4) & 3];
					enemies[i].w = sizetable[enemy_table[currenemy].etype & 3] >> 4;
					enemies[i].h = sizetable[enemy_table[currenemy].etype & 3] & 15;
					enemies[i].pts = ptstable[enemy_table[currenemy].etype & 3];
					enemies[i].shotf = 0;
					break;
				}
			currenemy++;
		}
		if (!enemy_table[currenemy].x && enemy_table[currenemy-1].x < (escroll >> 4)) {
			levelfsm = LEVEL_ENDING_WIN;
		}

		// Move ships using their callbacks, also process shooting
		for (unsigned i = 0; i < MAX_OS_ENEMIES; i++)
			if (enemies[i].desc) {
				int currx = ((enemies[i].desc->x << 4) - escroll);
				struct coord2d p = enemies[i].movefn(fcounter, currx, enemies[i].desc->y << 4,
				                                     enemies[i].desc->extra);
				enemies[i].x = p.x;
				enemies[i].y = p.y;
				enemies[i].shotf++;

				if ((!(randomu32() & 31)) && enemies[i].shotf > 50) {
					for (unsigned j = 0; j < MAX_ENM_SHOTS; j++)
						if (enemyshots[j].x < -16) {
							unsigned t = (enemies[i].desc->etype >> 2) & 3;
							enemyshots[j].rend = shotrendtable[t];
							enemyshots[j].w = shotsize[t] >> 4;
							enemyshots[j].h = shotsize[t] & 15;
							enemyshots[j].x = (p.x >> 4) - ((enemies[i].w + enemyshots[j].w) << 2);
							enemyshots[j].y = (p.y >> 4);
							enemies[i].shotf = 0;
							break;
						}
				}
			}

		int nextsprite = 0;
		SET_SPRITE_PLUS(nextsprite,  10, 10, SPRITE_SIZE(1, 1),
			TILE_ATTR_FULL(PAL3, 0, 0, 1, BARTILS_TB + 2));
		SET_SPRITE_PLUS(nextsprite, 146, 10, SPRITE_SIZE(1, 1),
			TILE_ATTR_FULL(PAL3, 0, 0, 0, BARTILS_TB + 2));
		if (lifepts > 0) {
			for (unsigned i = 0; i < (lifepts >> 1); i++)
				SET_SPRITE_PLUS(nextsprite,  18 + (i << 3), 10, SPRITE_SIZE(1, 1),
					TILE_ATTR_FULL(PAL3, 0, 0, 0, BARTILS_TB));
			if (lifepts & 1)
				SET_SPRITE_PLUS(nextsprite, 18 + ((lifepts >> 1) << 3), 10, SPRITE_SIZE(1, 1),
					TILE_ATTR_FULL(PAL3, 0, 0, 0, BARTILS_TB + 1));
		}

		if (!invincible || ((invincible > 64) && (fcounter & 4)) || ((invincible <= 64) && (fcounter & 8))) {
			unsigned pframe = (((fcounter & 1) ? ((fcounter >> 4) & 1) + 1 : 0) << 4) + PLAYERS_TB;
			if (levelfsm == LEVEL_RESTART)
				pframe = BIGEXPL_TB + ((lvlcnter >> 2) << 4);
			if (levelfsm != LEVEL_RESTARTING)
				SET_SPRITE_PLUS(nextsprite, (char_x >> 4) - PLAYER_W/2, (char_y >> 4) - PLAYER_H/2,
					SPRITE_SIZE(4, 4), TILE_ATTR_FULL(PAL3, 0, 0, 0, pframe));
		}

		// Draw shots
		for (unsigned i = 0; i < MAX_USR_SHOTS; i++)
			if (usershots[i].x > 0 && (fcounter & 1))
				SET_SPRITE_PLUS(nextsprite, usershots[i].x - 8, usershots[i].y - 4, SPRITE_SIZE(2, 1),
					TILE_ATTR_FULL(PAL2, 0, 0, 0, FIRESHT_TB + ((fcounter >> 2) & 1) * 2));

		for (unsigned i = 0; i < MAX_ENM_SHOTS; i++)
			if (enemyshots[i].x > -16 && (fcounter & 1) && enemyshots[i].rend)
				SET_SPRITE_PLUS(nextsprite, enemyshots[i].x - (enemyshots[i].w << 2),
					enemyshots[i].y - (enemyshots[i].h << 2),
					SPRITE_SIZE(enemyshots[i].w, enemyshots[i].h),
					TILE_ATTR_FULL(PAL2, 0, 0, 0, enemyshots[i].rend(fcounter)));

		// Draw all enemies
		for (unsigned i = 0; i < MAX_OS_ENEMIES; i++) {
			if (enemies[i].desc) {
				int x = (enemies[i].x >> 4) - (enemies[i].w << 2);
				int y = (enemies[i].y >> 4) - (enemies[i].h << 2);
				SET_SPRITE_PLUS(nextsprite, x, y,
					SPRITE_SIZE(enemies[i].w, enemies[i].h),
					TILE_ATTR_FULL(PAL1, 0, 0, 0, enemies[i].framecalc(fcounter)));
			}
		}

		// Draw explosions with priority
		for (unsigned i = 0; i < MAX_EXPLOSIONS; i++) {
			switch (explosions[i].type) {
			case 0:  // Small explosion
				SET_SPRITE_PLUS(nextsprite, explosions[i].x - 8, explosions[i].y - 8,
					SPRITE_SIZE(2, 2), TILE_ATTR_FULL(PAL2, 1, 0, 0,
					SMLEXPL_TB + ((explosions[i].frame >> 2) << 2)));
				break;
			case 1:
				SET_SPRITE_PLUS(nextsprite, explosions[i].x - 16, explosions[i].y - 16,
					SPRITE_SIZE(4, 4), TILE_ATTR_FULL(PAL2, 1, 0, 0,
					BIGEXPL_TB + ((explosions[i].frame >> 2) << 4)));
				break;
			case 2:
				SET_SPRITE_PLUS(nextsprite, explosions[i].x - 16, explosions[i].y - 16,
					SPRITE_SIZE(4, 4), TILE_ATTR_FULL(PAL2, 1, 0, 0,
					BIGEXPL_TB + 6*16 + ((explosions[i].frame >> 2) << 4)));
				break;
			}
			if (++explosions[i].frame >= 6*4-1)
				explosions[i].type = 0xff;
		}

		/*SET_SPRITE_PLUS(nextsprite, 68, 90, SPRITE_SIZE(4, 2),
			TILE_ATTR_FULL(PAL1, 0, 0, 0, ENEMYML_TB));

		if (fcounter & 1) {
			SET_SPRITE_PLUS(nextsprite, 100, 90, SPRITE_SIZE(4, 2),
				TILE_ATTR_FULL(PAL2, 0, 0, 0, FLBOOST_TB + (((fcounter >> 3) & 3) << 3)));
		}*/

		if (invincible)
			invincible--;

		VDP_setSpriteLink(nextsprite-1, 0);

		VDP_waitVSync();
		for (unsigned i = 0; i < 4; i++)
			VDP_setPalette(PAL0 + i, workpal[i]);
		VDP_updateSprites(MAX_SPRITES_ONSCREEN, FALSE);

		fcounter++;
	}

	XGM_stopPlay();

	return retcode;
}


