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

  guint stroke_pattern_set	    : 1;
  guint fill_pattern_set	    : 1;
  cairo_operator_t  op              : 6;
  cairo_antialias_t antialias       : 4;
  cairo_fill_rule_t fill_rule       : 3;
  cairo_line_cap_t  line_cap        : 4;
  cairo_line_join_t line_join       : 4;
  guint hint_metrics		    : 2;
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


GType           goo_canvas_style_get_type          (void) G_GNUC_CONST;
GooCanvasStyle* goo_canvas_style_new               (void);

void         goo_canvas_style_set_stroke_pattern (GooCanvasStyle    *style,
						  cairo_pattern_t   *pattern);
void         goo_canvas_style_set_fill_pattern   (GooCanvasStyle    *style,
						  cairo_pattern_t   *pattern);


G_END_DECLS

#endif /* __GOO_CANVAS_STYLE_H__ */
