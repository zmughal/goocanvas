/*
 * GooCanvas. Copyright (C) 2005-2010 Damon Chaplin.
 * Released under the GNU LGPL license. See COPYING for details.
 *
 * goocanvasstyle.h - 
 */
#ifndef __GOO_CANVAS_STYLE_H__
#define __GOO_CANVAS_STYLE_H__

#include <gtk/gtk.h>
#include <goocanvasutils.h>

G_BEGIN_DECLS


/**
 * GooCanvasStyleValuesMask
 * @GOO_CANVAS_STYLE_STROKE_PATTERN: the stroke pattern has been set.
 * @GOO_CANVAS_STYLE_FILL_PATTERN: the fill pattern has been set.
 * @GOO_CANVAS_STYLE_FILL_RULE: the fill rule has been set.
 * @GOO_CANVAS_STYLE_OPERATOR: the operator has been set.
 * @GOO_CANVAS_STYLE_ANTIALIAS: the antialias setting has been set.
 * @GOO_CANVAS_STYLE_LINE_WIDTH: the line width has been set.
 * @GOO_CANVAS_STYLE_LINE_WIDTH_TOLERANCE: the line width tolerance has been set.
 * @GOO_CANVAS_STYLE_LINE_WIDTH_IS_UNSCALED: the line width is unscaled flag has been set.
 * @GOO_CANVAS_STYLE_LINE_CAP: the line cap style has been set.
 * @GOO_CANVAS_STYLE_LINE_JOIN: the line join style has been set.
 * @GOO_CANVAS_STYLE_LINE_JOIN_MITER_LIMIT: the miter limit of line joins has
 *  been set.
 * @GOO_CANVAS_STYLE_LINE_DASH: the line dash pattern has been set.
 * @GOO_CANVAS_STYLE_FONT_DESCRIPTION: the font description has been set.
 * @GOO_CANVAS_STYLE_HINT_METRICS: the hint metrics setting has been set.
 *
 * Specifies which fields of a #GooCanvasStyle have been set.
 */
typedef enum
{
  GOO_CANVAS_STYLE_STROKE_PATTERN		= 1 << 0,
  GOO_CANVAS_STYLE_FILL_PATTERN			= 1 << 1,
  GOO_CANVAS_STYLE_FILL_RULE			= 1 << 2,
  GOO_CANVAS_STYLE_OPERATOR			= 1 << 3,
  GOO_CANVAS_STYLE_ANTIALIAS			= 1 << 4,

  GOO_CANVAS_STYLE_LINE_WIDTH			= 1 << 5,
  GOO_CANVAS_STYLE_LINE_WIDTH_TOLERANCE		= 1 << 6,
  GOO_CANVAS_STYLE_LINE_WIDTH_IS_UNSCALED	= 1 << 7,
  GOO_CANVAS_STYLE_LINE_CAP			= 1 << 8,
  GOO_CANVAS_STYLE_LINE_JOIN			= 1 << 9,
  GOO_CANVAS_STYLE_LINE_JOIN_MITER_LIMIT	= 1 << 10,
  GOO_CANVAS_STYLE_LINE_DASH			= 1 << 11,

  GOO_CANVAS_STYLE_FONT_DESCRIPTION		= 1 << 12,
  GOO_CANVAS_STYLE_HINT_METRICS			= 1 << 13
} GooCanvasStyleValuesMask;


#define GOO_TYPE_CANVAS_STYLE            (goo_canvas_style_get_type ())
#define GOO_CANVAS_STYLE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOO_TYPE_CANVAS_STYLE, GooCanvasStyle))
#define GOO_CANVAS_STYLE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GOO_TYPE_CANVAS_STYLE, GooCanvasStyleClass))
#define GOO_IS_CANVAS_STYLE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOO_TYPE_CANVAS_STYLE))
#define GOO_IS_CANVAS_STYLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GOO_TYPE_CANVAS_STYLE))
#define GOO_CANVAS_STYLE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GOO_TYPE_CANVAS_STYLE, GooCanvasStyleClass))


typedef struct _GooCanvasStyle       GooCanvasStyle;
typedef struct _GooCanvasStyleClass  GooCanvasStyleClass;

struct _GooCanvasStyle
{
  GObject parent_object;

  /*< private >*/
  cairo_pattern_t *stroke_pattern;
  cairo_pattern_t *fill_pattern;

  GooCanvasLineDash *dash;
  PangoFontDescription *font_desc;

  gdouble line_width;
  gdouble line_width_tolerance;
  gdouble line_join_miter_limit;

  /* This specifies which fields are actually set. If the STROKE_PATTERN bit
     is set, and stroke_pattern is NULL, no stroke will be drawn. */
  GooCanvasStyleValuesMask mask;

  cairo_operator_t  op              : 6;
  cairo_antialias_t antialias       : 4;
  cairo_fill_rule_t fill_rule       : 3;
  cairo_line_cap_t  line_cap        : 4;
  cairo_line_join_t line_join       : 4;
  guint hint_metrics		    : 2;
  guint line_width_is_unscaled	    : 1;
};

struct _GooCanvasStyleClass
{
  GObjectClass parent_class;

  /*< private >*/

  /* Padding for future expansion */
  void (*_goo_canvas_reserved1) (void);
  void (*_goo_canvas_reserved2) (void);
  void (*_goo_canvas_reserved3) (void);
  void (*_goo_canvas_reserved4) (void);
};


GType           goo_canvas_style_get_type        (void) G_GNUC_CONST;
GooCanvasStyle* goo_canvas_style_new             (void);

GooCanvasStyle* goo_canvas_style_copy		 (GooCanvasStyle* style);

void         goo_canvas_style_set_stroke_pattern (GooCanvasStyle    *style,
						  cairo_pattern_t   *pattern);
void         goo_canvas_style_set_fill_pattern   (GooCanvasStyle    *style,
						  cairo_pattern_t   *pattern);


G_END_DECLS

#endif /* __GOO_CANVAS_STYLE_H__ */
