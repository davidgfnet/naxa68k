#!/bin/bash

set -e

# This script uses the golden image sets and golden sources for levels to
# (re-)generate the include files used by the game to actually load art.
# The script is somewhat manual and might require some manual steps to
# be re-run should the art change significantly (for instance palettes
# can exceed their limits and break boundaries in some cases!).

# Sprites
mkdir -p src/assets/sprites/

# Main char 16 pixels from top view
types="M F"
declare -A chars
chars=([16]=16 [24]=8 ["C24"]=16)
for t in $types; do
	for s in "${!chars[@]}"; do
		echo "Processing char type $t size $s and numsp ${chars[$s]}"
		palette=`./tools/palette.py art/golden/mainchar/mainchar${s}${t}.png`
		./tools/spriter.py --comprsprites --nsprites ${chars[$s]} --outfile src/assets/sprites/mainchar${s}${t}.h --palette mainchar${s}${t}:$palette art/golden/mainchar/mainchar${s}${t}.png
	done
done
palette=`./tools/palette.py art/golden/mainchar/worker.png`
./tools/spriter.py --comprsprites --nsprites 5 --outfile src/assets/sprites/worker.h --palette worker:$palette art/golden/mainchar/worker.png
palette=`./tools/palette.py art/golden/mainchar/worker.png`
./tools/spriter.py --comprsprites --outfile src/assets/sprites/face0.h --palette $palette art/golden/mainchar/face0.png
palette=`./tools/palette.py art/golden/mainchar/mainchar24M.png`
./tools/spriter.py --comprsprites --outfile src/assets/sprites/face1.h --palette $palette art/golden/mainchar/face1.png
palette=`./tools/palette.py art/golden/mainchar/mainchar24F.png`
./tools/spriter.py --comprsprites --outfile src/assets/sprites/face2.h --palette $palette art/golden/mainchar/face2.png

# Fonts
./tools/spriter.py --comprsprites --outfile src/assets/sprites/font16x16.h --palette font:eee,000 art/golden/font16x16.png

# Some stuff used by most levels
palette=`./tools/palette.py art/golden/mainchar/portal.png`
./tools/spriter.py --outfile src/assets/sprites/portal.h art/golden/mainchar/portal.png --comprsprites --nsprites=22 --palette portal:$palette

# Menu
mkdir -p src/assets/menu/
./tools/tiler.py --outfile src/assets/menu/tv_planeb.h art/golden/menu/tv_planeb.png --palname tv_planeb --compress
./tools/tiler.py --outfile src/assets/menu/tv_planea1.h art/golden/menu/tv_planea1.png --palname tv_planea1 --u8tilemap
./tools/tiler.py --outfile src/assets/menu/tv_planea2.h art/golden/menu/tv_planea2.png --palname tv_planea2 --u8tilemap
./tools/tiler.py --outfile src/assets/menu/tv_planea3.h art/golden/menu/tv_planea3.png --palname tv_planea3 --u8tilemap
palette=`./tools/palette.py art/golden/menu/pressstart.png`
./tools/spriter.py --outfile src/assets/menu/menu.h art/golden/menu/{titles,pressstart}.png --palette ps:$palette
palette=`./tools/palette.py art/golden/menu/chars.png`
./tools/spriter.py --outfile src/assets/menu/chars.h art/golden/menu/chars.png --palette char:$palette
./tools/tiler.py --outfile src/assets/menu/gameover.h art/golden/menu/gameover.png --palname gameover --compress

# Intro animations and all that stuff, use a loop to generate all assets.
mkdir -p src/assets/intro/
for n in `seq 1 6`; do
	if [ ! -f src/assets/intro/scene_map0$n.h ]; then
		./tools/tiler.py --compress --notilemap --outfile src/assets/intro/scene_map0$n.h \
			art/golden/intro/scene_map0$n.png --palname scene_map0$n
	fi
done
for n in `seq 1 4`; do
	if [ ! -f src/assets/intro/scene0$n.h ]; then
		./tools/tiler.py --compress --notilemap --outfile src/assets/intro/scene0$n.h \
			art/golden/intro/scene0$n.png --palname scene0$n
	fi
done
./tools/tiler.py --compress --notilemap --outfile src/assets/intro/scene01C.h art/golden/intro/scene01C.png --palname scene01C
./tools/tiler.py --compress --notilemap --outfile src/assets/intro/scene04B.h art/golden/intro/scene04B.png --palname scene04B

# Meta board level
mkdir -p src/assets/metalevel/
palette=`./tools/palette.py art/golden/board/bg.png`
./tools/tiler.py art/golden/board/bg.png --palette "0,eee,$palette" --compress --outfile src/assets/metalevel/bg.h --palname bg
./tools/leveltool.py --basename board --level art/resources/metalevel.tmx --outfile src/assets/metalevel/collision.h
palette=`./tools/palette.py art/golden/board/{level,electric,door,doorh,cart}.png`
./tools/tiler.py art/golden/board/level.png --compress --outfile src/assets/metalevel/level.h --palname level --palette $palette
./tools/spriter.py --comprsprites --outfile src/assets/sprites/stuff.h --palette base:$palette art/golden/mainchar/bubble.png art/golden/board/{electric,door,doorh,cart}.png

# Level 1: arcade-like
mkdir -p src/assets/level1/
palette=`./tools/palette.py art/golden/arcade/bg.png`
./tools/spriter.py --comprsprites --outfile src/assets/level1/bg.h art/golden/arcade/bg.png --palette bg:$palette
palette=`./tools/palette.py art/golden/arcade/enemies.png`
./tools/spriter.py --comprsprites --outfile src/assets/level1/enemies.h art/golden/arcade/enemies.png --palette enemies:$palette
palette=`./tools/palette.py art/golden/mainchar/portal.png`
./tools/spriter.py --comprsprites --outfile src/assets/sprites/portal.h art/golden/mainchar/portal.png --palette portal:$palette --nsprites=44
./tools/tiler.py art/golden/arcade/level.png --outfile src/assets/level1/level.h --palname level --u8tilemap --compress
./tools/levels/level1.py --level art/resources/level1.tmx --outfile src/assets/level1/aux.h

# Level 2: Bomberman clone
mkdir -p src/assets/level2/

# Level Image to TMX format (do not use but edit TMX directly)
# ./tools/tmxtiler.py --image art/golden/bomberman/level.png --outfile art/resources/level2_bomberman
# Convert TMX planes and collision to a resource
./tools/leveltool.py --basename level2 --level art/resources/level2_bomberman.tmx --outfile src/assets/level2/level.h
./tools/levels/level2.py --level art/resources/level2_bomberman_aux.tmx --outfile src/assets/level2/aux.h

# Plane A sprites 
palette=`./tools/palette.py art/golden/bomberman/destroyable.png art/golden/bomberman/key_anim.png`
./tools/spriter.py --comprsprites --outfile src/assets/level2/planea.h art/golden/bomberman/destroyable.png art/golden/bomberman/key_anim.png --palette planea:$palette

# Some misc sprites
palette=`./tools/palette.py art/golden/bomberman/level.png`
./tools/spriter.py --comprsprites --outfile src/assets/level2/door_anim.h art/golden/bomberman/door_anim.png --palette $palette
./tools/spriter.py --comprsprites --outfile src/assets/level2/bomb.h art/golden/bomberman/bomb.png art/golden/bomberman/explosion.png --palette $palette

# Enemy sprites
palette=`./tools/palette.py art/golden/bomberman/enemy_*png`
./tools/spriter.py --comprsprites --outfile src/assets/level2/enemies.h art/golden/bomberman/enemy_*png --palette enemies:$palette
./tools/spriter.py --comprsprites --outfile src/assets/level2/flame_anim.h art/golden/bomberman/flame_anim.png --palette $palette
./tools/spriter.py --comprsprites --outfile src/assets/level2/flames.h art/golden/bomberman/flames.png --nsprites=8 --palette $palette
./tools/spriter.py --comprsprites  --outfile src/assets/level2/heart.h art/golden/bomberman/heart.png --palette $palette
./tools/spriter.py --comprsprites  --outfile src/assets/level2/collectable.h art/golden/bomberman/collectable.png --palette $palette


# Level 3: Shooter
mkdir -p src/assets/level3/
# Could not fit in game :(
# ./tools/tiler.py art/golden/shooter/cave_{a,b}.png --outfile src/assets/level3/cave.h --palname cave --compress
# Landscape is compressed a bit more :)
palette=`./tools/palette.py art/golden/shooter/landscape_*.png`
./tools/tiler.py art/golden/shooter/landscape_a.png --outfile src/assets/level3/landscape_a.h --palname landscape_a --compress --u8tilemap --palette $palette
./tools/tiler.py art/golden/shooter/landscape_b.png --outfile src/assets/level3/landscape_b.h --palname landscape_b --compress --u8tilemap --palette $palette

palette=`./tools/palette.py art/golden/shooter/player.png`
./tools/spriter.py --comprsprites --outfile src/assets/level3/player.h art/golden/shooter/{player,bar}.png --palette player:$palette

# Enemy sprites
palette=`./tools/palette.py art/golden/shooter/enemy_*png`
./tools/spriter.py --comprsprites --outfile src/assets/level3/enemies.h art/golden/shooter/enemy_*png --palette enemies:$palette

# Effects (weapons and explosions)
palette=`./tools/palette.py art/golden/shooter/{explosion,flame_boost,fireweapon_16x24,fireball,shot,smallexplosion}.png`
./tools/spriter.py --comprsprites --outfile src/assets/level3/effects.h art/golden/shooter/{explosion,flame_boost,fireweapon_16x24,fireball,shot,smallexplosion}.png --palette effects:$palette

# Level 4: dungeon
# Trick for fountain: rotate three colours, one of them being 80c0c0 at a time,
# while the other two are 4080a0
mkdir -p src/assets/level4/

# Level for each cave (with high and low prio planes: lowB and highA)
# Also output the aux map (which includes collision and whatnot)
# Generate a single sprite set (max 256 tiles) for all caves, fit in u8
palette=`./tools/palette.py art/golden/dungeon/cave*png`
./tools/tiler.py --compress --u8tilemap --outfile src/assets/level4/caveL.h --palname planeb --palette $palette art/golden/dungeon/cave*L.png
# High planes have very little info, pack them even more
./tools/tiler.py --compress --u8tilemap --comptilem --outfile src/assets/level4/caveH.h --palname planebH --palette $palette  art/golden/dungeon/cave*H.png
for n in `seq 1 3`; do
	./tools/levels/level4.py --level art/resources/level4_cave${n}.tmx \
		--outfile src/assets/level4/cave${n}_aux.h --basenum $n
done
./tools/spriter.py --comprsprites --outfile src/assets/level4/door.h --palette $palette art/golden/dungeon/door.png

palette=`./tools/palette.py art/golden/dungeon/{trap,collectable}.png`
# Traps use the items palette also
./tools/spriter.py --comprsprites --outfile src/assets/level4/collectable.h --palette collectable:$palette art/golden/dungeon/{trap,collectable}.png

# Enemies
palette=`./tools/palette.py art/golden/dungeon/{weapon,enemies,heart}.png`
./tools/spriter.py --comprsprites --outfile src/assets/level4/enemy.h --palette enemy:$palette art/golden/dungeon/{weapon,enemies,keys,heart}.png

