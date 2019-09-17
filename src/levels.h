
#ifndef __LEVELS___HH___
#define __LEVELS___HH___

// Levels can return a value:
// 0 means the level was successfully achieved, moves on to the next one!
// 1 means it failed, but can re-attempt it.
// 2 means game over!

#define LEVEL_ACHIEVED    0
#define LEVEL_FAILED      1

unsigned metalevel(uint8_t subst);
unsigned level1();
unsigned level2();
unsigned level3();
unsigned level4();
unsigned menulvl();
void dointro();
void docredits();
void gameover();

#endif

