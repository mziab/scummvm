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

#ifndef SHERLOCK_GRAPHICS_H
#define SHERLOCK_GRAPHICS_H

#include "common/rect.h"
#include "graphics/surface.h"
#include "sherlock/resources.h"

namespace Sherlock {

class Surface : public Graphics::Surface {
private:
	bool _freePixels;

	/**
	 * Clips the given source bounds so the passed destBounds will be entirely on-screen
	 */
	bool clip(Common::Rect &srcBounds, Common::Rect &destBounds);
protected:
	virtual void addDirtyRect(const Common::Rect &r) {}
public:
	Surface(uint16 width, uint16 height);
	Surface();
	virtual ~Surface();

	/**
	 * Sets up an internal surface with the specified dimensions that will be automatically freed
	 * when the surface object is destroyed
	 */
	void create(uint16 width, uint16 height);

	/**
	 * Copy a surface into this one
	 */
	void blitFrom(const Graphics::Surface &src);

	/**
	 * Draws a surface at a given position within this surface
	 */
	void blitFrom(const Graphics::Surface &src, const Common::Point &pt);

	/**
	 * Draws a sub-section of a surface at a given position within this surface
	 */
	void blitFrom(const Graphics::Surface &src, const Common::Point &pt,
		const Common::Rect &srcBounds);
	
	/**
	 * Draws an image frame at a given position within this surface with transparency
	 */
	void transBlitFrom(const ImageFrame &src, const Common::Point &pt,
		bool flipped = false, int overrideColor = 0);
	
	/**
	 * Draws a surface at a given position within this surface with transparency
	 */
	void transBlitFrom(const Graphics::Surface &src, const Common::Point &pt,
		bool flipped = false, int overrideColor = 0);

	/**
	 * Fill a given area of the surface with a given color
	 */
	void fillRect(int x1, int y1, int x2, int y2, byte color);
	
	/**
	 * Fill a given area of the surface with a given color
	 */
	void fillRect(const Common::Rect &r, byte color);

	/**
	 * Clear the screen
	 */
	void clear();
};

} // End of namespace Sherlock

#endif
