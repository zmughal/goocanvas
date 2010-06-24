/*
 * GooCanvas. Copyright (C) 2005 Damon Chaplin.
 * Released under the GNU LGPL license. See COPYING for details.
 *
 * goocanvasellipse.c - ellipse item.
 */

/**
 * SECTION:goocanvasellipse
 * @Title: GooCanvasEllipse
 * @Short_Description: an ellipse item.
 *
 * GooCanvasEllipse represents an ellipse item.
 *
 * It is a subclass of #GooCanvasItemSimple and so inherits all of the style
 * properties such as "stroke-color", "fill-color" and "line-width".
 *
 * It also implements the #GooCanvasItem interface, so you can use the
 * #GooCanvasItem functions such as goo_canvas_item_raise() and
 * goo_canvas_item_rotate().
 *
 * To create a #GooCanvasEllipse use goo_canvas_ellipse_new().
 *
 * To get or set the properties of an existing #GooCanvasEllipse, use
 * g_object_get() and g_object_set().
 *
 * The ellipse can be specified either with the "center-x", "center-y",
 * "radius-x" and "radius-y" properties, or with the "x", "y", "width" and
 * "height" properties.
 */
#include <config.h>
#include <math.h>
#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include "goocanvasellipse.h"


enum {
  PROP_0,

  PROP_CENTER_X,
  PROP_CENTER_Y,
  PROP_RADIUS_X,
  PROP_RADIUS_Y,

  PROP_X,
  PROP_Y,
  PROP_WIDTH,
  PROP_HEIGHT
};


G_DEFINE_TYPE (GooCanvasEllipse, goo_canvas_ellipse, GOO_TYPE_CANVAS_ITEM_SIMPLE)


static void
goo_canvas_ellipse_init (GooCanvasEllipse *ellipse)
{

}


/**
 * goo_canvas_ellipse_new:
 * @parent: the parent item, or %NULL. If a parent is specified, it will assume
 *  ownership of the item, and the item will automatically be freed when it is
 *  removed from the parent. Otherwise call g_object_unref() to free it.
 * @center_x: the x coordinate of the center of the ellipse.
 * @center_y: the y coordinate of the center of the ellipse.
 * @radius_x: the horizontal radius of the ellipse.
 * @radius_y: the vertical radius of the ellipse.
 * @...: optional pairs of property names and values, and a terminating %NULL.
 * 
 * Creates a new ellipse item.
 *
 * <!--PARAMETERS-->
 *
 * Here's an example showing how to create an ellipse centered at (100.0,
 * 100.0), with a horizontal radius of 50.0 and a vertical radius of 30.0.
 * It is drawn with a red outline with a width of 5.0 and filled with blue:
 *
 * <informalexample><programlisting>
 *  GooCanvasItem *ellipse = goo_canvas_ellipse_new (mygroup, 100.0, 100.0, 50.0, 30.0,
 *                                                   "stroke-color", "red",
 *                                                   "line-width", 5.0,
 *                                                   "fill-color", "blue",
 *                                                   NULL);
 * </programlisting></informalexample>
 * 
 * Returns: a new ellipse item.
 **/
GooCanvasItem*
goo_canvas_ellipse_new (GooCanvasItem *parent,
			gdouble        center_x,
			gdouble        center_y,
			gdouble        radius_x,
			gdouble        radius_y,
			...)
{
  GooCanvasItem *item;
  GooCanvasEllipse *ellipse;
  const char *first_property;
  va_list var_args;

  item = g_object_new (GOO_TYPE_CANVAS_ELLIPSE, NULL);
  ellipse = (GooCanvasEllipse*) item;

  ellipse->center_x = center_x;
  ellipse->center_y = center_y;
  ellipse->radius_x = radius_x;
  ellipse->radius_y = radius_y;

  va_start (var_args, radius_y);
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
goo_canvas_ellipse_get_property (GObject              *object,
				 guint                 prop_id,
				 GValue               *value,
				 GParamSpec           *pspec)
{
  GooCanvasEllipse *ellipse = (GooCanvasEllipse*) object;

  switch (prop_id)
    {
    case PROP_CENTER_X:
      g_value_set_double (value, ellipse->center_x);
      break;
    case PROP_CENTER_Y:
      g_value_set_double (value, ellipse->center_y);
      break;
    case PROP_RADIUS_X:
      g_value_set_double (value, ellipse->radius_x);
      break;
    case PROP_RADIUS_Y:
      g_value_set_double (value, ellipse->radius_y);
      break;
    case PROP_X:
      g_value_set_double (value, ellipse->center_x - ellipse->radius_x);
      break;
    case PROP_Y:
      g_value_set_double (value, ellipse->center_y - ellipse->radius_y);
      break;
    case PROP_WIDTH:
      g_value_set_double (value, 2.0 * ellipse->radius_x);
      break;
    case PROP_HEIGHT:
      g_value_set_double (value, 2.0 * ellipse->radius_y);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


static void
goo_canvas_ellipse_set_property (GObject              *object,
				 guint                 prop_id,
				 const GValue         *value,
				 GParamSpec           *pspec)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) object;
  GooCanvasEllipse *ellipse = (GooCanvasEllipse*) object;
  gdouble x, y;

  switch (prop_id)
    {
    case PROP_CENTER_X:
      ellipse->center_x = g_value_get_double (value);
      g_object_notify (object, "x");
      break;
    case PROP_CENTER_Y:
      ellipse->center_y = g_value_get_double (value);
      g_object_notify (object, "y");
      break;
    case PROP_RADIUS_X:
      ellipse->radius_x = g_value_get_double (value);
      g_object_notify (object, "width");
      break;
    case PROP_RADIUS_Y:
      ellipse->radius_y = g_value_get_double (value);
      g_object_notify (object, "height");
      break;
    case PROP_X:
      ellipse->center_x = g_value_get_double (value) + ellipse->radius_x;
      g_object_notify (object, "center-x");
      break;
    case PROP_Y:
      ellipse->center_y = g_value_get_double (value) + ellipse->radius_y;
      g_object_notify (object, "center-y");
      break;
    case PROP_WIDTH:
      /* Calculate the current x coordinate. */
      x = ellipse->center_x - ellipse->radius_x;
      /* Calculate the new radius_x, which is half the width. */
      ellipse->radius_x = g_value_get_double (value) / 2.0;
      /* Now calculate the new center_x. */
      ellipse->center_x = x + ellipse->radius_x;

      g_object_notify (object, "center-x");
      g_object_notify (object, "radius-x");
      break;
    case PROP_HEIGHT:
      /* Calculate the current y coordinate. */
      y = ellipse->center_y - ellipse->radius_y;
      /* Calculate the new radius_y, which is half the height. */
      ellipse->radius_y = g_value_get_double (value) / 2.0;
      /* Now calculate the new center_y. */
      ellipse->center_y = y + ellipse->radius_y;

      g_object_notify (object, "center-y");
      g_object_notify (object, "radius-y");
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }

  goo_canvas_item_simple_changed (simple, TRUE);
}


static void
goo_canvas_ellipse_create_path (GooCanvasItemSimple *simple,
				cairo_t             *cr)
{
  GooCanvasEllipse *ellipse = (GooCanvasEllipse*) simple;

  cairo_new_path (cr);
  cairo_save (cr);
  cairo_translate (cr, ellipse->center_x, ellipse->center_y);
  cairo_scale (cr, ellipse->radius_x, ellipse->radius_y);
  cairo_arc (cr, 0.0, 0.0, 1.0, 0.0, 2.0 * M_PI);
  cairo_restore (cr);
}


static void
goo_canvas_ellipse_class_init (GooCanvasEllipseClass *klass)
{
  GObjectClass *gobject_class = (GObjectClass*) klass;
  GooCanvasItemSimpleClass *simple_class = (GooCanvasItemSimpleClass*) klass;

  gobject_class->get_property = goo_canvas_ellipse_get_property;
  gobject_class->set_property = goo_canvas_ellipse_set_property;

  simple_class->simple_create_path = goo_canvas_ellipse_create_path;

  g_object_class_install_property (gobject_class, PROP_CENTER_X,
				   g_param_spec_double ("center-x",
							_("Center X"),
							_("The x coordinate of the center of the ellipse"),
							-G_MAXDOUBLE,
							G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_CENTER_Y,
				   g_param_spec_double ("center-y",
							_("Center Y"),
							_("The y coordinate of the center of the ellipse"),
							-G_MAXDOUBLE,
							G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_RADIUS_X,
				   g_param_spec_double ("radius-x",
							_("Radius X"),
							_("The horizontal radius of the ellipse"),
							0.0, G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_RADIUS_Y,
				   g_param_spec_double ("radius-y",
							_("Radius Y"),
							_("The vertical radius of the ellipse"),
							0.0, G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_X,
				   g_param_spec_double ("x",
							"X",
							_("The x coordinate of the left side of the ellipse"),
							-G_MAXDOUBLE,
							G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_Y,
				   g_param_spec_double ("y",
							"Y",
							_("The y coordinate of the top of the ellipse"),
							-G_MAXDOUBLE,
							G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_WIDTH,
				   g_param_spec_double ("width",
							_("Width"),
							_("The width of the ellipse"),
							0.0, G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_HEIGHT,
				   g_param_spec_double ("height",
							_("Height"),
							_("The height of the ellipse"),
							0.0, G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));
}
