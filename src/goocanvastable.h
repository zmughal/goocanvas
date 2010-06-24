/*
 * GooCanvas. Copyright (C) 2005-6 Damon Chaplin.
 * Released under the GNU LGPL license. See COPYING for details.
 *
 * goocanvastable.h - table item.
 */
#ifndef __GOO_CANVAS_TABLE_H__
#define __GOO_CANVAS_TABLE_H__

#include <gtk/gtk.h>
#include "goocanvasgroup.h"

G_BEGIN_DECLS


typedef struct _GooCanvasTableDimension   GooCanvasTableDimension;
struct _GooCanvasTableDimension
{
  gint size;

  gdouble default_spacing;

  /* These are specific spacings for particular rows or columns. A negative
     value indicates that the default should be used. */
  gdouble *spacings;

  guint homogeneous : 1;
};


#define GOO_TYPE_CANVAS_TABLE            (goo_canvas_table_get_type ())
#define GOO_CANVAS_TABLE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOO_TYPE_CANVAS_TABLE, GooCanvasTable))
#define GOO_CANVAS_TABLE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GOO_TYPE_CANVAS_TABLE, GooCanvasTableClass))
#define GOO_IS_CANVAS_TABLE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOO_TYPE_CANVAS_TABLE))
#define GOO_IS_CANVAS_TABLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GOO_TYPE_CANVAS_TABLE))
#define GOO_CANVAS_TABLE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GOO_TYPE_CANVAS_TABLE, GooCanvasTableClass))


typedef struct _GooCanvasTable       GooCanvasTable;
typedef struct _GooCanvasTableClass  GooCanvasTableClass;

typedef struct _GooCanvasTableLayoutData GooCanvasTableLayoutData;

/**
 * GooCanvasTable
 *
 * The #GooCanvasTable-struct struct contains private data only.
 */
struct _GooCanvasTable
{
  GooCanvasGroup parent;

  /* The explicitly set width or height, or -1. */
  gdouble width, height;

  /* One for rows & one for columns. */
  GooCanvasTableDimension dimensions[2];

  gdouble border_width;

  /* An array of GooCanvasTableChild. */
  GArray *children;

  GooCanvasTableLayoutData *layout_data;
};

struct _GooCanvasTableClass
{
  GooCanvasGroupClass parent_class;

  /*< private >*/

  /* Padding for future expansion */
  void (*_goo_canvas_reserved1) (void);
  void (*_goo_canvas_reserved2) (void);
  void (*_goo_canvas_reserved3) (void);
  void (*_goo_canvas_reserved4) (void);
};


GType          goo_canvas_table_get_type    (void) G_GNUC_CONST;
GooCanvasItem* goo_canvas_table_new         (GooCanvasItem  *parent,
					     ...);




G_END_DECLS

#endif /* __GOO_CANVAS_TABLE_H__ */
