/*
 * GooCanvas. Copyright (C) 2005 Damon Chaplin.
 * Released under the GNU LGPL license. See COPYING for details.
 *
 * goocanvasimage.c - image item.
 */

/**
 * SECTION:goocanvasimage
 * @Title: GooCanvasImage
 * @Short_Description: an image item.
 *
 * GooCanvasImage represents an image item.
 *
 * It is a subclass of #GooCanvasItemSimple and so inherits all of the style
 * properties such as "operator" and "pointer-events".
 *
 * It also implements the #GooCanvasItem interface, so you can use the
 * #GooCanvasItem functions such as goo_canvas_item_raise() and
 * goo_canvas_item_rotate().
 *
 * To create a #GooCanvasImage use goo_canvas_image_new().
 *
 * To get or set the properties of an existing #GooCanvasImage, use
 * g_object_get() and g_object_set().
 */
#include <config.h>
#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include "goocanvasprivate.h"
#include "goocanvasimage.h"
#include "goocanvas.h"
#include "goocanvasutils.h"


enum {
  PROP_0,

  PROP_PATTERN,
  PROP_X,
  PROP_Y,
  PROP_WIDTH,
  PROP_HEIGHT,
  PROP_SCALE_TO_FIT,
  PROP_ALPHA,

  /* Convenience properties. */
  PROP_PIXBUF
};


G_DEFINE_TYPE (GooCanvasImage, goo_canvas_image, GOO_TYPE_CANVAS_ITEM_SIMPLE)


static void
goo_canvas_image_init (GooCanvasImage *image)
{
  image->alpha = 1.0;
}


/**
 * goo_canvas_image_new:
 * @parent: the parent item, or %NULL. If a parent is specified, it will assume
 *  ownership of the item, and the item will automatically be freed when it is
 *  removed from the parent. Otherwise call g_object_unref() to free it.
 * @pixbuf: the #GdkPixbuf containing the image data, or %NULL.
 * @x: the x coordinate of the image.
 * @y: the y coordinate of the image.
 * @...: optional pairs of property names and values, and a terminating %NULL.
 * 
 * Creates a new image item.
 * 
 * <!--PARAMETERS-->
 *
 * Here's an example showing how to create an image at (100.0, 100.0), using
 * the given pixbuf at its natural width and height:
 *
 * <informalexample><programlisting>
 *  GooCanvasItem *image = goo_canvas_image_new (mygroup, pixbuf, 100.0, 100.0,
 *                                               NULL);
 * </programlisting></informalexample>
 *
 * Returns: a new image item.
 **/
GooCanvasItem*
goo_canvas_image_new (GooCanvasItem *parent,
		      GdkPixbuf     *pixbuf,
		      gdouble        x,
		      gdouble        y,
		      ...)
{
  GooCanvasItem *item;
  GooCanvasImage *image;
  const char *first_property;
  va_list var_args;

  item = g_object_new (GOO_TYPE_CANVAS_IMAGE, NULL);
  image = (GooCanvasImage*) item;

  image->x = x;
  image->y = y;

  if (pixbuf)
    {
      image->pattern = goo_canvas_cairo_pattern_from_pixbuf (pixbuf);
      image->width = gdk_pixbuf_get_width (pixbuf);
      image->height = gdk_pixbuf_get_height (pixbuf);
    }

  va_start (var_args, y);
  first_property = va_arg (var_args, char*);
  if (first_property)
    g_object_set_valist ((GObject*) item, first_property, var_args);
  va_end (var_args);

  if (parent)
    {
      goo_canvas_item_add_child (parent, item, -1);
      g_object_unref (item);
    }

  return item;
}


static void
goo_canvas_image_dispose (GObject *object)
{
  GooCanvasImage *image = (GooCanvasImage*) object;

  cairo_pattern_destroy (image->pattern);
  image->pattern = NULL;

  G_OBJECT_CLASS (goo_canvas_image_parent_class)->dispose (object);
}


static void
goo_canvas_image_get_property (GObject              *object,
			       guint                 prop_id,
			       GValue               *value,
			       GParamSpec           *pspec)
{
  GooCanvasImage *image = (GooCanvasImage*) object;

  switch (prop_id)
    {
    case PROP_PATTERN:
      g_value_set_boxed (value, image->pattern);
      break;
    case PROP_X:
      g_value_set_double (value, image->x);
      break;
    case PROP_Y:
      g_value_set_double (value, image->y);
      break;
    case PROP_WIDTH:
      g_value_set_double (value, image->width);
      break;
    case PROP_HEIGHT:
      g_value_set_double (value, image->height);
      break;
    case PROP_SCALE_TO_FIT:
      g_value_set_boolean (value, image->scale_to_fit);
      break;
    case PROP_ALPHA:
      g_value_set_double (value, image->alpha);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}


static void
goo_canvas_image_set_property (GObject              *object,
			       guint                 prop_id,
			       const GValue         *value,
			       GParamSpec           *pspec)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) object;
  GooCanvasImage *image = (GooCanvasImage*) object;
  GdkPixbuf *pixbuf;
  gboolean recompute_bounds = TRUE;

  switch (prop_id)
    {
    case PROP_PATTERN:
      cairo_pattern_destroy (image->pattern);
      image->pattern = g_value_get_boxed (value);
      cairo_pattern_reference (image->pattern);
      break;
    case PROP_X:
      image->x = g_value_get_double (value);
      break;
    case PROP_Y:
      image->y = g_value_get_double (value);
      break;
    case PROP_WIDTH:
      image->width = g_value_get_double (value);
      break;
    case PROP_HEIGHT:
      image->height = g_value_get_double (value);
      break;
    case PROP_SCALE_TO_FIT:
      image->scale_to_fit = g_value_get_boolean (value);
      break;
    case PROP_PIXBUF:
      cairo_pattern_destroy (image->pattern);
      pixbuf = g_value_get_object (value);
      image->pattern = pixbuf ? goo_canvas_cairo_pattern_from_pixbuf (pixbuf) : NULL;
      image->width = pixbuf ? gdk_pixbuf_get_width (pixbuf) : 0;
      image->height = pixbuf ? gdk_pixbuf_get_height (pixbuf) : 0;
      break;
    case PROP_ALPHA:
      image->alpha = g_value_get_double (value);
      recompute_bounds = FALSE;
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }

  goo_canvas_item_simple_changed (simple, recompute_bounds);
}


static gboolean
goo_canvas_image_is_item_at (GooCanvasItemSimple *simple,
			     gdouble              x,
			     gdouble              y,
			     cairo_t             *cr,
			     gboolean             is_pointer_event)
{
  GooCanvasImage *image = (GooCanvasImage*) simple;

  if (x < image->x || (x > image->x + image->width)
      || y < image->y || (y > image->y + image->height))
    return FALSE;

  return TRUE;
}


static void
goo_canvas_image_update  (GooCanvasItemSimple  *simple,
			  cairo_t              *cr)
{
  GooCanvasImage *image = (GooCanvasImage*) simple;

  /* Compute the new bounds. */
  simple->bounds.x1 = image->x;
  simple->bounds.y1 = image->y;
  simple->bounds.x2 = image->x + image->width;
  simple->bounds.y2 = image->y + image->height;
}


static void
goo_canvas_image_paint (GooCanvasItemSimple   *simple,
			cairo_t               *cr,
			const GooCanvasBounds *bounds,
			gdouble                scale)
{
  GooCanvasImage *image = (GooCanvasImage*) simple;
  cairo_matrix_t matrix = { 1, 0, 0, 1, 0, 0 };
  cairo_surface_t *surface;
  gdouble width, height;

  if (!image->pattern)
    return;

#if 1
  if (image->scale_to_fit)
    {
      if (cairo_pattern_get_surface (image->pattern, &surface)
	  == CAIRO_STATUS_SUCCESS
	  && cairo_surface_get_type (surface) == CAIRO_SURFACE_TYPE_IMAGE)
	{
	  width = cairo_image_surface_get_width (surface);
	  height = cairo_image_surface_get_height (surface);
	  cairo_matrix_scale (&matrix, width / image->width,
			      height / image->height);
	}
    }

  cairo_matrix_translate (&matrix, -image->x, -image->y);

  cairo_pattern_set_matrix (image->pattern, &matrix);
  goo_canvas_item_simple_set_fill_options (simple, cr);
  cairo_set_source (cr, image->pattern);
  cairo_rectangle (cr, image->x, image->y,
		   image->width, image->height);
  /* To have better performance, we don't use cairo_paint_with_alpha if
   * the image is not transparent at all. */
  if (image->alpha != 1.0)
    cairo_paint_with_alpha (cr, image->alpha);
  else
    cairo_fill (cr);
#else
  /* Using cairo_paint() used to be much slower than cairo_fill(), though
     they seem similar now. I'm not sure if it matters which we use. */
  cairo_matrix_init_translate (&matrix, -image->x, -image->y);
  cairo_pattern_set_matrix (image->pattern, &matrix);
  goo_canvas_style_set_fill_options (simple->style, cr);
  cairo_set_source (cr, image->pattern);
  cairo_paint (cr);
#endif
}


static void
goo_canvas_image_class_init (GooCanvasImageClass *klass)
{
  GObjectClass *gobject_class = (GObjectClass*) klass;
  GooCanvasItemSimpleClass *simple_class = (GooCanvasItemSimpleClass*) klass;

  gobject_class->dispose  = goo_canvas_image_dispose;

  gobject_class->get_property = goo_canvas_image_get_property;
  gobject_class->set_property = goo_canvas_image_set_property;

  simple_class->simple_update      = goo_canvas_image_update;
  simple_class->simple_paint       = goo_canvas_image_paint;
  simple_class->simple_is_item_at  = goo_canvas_image_is_item_at;

  g_object_class_install_property (gobject_class, PROP_PATTERN,
                                   g_param_spec_boxed ("pattern",
						       _("Pattern"),
						       _("The cairo pattern to paint"),
						       GOO_TYPE_CAIRO_PATTERN,
						       G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_X,
				   g_param_spec_double ("x",
							"X",
							_("The x coordinate of the image"),
							-G_MAXDOUBLE,
							G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_Y,
				   g_param_spec_double ("y",
							"Y",
							_("The y coordinate of the image"),
							-G_MAXDOUBLE,
							G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_WIDTH,
				   g_param_spec_double ("width",
							_("Width"),
							_("The width of the image"),
							0.0, G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_HEIGHT,
				   g_param_spec_double ("height",
							_("Height"),
							_("The height of the image"),
							0.0, G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_SCALE_TO_FIT, 
                                   g_param_spec_boolean ("scale-to-fit",
							 _("Scale To Fit"),
							 _("If the image is scaled to fit the width and height settings"),
							 FALSE,
							 G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_ALPHA,
				   g_param_spec_double ("alpha",
							_("Alpha"),
							_("The opacity of the image, 0.0 is fully transparent, and 1.0 is opaque."),
							0.0, 1.0, 1.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_PIXBUF,
				   g_param_spec_object ("pixbuf",
							_("Pixbuf"),
							_("The GdkPixbuf to display"),
							GDK_TYPE_PIXBUF,
							G_PARAM_WRITABLE));
}
