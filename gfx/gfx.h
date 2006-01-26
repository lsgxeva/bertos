/*!
 * \file
 * Copyright 2003, 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See README.devlib for information.
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * \brief General pourpose graphics routines
 */

/*#*
 *#* $Log$
 *#* Revision 1.7  2006/01/26 00:36:48  bernie
 *#* Const correctness for some new functions.
 *#*
 *#* Revision 1.6  2006/01/23 23:13:04  bernie
 *#* RECT_WIDTH(), RECT_HEIGHT(), RASTER_SIZE(): New macros.
 *#*
 *#* Revision 1.5  2006/01/17 02:31:29  bernie
 *#* Add bitmap format support; Improve some comments.
 *#*
 *#* Revision 1.4  2006/01/16 03:30:57  bernie
 *#* Make header C++ friendly.
 *#*
 *#* Revision 1.3  2005/11/27 23:33:40  bernie
 *#* Use appconfig.h instead of cfg/config.h.
 *#*
 *#* Revision 1.2  2005/11/04 18:17:45  bernie
 *#* Fix header guards and includes for new location of gfx module.
 *#*
 *#* Revision 1.1  2005/11/04 18:11:35  bernie
 *#* Move graphics stuff from mware/ to gfx/.
 *#*
 *#* Revision 1.12  2005/11/04 16:20:02  bernie
 *#* Fix reference to README.devlib in header.
 *#*
 *#* Revision 1.11  2005/04/11 19:10:28  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.10  2005/03/01 23:26:45  bernie
 *#* Use new CPU-neutral program-memory API.
 *#*
 *#* Revision 1.9  2005/01/20 18:46:31  aleph
 *#* Fix progmem includes.
 *#*/

#ifndef GFX_GFX_H
#define GFX_GFX_H

#include <appconfig.h>
#include <cfg/compiler.h>
#include <cfg/cpu.h>

EXTERN_C_BEGIN

/*! Common type for coordinates expressed in pixel units */
typedef int coord_t;

#if CONFIG_GFX_VCOORDS
/*! Common type for coordinates expressed in logical units */
typedef float vcoord_t;
#endif /* CONFIG_GFX_VCOORDS */


/**
 * Describe a rectangular area with coordinates expressed in pixels.
 *
 * The rectangle is represented in terms of its top/left and
 * right/bottom borders.
 *
 * In some cases, rectangles are assumed to obey to the
 * following invariants:
 *
 *    xmin <= xmax
 *    ymin <= ymax
 *
 * Oddly, the xmin and ymin coordinates are inclusive, while the
 * xmax and ymax coordinates are non-inclusive.  This design
 * decision makes several computations simpler and lets you
 * specify empty (0x0) rectangles without breaking the
 * invariants.
 *
 * Computing the size of a rectangle can be done by simply
 * subtracting the maximum X or Y coordinate from the minimum
 * X or Y coordinate.
 */
typedef struct Rect { coord_t xmin, ymin, xmax, ymax; } Rect;

/**
 * Return the width of a rectangle in pixels.
 *
 * \note The argument \a r is evaluated twice.
 */
#define RECT_WIDTH(r)   ((r)->xmax - (r)->xmin)

/**
 * Return the height of a rectangle in pixels.
 *
 * \note The argument \a r is evaluated twice.
 */
#define RECT_HEIGHT(r)  ((r)->ymax - (r)->ymin)


/*!
 * Control structure to draw in a bitmap
 */
typedef struct Bitmap
{
	uint8_t *raster;        /*!< Pointer to byte array to hold the data */
	coord_t width, height;  /*!< Width/Height in pixels */
	coord_t stride;		/*!< Bytes per row. */
	coord_t penX, penY;     /*!< Current pen position MoveTo()/LineTo() */

	Rect cr;                /*!< Clip drawing inside this rectangle */

#if CONFIG_GFX_VCOORDS
	/*!
	 * \name Logical coordinate system
	 * \{
	 */
	vcoord_t orgX, orgY;
	vcoord_t scaleX, scaleY;
	/*\}*/
#endif /* CONFIG_GFX_VCOORDS */

} Bitmap;

#if CONFIG_BITMAP_FMT == BITMAP_FMT_PLANAR_H_MSB
	/**
	 * Compute the size in bytes of a raster suitable for
	 * holding a bitmap of \a width x \a height pixels.
	 */
	#define RASTER_SIZE(width, height) ( ((width) + 7 / 8) * (height) )

#elif CONFIG_BITMAP_FMT == BITMAP_FMT_PLANAR_V_LSB
	/**
	 * Compute the size in bytes of a raster suitable for
	 * holding a bitmap of \a width x \a height pixels.
	 */
	#define RASTER_SIZE(width, height) ( (width) * (((height) + 7) / 8) )
#else
	#error Unknown value of CONFIG_BITMAP_FMT
#endif /* CONFIG_BITMAP_FMT */


/* Function prototypes */
extern void gfx_bitmapInit (Bitmap *bm, uint8_t *raster, coord_t w, coord_t h);
extern void gfx_bitmapClear(Bitmap *bm);
extern void gfx_blit       (Bitmap *dst, const Rect *rect, const Bitmap *src, coord_t srcx, coord_t srcy);
extern void gfx_line       (Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2);
extern void gfx_rectDraw   (Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2);
extern void gfx_rectFillC  (Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2, uint8_t color);
extern void gfx_rectFill   (Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2);
extern void gfx_rectClear  (Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2);
extern void gfx_moveTo     (Bitmap *bm, coord_t x,  coord_t y);
extern void gfx_lineTo     (Bitmap *bm, coord_t x,  coord_t y);
extern void gfx_setClipRect(Bitmap *bm, coord_t xmin, coord_t ymin, coord_t xmax, coord_t ymax);

#if CPU_HARVARD
	#include <mware/pgm.h>
	extern void gfx_blit_P(Bitmap *bm, const pgm_uint8_t *raster);
#endif

#if CONFIG_GFX_VCOORDS
extern void gfx_setViewRect(Bitmap *bm, vcoord_t x1, vcoord_t y1, vcoord_t x2, vcoord_t y2);
extern coord_t gfx_transformX(Bitmap *bm, vcoord_t x);
extern coord_t gfx_transformY(Bitmap *bm, vcoord_t y);
extern void gfx_vline(Bitmap *bm, vcoord_t x1, vcoord_t y1, vcoord_t x2, vcoord_t y2);
#endif /* CONFIG_GFX_VCOORDS */

EXTERN_C_END

#endif /* GFX_GFX_H */
