/*
 * Multi-scroll example application.
 *
 * This is based off of a Sifteo SDK example which implements
 * scrolling on a single cube.
 *
 * Adrian Chadd <adrian@freebsd.org>
 */

#ifndef	__SCROLLOBJ_H__
#define	__SCROLLOBJ_H__

#include <sifteo.h>
#include "assets.gen.h"
using namespace Sifteo;

class ScrollObj {
	private:
		VideoBuffer vid;
		float cur_x;
		int prev_xt;

	public:
		void init(CubeID cube)
		{
			vid.attach(cube);
			cur_x = 0.0;
			prev_xt = 0;
		}

		void
		pictureAttach()
		{

			/* Just default to BG0 for now */
			vid.initMode(BG0);
		}

		void
		displayAttachPicture()
		{
			vid.initMode(BG0_ROM);
			vid.bg0rom.fill(vec(0,0), vec(16,16), BG0ROMDrawable::SOLID_BG);
			vid.bg0rom.text(vec(1,1), "Hold on!", BG0ROMDrawable::BLUE);
			vid.bg0rom.text(vec(1,14), "Adding Cube...", BG0ROMDrawable::BLUE);
		}

		void displayLoadGraph(unsigned p)
		{
			vid.bg0rom.hBargraph(
			    vec(0, 4),
			    p,
			    BG0ROMDrawable::ORANGE, 8);
		}

		void drawPicture()
		{
			for (int col = -1; col < 17; col++) {
				drawColumn(col);
			}
		}

		void drawColumn(int x)
		{
			// Draw a vertical column of tiles
			int bg0Width = vid.bg0.tileWidth();
			int dstx = umod(x, bg0Width);
			int srcx = umod(x, Background.tileWidth());
			vid.bg0.image(vec(dstx, 0), vec(1, bg0Width),
			    Background, vec(srcx, 0));
		}

		void update()
		{
			// Scroll based on accelerometer tilt
			Int2 accel = vid.physicalAccel().xy();

			// Floating point pixels
			cur_x += accel.x * (40.0f / 128.0f);

			// Integer pixels
			int xi = cur_x + 0.5f;

			// Integer tiles
			int xt = cur_x / 8;

			while (prev_xt < xt) {
				// Fill in new tiles, just past the right edge
				// of the screen
				drawColumn(prev_xt + 17);
				prev_xt++;
			}

			while (prev_xt > xt) {
				// Fill in new tiles, just past the left edge
				drawColumn(prev_xt - 2);
				prev_xt--;
			}

			// pixel-level scrolling within the current column
			vid.bg0.setPanning(vec(xi, 0));
		}
};

#endif	/* __SCROLLOBJ_H__ */
