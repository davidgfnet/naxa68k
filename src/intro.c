#include <genesis.h>
#include "config.h"
#include "util.h"
#include "decomp.h"

#include "assets/intro/scene_map01.h"
#include "assets/intro/scene_map02.h"
#include "assets/intro/scene_map03.h"
#include "assets/intro/scene_map04.h"
#include "assets/intro/scene_map05.h"
#include "assets/intro/scene_map06.h"
#include "assets/intro/scene01.h"
#include "assets/intro/scene02.h"
#include "assets/intro/scene03.h"
#include "assets/intro/scene04.h"
#include "assets/intro/scene01C.h"
#include "assets/intro/scene04B.h"
#include "resources.h"

// Intro to the game, just renders picstures and text with
// some background music. Pictures take so much space that
// need to be loaded between frames (fade in/out dark).

struct t_frame;
typedef	void (*t_fadefn)(uint16_t*, struct t_frame*, unsigned, unsigned);
enum IntroFSM { loadFrame, fadeInFrame, DisplayFrame, fadeOutFrame };

struct t_frame {
	// This defines an image
	const uint8_t *sprites;
	const uint16_t *pal[2];
	unsigned fin, disp, fout;
	// This defines a text (max 32 chars per line)
	const char *text[3];
	// Fade in and out functions
	t_fadefn fincb, foutcb;
};

void fade_dark(uint16_t *workpal, struct t_frame *f, unsigned amount, unsigned tm) {
	for (unsigned j = 0; j < 2; j++)
		for (unsigned i = 0; i < 16; i++)
			if (f->pal[j])
				workpal[(j<<4) + i] = colinterpB(f->pal[j][i], amount, tm);
}

void fade_bright(uint16_t *workpal, struct t_frame *f, unsigned amount, unsigned tm) {
	for (unsigned j = 0; j < 2; j++)
		for (unsigned i = 0; i < 16; i++)
			if (f->pal[j])
				workpal[(j<<4) + i] = colinterpW(f->pal[j][i], amount, tm);
}

void fade_bright_2(uint16_t *workpal, struct t_frame *f, unsigned amount, unsigned tm) {
	for (unsigned j = 0; j < 2; j++)
		for (unsigned i = 0; i < 16; i++)
			if (f->pal[j])
				workpal[(j<<4) + i] = colinterpW(f->pal[j][i], (amount >> 1), tm);
}

void load_char_tile() {
	struct t_decomp_state st;
	decomp_init(&st);
	uint8_t buf[2048];
	decompress(tiles_compr_scene01C, &st, buf, sizeof(buf));

	// Place it after tiles 40*30
	VDP_loadTileData((uint32_t*)buf, 30*40, 6*5*2, CPU);
	// Also set up plane A to show it (160, 132)
	unsigned btile = 30*40 + (playerchar ? 6*5 : 0);
	for (unsigned x = 0; x < 6; x++)
		for (unsigned y = 0; y < 5; y++)
			VDP_setTileMapXY(PLAN_A, TILE_ATTR_FULL(PAL1, 0, 0, 0, btile + y + x*5), x+20, y+17);
}

void load_meow() {
	struct t_decomp_state st;
	decomp_init(&st);
	uint8_t buf[3100];
	decompress(tiles_compr_scene04B, &st, buf, sizeof(buf));

	// Place it after tiles 40*30
	VDP_loadTileData((uint32_t*)buf, 30*40, 12*8, CPU);
	// Also set up plane A to show it (160, 132)
	for (unsigned x = 0; x < 12; x++)
		for (unsigned y = 0; y < 8; y++)
			VDP_setTileMapXY(PLAN_A, TILE_ATTR_FULL(PAL1, 0, 0, 0, 30*40 + y + x*8), x+26, y+9);
	VDP_setVerticalScroll(PLAN_A, 256);
}


void dointro() {
	Z80_init();
	// Reset to a known state
	VDP_resetSprites();
	VDP_updateSprites(16, FALSE);

	// Use Plane B only
	VDP_fillTileMap(VDP_PLAN_A, TILE_ATTR_FULL(PAL0, 0, 0, 0, GAME_TRANS_TILE), 0, 64*64);
	VDP_fillTileMap(VDP_PLAN_B, TILE_ATTR_FULL(PAL0, 0, 0, 0, GAME_TRANS_TILE), 0, 64*64);
	VDP_setVerticalScroll(PLAN_B, 0);
	VDP_setVerticalScroll(PLAN_A, 0);
	for (unsigned x = 0; x < 40; x++)
		for (unsigned y = 0; y < 30; y++)
			VDP_setTileMapXY(PLAN_B, TILE_ATTR_FULL(PAL0, 0, 0, 0, y + x*30), x, y);

	const uint16_t bw_pal[16] = {0, 0, 0xeee};
	const uint16_t allw_pal[16] = {0xeee, 0xeee, 0xeee};
	struct t_frame frames[] = {
		{ NULL, {bw_pal, bw_pal}, 2, 6, 2},
		{ NULL, {bw_pal, bw_pal}, 4, 7, 4,
		 {"    Just another    ",
          "       day at       ",
          "   Weingartenrain   "}},
		{ tiles_compr_scene_map01, {scene_map01_palette, 0}, 4, 6, 3 },
		{ tiles_compr_scene_map02, {scene_map02_palette, 0}, 4, 6, 3 },
		{ tiles_compr_scene_map03, {scene_map03_palette, 0}, 4, 6, 3 },
		{ tiles_compr_scene_map04, {scene_map04_palette, 0}, 4, 6, 3 },
		{ tiles_compr_scene_map05, {scene_map05_palette, 0}, 4, 6, 3 },
		{ tiles_compr_scene_map06, {scene_map06_palette, 0}, 4, 6, 3 },
		{ NULL, {bw_pal, bw_pal}, 2, 3, 2 },
		{ NULL, {bw_pal, bw_pal}, 4, 8, 4,
		 {"   But something    ",
          "    is about to     ",
		  "      happen       "}},
		{ NULL, {bw_pal, bw_pal}, 2, 3, 2 },
		{ NULL, {bw_pal, bw_pal}, 4, 8, 4,
		 {"  they just don't   ",
		  "    know it yet     "}},
		{ tiles_compr_scene01, {scene01_palette, scene01C_palette}, 4, 8, 3 },
		{ NULL, {bw_pal, bw_pal}, 4, 7, 4,
		 {"  a Megadrive game  "}},
		{ NULL, {bw_pal, bw_pal}, 4, 8, 4,
		 {"   might ruin the   ",
		  "   long awaited     ",
		  "      wedding       "}},
		{ tiles_compr_scene02, {scene02_palette, 0}, 4, 8, 3 },
		{ NULL, {bw_pal, bw_pal}, 4, 7, 4,
		 {"   the Megadrive    ",
		  "  was not working   "}},
		{ NULL, {bw_pal, bw_pal}, 4, 7, 4,
		 {"   Rebooting will   ",
		  "     probably do    "}},
		{ NULL, {bw_pal, bw_pal}, 4, 7, 4,
		 {"     we are top     ",
		  "     engineers      ",
		  "       Right?       "}},
		{ tiles_compr_scene03, {scene03_palette, 0}, 4, 5, 2, .foutcb = fade_bright_2},
		{ tiles_compr_scene03, {scene03_palette, 0}, 2, 1, 2, .fincb = fade_bright_2, .foutcb = fade_bright_2},
		{ tiles_compr_scene03, {scene03_palette, 0}, 2, 1, 4, .fincb = fade_bright_2, .foutcb = fade_bright},
		{ NULL, {allw_pal, allw_pal}, 1, 3, 1 },
		{ tiles_compr_scene04, {scene04_palette, scene04B_palette}, 4, 9, 3, .fincb = fade_bright},
		{ NULL, {bw_pal, bw_pal}, 4, 7, 4,
		 {"   This looks bad   "}},
		{ NULL, {bw_pal, bw_pal}, 4, 7, 4,
		 {" You ready for the  ",
		  "    challenge?      "}},
	};
	#define frame_cnt (sizeof(frames)/sizeof(frames[0]))

	VDP_setEnable(1);

	XGM_setMusicTempo(60);
	XGM_setLoopNumber(0);
	XGM_startPlay(music_intro);

	// We will just use the stack to temporarily place images
	// while decompressing them, since there's not enough VRAM!
	uint8_t tempb[30*40*32 + 64];

	enum IntroFSM fsm = loadFrame;
	unsigned currframe = 0, vsynccnt = 0;
	uint16_t workpal[2][16] = {0};
	while (true) {
		if (JOY_readJoypad(JOY_1) & BUTTON_START) {
			XGM_stopPlay();   // Ability to skip intro
			break;
		}

		switch (fsm) {
		case loadFrame:
			VDP_setEnable(0);
			VDP_fillTileMap(VDP_PLAN_A, TILE_ATTR_FULL(PAL1, 0, 0, 0, GAME_TRANS_TILE), 0, 64*64);

			// Load the characters at line 20-22, also load the tiles at 0
			setup_font16(0);
			for (unsigned i = 0; i < 3; i++)
				print_font16(frames[currframe].text[i], PLAN_A, PAL1, 0, 0, 20+i*3);

			VDP_setVerticalScroll(PLAN_B, 256);
			if (frames[currframe].sprites) {
				// Load all tiles (FIXME how does this work with sound?)
				VDP_loadTileData((uint32_t*)tempb, 0, 30*40, CPU);
				VDP_setVerticalScroll(PLAN_B, 0);
			}

			// FIXME: Make this more generic
			if (currframe == 12)
				load_char_tile();
			if (currframe == 23)
				load_meow();

			vsynccnt = 0;
			fsm = fadeInFrame;
			VDP_setEnable(1);
			break;
		case fadeInFrame: {
			// Calculate the next palette transition.
			t_fadefn ffn = frames[currframe].fincb ? frames[currframe].fincb : fade_dark;
			ffn(&workpal[0][0], &frames[currframe], vsynccnt, frames[currframe].fin);
			VDP_waitVSync();
			if (vsynccnt++ >= (1 << frames[currframe].fin)) {
				fsm = DisplayFrame;
				vsynccnt = 0;
			}
			} break;
		case DisplayFrame: {
			// Do nothing but checking when do we jump next frame
			unsigned starttick = getTick();
			unsigned nextf = currframe+1;
			if (nextf < frame_cnt && frames[nextf].sprites) {
				// Skip this in case it's the same image!
				if (frames[currframe].sprites != frames[nextf].sprites) {
					struct t_decomp_state st;
					decomp_init(&st);
					decompress(frames[nextf].sprites, &st, tempb, sizeof(tempb));
				}
			}
			// Calculate how many frames we think elapsed
			unsigned elpasedframes = ((getTick() - starttick) >> 8) * (IS_PALSYSTEM ? 50 : 60);
			vsynccnt = elpasedframes;

			while (vsynccnt++ < (1 << (frames[currframe].disp-1)))
				VDP_waitVSync();
			VDP_setVerticalScroll(PLAN_A, 0);
			while (vsynccnt++ < (1 << frames[currframe].disp))
				VDP_waitVSync();

			fsm = fadeOutFrame;
			vsynccnt = 1 << frames[currframe].fout;
			} break;
		case fadeOutFrame: {
			// Calculate the next palette transition.
			t_fadefn ffn = frames[currframe].foutcb ? frames[currframe].foutcb : fade_dark;
			ffn(&workpal[0][0], &frames[currframe], vsynccnt, frames[currframe].fout);
			VDP_waitVSync();
			// Make PlaneA and B all transparent using scroll
			if (vsynccnt-- <= 0) {
				fsm = loadFrame;
				vsynccnt = 0;
				currframe++;
			}
			} break;
		};

		VDP_setPalette(PAL0, workpal[0]);
		VDP_setPalette(PAL1, workpal[1]);

		if (currframe >= sizeof(frames)/sizeof(frames[0]))
			break;
	}

	// Wait for music to end...
	while (XGM_isPlaying());

	XGM_stopPlay();
}


