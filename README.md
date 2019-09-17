
Project naxa68k
===============

aka Late for the Big Day

Contents of the repo
--------------------

 * art: Where the beauty lies. Check the credits for licenses please.
   - golden: Contains images and their XCF Gimp goldens.
   - music: Deflemask tracks, both originals and my remixes.
   - resources: Mostly TMX (Tiled) levels, partially autogenerated and hand tweaked.
   - sound: SXFR parameter files, for the original SFXR (http://drpetter.se/project_sfxr.html) 
 * cart-pcb: Contains the cartridge design (for Eagle CAD) and Gerber files
 * cart-pcb/art: Art & Cover for the cartridge and cart's case.
 * res: Resources for SGDK, contains SFX and Music in VGM/WAV format (later autogenerated into resources.o).
 * src: Source code, mainly .c and .h stuff. assets subdir contains autogenerated art that gets built in.
 * tools: Python code to convert levels/images into game-readable stuff (autogenerates assets).


(Re)-generating assets
----------------------

I checked in all the assets (usually .h files with huge arrays under src/assets) but if you modify
any image you might wanna regenerate (and rebuild) the project for it to get updated. To do that just
run ./resgen.sh. You will need pypy3 (and probably python3 too) and a bunch of libs/deps which might not
be trivial to get, you've been warned! The reason to use pypy3 is that my compressor code takes 50x time
to run if cPython interpreter is used :) I'd probably code that in C++ now but whatever...

Building this
-------------

If you are brave enough of course! I used Linux and SGDK 1.34. In particular I used this repo:
https://github.com/kubilus1/gendev which at the time, was at 1.34 (now seems to be updated to 1.40).
Once that is installed and ready to be used, one just needs to invoke the provided Makefile, which
will build and generate a binary image that can be used in emulators or burned to a cart.

`make -f $GENDEV/sgdk/mkfiles/makefile.gen all`

Where GENDEV points to the SDK directory, usually /opt/gendev

Known bugs
----------

There's so many :) I might release a v1.1 to fix some of the most annoying ones, cause it's a bit of a pain
to play sometimes. There's also some other bugs that are less obvious, in particular mismatches between
game behaviour and what I intended the game to be.

 - Ghosts in the first level should move in a harmonic fashion in the long corridor, otherwise it's too hard.
 - Bomberman game has some invisible collision block, no idea why, but seems to happen consistenly?
 - The dungeon level needs door anims and better UI to signal object collection, it's confusing as it is.
 - Lack of overall life count. Perhaps the arcade level should have its own life counter or infinite tries.
 - A way to jump to credits would be nice.
 - The lack of password restore a game, not a big deal tho due to emulation savestates I guess.


