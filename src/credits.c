#include <genesis.h>
#include "config.h"
#include "util.h"
#include "level_functions.h"
#include "decomp.h"

#include "resources.h"

// Credits! Just some text on screen with a background music
// The selected song (VGM only unfortunately) lasts for 1:54

#define FADEIN_SH  4
#define FADEOUT_SH 4
#define DISP_SH    7

enum CreditsFSM { loadFrame, fadeInFrame, DisplayFrame, fadeOutFrame };

struct t_frame {
	// This defines a credit frame, with some text
	const char *text[4];
};

static void fade_dark(const uint16_t *refpal, uint16_t *workpal, unsigned amount, unsigned tm) {
	for (unsigned i = 0; i < 16; i++)
		workpal[i] = colinterpB(refpal[i], amount, tm);
}

void docredits() {
	// Reset to a known state
	VDP_resetSprites();
	Z80_init();

	// Use Plane B only
	VDP_fillTileMap(VDP_PLAN_A, TILE_ATTR_FULL(PAL0, 0, 0, 0, GAME_TRANS_TILE), 0, 64*64);
	VDP_fillTileMap(VDP_PLAN_B, TILE_ATTR_FULL(PAL0, 0, 0, 0, GAME_TRANS_TILE), 0, 64*64);
	VDP_setVerticalScroll(PLAN_B, 0);
	VDP_setVerticalScroll(PLAN_A, 0);
	// Load the characters at line 20-22, also load the tiles at 0
	setup_font16(0);

	const uint16_t bw_pal[16] = {0, 0, 0xeee};
	struct t_frame frames[] = {
		{{"Late", "for the", "Big Day"}},
		{{"", "a.k.a.", "project naxa68k"}},
		{{"can be found at", "github.com", "davidgf/naxa68k"}},
		{{"", "CREDITS"}},
		{{"", "Original idea", "davidgfnet"}},
		{{"Programming", "davidgfnet"}},
		{{"", "Script", "davidgfnet"}},
		{{"Music"}},
		{{"Intro song", "by", "TristEndo"}},
		{{"'Sled Storm:   ", "   Widdows Peak'"}},
		{{"Arcade area music", "by pedipanol", "wtf it really works"}},
		{{"Bomb arena track", "by kgb525", "\"Sundown Station\""}},
		{{"Shoot'em up level", "soundtrack by", "LukeMcQueen"}},
		{{"Hyperspace   ", "   Adventurer"}},
		{{"Boss music by sc00p", "Battle of         ", "Kenshin Stronghold"}},
		{{"Dungeon song", "by passarbye", "\"Sprigget\""}},
		{{"Credits song", "by Jredd", "\"Coda-Cola Zone\""}},
		{{"Art"}},
		{{"Greetings to", "itch.io artits"}},
		{{"0x72", "elthen", "ansimuz"}},
		{{"laredgames", "subliminalman"}},
		{{"Others"}},
		{{"Stef and the", "SGDK community"}},
		{{"", "Mike Pavone", "for Blast'em"}},
		{{"The end!"}},
		{{"Congrats!"}},
	};
	#define frame_cnt (sizeof(frames)/sizeof(frames[0]))

	VDP_setEnable(1);

	XGM_setMusicTempo(50);
	XGM_setLoopNumber(0);
	XGM_startPlay(music_credits);

	enum CreditsFSM fsm = loadFrame;
	unsigned currframe = 0, vsynccnt = 0;
	uint16_t workpal[16] = {0};
	while (true) {
		switch (fsm) {
		case loadFrame:
			VDP_setEnable(0);
			VDP_fillTileMap(VDP_PLAN_A, TILE_ATTR_FULL(PAL0, 0, 0, 0, GAME_TRANS_TILE), 0, 64*64);

			for (unsigned i = 0; i < 3; i++) {
				unsigned pos = (20 - strlen(frames[currframe].text[i]) + 1) >> 1;
				print_font16(frames[currframe].text[i], PLAN_A, PAL0, 0, pos << 1, 11+i*3);
			}

			vsynccnt = 0;
			fsm = fadeInFrame;
			VDP_setEnable(1);
			break;
		case fadeInFrame:
			// Calculate the next palette transition.
			fade_dark(bw_pal, &workpal[0], vsynccnt, FADEIN_SH);
			VDP_waitVSync();
			if (vsynccnt++ >= (1 << FADEIN_SH)) {
				fsm = DisplayFrame;
				vsynccnt = 0;
			}
			break;
		case DisplayFrame: {
			while (vsynccnt++ < 184)
				VDP_waitVSync();

			fsm = fadeOutFrame;
			vsynccnt = 1 << FADEOUT_SH;
			} break;
		case fadeOutFrame:
			// Calculate the next palette transition.
			fade_dark(bw_pal, &workpal[0], vsynccnt, FADEOUT_SH);
			VDP_waitVSync();
			// Make PlaneA and B all transparent using scroll
			if (vsynccnt-- <= 0) {
				fsm = loadFrame;
				vsynccnt = 0;
				currframe++;
			}
			break;
		};

		VDP_setPalette(PAL0, workpal);

		if (currframe >= sizeof(frames)/sizeof(frames[0]))
			break;
	}

	// Wait for music to end...
	while (XGM_isPlaying());

	XGM_stopPlay();
}


