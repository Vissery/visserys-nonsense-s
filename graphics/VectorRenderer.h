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
 * $URL$
 * $Id$
 *
 */

#ifndef VECTOR_RENDERER_H
#define VECTOR_RENDERER_H

#include "common/scummsys.h"
#include "common/system.h"

#include "graphics/surface.h"
#include "graphics/colormasks.h"

#include "gui/ThemeRenderer.h"


namespace Graphics {
class VectorRenderer;
struct DrawStep;

struct DrawStep {
	struct { 
		uint8 r, g, b;
		bool set;
	}	
	fgColor, /** Foreground color */
	bgColor, /** backgroudn color */
	gradColor1, /** gradient start*/
	gradColor2, /** gradient end */
	bevelColor;

	bool autoWidth, autoHeight;
	int16 x, y, w, h; /** width, height and position, if not measured automatically.
	 					  negative values mean counting from the opposite direction */
	
	enum VectorAlignment {
		kVectorAlignManual,
		kVectorAlignLeft,
		kVectorAlignRight,
		kVectorAlignBottom,
		kVectorAlignTop,
		kVectorAlignCenter
	} xAlign, yAlign;

	uint8 shadow, stroke, factor, radius, bevel; /** Misc options... */

	uint8 fillMode; /** active fill mode */
	uint32 extraData; /** Generic parameter for extra options (orientation/bevel) */

	uint32 scale; /** scale of all the coordinates in FIXED POINT with 16 bits mantissa */

	void (VectorRenderer::*drawingCall)(const Common::Rect &, const DrawStep &); /** Pointer to drawing function */
	Graphics::Surface *blitSrc;
};

VectorRenderer *createRenderer(int mode);

/**
 * VectorRenderer: The core Vector Renderer Class
 *
 * This virtual class exposes the API with all the vectorial
 * rendering functions that may be used to draw on a given Surface.
 *
 * This class must be instantiated as one of its children, which implement
 * the actual rendering functionality for each Byte Depth / Byte Format
 * combination, and may also contain platform specific code.
 *
 * TODO: Expand documentation.
 *
 * @see VectorRendererSpec
 * @see VectorRendererAA
 */
class VectorRenderer {
public:
	VectorRenderer() : _shadowOffset(0), _fillMode(kFillDisabled), 
		_activeSurface(NULL), _strokeWidth(1), _gradientFactor(1), _disableShadows(false) {
	
	}

	virtual ~VectorRenderer() {}

	/** Specifies the way in which a shape is filled */
	enum FillMode {
		kFillDisabled = 0,
		kFillForeground = 1,
		kFillBackground = 2,
		kFillGradient = 3
	};

	enum TriangleOrientation {
		kTriangleAuto = 0,
		kTriangleUp,
		kTriangleDown,
		kTriangleLeft,
		kTriangleRight
	};
	
	enum ConvolutionData {
		kConvolutionSoftBlur,
		kConvolutionHardBlur,
		kConvolutionGaussianBlur,
		kConvolutionEmboss,
		kConvolutionSharpen,
		kConvolutionEdgeDetect,
		kConvolutionMAX
	};
	
	struct ConvolutionDataSet {
		int matrix[3][3];
		int divisor;
		int offset;
	};

	/**
	 * Draws a line by considering the special cases for optimization.
	 *
	 * @param x1 Horizontal (X) coordinate for the line start
	 * @param x2 Horizontal (X) coordinate for the line end
	 * @param y1 Vertical (Y) coordinate for the line start
	 * @param y2 Vertical (Y) coordinate for the line end
	 */
	virtual void drawLine(int x1, int y1, int x2, int y2) = 0;

	/**
	 * Draws a circle centered at (x,y) with radius r.
	 *
	 * @param x Horizontal (X) coordinate for the center of the circle
	 * @param y Vertical (Y) coordinate for the center of the circle
	 * @param r Radius of the circle.
	 */
	virtual void drawCircle(int x, int y, int r) = 0;

	/**
	 * Draws a square starting at (x,y) with the given width and height.
	 *
	 * @param x Horizontal (X) coordinate for the center of the square
	 * @param y Vertical (Y) coordinate for the center of the square
	 * @param w Width of the square.
	 * @param h Height of the square
	 */
	virtual void drawSquare(int x, int y, int w, int h) = 0;

	/**
	 * Draws a rounded square starting at (x,y) with the given width and height.
	 * The corners of the square are rounded with the given radius.
	 *
	 * @param x Horizontal (X) coordinate for the center of the square
	 * @param y Vertical (Y) coordinate for the center of the square
	 * @param w Width of the square.
	 * @param h Height of the square
	 * @param r Radius of the corners.
	 */
	virtual void drawRoundedSquare(int x, int y, int r, int w, int h) = 0;

	/**
	 * Draws a triangle starting at (x,y) with the given base and height.
	 * The triangle will always be isosceles, with the given base and height.
	 * The orientation parameter controls the position of the base of the triangle.
	 *
	 * @param x Horizontal (X) coordinate for the top left corner of the triangle
	 * @param y Vertical (Y) coordinate for the top left corner of the triangle
	 * @param base Width of the base of the triangle
	 * @param h Height of the triangle
	 * @param orient Orientation of the triangle.
	 */
	virtual void drawTriangle(int x, int y, int base, int height, TriangleOrientation orient) = 0;

	/**
	 * Draws a beveled square like the ones in the Classic GUI themes.
	 * Beveled squares are always drawn with a transparent background. Draw them on top
	 * of a standard square to fill it.
	 *
	 * @param x Horizontal (X) coordinate for the center of the square
	 * @param y Vertical (Y) coordinate for the center of the square
	 * @param w Width of the square.
	 * @param h Height of the square
	 * @param bevel Amount of bevel. Must be positive.
	 */
	virtual void drawBeveledSquare(int x, int y, int w, int h, int bevel) = 0;
	
	/**
	 * Draws a tab-like shape, specially thought for the Tab widget.
	 * If a radius is given, the tab will have rounded corners. Otherwise,
	 * the tab will be squared.
	 *
	 * @param x Horizontal (X) coordinate for the tab
	 * @param y Vertical (Y) coordinate for the tab
	 * @param w Width of the tab
	 * @param h Height of the tab
	 * @param r Radius of the corners of the tab (0 for squared tabs).
	 */
	virtual void drawTab(int x, int y, int r, int w, int h) = 0;

	/**
	 * Gets the pixel pitch for the current drawing surface.
	 * Note: This is a real pixel-pitch, not a byte-pitch.
	 * That means it can be safely used in pointer arithmetics and
	 * in pixel manipulation.
	 *
	 * @return integer with the active bytes per pixel
	 */
	virtual uint16 surfacePitch() {
		return _activeSurface->pitch / _activeSurface->bytesPerPixel;
	}

	/**
	 * Gets the BYTES (not bits) per Pixel we are working on,
	 * based on the active drawing surface.
	 *
	 * @return integer byte with the active bytes per pixel value
	 */
	virtual uint8 bytesPerPixel() {
		return _activeSurface->bytesPerPixel;
	}

	/**
	 * Set the active foreground painting color for the renderer.
	 * All the foreground drawing from then on will be done with that color, unless
	 * specified otherwise.
	 *
	 * Foreground drawing means all outlines and basic shapes.
	 *
	 * @param r	value of the red color byte
	 * @param g	value of the green color byte
	 * @param b	value of the blue color byte
	 */
	virtual void setFgColor(uint8 r, uint8 g, uint8 b) = 0;

	/**
	 * Set the active background painting color for the renderer.
	 * All the background drawing from then on will be done with that color, unless
	 * specified otherwise.
	 *
	 * Background drawing means all the shape filling.
	 *
	 * @param r	value of the red color byte
	 * @param g	value of the green color byte
	 * @param b	value of the blue color byte
	 */
	virtual void setBgColor(uint8 r, uint8 g, uint8 b) = 0;
	
	virtual void setBevelColor(uint8 r, uint8 g, uint8 b) = 0;

	/**
	 * Set the active gradient color. All shapes drawn using kFillGradient
	 * as their fill mode will use this VERTICAL gradient as their fill color.
	 *
	 * @param r1	value of the red color byte for the start color
	 * @param g1	value of the green color byte for the start color
	 * @param b1	value of the blue color byte for the start color
	 * @param r2	value of the red color byte for the end color
	 * @param g2	value of the green color byte for the end color
	 * @param b2	value of the blue color byte for the end color
	 */
	virtual void setGradientColors(uint8 r1, uint8 g1, uint8 b1, uint8 r2, uint8 g2, uint8 b2) = 0;

	/**
	 * Sets the active drawing surface. All drawing from this
	 * point on will be done on that surface.
	 *
	 * @param surface Pointer to a Surface object.
	 */
	virtual void setSurface(Surface *surface) {
		_activeSurface = surface;
	}

	/**
	 * Fills the active surface with the specified fg/bg color or the active gradient.
	 * Defaults to using the active Foreground color for filling.
	 *
	 * @param mode Fill mode (bg, fg or gradient) used to fill the surface
	 */
	virtual void fillSurface() = 0;

	/**
	 * Clears the active surface.
	 */
	virtual void clearSurface() {
		byte *src = (byte *)_activeSurface->pixels;
		memset(src, 0, _activeSurface->w * _activeSurface->h * _activeSurface->bytesPerPixel);
	}

	/**
	 * Sets the active fill mode for all shapes.
	 *
	 * @see VectorRenderer::FillMode
	 * @param mode Specified fill mode.
	 */
	virtual void setFillMode(FillMode mode) {
		_fillMode = mode;
	}

	/**
	 * Sets the stroke width. All shapes drawn with a stroke will
	 * have that width. Pass 0 to disable shape stroking.
	 *
	 * @param width Witdh of the stroke in pixels.
	 */
	virtual void setStrokeWidth(int width) {
		_strokeWidth = width;
	}

	/**
	 * Enables adding shadows to all drawn primitives.
	 * Shadows are drawn automatically under the shapes. The given offset
	 * controls their intensity and size (the higher the offset, the
	 * bigger the shadows). If the offset is 0, no shadows are drawn.
	 *
	 * @param offset Shadow offset.
	 */
	virtual void setShadowOffset(int offset) {
		if (offset >= 0)
			_shadowOffset = offset;
	}
	
	virtual void setBevel(int amount) {
		if (amount >= 0)
			_bevel = amount;
	}

	/**
	 * Sets the multiplication factor of the active gradient.
	 *
	 * @see _gradientFactor
	 * @param factor Multiplication factor.
	 */
	virtual void setGradientFactor(int factor) {
		if (factor > 0)
			_gradientFactor = factor;
	}

	/**
	 * Translates the position data inside a DrawStep into actual
	 * screen drawing positions.
	 */
	void stepGetPositions(const DrawStep &step, const Common::Rect &area, uint16 &in_x, uint16 &in_y, uint16 &in_w, uint16 &in_h);

	/**
	 * Translates the radius data inside a drawstep into the real radius
	 * for the shape. Used for automatic radius calculations.
	 */
	int stepGetRadius(const DrawStep &step, const Common::Rect &area);

	/**
	 * DrawStep callback functions for each drawing feature 
	 */
	void drawCallback_CIRCLE(const Common::Rect &area, const DrawStep &step) {
		uint16 x, y, w, h, radius;

		radius = stepGetRadius(step, area);
		stepGetPositions(step, area, x, y, w, h);

		drawCircle(x + radius, y + radius, radius);
	}

	void drawCallback_SQUARE(const Common::Rect &area, const DrawStep &step) {
		uint16 x, y, w, h;
		stepGetPositions(step, area, x, y, w, h);
		drawSquare(x, y, w, h);
	}

	void drawCallback_LINE(const Common::Rect &area, const DrawStep &step) {
		uint16 x, y, w, h;
		stepGetPositions(step, area, x, y, w, h);
		drawLine(x, y, x + w, y + w);
	}

	void drawCallback_ROUNDSQ(const Common::Rect &area, const DrawStep &step) {
		uint16 x, y, w, h;
		stepGetPositions(step, area, x, y, w, h);
		drawRoundedSquare(x, y, stepGetRadius(step, area), w, h);
	}

	void drawCallback_FILLSURFACE(const Common::Rect &area, const DrawStep &step) {
		fillSurface();
	}

	void drawCallback_TRIANGLE(const Common::Rect &area, const DrawStep &step) {
		uint16 x, y, w, h;
		stepGetPositions(step, area, x, y, w, h);
		drawTriangle(x, y, w, h, (TriangleOrientation)step.extraData);
	}

	void drawCallback_BEVELSQ(const Common::Rect &area, const DrawStep &step) {
		uint16 x, y, w, h;
		stepGetPositions(step, area, x, y, w, h);
		drawBeveledSquare(x, y, w, h, _bevel);
	}
	
	void drawCallback_TAB(const Common::Rect &area, const DrawStep &step) {
		uint16 x, y, w, h;
		stepGetPositions(step, area, x, y, w, h);
		drawTab(x, y, stepGetRadius(step, area), w, h);
	}
	
	void drawCallback_BITMAP(const Common::Rect &area, const DrawStep &step) {
		uint16 x, y, w, h;
		stepGetPositions(step, area, x, y, w, h);
		blitAlphaBitmap(step.blitSrc, Common::Rect(x, y, x + w, y + h));
	}

	void drawCallback_VOID(const Common::Rect &area, const DrawStep &step) {}

	/**
	 * Draws the specified draw step on the screen.
	 * 
	 * @see DrawStep
	 * @param area Zone to paint on
	 * @param step Pointer to a DrawStep struct.
	 */
	virtual void drawStep(const Common::Rect &area, const DrawStep &step, uint32 extra = 0);

	/**
	 * Copies the part of the current frame to the system overlay.
	 *
	 * @param sys Pointer to the global System class
	 * @param r Zone of the surface to copy into the overlay.
	 */
	virtual void copyFrame(OSystem *sys, const Common::Rect &r) = 0;
	
	/**
	 * Copies the current surface to the system overlay 
	 *
	 * @param sys Pointer to the global System class
	 */
	virtual void copyWholeFrame(OSystem *sys) = 0;

	/**
	 * Blits a given graphics surface on top of the current drawing surface.
	 *
	 * Note that the source surface and the active
	 * surface are expected to be of the same size, hence the area delimited
	 * by "r" in the source surface will be blitted into the area delimited by 
	 * "r" on the current surface.
	 *
	 * If you wish to blit a smaller surface into the active drawing area, use
	 * VectorRenderer::blitSubSurface().
	 *
	 * @param source Surface to blit into the drawing surface.
	 * @param r Position in the active drawing surface to do the blitting.
	 */
	virtual void blitSurface(const Graphics::Surface *source, const Common::Rect &r) = 0;
	
	/**
	 * Blits a given graphics surface into a small area of the current drawing surface.
	 *
	 * Note that the given surface is expected to be smaller than the
	 * active drawing surface, hence the WHOLE source surface will be
	 * blitted into the active surface, at the position specified by "r".
	 */
	virtual void blitSubSurface(const Graphics::Surface *source, const Common::Rect &r) = 0;
	
	virtual void blitAlphaBitmap(const Graphics::Surface *source, const Common::Rect &r) = 0;
	
	/**
	 * Draws a string into the screen. Wrapper for the Graphics::Font string drawing
	 * method.
	 */
	virtual void drawString(const Graphics::Font *font, const Common::String &text, const Common::Rect &area, GUI::Theme::TextAlign alignH, GUI::Theme::TextAlignVertical alignV, int deltax, bool useEllipsis) = 0;
	
	/**
	 * Allows to temporarily enable/disable all shadows drawing.
	 * i.e. for performance issues, blitting, etc
	 */
	virtual void disableShadows() { _disableShadows = true; }
	virtual void enableShadows() { _disableShadows = false; }
	
	/**
	 * Applies a convolution matrix on the given surface area.
	 * Call applyConvolutionMatrix() instead if you want to use
	 * the embeded matrixes (blur/sharpen masks, bevels, etc).
	 *
	 * @param area Area in which the convolution matrix will be applied.
	 * @param filter Convolution matrix (3X3)
	 * @param filterDiv Divisor for the convolution matrix.
	 *					Make sure this equals the total sum of the elements
	 *					of the matrix or brightness data will be distorted.
	 * @param offset Offset on the convolution area.
	 */
	virtual void areaConvolution(const Common::Rect &area, const int filter[3][3], int filterDiv, int offset) = 0;
	
	/**
	 * Applies one of the predefined convolution effects on the given area.
	 *
	 * WARNING: Because of performance issues, this is currently disabled on all renderers.
	 *
	 * @param id Id of the convolution data set (see VectorRenderer::ConvolutionData)
	 * @param area Area in which the convolution effect will be applied.
	 */
	virtual void applyConvolutionMatrix(const ConvolutionData id, const Common::Rect &area) {
#ifdef ENABLE_CONVOLUTIONS
		areaConvolution(area, _convolutionData[id].matrix, _convolutionData[id].divisor, _convolutionData[id].offset);
#endif
	}
	
	/**
	 * Applies a whole-screen shading effect, used before opening a new dialog.
	 * Currently supports screen dimmings and luminance (b&w).
	 */
	virtual void applyScreenShading(GUI::Theme::ShadingStyle) = 0;

protected:
	Surface *_activeSurface; /** Pointer to the surface currently being drawn */

	FillMode _fillMode; /** Defines in which way (if any) are filled the drawn shapes */
	
	int _shadowOffset; /** offset for drawn shadows */
	int _bevel; /** amount of fake bevel */
	bool _disableShadows; /** Disables temporarily shadow drawing for overlayed images. */
	int _strokeWidth; /** Width of the stroke of all drawn shapes */
	uint32 _dynamicData; /** Dynamic data from the GUI Theme that modifies the drawing of the current shape */

	int _gradientFactor; /** Multiplication factor of the active gradient */
	int _gradientBytes[3]; /** Color bytes of the active gradient, used to speed up calculation */
	
	static const ConvolutionDataSet _convolutionData[kConvolutionMAX];
	
	static const int _dimPercentValue = 256 * 50 / 100; /** default value for screen dimming (50%) */
};

/**
 * VectorRendererSpec: Specialized Vector Renderer Class
 *
 * This templated class implements the basic subset of vector operations for
 * all platforms by allowing the user to provide the actual Pixel Type and
 * pixel information structs.
 *
 * This class takes two template parameters:
 *
 * @param PixelType Defines a type which may hold the color value of a single
 *        pixel, such as "byte" or "uint16" for 8 and 16 BPP respectively.
 *
 * @param PixelFormat Defines the type of the PixelFormat struct which contains all
 *        the actual information of the pixels being used, as declared in "graphics/colormasks.h"
 *                    
 * TODO: Expand documentation.
 *
 * @see VectorRenderer
 */
template<typename PixelType, typename PixelFormat>
class VectorRendererSpec : public VectorRenderer {
	typedef VectorRenderer Base;

public:
	VectorRendererSpec() {
		_bitmapAlphaColor = RGBToColor<PixelFormat>(255, 0, 255);
	}
	
	
	/**
	 * @see VectorRenderer::drawLine()
	 */
	void drawLine(int x1, int y1, int x2, int y2);

	/**
	 * @see VectorRenderer::drawCircle()
	 */
	void drawCircle(int x, int y, int r);

	/**
	 * @see VectorRenderer::drawSquare()
	 */
	void drawSquare(int x, int y, int w, int h);

	/**
	 * @see VectorRenderer::drawRoundedSquare()
	 */
	void drawRoundedSquare(int x, int y, int r, int w, int h);

	/**
	 * @see VectorRenderer::drawTriangle()
	 */
	void drawTriangle(int x, int y, int base, int height, TriangleOrientation orient);
	
	/**
	 * @see VectorRenderer::drawTab()
	 */
	void drawTab(int x, int y, int r, int w, int h);

	void drawBeveledSquare(int x, int y, int w, int h, int bevel) {
		drawBevelSquareAlg(x, y, w, h, bevel, _bevelColor, _fgColor, Base::_fillMode != kFillDisabled);
	}
	
	void drawString(const Graphics::Font *font, const Common::String &text, 
					const Common::Rect &area, GUI::Theme::TextAlign alignH,
					GUI::Theme::TextAlignVertical alignV, int deltax, bool elipsis);

	/**
	 * @see VectorRenderer::setFgColor()
	 */
	void setFgColor(uint8 r, uint8 g, uint8 b) {
		this->_fgColor = RGBToColor<PixelFormat>(r, g, b);
	}

	/**
	 * @see VectorRenderer::setBgColor()
	 */
	void setBgColor(uint8 r, uint8 g, uint8 b) {
		this->_bgColor = RGBToColor<PixelFormat>(r, g, b);
	}
	
	void setBevelColor(uint8 r, uint8 g, uint8 b) {
		this->_bevelColor = RGBToColor<PixelFormat>(r, g, b);
	}

	/**
	 * @see VectorRenderer::setGradientColors()
	 */
	void setGradientColors(uint8 r1, uint8 g1, uint8 b1, uint8 r2, uint8 g2, uint8 b2) {
		_gradientEnd = RGBToColor<PixelFormat>(r2, g2, b2);
		_gradientStart = RGBToColor<PixelFormat>(r1, g1, b1);

		Base::_gradientBytes[0] = (_gradientEnd & PixelFormat::kRedMask) - (_gradientStart & PixelFormat::kRedMask);
		Base::_gradientBytes[1] = (_gradientEnd & PixelFormat::kGreenMask) - (_gradientStart & PixelFormat::kGreenMask);
		Base::_gradientBytes[2] = (_gradientEnd & PixelFormat::kBlueMask) - (_gradientStart & PixelFormat::kBlueMask);
	}

	/**
	 * @see VectorRenderer::fillSurface()
	 */
	void fillSurface() {
		PixelType *ptr = (PixelType *)_activeSurface->getBasePtr(0, 0);

		int w = _activeSurface->w;
		int h = _activeSurface->h ;
		int pitch = surfacePitch();

		if (Base::_fillMode == kFillBackground)
			colorFill(ptr, ptr + w * h, _bgColor);
		else if (Base::_fillMode == kFillForeground)
			colorFill(ptr, ptr + w * h, _fgColor);
		else if (Base::_fillMode == kFillGradient) {
			int i = h;
			while (i--) {
				colorFill(ptr, ptr + w, calcGradient(h - i, h));
				ptr += pitch;
			}
		}
	}

	/**
	 * @see VectorRenderer::copyFrame()
	 */
	virtual void copyFrame(OSystem *sys, const Common::Rect &r) {
#ifdef OVERLAY_MULTIPLE_DEPTHS
		sys->copyRectToOverlay((const PixelType*)_activeSurface->getBasePtr(r.left, r.top),
			_activeSurface->w, r.left, r.top, r.width(), r.height());
#else
		sys->copyRectToOverlay((const OverlayColor*)_activeSurface->getBasePtr(r.left, r.top),
			_activeSurface->w, r.left, r.top, r.width(), r.height());
#endif
	}

	virtual void copyWholeFrame(OSystem *sys) {
#ifdef OVERLAY_MULTIPLE_DEPTHS
		sys->copyRectToOverlay((const PixelType*)_activeSurface->getBasePtr(0, 0),
			_activeSurface->w, 0, 0, _activeSurface->w, _activeSurface->h);
#else
		sys->copyRectToOverlay((const OverlayColor*)_activeSurface->getBasePtr(0, 0),
			_activeSurface->w, 0, 0, _activeSurface->w, _activeSurface->h);
#endif
	}

	/**
	 * @see VectorRenderer::blitSurface()
	 */
	virtual void blitSurface(const Graphics::Surface *source, const Common::Rect &r) {
		assert(source->w == _activeSurface->w && source->h == _activeSurface->h);
		
		PixelType *dst_ptr = (PixelType *)_activeSurface->getBasePtr(r.left, r.top);
		PixelType *src_ptr = (PixelType *)source->getBasePtr(r.left, r.top);

		int dst_pitch = surfacePitch();
		int src_pitch = source->pitch / source->bytesPerPixel;

		int h = r.height(), w = r.width();

		while (h--) {
			colorCopy(src_ptr, dst_ptr, w);
			dst_ptr += dst_pitch;
			src_ptr += src_pitch;
		}
	}
	
	virtual void blitSubSurface(const Graphics::Surface *source, const Common::Rect &r) {
		PixelType *dst_ptr = (PixelType *)_activeSurface->getBasePtr(r.left, r.top);
		PixelType *src_ptr = (PixelType *)source->getBasePtr(0, 0);
		
		int dst_pitch = surfacePitch();
		int src_pitch = source->pitch / source->bytesPerPixel;
		
		int h = r.height(), w = r.width();
		
		while (h--) {
			colorCopy(src_ptr, dst_ptr, w);
			dst_ptr += dst_pitch;
			src_ptr += src_pitch;
		}
	}
	
	virtual void blitAlphaBitmap(const Graphics::Surface *source, const Common::Rect &r) {
		int16 x = r.left;
		int16 y = r.top;
		
		if (r.width() > source->w)
			x = x + (r.width() >> 1) - (source->w >> 1);
			
		if (r.height() > source->h)
			y = y + (r.height() >> 1) - (source->h >> 1);
			
		PixelType *dst_ptr = (PixelType *)_activeSurface->getBasePtr(x, y);
		PixelType *src_ptr = (PixelType *)source->getBasePtr(0, 0);
		
		int dst_pitch = surfacePitch();
		int src_pitch = source->pitch / source->bytesPerPixel;
		
		int w, h = source->h;
		
		while (h--) {
			w = source->w;
			
			while (w--) {
				if (*src_ptr != _bitmapAlphaColor)
					*dst_ptr = *src_ptr;
					
				dst_ptr++;
				src_ptr++;
			}
			
			dst_ptr = dst_ptr - source->w + dst_pitch;
			src_ptr = src_ptr - source->w + src_pitch;
		}
		
	}
	
	virtual void applyScreenShading(GUI::Theme::ShadingStyle shadingStyle) {
		int pixels = _activeSurface->w * _activeSurface->h;
		PixelType *ptr = (PixelType *)_activeSurface->getBasePtr(0, 0);
		uint8 r, g, b;
		uint lum;
		
		if (shadingStyle == GUI::Theme::kShadingDim) {
			while (pixels--) {
				colorToRGB<PixelFormat>(*ptr, r, g, b);
				r = r * _dimPercentValue >> 8;
				g = g * _dimPercentValue >> 8;
				b = b * _dimPercentValue >> 8;
				*ptr++ = RGBToColor<PixelFormat>(r, g, b);
			}
		} else if (shadingStyle == GUI::Theme::kShadingLuminance) {
			while (pixels--) {
				colorToRGB<PixelFormat>(*ptr, r, g, b);
				lum = (r >> 2) + (g >> 1) + (b >> 3);
				*ptr++ = RGBToColor<PixelFormat>(lum, lum, lum);
			}
		}
	}

protected:

	/**
	 * Draws a single pixel on the surface with the given coordinates and
	 * the given color.
	 *
	 * @param x Horizontal coordinate of the pixel.
	 * @param y Vertical coordinate of the pixel.
	 * @param color Color of the pixel
	 */
	virtual inline void putPixel(int x, int y, PixelType color) {
		PixelType *ptr = (PixelType *)_activeSurface->getBasePtr(x, y);
		*ptr = color;
	}

	/**
	 * Blends a single pixel on the surface with the given coordinates, color
	 * and Alpha intensity.
	 *
	 * @param x Horizontal coordinate of the pixel.
	 * @param y Vertical coordinate of the pixel.
	 * @param color Color of the pixel
	 * @param alpha Alpha intensity of the pixel (0-255)
	 */
	virtual inline void blendPixel(int x, int y, PixelType color, uint8 alpha) {
		if (alpha == 255)
			putPixel(x, y, color);
		else if (alpha > 0)
			blendPixelPtr((PixelType*)Base::_activeSurface->getBasePtr(x, y), color, alpha);
	}

	/**
	 * Blends a single pixel on the surface in the given pixel pointer, using supplied color
	 * and Alpha intensity.
	 * 
	 * This is implemented to prevent blendPixel() to calculate the surface pointer on each call.
	 * Optimized drawing algorithms should call this function when possible.
	 *
	 * @see blendPixel
	 * @param ptr Pointer to the pixel to blend on top of
	 * @param color Color of the pixel
	 * @param alpha Alpha intensity of the pixel (0-255)
	 */
	virtual inline void blendPixelPtr(PixelType *ptr, PixelType color, uint8 alpha)	{
		if (!ptr) return;
			
		if (alpha == 255) {
			*ptr = color;
			return;
		}

		register int idst = *ptr;
		register int isrc = color;

		*ptr = (PixelType)(
			(PixelFormat::kRedMask & ((idst & PixelFormat::kRedMask) +
			((int)(((int)(isrc & PixelFormat::kRedMask) -
			(int)(idst & PixelFormat::kRedMask)) * alpha) >> 8))) |
			(PixelFormat::kGreenMask & ((idst & PixelFormat::kGreenMask) +
			((int)(((int)(isrc & PixelFormat::kGreenMask) -
			(int)(idst & PixelFormat::kGreenMask)) * alpha) >> 8))) |
			(PixelFormat::kBlueMask & ((idst & PixelFormat::kBlueMask) +
			((int)(((int)(isrc & PixelFormat::kBlueMask) -
			(int)(idst & PixelFormat::kBlueMask)) * alpha) >> 8))) );
	}

	/**
	 * PRIMITIVE DRAWING ALGORITHMS
	 *
	 * Generic algorithms for drawing all kinds of aliased primitive shapes.
	 * These may be overloaded in inheriting classes to implement platform-specific
	 * optimizations or improve looks.
	 *
	 * @see VectorRendererAA
	 * @see VectorRendererAA::drawLineAlg
	 * @see VectorRendererAA::drawCircleAlg
	 */
	virtual void drawLineAlg(int x1, int y1, int x2, int y2, int dx, int dy, PixelType color);
	virtual void drawCircleAlg(int x, int y, int r, PixelType color, FillMode fill_m);
	virtual void drawRoundedSquareAlg(int x1, int y1, int r, int w, int h, PixelType color, FillMode fill_m);
	virtual void drawSquareAlg(int x, int y, int w, int h, PixelType color, FillMode fill_m);
	virtual void drawTriangleVertAlg(int x, int y, int w, int h, bool inverted, PixelType color, FillMode fill_m);
	virtual void drawTriangleFast(int x, int y, int size, bool inverted, PixelType color, FillMode fill_m);
	virtual void drawBevelSquareAlg(int x, int y, int w, int h, int bevel, PixelType top_color, PixelType bottom_color, bool fill);
	virtual void drawTabAlg(int x, int y, int w, int h, int r, PixelType color, VectorRenderer::FillMode fill_m, int baseLeft = 0, int baseRight = 0);
	virtual void drawBevelTabAlg(int x, int y, int w, int h, int bevel, PixelType topColor, PixelType bottomColor, int baseLeft = 0, int baseRight = 0);

	/**
	 * SHADOW DRAWING ALGORITHMS
	 *
	 * Optimized versions of the primitive drawing algorithms with alpha blending
	 * for shadow drawing.
	 * There functions may be overloaded in inheriting classes to improve performance
	 * in the slowest platforms where pixel alpha blending just doesn't cut it.
	 *
	 * @param blur Intensity/size of the shadow.
	 */
	virtual void drawSquareShadow(int x, int y, int w, int h, int blur);
	virtual void drawRoundedSquareShadow(int x, int y, int r, int w, int h, int blur);
	virtual void drawRoundedSquareFakeBevel(int x, int y, int r, int w, int h, int amount);

	/**
	 * Calculates the color gradient on a given point.
	 * This function assumes that the gradient start/end colors have been set
	 * beforehand from the API function call.
	 *
	 * @param pos Progress of the gradient.
	 * @param max Maximum amount of the progress.
	 * @return Composite color of the gradient at the given "progress" amount.
	 */
	virtual inline PixelType calcGradient(uint32 pos, uint32 max) {
		PixelType output = 0;
		pos = (MIN(pos * Base::_gradientFactor, max) << 12) / max;
		
		output |= (_gradientStart + ((Base::_gradientBytes[0] * pos) >> 12)) & PixelFormat::kRedMask;
		output |= (_gradientStart + ((Base::_gradientBytes[1] * pos) >> 12)) & PixelFormat::kGreenMask;
		output |= (_gradientStart + ((Base::_gradientBytes[2] * pos) >> 12)) & PixelFormat::kBlueMask;
		output |= ~(PixelFormat::kRedMask | PixelFormat::kGreenMask | PixelFormat::kBlueMask);
	
		return output;
	}

	/**
	 * Fills several pixels in a row with a given color and the specifed alpha blending.
	 *
	 * @see blendPixelPtr
	 * @see blendPixel
	 * @param first Pointer to the first pixel to fill.
	 * @param last Pointer to the last pixel to fill.
	 * @param color Color of the pixel
	 * @param alpha Alpha intensity of the pixel (0-255)
	 */
	virtual inline void blendFill(PixelType *first, PixelType *last, PixelType color, uint8 alpha) {
		while (first != last)
			blendPixelPtr(first++, color, alpha);
	}

	/**
	 * Fills several pixels in a row with a given color.
	 *
	 * This is a replacement function for Common::set_to, using an unrolled
	 * loop to maximize performance on most architectures.
	 * This function may (and should) be overloaded in any child renderers
	 * for portable platforms with platform-specific assembly code.
	 *
	 * This fill operation is extensively used throughout the renderer, so this
	 * counts as one of the main bottlenecks. Please replace it with assembly 
	 * when possible!
	 *
	 * @param first Pointer to the first pixel to fill.
	 * @param last Pointer to the last pixel to fill.
	 * @param color Color of the pixel
	 */
	virtual inline void colorFill(PixelType *first, PixelType *last, PixelType color) {
		if (first == last) {
			*first = color;
			return;
		}

		register PixelType *ptr = first;
		register int count = (last - first);
		register int n = (count + 7) >> 3;
		switch (count % 8) {
		case 0: do { 
					*ptr++ = color;
		case 7:		*ptr++ = color;
		case 6:		*ptr++ = color;
		case 5:		*ptr++ = color;
		case 4:		*ptr++ = color;
		case 3:		*ptr++ = color;
		case 2:		*ptr++ = color;
		case 1:		*ptr++ = color;
				} while (--n > 0);
		}
	}

	/**
	 * Copies several pixes in a row from a surface to another one.
	 * Used for surface blitting.
	 * See colorFill() for optimization guidelines.
	 *
	 * @param src Source surface.
	 * @param dst Destination surface.
	 * @param count Amount of pixels to copy over.
	 */
	virtual inline void colorCopy(PixelType *src, PixelType *dst, int count) {
		register int n = (count + 7) >> 3;
		switch (count % 8) {
		case 0: do { 
					*dst++ = *src++;
		case 7:		*dst++ = *src++;
		case 6:		*dst++ = *src++;
		case 5:		*dst++ = *src++;
		case 4:		*dst++ = *src++;
		case 3:		*dst++ = *src++;
		case 2:		*dst++ = *src++;
		case 1:		*dst++ = *src++;
				} while (--n > 0);
		}
	}
	
	virtual void areaConvolution(const Common::Rect &area, const int filter[3][3], int filterDiv, int offset);

	PixelType _fgColor; /** Foreground color currently being used to draw on the renderer */
	PixelType _bgColor; /** Background color currently being used to draw on the renderer */

	PixelType _gradientStart; /** Start color for the fill gradient */
	PixelType _gradientEnd; /** End color for the fill gradient */
	
	PixelType _bevelColor;
	
	PixelType _bitmapAlphaColor;
};

/**
 * VectorRendererAA: Anti-Aliased Vector Renderer Class
 *
 * This templated class inherits all the functionality of the VectorRendererSpec
 * class but uses better looking yet slightly slower AA algorithms for drawing
 * most primivitves. May be used in faster platforms.
 *                    
 * TODO: Expand documentation.
 *
 * @see VectorRenderer
 * @see VectorRendererSpec
 */
template<typename PixelType, typename PixelFormat>
class VectorRendererAA : public VectorRendererSpec<PixelType, PixelFormat> {
	typedef VectorRendererSpec<PixelType, PixelFormat> Base;
protected:
	/**
	 * "Wu's Line Antialiasing Algorithm" as published by Xiaolin Wu, July 1991
	 * Based on the implementation found in Michael Abrash's Graphics Programming Black Book.
	 *
	 * Generic line drawing algorithm for the Antialiased renderer. Optimized with no
	 * floating point operations, assumes no special cases.
	 *
	 * @see VectorRenderer::drawLineAlg()
	 */
	virtual void drawLineAlg(int x1, int y1, int x2, int y2, int dx, int dy, PixelType color);

	/**
	 * "Wu's Circle Antialiasing Algorithm" as published by Xiaolin Wu, July 1991
	 * Based on the theoretical concept of the algorithm.
	 *
	 * Implementation of Wu's algorithm for circles using fixed point arithmetics.
	 * Could be quite fast.
	 *
	 * @see VectorRenderer::drawCircleAlg()
	 */
	virtual void drawCircleAlg(int x, int y, int r, PixelType color, VectorRenderer::FillMode fill_m);

	/**
	 * "Wu's Circle Antialiasing Algorithm" as published by Xiaolin Wu, July 1991,
	 * modified with corner displacement to allow drawing of squares with rounded
	 * corners.
	 *
	 * @see VectorRenderer::drawRoundedAlg()
	 */
	virtual void drawRoundedSquareAlg(int x1, int y1, int r, int w, int h, PixelType color, VectorRenderer::FillMode fill_m);
	
	virtual void drawRoundedSquareShadow(int x, int y, int r, int w, int h, int blur) {
		Base::drawRoundedSquareShadow(x, y, r, w, h, blur);
		VectorRenderer::applyConvolutionMatrix(VectorRenderer::kConvolutionHardBlur, Common::Rect(x, y, x + w + blur * 2, y + h + blur * 2));
	}
};

} // end of namespace Graphics

#endif
