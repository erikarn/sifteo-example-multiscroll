/*
 * Sifteo SDK Example.
 * Copyright <c> 2013 Sifteo, Inc. All rights reserved.
 */

#include <sifteo.h>
#include "assets.gen.h"

#include "ScrollObj.h"

#define	NUM_CUBES		3

using namespace Sifteo;

static AssetSlot MainSlot = AssetSlot::allocate()
    .bootstrap(GameAssets);

static Metadata M = Metadata()
    .title("BG0 Scroller SDK Example")
    .package("com.sifteo.sdk.scroller-bg0", "1.0")
    .cubeRange(NUM_CUBES);

void main()
{
	ScrollObj sco[NUM_CUBES];

	// Initialise all of the cubes - hardcode for 0..2 for now
	for (unsigned i = 0; i < NUM_CUBES; i++)
		sco[i].init(i);

	// Next, initial rendering
	for (unsigned i = 0; i < NUM_CUBES; i++)
		sco[i].drawPicture();

	// Next, run the basic game logic for each object

	for(;;) {
		for (unsigned i = 0; i < NUM_CUBES; i++)
			sco[i].update();
		System::paint();
	}
}
