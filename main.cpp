/*
 * Multi-scroll example application.
 *
 * This is based off of a Sifteo SDK example which implements
 * scrolling on a single cube.
 *
 * Adrian Chadd <adrian@freebsd.org>
 */

/*
 * Sifteo SDK Example.
 * Copyright <c> 2013 Sifteo, Inc. All rights reserved.
 */

#include <sifteo.h>
#include "assets.gen.h"

#include "ScrollObj.h"

using namespace Sifteo;

static AssetSlot gMainSlot = AssetSlot::allocate()
    .bootstrap(GameAssets);

static Metadata M = Metadata()
    .title("BG0 Scroller SDK Example")
    .package("com.sifteo.sdk.scroller-bg0", "1.0")
    .cubeRange(1, CUBE_ALLOCATION);

static AssetLoader loader; // global asset loader (each cube will have symmetric assets)
static AssetConfiguration<1> config; // global asset configuration (will just hold the bootstrap group)

// This is the set of actively running cubes
static CubeSet activeCubes;
// This is the set of attached or reattaching cubes that
// need loading
static CubeSet newCubes;

// This is the per-cube application state
ScrollObj sco[CUBE_ALLOCATION];

static void
onCubeRefresh(void *ctxt, unsigned cid)
{
	// XXX TODO: if we get a paint event for a currently
	// loading cube, let's call the redraw method for _that_
	// rather than the app redraw.
	if (! activeCubes.test(cid))
		return;

	sco[cid].drawPicture();
}

static void
onCubeAccelChanged(void *ctxt, unsigned cid)
{
	if (! activeCubes.test(cid))
		return;

	sco[cid].update();
}


static void
onCubeConnect(void* ctxt, unsigned cid)
{
#if 0
    // this cube is either new or reconnected
    if (lostCubes.test(cid)) {
        // this is a reconnected cube since it was already lost this paint()
        lostCubes.clear(cid);
        reconnectedCubes.mark(cid);
    } else {
        // this is a brand-spanking new cube
        newCubes.mark(cid);
    }
#endif
#if 0
    dirtyCubes.mark(cid);
#endif

	// Mark this cube as new so we reinitialise state
	newCubes.mark(cid);

	// Initialise the video setup for this cube
	sco[cid].init(cid);

	// begin showing some loading art (have to use BG0ROM since we don't have assets)
	sco[cid].displayAttachPicture();
}

static void
onCubeDisconnect(void *ctxt, unsigned cid)
{
	// Here's where we'd shut down the scrollobj state
	activeCubes.clear(cid);
	newCubes.clear(cid);
}

static void
startCubeRunning(unsigned cid)
{

	newCubes.clear(cid);
	activeCubes.mark(cid);
	sco[cid].pictureAttach();
	sco[cid].drawPicture();
}

void main()
{
	unsigned loader_is_running = 0;

	// initialize asset configuration and loader
	config.append(gMainSlot, GameAssets);
	loader.init();

	// Initialise cubesets
	activeCubes.clear();
	newCubes.clear();

	// Initialise all of the cubes - hardcode for 0..2 for now
	for (CubeID cid : CubeSet::connected()) {
		activeCubes.mark(cid);
		sco[cid].init(cid);
	}

	// Next, register event handlers
	Events::cubeRefresh.set(onCubeRefresh);
	Events::cubeAccelChange.set(onCubeAccelChanged);
	Events::cubeConnect.set(onCubeConnect);
	Events::cubeDisconnect.set(onCubeDisconnect);

	// Next, initial setup and display rendering
	for (CubeID cid : CubeSet::connected()) {
		sco[cid].pictureAttach();
		sco[cid].drawPicture();
	}

	// And now, we just post events
	for(;;) {
		// Update events
		System::paint();

		// If any cubes are new, we need to poke the loader
		if (! newCubes.empty() && (! loader_is_running)) {
			loader_is_running = 1;
			loader.start(config);
		}

		// If the loader is active, update the loading bargraphs
		// on loader cubes.
		if (loader_is_running && !loader.isComplete()) {
			for (CubeID cid : newCubes) {
				unsigned p;
				p = loader.cubeProgress(cid, 128);
				// If we're finish, transition to "run"
				// 0 means 'not started, or don't need assets',
				// so we shouldn't really just trust '0' as
				// "right".
				if (p == 128) {
					startCubeRunning(cid);
					continue;
				}
				// By doing this, we seem to instantly
				// transition the cube to running, even if
				// it's in the middle of loading things.
				// If we check isComplete(cid) then things
				// also don't work reliably.
				//
				// I'll have to poke the Sifteo team to see
				// what's going on under the hood here.
				if (p == 0) {
					startCubeRunning(cid);
					continue;
				}

				// Ok, update the graph here
				sco[cid].displayLoadGraph(p);
			}
		}

		// If the loader is active but no cubes are new,
		// wrap up.
		if (loader_is_running && newCubes.empty() && loader.isComplete()) {
			loader.finish();
			loader_is_running = 0;
		}
	}
}
