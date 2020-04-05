/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "ultima/ultima4/core/config.h"
#include "ultima/ultima4/game/context.h"
#include "ultima/ultima4/map/dungeonview.h"
#include "ultima/ultima4/core/error.h"
#include "ultima/ultima4/game/intro.h"
#include "ultima/ultima4/filesys/savegame.h"
#include "ultima/ultima4/core/settings.h"
#include "ultima/ultima4/gfx/scale.h"
#include "ultima/ultima4/gfx/screen.h"
#include "ultima/ultima4/events/event.h"
#include "ultima/ultima4/events/cursors.h"
#include "ultima/ultima4/gfx/image.h"
#include "ultima/ultima4/gfx/imagemgr.h"
#include "ultima/ultima4/map/tileanim.h"
#include "ultima/ultima4/map/tileset.h"
#include "ultima/ultima4/ultima4.h"
#include "ultima/ultima4/filesys/u4file.h"
#include "ultima/ultima4/core/utils.h"
#include "common/system.h"
#include "engines/util.h"

namespace Ultima {
namespace Ultima4 {

using Std::vector;

struct Cursor {
	Graphics::ManagedSurface _surface;
	Common::Point _hotspot;
};

Cursor *cursors[5];
Scaler filterScaler;

Cursor *screenInitCursor(const char *const xpm[]);


void screenInit_sys() {
	initGraphics(320 * settings._scale, 200 * settings._scale);

	// enable or disable the mouse cursor
	if (settings._mouseOptions._enabled) {
		g_system->showMouse(true);
#ifdef TODO
		cursors[0] = SDL_GetCursor();
#endif
		cursors[1] = screenInitCursor(w_xpm);
		cursors[2] = screenInitCursor(n_xpm);
		cursors[3] = screenInitCursor(e_xpm);
		cursors[4] = screenInitCursor(s_xpm);
	} else {
		g_system->showMouse(false);
	}

	filterScaler = scalerGet(settings._filter);
	if (!filterScaler)
		errorFatal("%s is not a valid filter", settings._filter.c_str());
}

void screenDelete_sys() {
	delete cursors[0];
	delete cursors[1];
	delete cursors[2];
	delete cursors[3];
	delete cursors[4];
}

/**
 * Force a redraw.
 */

//SDL_mutex *screenLockMutex = NULL;
int frameDuration = 0;

void screenRedrawTextArea(int x, int y, int width, int height) {
	g_system->updateScreen();
}

void screenWait(int numberOfAnimationFrames) {
	g_system->delayMillis(numberOfAnimationFrames * frameDuration);
}

bool continueScreenRefresh = true;

/**
 * Scale an image up.  The resulting image will be scale * the
 * original dimensions.  The original image is no longer deleted.
 * n is the number of tiles in the image; each tile is filtered
 * seperately. filter determines whether or not to filter the
 * resulting image.
 */
Image *screenScale(Image *src, int scale, int n, int filter) {
	Image *dest = NULL;
	bool isTransparent;
	unsigned int transparentIndex;
	bool alpha = src->isAlphaOn();

	if (n == 0)
		n = 1;

	isTransparent = src->getTransparentIndex(transparentIndex);
	src->alphaOff();

	while (filter && filterScaler && (scale % 2 == 0)) {
		dest = (*filterScaler)(src, 2, n);
		src = dest;
		scale /= 2;
	}
	if (scale == 3 && scaler3x(settings._filter)) {
		dest = (*filterScaler)(src, 3, n);
		src = dest;
		scale /= 3;
	}

	if (scale != 1)
		dest = (*scalerGet("point"))(src, scale, n);

	if (!dest)
		dest = Image::duplicate(src);

	if (isTransparent)
		dest->setTransparentIndex(transparentIndex);

	if (alpha)
		src->alphaOn();

	return dest;
}

/**
 * Scale an image down.  The resulting image will be 1/scale * the
 * original dimensions.  The original image is no longer deleted.
 */
Image *screenScaleDown(Image *src, int scale) {
	int x, y;
	Image *dest;
	bool isTransparent;
	unsigned int transparentIndex;
	bool alpha = src->isAlphaOn();

	isTransparent = src->getTransparentIndex(transparentIndex);

	src->alphaOff();

	dest = Image::create(src->width() / scale, src->height() / scale, src->isIndexed(), Image::HARDWARE);
	if (!dest)
		return NULL;

	if (!dest)
		dest = Image::duplicate(src);

	if (dest->isIndexed())
		dest->setPaletteFromImage(src);

	for (y = 0; y < src->height(); y += scale) {
		for (x = 0; x < src->width(); x += scale) {
			unsigned int index;
			src->getPixelIndex(x, y, index);
			dest->putPixelIndex(x / scale, y / scale, index);
		}
	}

	if (isTransparent)
		dest->setTransparentIndex(transparentIndex);

	if (alpha)
		src->alphaOn();

	return dest;
}

/**
 * Create a cursor object from an xpm.  Derived from example in
 * SDL documentation project.
 */
#define CURSORSIZE 32

Cursor *screenInitCursor(const char *const xpm[]) {
#ifdef TODO
	int i, row, col;
	uint8 data[(CURSORSIZE / 8)*CURSORSIZE];
	uint8 mask[(CURSORSIZE / 8)*CURSORSIZE];
	int hot_x, hot_y;

	i = -1;
	for (row = 0; row < CURSORSIZE; row++) {
		for (col = 0; col < CURSORSIZE; col++) {
			if (col % 8) {
				data[i] <<= 1;
				mask[i] <<= 1;
			} else {
				i++;
				data[i] = mask[i] = 0;
			}
			switch (xpm[4 + row][col]) {
			case 'X':
				data[i] |= 0x01;
				mask[i] |= 0x01;
				break;
			case '.':
				mask[i] |= 0x01;
				break;
			case ' ':
				break;
			}
		}
	}
	sscanf(xpm[4 + row], "%d,%d", &hot_x, &hot_y);
	return SDL_CreateCursor(data, mask, CURSORSIZE, CURSORSIZE, hot_x, hot_y);
#else
	return nullptr;
#endif
}

void screenSetMouseCursor(MouseCursor cursor) {
#ifdef TODO
	static int current = 0;

	if (cursor != current) {
		SDL_SetCursor(cursors[cursor]);
		current = cursor;
	}
#endif
}

} // End of namespace Ultima4
} // End of namespace Ultima
