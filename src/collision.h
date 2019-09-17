// Copyright 2019 David Guillen Fandos (david@davidgf.net)
// This code is released under the GPL2 license.

// Contains all the functions used for tile collision.

#ifndef __COLLISION__H__
#define __COLLISION__H__

#include <genesis.h>
#include <stdbool.h>

extern int char_w, char_h, char_x, char_y, char_vspeed, char_hspeed;
extern bool onfloor;

// Process collision routines and re-position the main character.
void collisionGravityProcess(const uint8_t *collmap, unsigned width);
// Collision of a zelda/pokemon like map.
void collisionMapProcess(const uint8_t *collmap, unsigned width);
void collisionMapProcess16(const uint8_t *collmap, unsigned width);

#endif

