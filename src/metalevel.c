#include <genesis.h>
#include "config.h"
#include "util.h"
#include "level_functions.h"
#include "collision.h"
#include "levels.h"

#include "resources.h"
#include "assets/metalevel/level.h"
#include "assets/metalevel/bg.h"
#include "assets/metalevel/collision.h"
#include "assets/sprites/mainchar24F.h"
#include "assets/sprites/mainchar24M.h"
#include "assets/sprites/face0.h"
#include "assets/sprites/face1.h"
#include "assets/sprites/face2.h"
#include "assets/sprites/stuff.h"
#include "assets/sprites/worker.h"

#define MAX_FALL_SPEED   (1 << 8)
#define MAX_RUN_SPEED    (1 << 4)
#define FALL_SPEED_STEP   3

#define MAINCHAR_W  12
#define MAINCHAR_H  20
#define DISP_MAINCHAR_W  16
#define DISP_MAINCHAR_H  24

#define PLANE_A_TB  (                                     0)  // Level PlaneB tiles ~311
#define PLANE_B_TB  (                   sprites_level_count)  // Level PlaneA tiles ~480
#define MAINCHR_TB  (PLANE_B_TB +          sprites_bg_count)  // Mainchar sprites   ~78
#define WORKERS_TB  (MAINCHR_TB + sprites_mainchar24F_count)  // Worker             ~60
#define ELECTRC_TB  (WORKERS_TB +      sprites_worker_count)  // Electric           ~32
#define DOORVER_TB  (ELECTRC_TB +    sprites_electric_count)  // Door               ~8
#define DOORHOR_TB  (DOORVER_TB +        sprites_door_count)  // DoorH              ~24
#define CARTDOR_TB  (DOORHOR_TB +       sprites_doorh_count)  // Cart               ~32
#define FACE0SP_TB  (CARTDOR_TB +        sprites_cart_count)  // Face0              ~4
#define FACE1SP_TB  (FACE0SP_TB +       sprites_face1_count)  // Face1              ~4
#define TXTREND_TB  (FACE1SP_TB +       sprites_face0_count)  // Reserved for the text rendering! ~120

// Text rendering is 30 8x8 tiles width and 4 lines.
#define LAST_TILE   (TXTREND_TB)   

// Font is aprox 384 tiles, include them here
_Static_assert(LAST_TILE < GAME_TRANS_TILE, "Tile count sanity");

levelForceReload_TEMPLATE(16A, uint16_t, tilemap_level_width, tilemap_level_height, PLAN_A, PAL0, PLANE_A_TB, NULL)
levelVblankWork_TEMPLATE(16A, uint16_t, tilemap_level_width, tilemap_level_height, PLAN_A, PAL0, PLANE_A_TB, NULL)

#define BLOCK_CLEAR(name, x0, x1, y0, y1, flgclr)    \
	void unblock_door_##name(uint8_t *colmap, uint32_t *flags) { \
		*flags &= ~(1 << flgclr);               \
		for (unsigned j = x0; j <= x1; j++)     \
			for (unsigned i = y0; i <= y1; i++) \
				colmap[i * collision_width8 + (j >> 3)] &= ~(1 << (j & 7)); \
	}

BLOCK_CLEAR(1, 83, 86, 20, 23, 0);
BLOCK_CLEAR(2, 79, 85, 42, 45, 1);
BLOCK_CLEAR(cart, 113, 116, 0, 17, 2);
BLOCK_CLEAR(h, 107, 109, 47, 48, 3);

void unblock_door_low(uint8_t *colmap, uint32_t *flags) {
	unblock_door_cart(colmap, flags);
	unblock_door_2(colmap, flags);
}

struct t_dialoge {
	bool right;
	const char *text;
};

struct t_cutscene {
	const struct t_dialoge *diag;
	int actv_elec;
	struct {
		int cpx, cpy, o; // Worker pos + orientation
		int mx, mv;      // Move after dialogue
		int camx, camy;  // Camera pos
	} move;
	struct {
		int minx, maxx, miny, maxy;   // Box + onfloor
	} acq;
	void (*usrcb)(uint8_t *colmap, uint32_t *flags);
};

// There's different sections on this metalevel:
// 0: Intro, walking down to blocked cartridge, and into bomberman level
// 1: Back from bomberman the cardridge is gone, down to the cave level.

const struct {
	uint16_t cx, cy;       // Initial char location.
} setups[] = {
	{180, 204},
	{848, 112},
	{798, 346},
	{146, 432},
};

const struct t_dialoge intro_diag1[] = {
	{TRUE,  "Hey there!\nYou allright?\nYou look\nconfused\nWhat happened\nout there?\n"},
	{FALSE, "Err...\nWTF!\nWhere am I?\nWho are you?\n"},
	{TRUE,  "I could ask the\nsame question!\nI'm Hideki...\nand you are?"},
	{FALSE, "...\n\n...\n\n..."},
	{TRUE,  "Oh I know you!\nOf course!\nYou are a...\n\n...player"},
	{FALSE, "That's right\nI was just..."},
	{TRUE,  "Playing huh?\nWith a massive\nthunderstorm\non top of us\nYou mad?\nWe almost die!"},
	{FALSE, "What?"},
	{TRUE,  "Yeah you\nirresponsible!\nAlmost fried\nthe Megadrive!"},
	{FALSE, "Excuse me?"},
	{TRUE,  "Yeah you\nsurely fried\nsome component!"},
	{FALSE, "Hold on,\ndo you mean...\nthat we are...\ninside the MD?"},
	{TRUE,  "Yeah of course,\nwhere else!?"},
	{FALSE, "That's no good!\nLemme go back!\n"},
	{TRUE,  "You ask me?\nI'm definitely\nnot holding you\nback!"},
	{FALSE, "So how do I\ngo back then?"},
	{TRUE,  "No idea, but\nlet's find out!"},
	{TRUE,  NULL},
};

const struct t_dialoge intro_diag2[] = {
	{TRUE,  "Bad news!\nLooks like we\nhave a dead\npart in the\nmotherboard\n\nWe will need\nto repair it"},
	{FALSE, "Ok...\n\nHow long will\nthat take?"},
	{TRUE,  "Mmh...\nA week?\nMaybe more"},
	{FALSE, "What! That's\nway too long!"},
	{TRUE,  "What's up?\nYou in a hurry?"},
	{FALSE, "Well I have a\nwedding ...\nLike ...\nTomorrow"},
	{TRUE,  "Heh you better\ncancel cause\nyou'll be\nlate to it"},
	{FALSE, "Impossible!\nI'm getting\nmarried!"},
	{TRUE,  "Oh boy, that\ncomplicates\nthings ..."},
	{FALSE, "Any\nalternatives?"},
	{TRUE,  "Well...\nYou can always\nhelp fix it"},
	{FALSE, "I'll do\nanything!"},
	{TRUE,  "Ok I can send\nyou fetch the\nspare parts\nfrom storage"},
	{FALSE, "That I can do!"},
	{TRUE,  "It's pretty\ndark down there"},
	{FALSE, "I'm not afraid!"},
	{TRUE,  "You will be!\nThere's some...\nInteresting\ncreatures"},
	{FALSE, "Creatures?\nWhat kind of\nplace is this?"},
	{TRUE,  "They must have\nescaped some\nold cartridge\nand now live\nin the dungeon"},
	{FALSE, "Well I can't\nbe late for\nthe wedding\nI'll take my\nchances"},
	{TRUE,  "Let's go then!"},
	{TRUE,  NULL},
};

const struct t_dialoge intro_diag3[] = {
	{TRUE,  "Looks like the\nway is blocked\nThe cartridge\nis too tight\nfor us to move"},
	{FALSE, "Damn it\nIs there\nanything that\nwe can do?"},
	{TRUE,  "Well last time\nthis happened\nwe took an\nunorthodox\nsolution..."},
	{FALSE, "What is it?"},
	{TRUE,  "We bombed it!"},
	{FALSE, "What?\nDid you destroy\nthe cart?"},
	{TRUE,  "Oh no! We just\nplay bomberman\nfor a while\n\nSee the cart\ngets loose with\nthe vibrations\nof  explosions"},
	{FALSE, "I can do that!\nHow do we play\nbomberman?"},
	{TRUE,  "Oh easy, just\nfollow me!"},
	{TRUE,  NULL},
};

const struct t_dialoge intro_diag4[] = {
	{TRUE,  "Ready?\nI'm launching\nyou in the\ngame now!"},
	{FALSE, "There we go!"},
	{TRUE,  NULL},
};

const struct t_dialoge postcart_diag1[] = {
	{TRUE,  "Look! It worked!\nBase level is\naccessible now"},
	{FALSE, "So what next?"},
	{TRUE,  "Now... to the\ndungeons!"},
	{TRUE,  NULL},
};

const struct t_dialoge postcart_diag2[] = {
	{TRUE,  "Go ahead!\nI will send you\ndown to the\ndungeons!"},
	{FALSE, "And how do\nI get back?"},
	{TRUE,  "You make sure\nyou grab the new\npart and I will\ntake care of it"},
	{FALSE, "You got it!"},
	{TRUE,  NULL},
};

const struct t_dialoge postcart_diag3[] = {
	{FALSE, "Here we go!"},
	{TRUE,  NULL},
};

const struct t_dialoge postcave_diag1[] = {
	{TRUE,  "Great! Now we\ncan repair the\nconsole so you\ncan get out\n"},
	{FALSE, "Let's do it!"},
	{TRUE,  "However getting\nout might take\nsome time"},
	{FALSE, "But why?"},
	{TRUE,  "We need to wait\nfor the next\nthunderstorm\nWhich can take\na while"},
	{FALSE, "I don't have\nthat much time!\nCan we speed it\nup somehow?"},
	{TRUE,  "I suppose we\ncould use the\nFM modulator\nbut..."},
	{FALSE, "But what?"},
	{TRUE,  "It's on the\nother side of\nthe board\n\nIt can take\nhours!"},
	{FALSE, "Do you have...\n...a car?"},
	{TRUE,  "No, but we've\ngot something\neven better!\nCome!"},
	{TRUE,  NULL},
};

const struct t_dialoge postcave_diag2[] = {
	{TRUE,  "You can use the\nspaceship!\nHowever you'll\nfind resistance\nIt's a game\nright?"},
	{FALSE, "Allright!"},
	{TRUE,  "Get ready!"},
	{TRUE,  NULL},
};

const struct t_dialoge postshooter_diag1[] = {
	{TRUE,  "Great! We have\nall we need\nto repair the\nmegadrive!\n"},
	{FALSE, "Yay!\n\nHopefully I\nwon't be late"},
	{TRUE,  "Allright! We\nare set!\nGo ahead!"},
	{TRUE,  NULL},
};

const struct t_dialoge postshooter_diag2[] = {
	{FALSE, "I guess this is\ngodbye then?"},
	{TRUE,  "Sounds like it\n\nGood luck out\nthere!\nDon't forget to\nplay every now\nand then"},
	{FALSE, "Oh never!\nSee you in the\nnext game then!"},
	{TRUE,  NULL},
};


const struct t_cutscene intro[] = {
	{ NULL,        -1, { 220, 192, 1, 0, 0, 200, 170 }, {   0,9999,   0,9999 }, NULL },
	{ intro_diag1, -1, { 576, 160, 1, 2, 0, 500, 160 }, { 520, 528, 176, 190 }, NULL },
	{ intro_diag2, -1, { 848, 112, 1, 2, 0, 936, 140 }, { 830, 844, 130, 150 }, unblock_door_1 },
	{ intro_diag3, -1, { 576, 160, 1,-2, 0, 500, 160 }, { 520, 528, 176, 190 }, NULL },
	{ intro_diag4,  0 },
	{ NULL },
};

const struct t_cutscene postcart[] = {
	{ NULL,           -1, { 848, 112, 0, 0, 0, 800, 140 }, {   0,9999,   0,9999 }, unblock_door_cart },
	{ postcart_diag1, -1, { 748, 336, 0, 2, 0, 848, 320 }, { 860, 870, 360, 380 }, NULL },
	{ postcart_diag2, -1, {   0,   0, 1,-2, 0, 930, 400 }, {1000,1008, 448, 470 }, unblock_door_h },
	{ postcart_diag3,  1 },
	{ NULL },
};

const struct t_cutscene postdungeon[] = {
	{ NULL,           -1, { 748, 336, 0, 0, 0, 848, 320 }, {   0,9999,   0,9999 }, unblock_door_low },
	{ postcave_diag1, -1, { 454, 312, 0,-2, 0, 536, 344 }, { 526, 534, 400, 430 }, NULL },
	{ postcave_diag2,  1 },
	{ NULL },
};

const struct t_cutscene postshooter[] = {
	{ NULL,              -1, { 176, 416, 1, 0, 0, 160, 400 }, {   0,9999,   0,9999 }, NULL },
	{ postshooter_diag1, -1, { 292, 416, 1, 2, 0, 180, 400 }, { 256, 264, 432, 460 }, NULL },
	{ postshooter_diag2,  2 },
	{ NULL },
};

const struct t_cutscene *scenes[] = {
	intro, postcart, postdungeon, postshooter
};

const struct t_dialoge *diag;
	struct {
		int cpx, cpy;   // Worker pos
		int mx, mv;     // Move after dialogue
	} move;
	struct {
		int minx, maxx, miny, maxy;   // Box + onfloor
	} acq;

static void clear_text() {
	// Will simply wipe all the sprite tiles.
	VDP_fillTileData(0, TXTREND_TB, 120, FALSE);
}

static void render_text(uint8_t *font, unsigned cnum, char c) {
	// Calculate the char position to fill
	unsigned addr = (c - 32) << 7;
	VDP_loadTileData((uint32_t*)&font[addr], TXTREND_TB + cnum * 4, 4, DMA);
}

unsigned metalevel(uint8_t section) {
	Z80_init();
	loadDecompressTiles(tiles_compr_level,     PLANE_A_TB, DMA);
	loadDecompressTiles(tiles_compr_bg,        PLANE_B_TB, DMA);
	loadDecompressTiles(playerchar ? sprites_compr_mainchar24F : sprites_compr_mainchar24M, MAINCHR_TB, DMA);
	loadDecompressTiles(sprites_compr_worker,  WORKERS_TB, DMA);
	loadDecompressTiles(sprites_compr_electric,ELECTRC_TB, DMA);
	loadDecompressTiles(sprites_compr_door,    DOORVER_TB, DMA);
	loadDecompressTiles(sprites_compr_doorh,   DOORHOR_TB, DMA);
	loadDecompressTiles(sprites_compr_cart,    CARTDOR_TB, DMA);
	loadDecompressTiles(sprites_compr_face0,   FACE0SP_TB, DMA);
	loadDecompressTiles(playerchar ? sprites_compr_face2 : sprites_compr_face1, FACE1SP_TB, DMA);
	clear_text();

	DMA_flushQueue();
	VDP_resetSprites();

	uint8_t localfont[400 * 32];
	load_font16(localfont);

	// Character tracking, initialize
	char_w = MAINCHAR_W;
	char_h = MAINCHAR_H;
	char_x = setups[section].cx << 4;
	char_y = setups[section].cy << 4;

	levelSetViewport(scenes[section][0].move.camx, scenes[section][0].move.camy);
	levelForceReload16A(&tilemap_level[0], NULL);

	// Load bg manually, wraps horizontaly only
	for (unsigned x = 0; x < tilemap_bg_width	; x++)
		for (unsigned y = 0; y < tilemap_bg_height; y++)
			VDP_setTileMapXY(PLAN_B, TILE_ATTR_FULL(PAL1, 0, 0, 0,
				PLANE_B_TB + tilemap_bg[x + y*tilemap_bg_width]), x, y);

	// Palette setup here:
	uint16_t refpal[4][16], workpal[4][16];
	memcpy(refpal[0], base_palette, 32);
	memcpy(refpal[1], bg_palette, 32);
	memcpy(refpal[2], worker_palette, 32);
	memcpy(refpal[3], playerchar ? mainchar24F_palette : mainchar24M_palette, 32);
	memset(workpal, 0, sizeof(workpal));

	VDP_setEnable(1);

	XGM_setMusicTempo(50);
	XGM_setLoopNumber(0);
	XGM_startPlay(music_board);

	uint8_t colmap[collision_height][collision_width8];
	memcpy(colmap, collision, sizeof(colmap));

	unsigned charanim = 0, orientation = 0;
	bool prevjump = FALSE;
	bool hold = TRUE;
	const struct t_dialoge * curr_diag = NULL;
	uint32_t text_fsm = 0, scene_fsm = 2, step = 0, fcounter = 0;
	uint16_t wkx = -1024, wky = 0, wfp = 4*12, wor = 0;
	int retcode = -1, active_elec = -1, speak_char = -1;
	uint32_t flags = ~0U;
	unsigned levelfsm = LEVEL_STARTING, lvlcnter = 0;
	while (retcode < 0) {
		// Meta level FSM
		switch (levelfsm) {
		case LEVEL_PLAY:
			lvlcnter = 0;
			break;
		case LEVEL_STARTING:
			// Palette setup here for fadeout
			for (unsigned i = 0; i < 4; i++)
				fade_dark_pal(workpal[i], &refpal[i][0], (lvlcnter >> 2), 3);

			if (lvlcnter++ >= 32)
				levelfsm = LEVEL_PLAY;

			break;
		case LEVEL_SPECIAL_ENDING:
			if (lvlcnter++ >= 60) {
				levelfsm = LEVEL_ENDING_WIN;
				lvlcnter = 0;
			}
			break;
		case LEVEL_ENDING_WIN:
			for (unsigned i = 0; i < 4; i++)
				fade_dark_pal(workpal[i], &refpal[i][0], 8 - (lvlcnter >> 2), 3);

			if (lvlcnter++ >= 32)
				retcode = LEVEL_ACHIEVED;
			break;
		}

		switch (scene_fsm) {
		case 0: // Show dialog
			curr_diag = scenes[section][step].diag;
			break;
		case 1: // Move character out of the screen!
			curr_diag = NULL;
			if (!scenes[section][step+1].diag) {
				// Last dialoge, end here
				active_elec = scenes[section][step].actv_elec;
				if (levelfsm == LEVEL_PLAY)
					levelfsm = LEVEL_SPECIAL_ENDING;
			} else {
				wkx += scenes[section][step].move.mx;
				wor = scenes[section][step].move.mx > 0 ? 0 : 1;
				wfp = ((fcounter >> 3) & 3) * 12;
				int scxoff = vp_x - _SCREEN_WIDTH/2;
				int scoordx = wkx - scxoff;
				if (scoordx < -64 || scoordx > _SCREEN_WIDTH+64)
					scene_fsm++;
				if (wkx == scenes[section][step].move.cpx &&
				    wky == scenes[section][step].move.cpy)
					scene_fsm++;
			}
			break;
		case 2:
			// Position char in next position and unblock mainchar
			wkx = scenes[section][step].move.cpx;
			wky = scenes[section][step].move.cpy;
			wor = scenes[section][step].move.o;
			if (scenes[section][step].usrcb)
				scenes[section][step].usrcb(&colmap[0][0], &flags);
			wfp = 4*12;
			scene_fsm++;
			hold = FALSE;
		case 3: // Wait for char to land in the acquire region.
			if (onfloor &&
			    (char_x >> 4) > scenes[section][step].acq.minx &&
			    (char_x >> 4) < scenes[section][step].acq.maxx &&
			    (char_y >> 4) > scenes[section][step].acq.miny &&
			    (char_y >> 4) < scenes[section][step].acq.maxy) {

				hold = TRUE;
				scene_fsm++;

				text_fsm = 0;
			}
			break;
		case 4:  // Position camera slowly
			if (vp_x < scenes[section][step].move.camx)
				vp_x++;
			if (vp_x > scenes[section][step].move.camx)
				vp_x--;
			if (vp_y < scenes[section][step].move.camy)
				vp_y++;
			if (vp_y > scenes[section][step].move.camy)
				vp_y--;

			if (vp_y == scenes[section][step].move.camy &&
			    vp_x == scenes[section][step].move.camx) {
				step++;
				scene_fsm = 0;
			}
		};

		VDP_setHorizontalScroll(PLAN_B, -((3*vp_x) >> 4));
		VDP_setVerticalScroll(PLAN_B, vp_y >> 3);

		// Process input
		uint16_t k1 = hold ? 0 : JOY_readJoypad(JOY_1);

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
		if (k1 & BUTTON_A)
			if (onfloor && !prevjump)
				char_vspeed = 0x40;
		prevjump = (k1 & BUTTON_A);

		// If the character is jumping, we lock the frame to "jumping"
		unsigned cframe = 7;
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
			collisionGravityProcess(&colmap[0][0], collision_width8);
		}
		char_vspeed -= FALL_SPEED_STEP;
		if (char_vspeed < -MAX_FALL_SPEED)
			char_vspeed = -MAX_FALL_SPEED;
		if (char_hspeed > MAX_RUN_SPEED)
			char_hspeed = MAX_RUN_SPEED;
		else if (char_hspeed < -MAX_RUN_SPEED)
			char_hspeed = -MAX_RUN_SPEED;

		// Check whether viewport needs to be repositioned.
		int scxoff = vp_x - _SCREEN_WIDTH/2;
		int scyoff = vp_y - _SCREEN_HEIGHT/2;

		int charpx = char_x >> 4;
		int charpy = char_y >> 4;
		if (!hold)
			levelSetViewportArea(charpx, charpy, 200, 100);

		unsigned nextsprite = 0;
		// Text has priority!
		if (speak_char >= 0) {
			for (unsigned j = 0; j < 2; j++)
				for (unsigned i = 0; i < 8; i++)
					SET_SPRITE_PLUS(nextsprite, 30 + (speak_char > 0 ? 20 : 0) + 32*i, 190 + 20 * j,
						SPRITE_SIZE(i < 7 ? 4 : 2, 2),
						TILE_ATTR_FULL(PAL1, 1, 0, 0, TXTREND_TB + 60 * j + i*8));

			SET_SPRITE_PLUS(nextsprite, speak_char > 0 ? 10 : 276 , 194,
				SPRITE_SIZE(4, 4), TILE_ATTR_FULL(speak_char > 0 ? PAL2 : PAL3, 0, 0, 0,
				speak_char > 0 ? FACE0SP_TB : FACE1SP_TB));
		}

		const uint16_t elect[3][2] = { {512, 160}, {992, 440}, {248, 416} };
		if (active_elec >= 0 && (fcounter & 1)) {
			SET_SPRITE_PLUS(nextsprite, elect[active_elec][0] - scxoff, elect[active_elec][1] - scyoff,
				SPRITE_SIZE(2, 4), TILE_ATTR_FULL(PAL0, 0, 0, 0, ELECTRC_TB + 8 * ((fcounter >> 3) & 3)));
			SET_SPRITE_PLUS(nextsprite, elect[active_elec][0] - scxoff + 8, elect[active_elec][1] - scyoff,
				SPRITE_SIZE(2, 4), TILE_ATTR_FULL(PAL0, 0, 0, 0, ELECTRC_TB + 8 * ((2 + (fcounter >> 3)) & 3)));
		}

		if (flags & 1) {
			SET_SPRITE_PLUS(nextsprite, 83*8 - scxoff, 20*8 - scyoff,
				SPRITE_SIZE(1, 4), TILE_ATTR_FULL(PAL0, 0, 0, 0, DOORVER_TB));
			SET_SPRITE_PLUS(nextsprite, 86*8 - scxoff, 20*8 - scyoff,
				SPRITE_SIZE(1, 4), TILE_ATTR_FULL(PAL0, 0, 0, 0, DOORVER_TB));
		}
		if (flags & 2) {
			SET_SPRITE_PLUS(nextsprite, 79*8 - scxoff, 42*8 - scyoff,
				SPRITE_SIZE(1, 4), TILE_ATTR_FULL(PAL0, 0, 0, 0, DOORVER_TB));
			SET_SPRITE_PLUS(nextsprite, 85*8 - scxoff, 42*8 - scyoff,
				SPRITE_SIZE(1, 4), TILE_ATTR_FULL(PAL0, 0, 0, 0, DOORVER_TB));
		}
		unsigned hpos = (flags & 4) ? 10*8 : 2*8;
		SET_SPRITE_PLUS(nextsprite, 113*8 - scxoff, hpos - scyoff,
			SPRITE_SIZE(4, 4), TILE_ATTR_FULL(PAL0, 0, 0, 0, CARTDOR_TB));
		SET_SPRITE_PLUS(nextsprite, 113*8 - scxoff, hpos + 4*8 - scyoff,
			SPRITE_SIZE(4, 4), TILE_ATTR_FULL(PAL0, 0, 0, 0, CARTDOR_TB + 16));
		if (flags & 8) {
			SET_SPRITE_PLUS(nextsprite, 107*8 - scxoff, 47*8 - scyoff,
				SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PAL0, 0, 0, 0, DOORHOR_TB));
		}

		SET_SPRITE_PLUS(nextsprite, charpx - scxoff - DISP_MAINCHAR_H/2,
			charpy - scyoff - DISP_MAINCHAR_H/2 - 2, SPRITE_SIZE(3, 3),
			TILE_ATTR_FULL(PAL3, 0, 0, orientation, MAINCHR_TB + 9 * cframe));

		SET_SPRITE_PLUS(nextsprite, wkx - scxoff, wky - scyoff,
			SPRITE_SIZE(3, 4), TILE_ATTR_FULL(PAL2, 0, 0, wor, WORKERS_TB + wfp));
		
		if (curr_diag) {// && !(fcounter & 3)) {
			// Render some text
			unsigned charstate = text_fsm & 0xFF;
			unsigned sentstate = (text_fsm >> 8) & 0xFF;
			unsigned diagpos   = text_fsm >> 16;
			// Search sentence to render
			const char *t = curr_diag[diagpos].text;
			if (!*t)
				scene_fsm++;
			else {
				for (unsigned i = 0; i < sentstate*2; i++) {
					while (*t && *t != '\n')
						t++;
					if (*t && *t == '\n')
						t++;
				}
				if (!(*t))
					text_fsm = (text_fsm + 0x10000) & ~0xFFFF;
				else {
					unsigned cn = 0, p = 0;
					for (unsigned i = 0; i < charstate; i++)
						if (t[i] == '\n') {
							cn += 15;
							p = 0;
						}
						else p++;

					if (cn >= 30 || !t[charstate]) {
						if (JOY_readJoypad(JOY_1) & ANY_BUTTON) {
							clear_text();
							speak_char = -1;
							text_fsm = (text_fsm + 0x100) & ~0xFF;
						}
					}
					else {
						speak_char = curr_diag[diagpos].right ? 1 : 0;
						if (t[charstate] != '\n')
							render_text(localfont, cn + p, t[charstate]);
						text_fsm++;
					}
				}
			}
		}

		VDP_setSpriteLink(nextsprite-1, 0);
		VDP_waitVSync();
		for (unsigned i = 0; i < 4; i++)
			VDP_setPalette(PAL0+i, workpal[i]);
		levelVblankWork16A(&tilemap_level[0], NULL);
		VDP_updateSprites(32, FALSE);     // Max 28 sprites
		fcounter++;
	}

	XGM_stopPlay();

	return retcode;
}

