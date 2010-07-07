/*
 * GooCanvas. Copyright (C) 2005-8 Damon Chaplin.
 * Released under the GNU LGPL license. See COPYING for details.
 *
 * goocanvasgrid.c - a grid item.
 */

/**
 * SECTION:goocanvasgrid
 * @Title: GooCanvasGrid
 * @Short_Description: a grid item.
 *
 * GooCanvasGrid represents a grid item.
 * A grid consists of a number of equally-spaced horizontal and vertical
 * grid lines, plus an optional border.
 *
 * It is a subclass of #GooCanvasItemSimple and so inherits all of the style
 * properties such as "stroke-color", "fill-color" and "line-width".
 *
 * It also implements the #GooCanvasItem interface, so you can use the
 * #GooCanvasItem functions such as goo_canvas_item_raise() and
 * goo_canvas_item_rotate().
 *
 * To create a #GooCanvasGrid use goo_canvas_grid_new().
 *
 * To get or set the properties of an existing #GooCanvasGrid, use
 * g_object_get() and g_object_set().
 *
 * The grid's position and size is specified with the #GooCanvasGrid:x,
 * #GooCanvasGrid:y, #GooCanvasGrid:width and #GooCanvasGrid:height properties.
 *
 * The #GooCanvasGrid:x-step and #GooCanvasGrid:y-step properties specify the 
 * distance between grid lines. The  #GooCanvasGrid:x-offset and
 * #GooCanvasGrid:y-offset properties specify the distance before the first
 * grid lines.
 *
 * The horizontal or vertical grid lines can be hidden using the
 * #GooCanvasGrid:show-horz-grid-lines and #GooCanvasGrid:show-vert-grid-lines
 * properties.
 *
 * The width of the border can be set using the #GooCanvasGrid:border-width
 * property. The border is drawn outside the area specified with the
 * #GooCanvasGrid:x, #GooCanvasGrid:y, #GooCanvasGrid:width and
 * #GooCanvasGrid:height properties.
 *
 * Other properties allow the colors and widths of the grid lines to be set.
 * The grid line color and width properties override the standard
 * #GooCanvasItemSimple:stroke-color and #GooCanvasItemSimple:line-width
 * properties, enabling different styles for horizontal and vertical grid lines.
 */
#include <config.h>
#include <math.h>
#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include "goocanvasprivate.h"
#include "goocanvas.h"


enum {
  PROP_0,

  PROP_X,
  PROP_Y,
  PROP_WIDTH,
  PROP_HEIGHT,
  PROP_X_STEP,
  PROP_Y_STEP,
  PROP_X_OFFSET,
  PROP_Y_OFFSET,
  PROP_HORZ_GRID_LINE_WIDTH,
  PROP_VERT_GRID_LINE_WIDTH,
  PROP_HORZ_GRID_LINE_PATTERN,
  PROP_VERT_GRID_LINE_PATTERN,
  PROP_BORDER_WIDTH,
  PROP_BORDER_PATTERN,
  PROP_SHOW_HORZ_GRID_LINES,
  PROP_SHOW_VERT_GRID_LINES,
  PROP_VERT_GRID_LINES_ON_TOP,

  /* Convenience properties. */
  PROP_HORZ_GRID_LINE_COLOR,
  PROP_HORZ_GRID_LINE_COLOR_RGBA,
  PROP_HORZ_GRID_LINE_PIXBUF,
  PROP_VERT_GRID_LINE_COLOR,
  PROP_VERT_GRID_LINE_COLOR_RGBA,
  PROP_VERT_GRID_LINE_PIXBUF,
  PROP_BORDER_COLOR,
  PROP_BORDER_COLOR_RGBA,
  PROP_BORDER_PIXBUF
};


G_DEFINE_TYPE (GooCanvasGrid, goo_canvas_grid, GOO_TYPE_CANVAS_ITEM_SIMPLE)


static void
goo_canvas_grid_init (GooCanvasGrid *grid)
{
  grid->x = 0.0;
  grid->y = 0.0;
  grid->width = 0.0;
  grid->height = 0.0;
  grid->x_step = 10.0;
  grid->y_step = 10.0;
  grid->x_offset = 0.0;
  grid->y_offset = 0.0;
  grid->horz_grid_line_width = -1.0;
  grid->vert_grid_line_width = -1.0;
  grid->horz_grid_line_pattern = NULL;
  grid->vert_grid_line_pattern = NULL;
  grid->border_width = -1.0;
  grid->border_pattern = NULL;
  grid->show_horz_grid_lines = TRUE;
  grid->show_vert_grid_lines = TRUE;
  grid->vert_grid_lines_on_top = FALSE;
}


/**
 * goo_canvas_grid_new:
 * @parent: the parent item, or %NULL. If a parent is specified, it will assume
 *  ownership of the item, and the item will automatically be freed when it is
 *  removed from the parent. Otherwise call g_object_unref() to free it.
 * @x: the x coordinate of the left of the grid.
 * @y: the y coordinate of the top of the grid.
 * @width: the width of the grid.
 * @height: the height of the grid.
 * @x_step: the distance between the vertical grid lines.
 * @y_step: the distance between the horizontal grid lines.
 * @x_offset: the distance before the first vertical grid line.
 * @y_offset: the distance before the first horizontal grid line.
 * @...: optional pairs of property names and values, and a terminating %NULL.
 * 
 * Creates a new grid item.
 *
 * <!--PARAMETERS-->
 *
 * Here's an example showing how to create a grid:
 *
 * <informalexample><programlisting>
 *  GooCanvasItem *grid = goo_canvas_grid_new (mygroup, 100.0, 100.0, 400.0, 200.0,
 *                                             20.0, 20.0, 10.0, 10.0,
 *                                             "horz-grid-line-width", 4.0,
 *                                             "horz-grid-line-color", "yellow",
 *                                             "vert-grid-line-width", 2.0,
 *                                             "vert-grid-line-color", "red",
 *                                             "border-width", 3.0,
 *                                             "border-color", "white",
 *                                             "fill-color", "blue",
 *                                             NULL);
 * </programlisting></informalexample>
 * 
 * Returns: a new grid item.
 **/
GooCanvasItem*
goo_canvas_grid_new (GooCanvasItem      *parent,
		     gdouble             x,
		     gdouble             y,
		     gdouble             width,
		     gdouble             height,
		     gdouble             x_step,
		     gdouble             y_step,
		     gdouble             x_offset,
		     gdouble             y_offset,
		     ...)
{
  GooCanvasItem *item;
  GooCanvasGrid *grid;
  va_list var_args;
  const char *first_property;

  item = g_object_new (GOO_TYPE_CANVAS_GRID, NULL);
  grid = (GooCanvasGrid*) item;

  grid->x = x;
  grid->y = y;
  grid->width = width;
  grid->height = height;
  grid->x_step = x_step;
  grid->y_step = y_step;
  grid->x_offset = x_offset;
  grid->y_offset = y_offset;

  va_start (var_args, y_offset);
  first_property = va_arg (var_args, char*);
  if (first_property)
    g_object_set_valist (G_OBJECT (item), first_property, var_args);
  va_end (var_args);

  if (parent)
    {
      goo_canvas_item_add_child (parent, item, -1);
      g_object_unref (item);
    }

  return item;
}


static void
goo_canvas_grid_get_property (GObject              *object,
			      guint                 prop_id,
			      GValue               *value,
			      GParamSpec           *pspec)
{
  GooCanvasGrid *grid = (GooCanvasGrid*) object;

  switch (prop_id)
    {
    case PROP_X:
      g_value_set_double (value, grid->x);
      break;
    case PROP_Y:
      g_value_set_double (value, grid->y);
      break;
    case PROP_WIDTH:
      g_value_set_double (value, grid->width);
      break;
    case PROP_HEIGHT:
      g_value_set_double (value, grid->height);
      break;
    case PROP_X_STEP:
      g_value_set_double (value, grid->x_step);
      break;
    case PROP_Y_STEP:
      g_value_set_double (value, grid->y_step);
      break;
    case PROP_X_OFFSET:
      g_value_set_double (value, grid->x_offset);
      break;
    case PROP_Y_OFFSET:
      g_value_set_double (value, grid->y_offset);
      break;
    case PROP_HORZ_GRID_LINE_WIDTH:
      g_value_set_double (value, grid->horz_grid_line_width);
      break;
    case PROP_VERT_GRID_LINE_WIDTH:
      g_value_set_double (value, grid->vert_grid_line_width);
      break;
    case PROP_HORZ_GRID_LINE_PATTERN:
      g_value_set_boxed (value, grid->horz_grid_line_pattern);
      break;
    case PROP_VERT_GRID_LINE_PATTERN:
      g_value_set_boxed (value, grid->vert_grid_line_pattern);
      break;
    case PROP_BORDER_WIDTH:
      g_value_set_double (value, grid->border_width);
      break;
    case PROP_BORDER_PATTERN:
      g_value_set_boxed (value, grid->border_pattern);
      break;
    case PROP_SHOW_HORZ_GRID_LINES:
      g_value_set_boolean (value, grid->show_horz_grid_lines);
      break;
    case PROP_SHOW_VERT_GRID_LINES:
      g_value_set_boolean (value, grid->show_vert_grid_lines);
      break;
    case PROP_VERT_GRID_LINES_ON_TOP:
      g_value_set_boolean (value, grid->vert_grid_lines_on_top);
      break;

  /* Convenience properties. */
    case PROP_HORZ_GRID_LINE_COLOR_RGBA:
      goo_canvas_get_rgba_value_from_pattern (grid->horz_grid_line_pattern, value);
      break;
    case PROP_VERT_GRID_LINE_COLOR_RGBA:
      goo_canvas_get_rgba_value_from_pattern (grid->vert_grid_line_pattern, value);
      break;
    case PROP_BORDER_COLOR_RGBA:
      goo_canvas_get_rgba_value_from_pattern (grid->border_pattern, value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


static void
goo_canvas_grid_set_property (GObject              *object,
			      guint                 prop_id,
			      const GValue         *value,
			      GParamSpec           *pspec)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) object;
  GooCanvasGrid *grid = (GooCanvasGrid*) object;

  switch (prop_id)
    {
    case PROP_X:
      grid->x = g_value_get_double (value);
      break;
    case PROP_Y:
      grid->y = g_value_get_double (value);
      break;
    case PROP_WIDTH:
      grid->width = g_value_get_double (value);
      break;
    case PROP_HEIGHT:
      grid->height = g_value_get_double (value);
      break;
    case PROP_X_STEP:
      grid->x_step = g_value_get_double (value);
      break;
    case PROP_Y_STEP:
      grid->y_step = g_value_get_double (value);
      break;
    case PROP_X_OFFSET:
      grid->x_offset = g_value_get_double (value);
      break;
    case PROP_Y_OFFSET:
      grid->y_offset = g_value_get_double (value);
      break;
    case PROP_HORZ_GRID_LINE_WIDTH:
      grid->horz_grid_line_width = g_value_get_double (value);
      break;
    case PROP_VERT_GRID_LINE_WIDTH:
      grid->vert_grid_line_width = g_value_get_double (value);
      break;
    case PROP_HORZ_GRID_LINE_PATTERN:
      cairo_pattern_destroy (grid->horz_grid_line_pattern);
      grid->horz_grid_line_pattern = g_value_get_boxed (value);
      cairo_pattern_reference (grid->horz_grid_line_pattern);
      break;
    case PROP_VERT_GRID_LINE_PATTERN:
      cairo_pattern_destroy (grid->vert_grid_line_pattern);
      grid->vert_grid_line_pattern = g_value_get_boxed (value);
      cairo_pattern_reference (grid->vert_grid_line_pattern);
      break;
    case PROP_BORDER_WIDTH:
      grid->border_width = g_value_get_double (value);
      break;
    case PROP_BORDER_PATTERN:
      cairo_pattern_destroy (grid->border_pattern);
      grid->border_pattern = g_value_get_boxed (value);
      cairo_pattern_reference (grid->border_pattern);
      break;
    case PROP_SHOW_HORZ_GRID_LINES:
      grid->show_horz_grid_lines = g_value_get_boolean (value);
      break;
    case PROP_SHOW_VERT_GRID_LINES:
      grid->show_vert_grid_lines = g_value_get_boolean (value);
      break;
    case PROP_VERT_GRID_LINES_ON_TOP:
      grid->vert_grid_lines_on_top = g_value_get_boolean (value);
      break;

  /* Convenience properties. */
    case PROP_HORZ_GRID_LINE_COLOR:
      cairo_pattern_destroy (grid->horz_grid_line_pattern);
      grid->horz_grid_line_pattern = goo_canvas_create_pattern_from_color_value (value);
      break;
    case PROP_HORZ_GRID_LINE_COLOR_RGBA:
      cairo_pattern_destroy (grid->horz_grid_line_pattern);
      grid->horz_grid_line_pattern = goo_canvas_create_pattern_from_rgba_value (value);
      break;
    case PROP_HORZ_GRID_LINE_PIXBUF:
      cairo_pattern_destroy (grid->horz_grid_line_pattern);
      grid->horz_grid_line_pattern = goo_canvas_create_pattern_from_pixbuf_value (value);
      break;

    case PROP_VERT_GRID_LINE_COLOR:
      cairo_pattern_destroy (grid->vert_grid_line_pattern);
      grid->vert_grid_line_pattern = goo_canvas_create_pattern_from_color_value (value);
      break;
    case PROP_VERT_GRID_LINE_COLOR_RGBA:
      cairo_pattern_destroy (grid->vert_grid_line_pattern);
      grid->vert_grid_line_pattern = goo_canvas_create_pattern_from_rgba_value (value);
      break;
    case PROP_VERT_GRID_LINE_PIXBUF:
      cairo_pattern_destroy (grid->vert_grid_line_pattern);
      grid->vert_grid_line_pattern = goo_canvas_create_pattern_from_pixbuf_value (value);
      break;

    case PROP_BORDER_COLOR:
      cairo_pattern_destroy (grid->border_pattern);
      grid->border_pattern = goo_canvas_create_pattern_from_color_value (value);
      break;
    case PROP_BORDER_COLOR_RGBA:
      cairo_pattern_destroy (grid->border_pattern);
      grid->border_pattern = goo_canvas_create_pattern_from_rgba_value (value);
      break;
    case PROP_BORDER_PIXBUF:
      cairo_pattern_destroy (grid->border_pattern);
      grid->border_pattern = goo_canvas_create_pattern_from_pixbuf_value (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }

  goo_canvas_item_simple_changed (simple, TRUE);
}


static void
goo_canvas_grid_update  (GooCanvasItemSimple *simple,
			 cairo_t             *cr)
{
  GooCanvasGrid *grid = (GooCanvasGrid*) simple;
  gdouble border_width = 0.0;

  /* We can quickly compute the bounds as being just the grid's size
     plus the border width around each edge. */
  if (grid->border_width > 0.0)
    border_width = grid->border_width;

  simple->bounds.x1 = grid->x - border_width;
  simple->bounds.y1 = grid->y - border_width;
  simple->bounds.x2 = grid->x + grid->width + border_width;
  simple->bounds.y2 = grid->y + grid->height + border_width;
}


static gdouble
calculate_start_position (gdouble start_pos,
			  gdouble step,
			  gdouble redraw_start_pos,
			  gdouble line_width)
{
  gdouble n = 0.0, result;

  /* We want the first position where pos + line_width/2 >= redraw_start_pos.
     i.e. start_pos + (n * step) + (line_width / 2) >= redraw_start_pos,
     or   (n * step) >= redraw_start_pos - start_pos - (line_width / 2),
     or   n >= (redraw_start_pos - start_pos - (line_width / 2) / step). */
  if (step > 0.0)
    n = ceil (((redraw_start_pos - start_pos - (line_width / 2.0))) / step);

  if (n <= 0.0)
    result = start_pos;
  else
    result = start_pos + (n * step);

  return result;
}


static void
paint_vertical_lines (GooCanvasItemSimple   *simple,
		      cairo_t               *cr,
		      const GooCanvasBounds *bounds,
		      gdouble                scale)
{
  GooCanvasGrid *grid = (GooCanvasGrid*) simple;
  double x, max_x, max_y, max_bounds_x, line_width;
  gboolean has_stroke;

  if (!grid->show_vert_grid_lines)
    return;

  max_x = grid->x + grid->width;
  max_y = grid->y + grid->height;

  has_stroke = goo_canvas_item_simple_set_stroke_options (simple, cr, GOO_CANVAS_OPERATION_PAINT, scale);
  line_width = goo_canvas_item_simple_get_line_width (simple);

  /* If the grid's vertical grid line pattern/color has been set, use that.
     If not, and we don't have a stroke color just return. */
  if (grid->vert_grid_line_pattern)
    cairo_set_source (cr, grid->vert_grid_line_pattern);
  else if (!has_stroke)
    return;

  /* If the grid's vertical grid line width has been set, use that. */
  if (grid->vert_grid_line_width > 0.0)
    {
      line_width = grid->vert_grid_line_width;
      cairo_set_line_width (cr, line_width);
    }

  cairo_set_line_cap (cr, CAIRO_LINE_CAP_BUTT);

  /* Calculate the first grid line that intersects the bounds to redraw. */
  x = calculate_start_position (grid->x + grid->x_offset,
				grid->x_step, bounds->x1, line_width);

  /* Calculate the last possible line position. */
  max_bounds_x = bounds->x2 + (line_width / 2.0);
  max_x = MIN (max_x, max_bounds_x);

  /* Add on a tiny fraction of step to avoid any double comparison issues. */
  max_x += grid->x_step * 0.00001;

  while (x <= max_x)
    {
      cairo_move_to (cr, x, grid->y);
      cairo_line_to (cr, x, max_y);
      cairo_stroke (cr);

      /* Avoid an infinite loop. */
      if (grid->x_step <= 0.0)
        break;

      x += grid->x_step;
    }
}


static void
paint_horizontal_lines (GooCanvasItemSimple   *simple,
			cairo_t               *cr,
			const GooCanvasBounds *bounds,
			gdouble                scale)
{
  GooCanvasGrid *grid = (GooCanvasGrid*) simple;
  double y, max_x, max_y, max_bounds_y, line_width;
  gboolean has_stroke;

  if (!grid->show_horz_grid_lines)
    return;

  max_x = grid->x + grid->width;
  max_y = grid->y + grid->height;

  has_stroke = goo_canvas_item_simple_set_stroke_options (simple, cr, GOO_CANVAS_OPERATION_PAINT, scale);
  line_width = goo_canvas_item_simple_get_line_width (simple);

  /* If the grid's horizontal grid line pattern/color has been set, use that.
     If not, and we don't have a stroke color just return. */
  if (grid->horz_grid_line_pattern)
    cairo_set_source (cr, grid->horz_grid_line_pattern);
  else if (!has_stroke)
    return;

  /* If the grid's horizontal grid line width has been set, use that. */
  if (grid->horz_grid_line_width > 0.0)
    {
      line_width = grid->horz_grid_line_width;
      cairo_set_line_width (cr, grid->horz_grid_line_width);
    }

  cairo_set_line_cap (cr, CAIRO_LINE_CAP_BUTT);

  /* Calculate the first grid line that intersects the bounds to redraw. */
  y = calculate_start_position (grid->y + grid->y_offset,
				grid->y_step, bounds->y1, line_width);

  /* Calculate the last possible line position. */
  max_bounds_y = bounds->y2 + (line_width / 2.0);
  max_y = MIN (max_y, max_bounds_y);

  /* Add on a tiny fraction of step to avoid any double comparison issues. */
  max_y += grid->y_step * 0.00001;

  while (y <= max_y)
    {
      cairo_move_to (cr, grid->x, y);
      cairo_line_to (cr, max_x, y);
      cairo_stroke (cr);

      /* Avoid an infinite loop. */
      if (grid->y_step <= 0.0)
        break;

      y += grid->y_step;
    }
}


static void
goo_canvas_grid_paint (GooCanvasItemSimple   *simple,
		       cairo_t               *cr,
		       const GooCanvasBounds *bounds,
		       gdouble                scale)
{
  GooCanvasGrid *grid = (GooCanvasGrid*) simple;
  GooCanvasBounds redraw_bounds = *bounds;
  gdouble half_border_width;

  /* Paint the background in the fill pattern/color, if one is set. */
  if (goo_canvas_item_simple_set_fill_options (simple, cr))
    {
      cairo_rectangle (cr, grid->x, grid->y,
		       grid->width, grid->height);
      cairo_fill (cr);
    }

  /* Clip to the grid's area while painting the grid lines. */
  cairo_save (cr);
  cairo_rectangle (cr, grid->x, grid->y,
		   grid->width, grid->height);
  cairo_clip (cr);

  /* Convert the bounds to be redrawn from device space to item space. */
  goo_canvas_convert_bounds_to_item_space (simple->canvas,
					   (GooCanvasItem*) simple,
					   &redraw_bounds);

  /* Paint the grid lines, in the required order. */
  if (grid->vert_grid_lines_on_top)
    {
      paint_horizontal_lines (simple, cr, &redraw_bounds, scale);
      paint_vertical_lines (simple, cr, &redraw_bounds, scale);
    }
  else
    {
      paint_vertical_lines (simple, cr, &redraw_bounds, scale);
      paint_horizontal_lines (simple, cr, &redraw_bounds, scale);
    }

  cairo_restore (cr);

  /* Paint the border. */
  if (grid->border_width > 0)
    {
      if (grid->border_pattern)
	cairo_set_source (cr, grid->border_pattern);
      else
	goo_canvas_item_simple_set_stroke_options (simple, cr, GOO_CANVAS_OPERATION_PAINT, scale);

      cairo_set_line_width (cr, grid->border_width);
      half_border_width = grid->border_width / 2.0;
      cairo_rectangle (cr, grid->x - half_border_width,
		       grid->y - half_border_width,
		       grid->width + grid->border_width,
		       grid->height + grid->border_width);
      cairo_stroke (cr);
    }
}


static void
goo_canvas_grid_class_init (GooCanvasGridClass *klass)
{
  GObjectClass *gobject_class = (GObjectClass*) klass;
  GooCanvasItemSimpleClass *simple_class = (GooCanvasItemSimpleClass*) klass;

  gobject_class->get_property = goo_canvas_grid_get_property;
  gobject_class->set_property = goo_canvas_grid_set_property;

  simple_class->simple_update      = goo_canvas_grid_update;
  simple_class->simple_paint       = goo_canvas_grid_paint;

  g_object_class_install_property (gobject_class, PROP_X,
				   g_param_spec_double ("x",
							"X",
							_("The x coordinate of the grid"),
							-G_MAXDOUBLE,
							G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_Y,
				   g_param_spec_double ("y",
							"Y",
							_("The y coordinate of the grid"),
							-G_MAXDOUBLE,
							G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_WIDTH,
				   g_param_spec_double ("width",
							_("Width"),
							_("The width of the grid"),
							0.0, G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_HEIGHT,
				   g_param_spec_double ("height",
							_("Height"),
							_("The height of the grid"),
							0.0, G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_X_STEP,
				   g_param_spec_double ("x-step",
							"X Step",
							_("The distance between the vertical grid lines"),
							0.0, G_MAXDOUBLE, 10.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_Y_STEP,
				   g_param_spec_double ("y-step",
							"Y Step",
							_("The distance between the horizontal grid lines"),
							0.0, G_MAXDOUBLE, 10.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_X_OFFSET,
				   g_param_spec_double ("x-offset",
							"X Offset",
							_("The distance before the first vertical grid line"),
							0.0, G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_Y_OFFSET,
				   g_param_spec_double ("y-offset",
							"Y Offset",
							_("The distance before the first horizontal grid line"),
							0.0, G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_HORZ_GRID_LINE_WIDTH,
                                   g_param_spec_double ("horz-grid-line-width",
                                                        _("Horizontal Grid Line Width"),
                                                        _("The width of the horizontal grid lines"),
							-G_MAXDOUBLE,
							G_MAXDOUBLE, -1.0,
                                                        G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_VERT_GRID_LINE_WIDTH,
                                   g_param_spec_double ("vert-grid-line-width",
							_("Vertical Grid Line Width"),
							_("The width of the vertical grid lines"),
							-G_MAXDOUBLE,
							G_MAXDOUBLE, -1.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_HORZ_GRID_LINE_PATTERN,
                                   g_param_spec_boxed ("horz-grid-line-pattern",
						       _("Horizontal Grid Line Pattern"),
						       _("The cairo pattern to paint the horizontal grid lines with"),
						       GOO_TYPE_CAIRO_PATTERN,
						       G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_VERT_GRID_LINE_PATTERN,
                                   g_param_spec_boxed ("vert-grid-line-pattern",
						       _("Vertical Grid Line Pattern"),
						       _("The cairo pattern to paint the vertical grid lines with"),
						       GOO_TYPE_CAIRO_PATTERN,
						       G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_BORDER_WIDTH,
				   g_param_spec_double ("border-width",
							_("Border Width"),
							_("The width of the border around the grid"),
							-G_MAXDOUBLE,
							G_MAXDOUBLE, -1.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_BORDER_PATTERN,
                                   g_param_spec_boxed ("border-pattern",
						       _("Border Pattern"),
						       _("The cairo pattern to paint the border with"),
						       GOO_TYPE_CAIRO_PATTERN,
						       G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_SHOW_HORZ_GRID_LINES,
                                   g_param_spec_boolean ("show-horz-grid-lines",
							 _("Show Horizontal Grid Lines"),
							 _("If the horizontal grid lines are shown"),
							 TRUE,
							 G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_SHOW_VERT_GRID_LINES,
                                   g_param_spec_boolean ("show-vert-grid-lines",
							 _("Show Vertical Grid Lines"),
							 _("If the vertical grid lines are shown"),
							 TRUE,
							 G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_VERT_GRID_LINES_ON_TOP,
                                   g_param_spec_boolean ("vert-grid-lines-on-top",
							 _("Vertical Grid Lines On Top"),
							 _("If the vertical grid lines are painted above the horizontal grid lines"),
							 FALSE,
							 G_PARAM_READWRITE));


  /* Convenience properties - some are writable only. */
  g_object_class_install_property (gobject_class, PROP_HORZ_GRID_LINE_COLOR,
				   g_param_spec_string ("horz-grid-line-color",
							_("Horizontal Grid Line Color"),
							_("The color to use for the horizontal grid lines"),
							NULL,
							G_PARAM_WRITABLE));

  g_object_class_install_property (gobject_class, PROP_HORZ_GRID_LINE_COLOR_RGBA,
				   g_param_spec_uint ("horz-grid-line-color-rgba",
						      _("Horizontal Grid Line Color RGBA"),
						      _("The color to use for the horizontal grid lines, specified as a 32-bit integer value"),
						      0, G_MAXUINT, 0,
						      G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_HORZ_GRID_LINE_PIXBUF,
                                   g_param_spec_object ("horz-grid-line-pixbuf",
							_("Horizontal Grid Line Pixbuf"),
							_("The pixbuf to use to draw the horizontal grid lines"),
                                                        GDK_TYPE_PIXBUF,
                                                        G_PARAM_WRITABLE));

  g_object_class_install_property (gobject_class, PROP_VERT_GRID_LINE_COLOR,
				   g_param_spec_string ("vert-grid-line-color",
							_("Vertical Grid Line Color"),
							_("The color to use for the vertical grid lines"),
							NULL,
							G_PARAM_WRITABLE));

  g_object_class_install_property (gobject_class, PROP_VERT_GRID_LINE_COLOR_RGBA,
				   g_param_spec_uint ("vert-grid-line-color-rgba",
						      _("Vertical Grid Line Color RGBA"),
						      _("The color to use for the vertical grid lines, specified as a 32-bit integer value"),
						      0, G_MAXUINT, 0,
						      G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_VERT_GRID_LINE_PIXBUF,
                                   g_param_spec_object ("vert-grid-line-pixbuf",
							_("Vertical Grid Line Pixbuf"),
							_("The pixbuf to use to draw the vertical grid lines"),
                                                        GDK_TYPE_PIXBUF,
                                                        G_PARAM_WRITABLE));

  g_object_class_install_property (gobject_class, PROP_BORDER_COLOR,
				   g_param_spec_string ("border-color",
							_("Border Color"),
							_("The color to use for the border"),
							NULL,
							G_PARAM_WRITABLE));

  g_object_class_install_property (gobject_class, PROP_BORDER_COLOR_RGBA,
				   g_param_spec_uint ("border-color-rgba",
						      _("Border Color RGBA"),
						      _("The color to use for the border, specified as a 32-bit integer value"),
						      0, G_MAXUINT, 0,
						      G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_BORDER_PIXBUF,
                                   g_param_spec_object ("border-pixbuf",
							_("Border Pixbuf"),
							_("The pixbuf to use to draw the border"),
                                                        GDK_TYPE_PIXBUF,
                                                        G_PARAM_WRITABLE));
}

