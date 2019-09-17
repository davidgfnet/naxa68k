
#ifndef __GAME_CONFIG_FILE_HH__
#define __GAME_CONFIG_FILE_HH__

// Some configuration bits that might be used by several bits of the games.

#define _SCREEN_WIDTH    320
#define _SCREEN_HEIGHT   240

#define GAME_HSCROLL_ADDR   0xB800  // 1KB for hscroll
#define GAME_SPRITES_ADDR   0xBC00  // 1KB for sprites (128 sprites max)
#define GAME_APLANES_ADDR   0xC000  // 8KB for A plane
#define GAME_BPLANES_ADDR   0xE000  // 8KB for B plane

// Define the max number of tiles we can have
// That comes from 64KB of VRAM minus 0xB800
// Transparent tile is defined as 4 transparent tiles to allow for 2x2 blocks
#define GAME_MAX_TILE_BYTES   (GAME_HSCROLL_ADDR)
#define GAME_MAX_TILES        (GAME_MAX_TILE_BYTES / 32)
#define GAME_TRANS_TILE       (GAME_MAX_TILES - 4)

// The game can be player with two chars: male and female, this is the knob
extern int playerchar;

#endif

