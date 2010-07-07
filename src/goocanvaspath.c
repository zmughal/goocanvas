/*
 * GooCanvas. Copyright (C) 2005-6 Damon Chaplin.
 * Released under the GNU LGPL license. See COPYING for details.
 *
 * goocanvaspath.c - a path item, very similar to the SVG path element.
 */

/**
 * SECTION:goocanvaspath
 * @Title: GooCanvasPath
 * @Short_Description: a path item (a series of lines and curves).
 *
 * GooCanvasPath represents a path item, which is a series of one or more
 * lines, bezier curves, or elliptical arcs.
 *
 * It is a subclass of #GooCanvasItemSimple and so inherits all of the style
 * properties such as "stroke-color", "fill-color" and "line-width".
 *
 * It also implements the #GooCanvasItem interface, so you can use the
 * #GooCanvasItem functions such as goo_canvas_item_raise() and
 * goo_canvas_item_rotate().
 *
 * #GooCanvasPath uses the same path specification strings as the Scalable
 * Vector Graphics (SVG) path element. For details see the
 * <ulink url="http://www.w3.org/Graphics/SVG/">SVG specification</ulink>.
 *
 * To create a #GooCanvasPath use goo_canvas_path_new().
 *
 * To get or set the properties of an existing #GooCanvasPath, use
 * g_object_get() and g_object_set().
 */
#include <config.h>
#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include "goocanvaspath.h"
#include "goocanvas.h"


enum {
  PROP_0,

  PROP_DATA,

  PROP_X,
  PROP_Y,
  PROP_WIDTH,
  PROP_HEIGHT
};


G_DEFINE_TYPE (GooCanvasPath, goo_canvas_path, GOO_TYPE_CANVAS_ITEM_SIMPLE)


static void
goo_canvas_path_init (GooCanvasPath *path)
{

}


/**
 * goo_canvas_path_new:
 * @parent: the parent item, or %NULL. If a parent is specified, it will assume
 *  ownership of the item, and the item will automatically be freed when it is
 *  removed from the parent. Otherwise call g_object_unref() to free it.
 * @path_data: the sequence of path commands, specified as a string using the
 *  same syntax as in the <ulink url="http://www.w3.org/Graphics/SVG/">Scalable
 *  Vector Graphics (SVG)</ulink> path element.
 * @...: optional pairs of property names and values, and a terminating %NULL.
 * 
 * Creates a new path item.
 * 
 * <!--PARAMETERS-->
 *
 * Here's an example showing how to create a red line from (20,20) to (40,40):
 *
 * <informalexample><programlisting>
 *  GooCanvasItem *path = goo_canvas_path_new (mygroup,
 *                                             "M 20 20 L 40 40",
 *                                             "stroke-color", "red",
 *                                             NULL);
 * </programlisting></informalexample>
 * 
 * This example creates a cubic bezier curve from (20,100) to (100,100) with
 * the control points at (20,50) and (100,50):
 *
 * <informalexample><programlisting>
 *  GooCanvasItem *path = goo_canvas_path_new (mygroup,
 *                                             "M20,100 C20,50 100,50 100,100",
 *                                             "stroke-color", "blue",
 *                                             NULL);
 * </programlisting></informalexample>
 *
 * This example uses an elliptical arc to create a filled circle with one
 * quarter missing:
 * 
 * <informalexample><programlisting>
 *  GooCanvasItem *path = goo_canvas_path_new (mygroup,
 *                                             "M200,500 h-150 a150,150 0 1,0 150,-150 z",
 *                                             "fill-color", "red",
 *                                             "stroke-color", "blue",
 *                                             "line-width", 5.0,
 *                                             NULL);
 * </programlisting></informalexample>
 * 
 * Returns: a new path item.
 **/
GooCanvasItem*
goo_canvas_path_new               (GooCanvasItem *parent,
				   const gchar   *path_data,
				   ...)
{
  GooCanvasItem *item;
  GooCanvasPath *path;
  const char *first_property;
  va_list var_args;

  item = g_object_new (GOO_TYPE_CANVAS_PATH, NULL);
  path = (GooCanvasPath*) item;

  path->path_commands = goo_canvas_parse_path_data (path_data);

  va_start (var_args, path_data);
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
goo_canvas_path_finalize (GObject *object)
{
  GooCanvasPath *path = (GooCanvasPath*) object;

  if (path->path_commands)
    {
      g_array_free (path->path_commands, TRUE);
      path->path_commands = NULL;
    }

  G_OBJECT_CLASS (goo_canvas_path_parent_class)->finalize (object);
}


static void
goo_canvas_path_common_get_extent (GooCanvas            *canvas,
                                   GooCanvasPath        *path,
                                   GooCanvasBounds      *bounds)
{
  cairo_t *cr;

  cr = goo_canvas_create_cairo_context (canvas);
  goo_canvas_create_path (path->path_commands, cr);
  cairo_fill_extents (cr, &bounds->x1, &bounds->y1, &bounds->x2, &bounds->y2);
  cairo_destroy (cr);
}


/* Moves all the absolute points in the command by the given amounts.
   Relative points don't need to be moved. */
static void
goo_canvas_path_move_command (GooCanvasPathCommand *cmd,
			      gdouble               x_offset,
			      gdouble               y_offset)
{
  switch (cmd->simple.type)
    {
    case GOO_CANVAS_PATH_MOVE_TO:
    case GOO_CANVAS_PATH_CLOSE_PATH:
    case GOO_CANVAS_PATH_LINE_TO:
    case GOO_CANVAS_PATH_HORIZONTAL_LINE_TO:
    case GOO_CANVAS_PATH_VERTICAL_LINE_TO:
      if (!cmd->simple.relative)
        {
          cmd->simple.x += x_offset;
          cmd->simple.y += y_offset;
        }
      break;
    case GOO_CANVAS_PATH_CURVE_TO:
    case GOO_CANVAS_PATH_SMOOTH_CURVE_TO:
    case GOO_CANVAS_PATH_QUADRATIC_CURVE_TO:
    case GOO_CANVAS_PATH_SMOOTH_QUADRATIC_CURVE_TO:
      if (!cmd->curve.relative)
        {
          cmd->curve.x += x_offset;
          cmd->curve.y += y_offset;
          cmd->curve.x1 += x_offset;
          cmd->curve.y1 += y_offset;
          cmd->curve.x2 += x_offset;
          cmd->curve.y2 += y_offset;
        }
      break;
    case GOO_CANVAS_PATH_ELLIPTICAL_ARC:
      if (!cmd->arc.relative)
        {
          cmd->arc.x += x_offset;
          cmd->arc.y += y_offset;
        }
      break;
    default:
      g_assert_not_reached();
      break;
    }
}


/* Scales all the points in the command by the given amounts. Absolute points
   are scaled about the given origin. */
static void
goo_canvas_path_scale_command (GooCanvasPathCommand *cmd,
			       gdouble               x_origin,
			       gdouble               y_origin,
			       gdouble               x_scale,
			       gdouble               y_scale)
{
  switch (cmd->simple.type)
    {
    case GOO_CANVAS_PATH_MOVE_TO:
    case GOO_CANVAS_PATH_CLOSE_PATH:
    case GOO_CANVAS_PATH_LINE_TO:
    case GOO_CANVAS_PATH_HORIZONTAL_LINE_TO:
    case GOO_CANVAS_PATH_VERTICAL_LINE_TO:
      if (cmd->simple.relative)
        {
          cmd->simple.x *= x_scale;
          cmd->simple.y *= y_scale;
        }
      else
        {
          cmd->simple.x = x_origin + (cmd->simple.x - x_origin) * x_scale;
          cmd->simple.y = y_origin + (cmd->simple.y - y_origin) * y_scale;
        }
      break;
    case GOO_CANVAS_PATH_CURVE_TO:
    case GOO_CANVAS_PATH_SMOOTH_CURVE_TO:
    case GOO_CANVAS_PATH_QUADRATIC_CURVE_TO:
    case GOO_CANVAS_PATH_SMOOTH_QUADRATIC_CURVE_TO:
      if (cmd->curve.relative)
        {
          cmd->curve.x *= x_scale;
          cmd->curve.y *= y_scale;
          cmd->curve.x1 *= x_scale;
          cmd->curve.y1 *= y_scale;
          cmd->curve.x2 *= x_scale;
          cmd->curve.y2 *= y_scale;
        }
      else
        {
          cmd->curve.x =  x_origin + (cmd->curve.x -  x_origin) * x_scale;
          cmd->curve.y =  y_origin + (cmd->curve.y -  y_origin) * y_scale;
          cmd->curve.x1 = x_origin + (cmd->curve.x1 - x_origin) * x_scale;
          cmd->curve.y1 = y_origin + (cmd->curve.y1 - y_origin) * y_scale;
          cmd->curve.x2 = x_origin + (cmd->curve.x2 - x_origin) * x_scale;
          cmd->curve.y2 = y_origin + (cmd->curve.y2 - y_origin) * y_scale;
        }
      break;
    case GOO_CANVAS_PATH_ELLIPTICAL_ARC:
      if (cmd->arc.relative)
        {
          cmd->arc.x *= x_scale;
          cmd->arc.y *= y_scale;
        }
      else
        {
          cmd->arc.x = x_origin + (cmd->arc.x - x_origin) * x_scale;
          cmd->arc.y = y_origin + (cmd->arc.y - y_origin) * y_scale;
        }
      break;
    default:
      g_assert_not_reached();
      break;
  }
}


static void
goo_canvas_path_get_property (GObject              *object,
			      guint                 prop_id,
			      GValue               *value,
			      GParamSpec           *pspec)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) object;
  GooCanvasPath *path = (GooCanvasPath*) object;
  GooCanvasBounds extent;

  switch (prop_id)
    {
    case PROP_X:
      goo_canvas_path_common_get_extent (simple->canvas, path, &extent);
      g_value_set_double (value, extent.x1);
      break;
    case PROP_Y:
      goo_canvas_path_common_get_extent (simple->canvas, path, &extent);
      g_value_set_double (value, extent.y1);
      break;
    case PROP_WIDTH:
      goo_canvas_path_common_get_extent (simple->canvas, path, &extent);
      g_value_set_double (value, extent.x2 - extent.x1);
      break;
    case PROP_HEIGHT:
      goo_canvas_path_common_get_extent (simple->canvas, path, &extent);
      g_value_set_double (value, extent.y2 - extent.y1);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}


static void
goo_canvas_path_set_property (GObject              *object,
			      guint                 prop_id,
			      const GValue         *value,
			      GParamSpec           *pspec)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) object;
  GooCanvasPath *path = (GooCanvasPath*) object;
  GooCanvasBounds extent;
  GooCanvasPathCommand *cmd;
  gdouble x_offset, y_offset, x_scale, y_scale;
  guint i;

  switch (prop_id)
    {
    case PROP_DATA:
      if (path->path_commands)
	g_array_free (path->path_commands, TRUE);
      path->path_commands = goo_canvas_parse_path_data (g_value_get_string (value));
      g_object_notify (object, "x");
      g_object_notify (object, "y");
      g_object_notify (object, "width");
      g_object_notify (object, "height");
      break;
    case PROP_X:
      if (path->path_commands->len > 0)
        {
	  /* Calculate the x offset from the current position. */
          goo_canvas_path_common_get_extent (simple->canvas, path, &extent);
          x_offset = g_value_get_double (value) - extent.x1;

	  /* Add the offset to all the absolute x coordinates. */
          for (i = 0; i < path->path_commands->len; i++)
            {
              cmd = &g_array_index (path->path_commands,
				    GooCanvasPathCommand, i);
              goo_canvas_path_move_command (cmd, x_offset, 0.0);
            }
          g_object_notify (object, "data");
        }
      break;
    case PROP_Y:
      if (path->path_commands->len > 0)
        {
	  /* Calculate the y offset from the current position. */
          goo_canvas_path_common_get_extent (simple->canvas, path, &extent);
          y_offset = g_value_get_double (value) - extent.y1;

	  /* Add the offset to all the absolute y coordinates. */
          for (i = 0; i < path->path_commands->len; i++)
            {
              cmd = &g_array_index (path->path_commands,
				    GooCanvasPathCommand, i);
              goo_canvas_path_move_command (cmd, 0.0, y_offset);
            }
          g_object_notify (object, "data");
        }
      break;
    case PROP_WIDTH:
      if (path->path_commands->len >= 2)
        {
          goo_canvas_path_common_get_extent (simple->canvas, path, &extent);
          if (extent.x2 - extent.x1 != 0.0)
            {
	      /* Calculate the amount to scale the path. */
              x_scale = g_value_get_double (value) / (extent.x2 - extent.x1);

	      /* Scale the x coordinates, relative to the left-most point. */
              for (i = 0; i < path->path_commands->len; i++)
                {
                  cmd = &g_array_index (path->path_commands,
					GooCanvasPathCommand, i);
                  goo_canvas_path_scale_command (cmd, extent.x1, 0.0,
						 x_scale, 1.0);
                }
              g_object_notify (object, "data");
            }
        }
      break;
    case PROP_HEIGHT:
      if (path->path_commands->len >= 2)
        {
          goo_canvas_path_common_get_extent (simple->canvas, path, &extent);
          if (extent.y2 - extent.y1 != 0.0)
            {
	      /* Calculate the amount to scale the polyline. */
              y_scale = g_value_get_double (value) / (extent.y2 - extent.y1);

	      /* Scale the y coordinates, relative to the top-most point. */
              for (i = 0; i < path->path_commands->len; i++)
                {
                  cmd = &g_array_index (path->path_commands,
					GooCanvasPathCommand, i);
                  goo_canvas_path_scale_command (cmd, 0.0, extent.y1,
						 1.0, y_scale);
                }
              g_object_notify (object, "data");
            }
        }
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }

  goo_canvas_item_simple_changed (simple, TRUE);
}


static void
goo_canvas_path_create_path (GooCanvasItemSimple *simple,
			     cairo_t             *cr)
{
  GooCanvasPath *path = (GooCanvasPath*) simple;

  goo_canvas_create_path (path->path_commands, cr);
}


static gboolean
goo_canvas_path_is_item_at (GooCanvasItemSimple *simple,
			    gdouble              x,
			    gdouble              y,
			    cairo_t             *cr,
			    gboolean             is_pointer_event)
{
  GooCanvasPointerEvents pointer_events = GOO_CANVAS_EVENTS_ALL;
  gboolean do_fill;

  /* By default only check the fill if a fill color/pattern is specified. */
  do_fill = goo_canvas_item_simple_set_fill_options (simple, cr);
  if (!do_fill)
    pointer_events &= ~GOO_CANVAS_EVENTS_FILL_MASK;

  /* If is_pointer_event is set use the pointer_events property instead. */
  if (is_pointer_event)
    pointer_events = simple->pointer_events;

  goo_canvas_path_create_path (simple, cr);
  if (goo_canvas_item_simple_check_in_path (simple, x, y, cr, pointer_events,
					    simple->canvas->scale))
    return TRUE;

  return FALSE;
}


static void
goo_canvas_path_class_init (GooCanvasPathClass *klass)
{
  GObjectClass *gobject_class = (GObjectClass*) klass;
  GooCanvasItemSimpleClass *simple_class = (GooCanvasItemSimpleClass*) klass;

  gobject_class->finalize     = goo_canvas_path_finalize;

  gobject_class->get_property = goo_canvas_path_get_property;
  gobject_class->set_property = goo_canvas_path_set_property;

  simple_class->simple_create_path = goo_canvas_path_create_path;
  simple_class->simple_is_item_at  = goo_canvas_path_is_item_at;

  /**
   * GooCanvasPath:data
   *
   * The sequence of path commands, specified as a string using the same syntax
   * as in the <ulink url="http://www.w3.org/Graphics/SVG/">Scalable Vector
   * Graphics (SVG)</ulink> path element.
   */
  g_object_class_install_property (gobject_class, PROP_DATA,
				   g_param_spec_string ("data",
							_("Path Data"),
							_("The sequence of path commands"),
							NULL,
							G_PARAM_WRITABLE));

  g_object_class_install_property (gobject_class, PROP_X,
				   g_param_spec_double ("x",
							"X",
							_("The x coordinate of the path"),
							-G_MAXDOUBLE,
							G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_Y,
				   g_param_spec_double ("y",
							"Y",
							_("The y coordinate of the path"),
							-G_MAXDOUBLE,
							G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_WIDTH,
				   g_param_spec_double ("width",
							_("Width"),
							_("The width of the path"),
							0.0, G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_HEIGHT,
				   g_param_spec_double ("height",
							_("Height"),
							_("The height of the path"),
							0.0, G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));
}
