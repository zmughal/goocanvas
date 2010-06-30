/*
 * GooCanvas. Copyright (C) 2005-6 Damon Chaplin.
 * Released under the GNU LGPL license. See COPYING for details.
 *
 * goocanvasitemsimple.c - abstract base class for canvas items.
 */

/**
 * SECTION:goocanvasitemsimple
 * @Title: GooCanvasItemSimple
 * @Short_Description: the base class for the standard canvas items.
 * @Stability_Level: 
 * @See_Also: 
 *
 * #GooCanvasItemSimple is used as a base class for all of the standard canvas
 * items. It can also be used as the base class for new custom canvas items.
 *
 * It provides default implementations for many of the #GooCanvasItem
 * methods.
 *
 * For very simple items, all that is needed is to implement the create_path()
 * method. (#GooCanvasEllipse, #GooCanvasRect and #GooCanvasPath do this.)
 *
 * More complicated items need to implement the update(), paint() and
 * is_item_at() methods instead. (#GooCanvasImage, #GooCanvasPolyline,
 * #GooCanvasText and #GooCanvasWidget do this.) They may also need to
 * override some of the other GooCanvasItem methods such as set_canvas(),
 * set_parent() or allocate_area() if special code is needed. (#GooCanvasWidget
 * does this to make sure the #GtkWidget is embedded in the #GooCanvas widget
 * correctly.)
 */
#include <config.h>
#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include "goocanvasprivate.h"
#include "goocanvasitemsimple.h"
#include "goocanvas.h"
#include "goocanvasatk.h"


enum {
  PROP_0,

  /* Basic drawing properties. */
  PROP_STROKE_PATTERN,
  PROP_FILL_PATTERN,
  PROP_FILL_RULE,
  PROP_OPERATOR,
  PROP_ANTIALIAS,

  /* Line style & width properties. */
  PROP_LINE_WIDTH,
  PROP_LINE_WIDTH_TOLERANCE,
  PROP_LINE_WIDTH_IS_UNSCALED,
  PROP_LINE_CAP,
  PROP_LINE_JOIN,
  PROP_LINE_JOIN_MITER_LIMIT,
  PROP_LINE_DASH,

  /* Font properties. */
  PROP_FONT,
  PROP_FONT_DESC,
  PROP_HINT_METRICS,

  /* Convenience properties. */
  PROP_STROKE_COLOR,
  PROP_STROKE_COLOR_RGBA,
  PROP_STROKE_PIXBUF,
  PROP_FILL_COLOR,
  PROP_FILL_COLOR_RGBA,
  PROP_FILL_PIXBUF,

  /* Other properties. Note that the order here is important PROP_TRANSFORM
     must be the first non-style property. see set_property(). */
  PROP_TRANSFORM,
  PROP_PARENT,
  PROP_VISIBILITY,
  PROP_VISIBILITY_THRESHOLD,
  PROP_POINTER_EVENTS,
  PROP_TITLE,
  PROP_DESCRIPTION,
  PROP_CAN_FOCUS,
  PROP_CLIP_PATH,
  PROP_CLIP_FILL_RULE,
  PROP_TOOLTIP
};

static gboolean accessibility_enabled = FALSE;

#define HORZ 0
#define VERT 1

typedef struct _GooCanvasItemSimpleChildLayoutData GooCanvasItemSimpleChildLayoutData;
struct _GooCanvasItemSimpleChildLayoutData
{
  gdouble requested_position[2];
  gdouble requested_size[2];
};


G_DEFINE_TYPE (GooCanvasItemSimple, goo_canvas_item_simple,
	       GOO_TYPE_CANVAS_ITEM)


static void
goo_canvas_item_simple_init (GooCanvasItemSimple *simple)
{
  GooCanvasBounds *bounds = &simple->bounds;

  bounds->x1 = bounds->y1 = bounds->x2 = bounds->y2 = 0.0;
  simple->visibility = GOO_CANVAS_ITEM_VISIBLE;
  simple->pointer_events = GOO_CANVAS_EVENTS_VISIBLE_PAINTED;
  simple->clip_fill_rule = CAIRO_FILL_RULE_WINDING;
  simple->need_update = TRUE;
  simple->need_entire_subtree_update = TRUE;
}


static void
goo_canvas_item_simple_dispose (GObject *object)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) object;
  gint i;

  /* Unref all the items in the group. */
  if (simple->children)
    {
      for (i = 0; i < simple->children->len; i++)
	{
	  GooCanvasItem *item = simple->children->pdata[i];
	  goo_canvas_item_set_parent (item, NULL);
	  g_object_unref (item);
	}
      g_ptr_array_set_size (simple->children, 0);
    }

  if (simple->style)
    {
      g_object_unref (simple->style);
      simple->style = NULL;
    }

  if (simple->clip_path_commands)
    {
      g_array_free (simple->clip_path_commands, TRUE);
      simple->clip_path_commands = NULL;
    }

  g_slice_free (cairo_matrix_t, simple->transform);
  simple->transform = NULL;

  G_OBJECT_CLASS (goo_canvas_item_simple_parent_class)->dispose (object);
}


static void
goo_canvas_item_simple_finalize (GObject *object)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) object;

  if (simple->children)
    g_ptr_array_free (simple->children, TRUE);

  G_OBJECT_CLASS (goo_canvas_item_simple_parent_class)->finalize (object);
}


static void
goo_canvas_item_simple_get_property (GObject              *object,
				     guint                 prop_id,
				     GValue               *value,
				     GParamSpec           *pspec)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) object;
  GooCanvasStyle *style = simple->style;
  AtkObject *accessible;
  gchar *font = NULL;

  switch (prop_id)
    {
    case PROP_PARENT:
      g_value_set_object (value, simple->parent);
      break;
    case PROP_TITLE:
      accessible = atk_gobject_accessible_for_object (object);
      g_value_set_string (value, atk_object_get_name (accessible));
      break;
    case PROP_DESCRIPTION:
      accessible = atk_gobject_accessible_for_object (object);
      g_value_set_string (value, atk_object_get_description (accessible));
      break;

      /* Basic drawing properties. */
    case PROP_STROKE_PATTERN:
      g_value_set_boxed (value, style ? style->stroke_pattern : NULL);
      break;
    case PROP_FILL_PATTERN:
      g_value_set_boxed (value, style ? style->fill_pattern : NULL);
      break;
    case PROP_FILL_RULE:
      g_value_set_enum (value, style ? style->fill_rule : CAIRO_FILL_RULE_WINDING);
      break;
    case PROP_OPERATOR:
      g_value_set_enum (value, style ? style->op : CAIRO_OPERATOR_OVER);
      break;
    case PROP_ANTIALIAS:
      g_value_set_enum (value, style ? style->antialias : CAIRO_ANTIALIAS_GRAY);
      break;

      /* Line style & width properties. */
    case PROP_LINE_WIDTH:
      g_value_set_double (value, style ? style->line_width : -1);
      break;
    case PROP_LINE_WIDTH_TOLERANCE:
      g_value_set_double (value, style ? style->line_width_tolerance : 0);
      break;
    case PROP_LINE_WIDTH_IS_UNSCALED:
      g_value_set_boolean (value, style ? style->line_width_is_unscaled : FALSE);
      break;
    case PROP_LINE_CAP:
      g_value_set_enum (value, style ? style->line_cap : CAIRO_LINE_CAP_BUTT);
      break;
    case PROP_LINE_JOIN:
      g_value_set_enum (value, style ? style->line_join : CAIRO_LINE_JOIN_MITER);
      break;
    case PROP_LINE_JOIN_MITER_LIMIT:
      g_value_set_double (value, style ? style->line_join_miter_limit : 10.0);
      break;
    case PROP_LINE_DASH:
      g_value_set_boxed (value, style ? style->dash : NULL);
      break;

      /* Font properties. */
    case PROP_FONT:
      if (style->font_desc)
	font = pango_font_description_to_string (style->font_desc);
      g_value_set_string (value, font);
      g_free (font);
      break;
    case PROP_FONT_DESC:
      g_value_set_boxed (value, style ? style->font_desc : NULL);
      break;
    case PROP_HINT_METRICS:
      g_value_set_enum (value, style->hint_metrics);
      break;

      /* Convenience properties. */
    case PROP_STROKE_COLOR_RGBA:
      goo_canvas_get_rgba_value_from_pattern (style ? style->stroke_pattern : NULL, value);
      break;
    case PROP_FILL_COLOR_RGBA:
      goo_canvas_get_rgba_value_from_pattern (style ? style->fill_pattern : NULL, value);
      break;

      /* Other properties. */
    case PROP_TRANSFORM:
      g_value_set_boxed (value, simple->transform);
      break;
    case PROP_VISIBILITY:
      g_value_set_enum (value, simple->visibility);
      break;
    case PROP_VISIBILITY_THRESHOLD:
      g_value_set_double (value, simple->visibility_threshold);
      break;
    case PROP_POINTER_EVENTS:
      g_value_set_flags (value, simple->pointer_events);
      break;
    case PROP_CAN_FOCUS:
      g_value_set_boolean (value, simple->can_focus);
      break;
    case PROP_CLIP_FILL_RULE:
      g_value_set_enum (value, simple->clip_fill_rule);
      break;
    case PROP_TOOLTIP:
      g_value_set_string (value, simple->tooltip);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


static void
goo_canvas_item_simple_set_property (GObject              *object,
				     guint                 prop_id,
				     const GValue         *value,
				     GParamSpec           *pspec)
{
  GooCanvasItem *item = (GooCanvasItem*) object;
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) object;
  GooCanvasStyle *style;
  GooCanvasItem *parent;
  AtkObject *accessible;
  cairo_pattern_t *pattern;
  gboolean need_update = TRUE, recompute_bounds = FALSE;
  cairo_matrix_t *transform;
  const char *font_name, *path_data;
  PangoFontDescription *font_desc = NULL;

  /* See if we need to create our own style or copy a shared style. */
  if (prop_id < PROP_TRANSFORM)
    {
      if (!simple->style)
	{
	  simple->style = goo_canvas_style_new ();
	}
      else if (((GObject*)simple->style)->ref_count > 1)
	{
	  g_object_unref ((GObject*) simple->style);
	  simple->style = goo_canvas_style_copy (simple->style);
	}
    }

  style = simple->style;

  switch (prop_id)
    {
    case PROP_PARENT:
      parent = g_value_get_object (value);
      goo_canvas_item_remove (item);
      goo_canvas_item_add_child (parent, item, -1);
      need_update = FALSE;
      break;
    case PROP_TITLE:
      accessible = atk_gobject_accessible_for_object (object);
      atk_object_set_name (accessible, g_value_get_string (value));
      need_update = FALSE;
      break;
    case PROP_DESCRIPTION:
      accessible = atk_gobject_accessible_for_object (object);
      atk_object_set_description (accessible, g_value_get_string (value));
      need_update = FALSE;
      break;

      /* Basic drawing properties. */
    case PROP_STROKE_PATTERN:
      goo_canvas_style_set_stroke_pattern (style, g_value_get_boxed (value));
      break;
    case PROP_FILL_PATTERN:
      goo_canvas_style_set_fill_pattern (style, g_value_get_boxed (value));
      break;
    case PROP_FILL_RULE:
      style->fill_rule = g_value_get_enum (value);
      break;
    case PROP_OPERATOR:
      style->op = g_value_get_enum (value);
      break;
    case PROP_ANTIALIAS:
      style->antialias = g_value_get_enum (value);
      break;

      /* Line style & width properties. */
    case PROP_LINE_WIDTH:
      style->line_width = g_value_get_double (value);
      recompute_bounds = TRUE;
      break;
    case PROP_LINE_WIDTH_TOLERANCE:
      style->line_width_tolerance = g_value_get_double (value);
      need_update = FALSE;
      break;
    case PROP_LINE_WIDTH_IS_UNSCALED:
      style->line_width_is_unscaled = g_value_get_boolean (value);
      break;
    case PROP_LINE_CAP:
      style->line_cap = g_value_get_enum (value);
      recompute_bounds = TRUE;
      break;
    case PROP_LINE_JOIN:
      style->line_join = g_value_get_enum (value);
      recompute_bounds = TRUE;
      break;
    case PROP_LINE_JOIN_MITER_LIMIT:
      style->line_join_miter_limit = g_value_get_double (value);
      recompute_bounds = TRUE;
      break;
    case PROP_LINE_DASH:
      goo_canvas_line_dash_unref (style->dash);
      style->dash = g_value_get_boxed (value);
      goo_canvas_line_dash_ref (style->dash);
      recompute_bounds = TRUE;
      break;

      /* Font properties. */
    case PROP_FONT:
      if (style->font_desc)
	pango_font_description_free (style->font_desc);
      font_name = g_value_get_string (value);
      if (font_name)
	style->font_desc = pango_font_description_from_string (font_name);
      else
	style->font_desc = NULL;
      recompute_bounds = TRUE;
      break;
    case PROP_FONT_DESC:
      if (style->font_desc)
	pango_font_description_free (style->font_desc);
      font_desc = g_value_get_boxed (value);
      if (font_desc)
	style->font_desc = pango_font_description_copy (font_desc);
      else
	style->font_desc = NULL;
      recompute_bounds = TRUE;
      break;
    case PROP_HINT_METRICS:
      style->hint_metrics = g_value_get_enum (value);
      recompute_bounds = TRUE;
      break;

      /* Convenience properties. */
    case PROP_STROKE_COLOR:
      pattern = goo_canvas_create_pattern_from_color_value (value);
      goo_canvas_style_set_stroke_pattern (style, pattern);
      cairo_pattern_destroy (pattern);
      break;
    case PROP_STROKE_COLOR_RGBA:
      pattern = goo_canvas_create_pattern_from_rgba_value (value);
      goo_canvas_style_set_stroke_pattern (style, pattern);
      cairo_pattern_destroy (pattern);
      break;
    case PROP_STROKE_PIXBUF:
      pattern = goo_canvas_create_pattern_from_pixbuf_value (value);
      goo_canvas_style_set_stroke_pattern (style, pattern);
      cairo_pattern_destroy (pattern);
      break;

    case PROP_FILL_COLOR:
      pattern = goo_canvas_create_pattern_from_color_value (value);
      goo_canvas_style_set_fill_pattern (style, pattern);
      cairo_pattern_destroy (pattern);
      break;
    case PROP_FILL_COLOR_RGBA:
      pattern = goo_canvas_create_pattern_from_rgba_value (value);
      goo_canvas_style_set_fill_pattern (style, pattern);
      cairo_pattern_destroy (pattern);
      break;
    case PROP_FILL_PIXBUF:
      pattern = goo_canvas_create_pattern_from_pixbuf_value (value);
      goo_canvas_style_set_fill_pattern (style, pattern);
      cairo_pattern_destroy (pattern);
      break;

      /* Other properties. */
    case PROP_TRANSFORM:
      g_slice_free (cairo_matrix_t, simple->transform);
      transform = g_value_get_boxed (value);
      simple->transform = goo_cairo_matrix_copy (transform);
      recompute_bounds = TRUE;
      break;
    case PROP_VISIBILITY:
      simple->visibility = g_value_get_enum (value);
      break;
    case PROP_VISIBILITY_THRESHOLD:
      simple->visibility_threshold = g_value_get_double (value);
      break;
    case PROP_POINTER_EVENTS:
      simple->pointer_events = g_value_get_flags (value);
      break;
    case PROP_CAN_FOCUS:
      simple->can_focus = g_value_get_boolean (value);
      break;
    case PROP_CLIP_PATH:
      if (simple->clip_path_commands)
	g_array_free (simple->clip_path_commands, TRUE);
      path_data = g_value_get_string (value);
      if (path_data)
	simple->clip_path_commands = goo_canvas_parse_path_data (path_data);
      else
	simple->clip_path_commands = NULL;
      recompute_bounds = TRUE;
      break;
    case PROP_CLIP_FILL_RULE:
      simple->clip_fill_rule = g_value_get_enum (value);
      recompute_bounds = TRUE;
      break;
    case PROP_TOOLTIP:
      simple->tooltip = g_value_dup_string (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }

  if (need_update)
    goo_canvas_item_simple_changed (simple, recompute_bounds);
}


static GooCanvas*
goo_canvas_item_simple_get_canvas  (GooCanvasItem *item)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  return simple->canvas;
}


static void
goo_canvas_item_simple_set_canvas  (GooCanvasItem *item,
				    GooCanvas     *canvas)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  gint i;

  if (simple->canvas == canvas)
    return;

  simple->canvas = canvas;

  /* Recursively set the canvas of all child items. */
  if (simple->children)
    {
      for (i = 0; i < simple->children->len; i++)
	{
	  GooCanvasItem *item = simple->children->pdata[i];
	  goo_canvas_item_set_canvas (item, canvas);
	}
    }
}


static GooCanvasItem*
goo_canvas_item_simple_get_parent (GooCanvasItem   *item)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  return simple->parent;
}


static void
goo_canvas_item_simple_set_parent (GooCanvasItem  *item,
				   GooCanvasItem  *parent)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  GooCanvas *canvas;

  simple->parent = parent;
  canvas = parent ? goo_canvas_item_get_canvas (parent) : NULL;
  goo_canvas_item_set_canvas (item, canvas);
  simple->need_update = TRUE;
  simple->need_entire_subtree_update = TRUE;
}


/**
 * goo_canvas_item_simple_changed:
 * @item: a #GooCanvasItemSimple.
 * @recompute_bounds: if the item's bounds need to be recomputed.
 * 
 * This function is intended to be used by subclasses of #GooCanvasItemSimple.
 *
 * It requests an update or redraw of the item as appropriate.
 **/
void
goo_canvas_item_simple_changed    (GooCanvasItemSimple *item,
				   gboolean             recompute_bounds)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;

  if (recompute_bounds)
    {
      item->need_entire_subtree_update = TRUE;
      if (!item->need_update)
	{
	  goo_canvas_item_request_update ((GooCanvasItem*) item);

	  /* Do this after requesting an update, since GooCanvasGroup will
	     ignore the update request if we do this first. */
	  item->need_update = TRUE;
	}
    }
  else
    {
      if (item->canvas)
	goo_canvas_request_item_redraw (item->canvas, &item->bounds, simple->is_static);
    }
}


static gboolean
goo_canvas_item_simple_get_transform (GooCanvasItem       *item,
				      cairo_matrix_t      *matrix)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;

  if (!simple->transform)
    return FALSE;

  *matrix = *simple->transform;
  return TRUE;
}


static void
goo_canvas_item_simple_set_transform (GooCanvasItem        *item,
				      const cairo_matrix_t *transform)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;

  if (transform)
    {
      if (!simple->transform)
	simple->transform = g_slice_new (cairo_matrix_t);

      *simple->transform = *transform;
    }
  else
    {
      g_slice_free (cairo_matrix_t, simple->transform);
      simple->transform = NULL;
    }

  goo_canvas_item_simple_changed (simple, TRUE);
}


static void
goo_canvas_item_simple_get_bounds  (GooCanvasItem   *item,
				    GooCanvasBounds *bounds)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;

  if (simple->need_update)
    goo_canvas_item_ensure_updated (item);
    
  *bounds = simple->bounds;
}


static GList*
goo_canvas_item_simple_get_items_at (GooCanvasItem  *item,
				     gdouble         x,
				     gdouble         y,
				     cairo_t        *cr,
				     gboolean        is_pointer_event,
				     gboolean        parent_visible,
				     GList          *found_items)
{
  GooCanvasItemSimpleClass *class = GOO_CANVAS_ITEM_SIMPLE_GET_CLASS (item);
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  double user_x = x, user_y = y, old_x0, old_y0;
  cairo_matrix_t matrix;
  gboolean visible = parent_visible;
  gint i;

  if (simple->need_update)
    goo_canvas_item_ensure_updated (item);

  /* Skip the item if the point isn't in the item's bounds. */
  if (simple->bounds.x1 > x || simple->bounds.x2 < x
      || simple->bounds.y1 > y || simple->bounds.y2 < y)
    return found_items;

  if (simple->visibility <= GOO_CANVAS_ITEM_INVISIBLE
      || (simple->visibility == GOO_CANVAS_ITEM_VISIBLE_ABOVE_THRESHOLD
	  && simple->canvas->scale < simple->visibility_threshold))
    visible = FALSE;

  /* Check if the item should receive events. */
  if (is_pointer_event
      && (simple->pointer_events == GOO_CANVAS_EVENTS_NONE
	  || ((simple->pointer_events & GOO_CANVAS_EVENTS_VISIBLE_MASK)
	      && !visible)))
    return found_items;

  cairo_save (cr);
  if (simple->transform)
    cairo_transform (cr, simple->transform);

  cairo_device_to_user (cr, &user_x, &user_y);

  /* Remove any current translation, to avoid the 16-bit cairo limit. */
  cairo_get_matrix (cr, &matrix);
  old_x0 = matrix.x0;
  old_y0 = matrix.y0;
  matrix.x0 = matrix.y0 = 0.0;
  cairo_set_matrix (cr, &matrix);

  /* If the item has a clip path, check if the point is inside it. */
  if (simple->clip_path_commands)
    {
      goo_canvas_create_path (simple->clip_path_commands, cr);
      cairo_set_fill_rule (cr, simple->clip_fill_rule);
      if (!cairo_in_fill (cr, user_x, user_y))
	{
	  cairo_restore (cr);
	  return found_items;
	}
    }

  if (class->simple_is_item_at (simple, user_x, user_y, cr, is_pointer_event))
    found_items = g_list_prepend (found_items, item);

  /* Step up from the bottom of the children to the top, adding any items
     found to the start of the list. */
  if (simple->children)
    {
      matrix.x0 = old_x0;
      matrix.y0 = old_y0;
      cairo_set_matrix (cr, &matrix);

      for (i = 0; i < simple->children->len; i++)
	{
	  GooCanvasItem *child = simple->children->pdata[i];

	  found_items = goo_canvas_item_get_items_at (child, x, y, cr,
						      is_pointer_event, visible,
						      found_items);
	}
    }

  cairo_restore (cr);

  return found_items;
}


static gboolean
goo_canvas_item_simple_default_is_item_at (GooCanvasItemSimple *simple,
					   double               x,
					   double               y,
					   cairo_t             *cr,
					   gboolean             is_pointer_event)
{
  GooCanvasItemSimpleClass *class = GOO_CANVAS_ITEM_SIMPLE_GET_CLASS (simple);
  GooCanvasPointerEvents pointer_events = GOO_CANVAS_EVENTS_ALL;

  if (is_pointer_event)
    pointer_events = simple->pointer_events;

  /* Use the virtual method subclasses define to create the path. */
  class->simple_create_path (simple, cr);

  if (goo_canvas_item_simple_check_in_path (simple, x, y, cr, pointer_events, TRUE))
    return TRUE;

  return FALSE;
}


static gboolean
goo_canvas_item_simple_is_visible  (GooCanvasItem   *item)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;

  if (simple->visibility <= GOO_CANVAS_ITEM_INVISIBLE
      || (simple->visibility == GOO_CANVAS_ITEM_VISIBLE_ABOVE_THRESHOLD
	  && simple->canvas->scale < simple->visibility_threshold))
    return FALSE;

  if (simple->parent)
    return goo_canvas_item_is_visible (simple->parent);

  return TRUE;
}


static gboolean
goo_canvas_item_simple_get_can_focus  (GooCanvasItem   *item)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;

  return simple->can_focus;
}


static gboolean
goo_canvas_item_simple_get_is_static  (GooCanvasItem   *item)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;

  return simple->is_static;
}


static void
goo_canvas_item_simple_set_is_static  (GooCanvasItem   *item,
				       gboolean         is_static)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  gint i;

  if (simple->is_static == is_static)
    return;

  simple->is_static = is_static;

  /* Recursively set the canvas of all child items. */
  if (simple->children)
    {
      for (i = 0; i < simple->children->len; i++)
	{
	  GooCanvasItem *item = simple->children->pdata[i];
	  goo_canvas_item_set_is_static (item, is_static);
	}
    }
}


static void
goo_canvas_item_simple_request_update  (GooCanvasItem *item)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;

  if (!simple->need_update)
    {
      simple->need_update = TRUE;

      if (simple->parent)
	goo_canvas_item_request_update (simple->parent);
      else if (simple->canvas)
	goo_canvas_request_update (simple->canvas);
    }
}


static void
goo_canvas_item_simple_update_internal  (GooCanvasItemSimple *simple,
					 cairo_t             *cr)
{
  GooCanvasItemSimpleClass *class = GOO_CANVAS_ITEM_SIMPLE_GET_CLASS (simple);
  GooCanvasBounds tmp_bounds;
  cairo_matrix_t transform;

  simple->need_update = FALSE;

  cairo_get_matrix (cr, &transform);

  class->simple_update (simple, cr);

  /* Modify the extents by the item's clip path. */
  if (simple->clip_path_commands)
    {
      cairo_identity_matrix (cr);
      goo_canvas_create_path (simple->clip_path_commands, cr);
      cairo_set_fill_rule (cr, simple->clip_fill_rule);
      cairo_fill_extents (cr, &tmp_bounds.x1, &tmp_bounds.y1,
			  &tmp_bounds.x2, &tmp_bounds.y2);
      simple->bounds.x1 = MAX (simple->bounds.x1, tmp_bounds.x1);
      simple->bounds.y1 = MAX (simple->bounds.y1, tmp_bounds.y1);
      simple->bounds.x2 = MIN (simple->bounds.x2, tmp_bounds.x2);
      simple->bounds.y2 = MIN (simple->bounds.y2, tmp_bounds.y2);

      if (simple->bounds.x1 > simple->bounds.x2)
	simple->bounds.x2 = simple->bounds.x1;
      if (simple->bounds.y1 > simple->bounds.y2)
	simple->bounds.y2 = simple->bounds.y1;
    }

  cairo_set_matrix (cr, &transform);
}


static void
goo_canvas_item_simple_default_update (GooCanvasItemSimple   *simple,
				       cairo_t               *cr)
{
  GooCanvasItemSimpleClass *class = GOO_CANVAS_ITEM_SIMPLE_GET_CLASS (simple);

  /* Use the identity matrix to get the bounds completely in user space. */
  cairo_identity_matrix (cr);

  class->simple_create_path (simple, cr);
  goo_canvas_item_simple_get_path_bounds (simple, cr, &simple->bounds, TRUE);
}


static void
goo_canvas_item_simple_update  (GooCanvasItem   *item,
				gboolean         entire_tree,
				cairo_t         *cr,
				GooCanvasBounds *bounds)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  GooCanvasBounds child_bounds;
  cairo_matrix_t matrix;
  double x_offset, y_offset;
  gint i;

  if (entire_tree || simple->need_update)
    {
      if (simple->need_entire_subtree_update)
	entire_tree = TRUE;
      simple->need_entire_subtree_update = FALSE;

      /* Request a redraw of the existing bounds. */
      goo_canvas_request_item_redraw (simple->canvas, &simple->bounds, simple->is_static);

      cairo_save (cr);
      if (simple->transform)
	cairo_transform (cr, simple->transform);

      /* Remove any current translation, to avoid the 16-bit cairo limit. */
      cairo_get_matrix (cr, &matrix);
      x_offset = matrix.x0;
      y_offset = matrix.y0;
      matrix.x0 = matrix.y0 = 0.0;
      cairo_set_matrix (cr, &matrix);

      goo_canvas_item_simple_update_internal (simple, cr);

      goo_canvas_item_simple_user_bounds_to_device (simple, cr,
						    &simple->bounds);

      /* Add the translation back to the bounds. */
      simple->bounds.x1 += x_offset;
      simple->bounds.y1 += y_offset;
      simple->bounds.x2 += x_offset;
      simple->bounds.y2 += y_offset;

#if 0
      g_print ("Simple '%s' bounds: %g, %g  %g x %g\n",
	       g_object_get_data (G_OBJECT (simple), "id"),
	       simple->bounds.x1, simple->bounds.y1,
	       simple->bounds.x2 - simple->bounds.x1,
	       simple->bounds.y2 - simple->bounds.y1);
#endif

      cairo_restore (cr);

      /* Now handle any children. */
      if (simple->children)
	{
	  cairo_save (cr);
	  if (simple->transform)
	    cairo_transform (cr, simple->transform);

	  for (i = 0; i < simple->children->len; i++)
	    {
	      GooCanvasItem *child = simple->children->pdata[i];

	      goo_canvas_item_update (child, entire_tree, cr, &child_bounds);
          
	      /* If the child has non-empty bounds, compute the union. */
	      if (child_bounds.x1 < child_bounds.x2
		  && child_bounds.y1 < child_bounds.y2)
		{
		  simple->bounds.x1 = MIN (simple->bounds.x1, child_bounds.x1);
		  simple->bounds.y1 = MIN (simple->bounds.y1, child_bounds.y1);
		  simple->bounds.x2 = MAX (simple->bounds.x2, child_bounds.x2);
		  simple->bounds.y2 = MAX (simple->bounds.y2, child_bounds.y2);

#if 0
		  g_print ("Child '%s' bounds: %g, %g  %g x %g\n",
			   g_object_get_data (G_OBJECT (child), "id"),
			   child_bounds.x1, child_bounds.y1,
			   child_bounds.x2 - child_bounds.x1,
			   child_bounds.y2 - child_bounds.y1);
#endif
		}
	    }
	  cairo_restore (cr);

#if 0
	  g_print ("Simple '%s' bounds with children: %g, %g  %g x %g\n",
		   g_object_get_data (G_OBJECT (simple), "id"),
		   simple->bounds.x1, simple->bounds.y1,
		   simple->bounds.x2 - simple->bounds.x1,
		   simple->bounds.y2 - simple->bounds.y1);
#endif
	}

      /* Request a redraw of the new bounds. */
      goo_canvas_request_item_redraw (simple->canvas, &simple->bounds, simple->is_static);
    }

  *bounds = simple->bounds;
}


static gboolean
goo_canvas_item_simple_get_requested_area (GooCanvasItem    *item,
					   cairo_t          *cr,
					   GooCanvasBounds  *requested_area)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  GooCanvasItemSimpleChildLayoutData *layout_data;
  GooCanvasBounds bounds;
  cairo_matrix_t matrix;
  double x_offset, y_offset;
  gint i;

  /* Request a redraw of the existing bounds. */
  goo_canvas_request_item_redraw (simple->canvas, &simple->bounds, simple->is_static);

  cairo_save (cr);
  if (simple->transform)
    cairo_transform (cr, simple->transform);

  /* Remove any current translation, to avoid the 16-bit cairo limit. */
  cairo_get_matrix (cr, &matrix);
  x_offset = matrix.x0;
  y_offset = matrix.y0;
  matrix.x0 = matrix.y0 = 0.0;
  cairo_set_matrix (cr, &matrix);

  goo_canvas_item_simple_update_internal (simple, cr);

  /* FIXME: Should we make sure the children get updated even if we are
     hidden? If we don't, we need to ensure they do get updated if the
     visibility changes. */
  if (simple->visibility == GOO_CANVAS_ITEM_HIDDEN)
    {
      simple->bounds.x1 = simple->bounds.x2 = 0.0;
      simple->bounds.y1 = simple->bounds.y2 = 0.0;
      cairo_restore (cr);
      return FALSE;
    }

  /* FIXME: Maybe optimize by just converting the offsets to user space
     and adding them? */

  /* Convert to device space. */
  cairo_user_to_device (cr, &simple->bounds.x1, &simple->bounds.y1);
  cairo_user_to_device (cr, &simple->bounds.x2, &simple->bounds.y2);

  /* Add the translation back to the bounds. */
  simple->bounds.x1 += x_offset;
  simple->bounds.y1 += y_offset;
  simple->bounds.x2 += x_offset;
  simple->bounds.y2 += y_offset;

  /* Restore the item's proper transformation matrix. */
  matrix.x0 = x_offset;
  matrix.y0 = y_offset;
  cairo_set_matrix (cr, &matrix);

  /* Convert back to user space. */
  cairo_device_to_user (cr, &simple->bounds.x1, &simple->bounds.y1);
  cairo_device_to_user (cr, &simple->bounds.x2, &simple->bounds.y2);

  /* Handle any children. */
  if (simple->children && simple->children->len)
    {
      layout_data = g_new (GooCanvasItemSimpleChildLayoutData,
			   simple->children->len);
      g_object_set_data_full ((GObject*) simple, "child-layout-data",
			      layout_data, g_free);

#if 0
      g_print ("Simple '%s' base requested area: %g, %g  %g x %g\n",
	       g_object_get_data (G_OBJECT (simple), "id"),
	       simple->bounds.x1, simple->bounds.y1,
	       simple->bounds.x2 - simple->bounds.x1,
	       simple->bounds.y2 - simple->bounds.y1);
#endif

      for (i = 0; i < simple->children->len; i++)
	{
	  GooCanvasItem *child = simple->children->pdata[i];

	  /* Children will return FALSE if they don't need space allocated. */
	  if (goo_canvas_item_get_requested_area (child, cr, &bounds))
	    {
	      /* Remember the requested position and size of the child. */
	      layout_data[i].requested_position[HORZ] = bounds.x1;
	      layout_data[i].requested_position[VERT] = bounds.y1;
	      layout_data[i].requested_size[HORZ] = bounds.x2 - bounds.x1;
	      layout_data[i].requested_size[VERT] = bounds.y2 - bounds.y1;

#if 0
	      g_print ("Child '%s' requested area: %g, %g  %g x %g\n",
		       g_object_get_data (G_OBJECT (child), "id"),
		       layout_data[i].requested_position[HORZ],
		       layout_data[i].requested_position[VERT],
		       layout_data[i].requested_size[HORZ],
		       layout_data[i].requested_size[VERT]);
#endif
	      simple->bounds.x1 = MIN (simple->bounds.x1, bounds.x1);
	      simple->bounds.y1 = MIN (simple->bounds.y1, bounds.y1);
	      simple->bounds.x2 = MAX (simple->bounds.x2, bounds.x2);
	      simple->bounds.y2 = MAX (simple->bounds.y2, bounds.y2);
	    }
	  else
	    {
	      layout_data[i].requested_position[HORZ] = 0.0;
	      layout_data[i].requested_position[VERT] = 0.0;
	      layout_data[i].requested_size[HORZ] = -1.0;
	      layout_data[i].requested_size[VERT] = -1.0;
	    }
	}
    }

  /* Copy the user bounds to the requested area. */
  *requested_area = simple->bounds;

  /* Convert to the parent's coordinate space. */
  goo_canvas_item_simple_user_bounds_to_parent (simple, cr, requested_area);

  /* Convert the item's bounds to device space. */
  goo_canvas_item_simple_user_bounds_to_device (simple, cr, &simple->bounds);

  cairo_restore (cr);

#if 0
  if (simple->children && simple->children->len)
    g_print ("Total requested area: %g, %g  %g x %g\n",
	     requested_area->x1, requested_area->y1,
	     requested_area->x2 - requested_area->x1,
	     requested_area->y2 - requested_area->y1);
#endif

  return TRUE;
}


static void
goo_canvas_item_simple_allocate_area      (GooCanvasItem         *item,
					   cairo_t               *cr,
					   const GooCanvasBounds *requested_area,
					   const GooCanvasBounds *allocated_area,
					   gdouble                x_offset,
					   gdouble                y_offset)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  GooCanvasItemSimpleChildLayoutData *layout_data;
  GooCanvasBounds child_requested_area, child_allocated_area;
  gdouble child_x_offset = x_offset, child_y_offset = y_offset;
  gdouble width, height;
  gint i;

  /* Simple items can't resize at all, so we just adjust the bounds x & y
     positions here, and let the item be clipped if necessary. */
  simple->bounds.x1 += x_offset;
  simple->bounds.y1 += y_offset;
  simple->bounds.x2 += x_offset;
  simple->bounds.y2 += y_offset;

  /* Request a redraw of the new bounds. */
  goo_canvas_request_item_redraw (simple->canvas, &simple->bounds, simple->is_static);

  /* Now handle any children. */
  if (simple->children && simple->children->len)
    {
      layout_data = g_object_get_data ((GObject*) simple, "child-layout-data");

      cairo_save (cr);
      if (simple->transform)
	cairo_transform (cr, simple->transform);

      /* Convert the offsets to our coordinate space. */
      cairo_device_to_user (cr, &child_x_offset, &child_y_offset);

      for (i = 0; i < simple->children->len; i++)
	{
	  GooCanvasItem *child = simple->children->pdata[i];

	  width = layout_data[i].requested_size[HORZ];
	  height = layout_data[i].requested_size[VERT];

	  /* We use the requested area we saved in get_requested_area(). */
	  child_requested_area.x1 = layout_data[i].requested_position[HORZ];
	  child_requested_area.y1 = layout_data[i].requested_position[VERT];
	  child_requested_area.x2 = child_requested_area.x1 + width;
	  child_requested_area.y2 = child_requested_area.y1 + height;

	  /* For the allocated area, we use the requested area shifted by the
	     offsets in this item's coordinate space. */
	  child_allocated_area.x1 = child_requested_area.x1;
	  child_allocated_area.y1 = child_requested_area.y1;
	  child_allocated_area.x1 += child_x_offset;
	  child_allocated_area.y1 += child_y_offset;
	  child_allocated_area.x2 = child_allocated_area.x1 + width;
	  child_allocated_area.y2 = child_allocated_area.y1 + height;

#if 0
	  g_print ("Child '%s' allocated area: %g, %g  %g x %g\n",
		   g_object_get_data (G_OBJECT (child), "id"),
		   child_allocated_area.x1, child_allocated_area.y1,
		   child_allocated_area.x2 - child_allocated_area.x1,
		   child_allocated_area.y2 - child_allocated_area.y1);
#endif
	  goo_canvas_item_allocate_area (child, cr, &child_requested_area,
					 &child_allocated_area,
					 x_offset, y_offset);

	}

      /* Free the layout data. */
      g_object_set_data ((GObject*) simple, "child-layout-data", NULL);

      cairo_restore (cr);
    }
}


static void
goo_canvas_item_simple_paint (GooCanvasItem         *item,
			      cairo_t               *cr,
			      const GooCanvasBounds *bounds,
			      gdouble                scale)
{
  GooCanvasItemSimpleClass *class = GOO_CANVAS_ITEM_SIMPLE_GET_CLASS (item);
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  gint i;

  /* Skip the item if the bounds don't intersect the expose rectangle. */
  if (simple->bounds.x1 > bounds->x2 || simple->bounds.x2 < bounds->x1
      || simple->bounds.y1 > bounds->y2 || simple->bounds.y2 < bounds->y1)
    return;

  /* Check if the item should be visible. */
  if (simple->visibility <= GOO_CANVAS_ITEM_INVISIBLE
      || (simple->visibility == GOO_CANVAS_ITEM_VISIBLE_ABOVE_THRESHOLD
	  && scale < simple->visibility_threshold))
    return;

  cairo_save (cr);
  if (simple->transform)
    cairo_transform (cr, simple->transform);

  /* Clip with the item's clip path, if it is set. */
  if (simple->clip_path_commands)
    {
      goo_canvas_create_path (simple->clip_path_commands, cr);
      cairo_set_fill_rule (cr, simple->clip_fill_rule);
      cairo_clip (cr);
    }

  class->simple_paint (simple, cr, bounds);

  cairo_restore (cr);

  /* Paint the children. */
  if (simple->children)
    {
      cairo_save (cr);
      if (simple->transform)
	cairo_transform (cr, simple->transform);

      for (i = 0; i < simple->children->len; i++)
	{
	  GooCanvasItem *child = simple->children->pdata[i];
	  goo_canvas_item_paint (child, cr, bounds, scale);
	}

      cairo_restore (cr);
    }
}


static void
goo_canvas_item_simple_default_paint (GooCanvasItemSimple   *simple,
				      cairo_t               *cr,
				      const GooCanvasBounds *bounds)
{
  GooCanvasItemSimpleClass *class = GOO_CANVAS_ITEM_SIMPLE_GET_CLASS (simple);

  class->simple_create_path (simple, cr);
  goo_canvas_item_simple_paint_path (simple, cr);
}


static void
goo_canvas_item_simple_default_create_path (GooCanvasItemSimple   *simple,
					    cairo_t               *cr)
{
  /* Do nothing. */
}


static gboolean
goo_canvas_item_simple_query_tooltip (GooCanvasItem  *item,
				      gdouble         x,
				      gdouble         y,
				      gboolean        keyboard_tip,
				      GtkTooltip     *tooltip)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;

  if (simple->tooltip)
    {
      gtk_tooltip_set_markup (tooltip, simple->tooltip);
      return TRUE;
    }

  return FALSE;
}


/**
 * goo_canvas_item_simple_paint_path:
 * @item: a #GooCanvasItemSimple.
 * @cr: a cairo context.
 * 
 * This function is intended to be used by subclasses of #GooCanvasItemSimple.
 *
 * It paints the current path, using the item's style settings.
 **/
void
goo_canvas_item_simple_paint_path (GooCanvasItemSimple *simple,
				   cairo_t             *cr)
{
  if (goo_canvas_item_simple_set_fill_options (simple, cr))
    cairo_fill_preserve (cr);

  if (goo_canvas_item_simple_set_stroke_options (simple, cr, FALSE))
    cairo_stroke (cr);

  cairo_new_path (cr);
}


/* Returns the bounds of the path, using the item's stroke and fill options,
   in device coords. Note that the bounds include both the stroke and the
   fill extents, even if they will not be painted. (We need this to handle
   the "pointer-events" property.) */
/**
 * goo_canvas_item_simple_get_path_bounds:
 * @item: a #GooCanvasItemSimple.
 * @cr: a cairo context.
 * @bounds: the #GooCanvasBounds struct to store the resulting bounding box.
 * @add_tolerance: if the line width tolerance setting should be added to the
 *  line width when calculating the bounds.
 * 
 * This function is intended to be used by subclasses of #GooCanvasItemSimple,
 * typically in their update() or get_requested_area() methods.
 *
 * It calculates the bounds of the current path, using the item's style
 * settings, and stores the results in the given #GooCanvasBounds struct.
 *
 * The returned bounds contains the bounding box of the path in device space,
 * converted to user space coordinates. To calculate the bounds completely in
 * user space, use cairo_identity_matrix() to temporarily reset the current
 * transformation matrix to the identity matrix.
 **/
void
goo_canvas_item_simple_get_path_bounds (GooCanvasItemSimple *simple,
					cairo_t             *cr,
					GooCanvasBounds     *bounds,
					gboolean             add_tolerance)
{
  GooCanvasBounds fill_bounds, stroke_bounds;

  /* Calculate the filled extents. */
  goo_canvas_item_simple_set_fill_options (simple, cr);
  cairo_fill_extents (cr, &fill_bounds.x1, &fill_bounds.y1,
		      &fill_bounds.x2, &fill_bounds.y2);

  /* Check the stroke. */
  goo_canvas_item_simple_set_stroke_options (simple, cr, add_tolerance);
  cairo_stroke_extents (cr, &stroke_bounds.x1, &stroke_bounds.y1,
			&stroke_bounds.x2, &stroke_bounds.y2);

  /* Workaround for cairo < 1.4.0. It used to just return odd values
     if the path had empty bounds. This fix will work, but only if there is
     no transform currently set, since cairo will convert to user space. */
  if (cairo_version () < CAIRO_VERSION_ENCODE (1, 4, 0))
    {
      if (fill_bounds.x1 == 32767.0 && fill_bounds.x2 == -32768.0)
	fill_bounds.x1 = fill_bounds.x2 = 0.0;
      if (stroke_bounds.x1 == 32767.0 && stroke_bounds.x2 == -32768.0)
	stroke_bounds.x1 = stroke_bounds.x2 = 0.0;
    }

  if (fill_bounds.x1 == 0.0 && fill_bounds.x2 == 0.0)
    {
      /* The fill bounds are empty so just use the stroke bounds.
	 If the stroke bounds are also empty the bounds will be all 0.0. */
      bounds->x1 = MIN (stroke_bounds.x1, stroke_bounds.x2);
      bounds->x2 = MAX (stroke_bounds.x1, stroke_bounds.x2);
      bounds->y1 = MIN (stroke_bounds.y1, stroke_bounds.y2);
      bounds->y2 = MAX (stroke_bounds.y1, stroke_bounds.y2);
    }
  else if (stroke_bounds.x1 == 0.0 && stroke_bounds.x2 == 0.0)
    {
      /* The stroke bounds are empty so just use the fill bounds. */
      bounds->x1 = MIN (fill_bounds.x1, fill_bounds.x2);
      bounds->x2 = MAX (fill_bounds.x1, fill_bounds.x2);
      bounds->y1 = MIN (fill_bounds.y1, fill_bounds.y2);
      bounds->y2 = MAX (fill_bounds.y1, fill_bounds.y2);
    }
  else
    {
      /* Both fill & stoke bounds are non-empty so combine them. */
      bounds->x1 = MIN (fill_bounds.x1, fill_bounds.x2);
      bounds->x2 = MAX (fill_bounds.x1, fill_bounds.x2);
      bounds->y1 = MIN (fill_bounds.y1, fill_bounds.y2);
      bounds->y2 = MAX (fill_bounds.y1, fill_bounds.y2);

      bounds->x1 = MIN (bounds->x1, stroke_bounds.x1);
      bounds->x1 = MIN (bounds->x1, stroke_bounds.x2);

      bounds->x2 = MAX (bounds->x2, stroke_bounds.x1);
      bounds->x2 = MAX (bounds->x2, stroke_bounds.x2);

      bounds->y1 = MIN (bounds->y1, stroke_bounds.y1);
      bounds->y1 = MIN (bounds->y1, stroke_bounds.y2);

      bounds->y2 = MAX (bounds->y2, stroke_bounds.y1);
      bounds->y2 = MAX (bounds->y2, stroke_bounds.y2);
    }
}


/**
 * goo_canvas_item_simple_user_bounds_to_device:
 * @item: a #GooCanvasItemSimple.
 * @cr: a cairo context.
 * @bounds: the bounds of the item, in the item's coordinate space.
 * 
 * This function is intended to be used by subclasses of #GooCanvasItemSimple,
 * typically in their update() or get_requested_area() methods.
 *
 * It converts the item's bounds to a bounding box in the canvas (device)
 * coordinate space.
 **/
void
goo_canvas_item_simple_user_bounds_to_device (GooCanvasItemSimple *item,
					      cairo_t             *cr,
					      GooCanvasBounds     *bounds)
{
  GooCanvasBounds tmp_bounds = *bounds, tmp_bounds2 = *bounds;

  /* Convert the top-left and bottom-right corners to device coords. */
  cairo_user_to_device (cr, &tmp_bounds.x1, &tmp_bounds.y1);
  cairo_user_to_device (cr, &tmp_bounds.x2, &tmp_bounds.y2);

  /* Now convert the top-right and bottom-left corners. */
  cairo_user_to_device (cr, &tmp_bounds2.x1, &tmp_bounds2.y2);
  cairo_user_to_device (cr, &tmp_bounds2.x2, &tmp_bounds2.y1);

  /* Calculate the minimum x coordinate seen and put in x1. */
  bounds->x1 = MIN (tmp_bounds.x1, tmp_bounds.x2);
  bounds->x1 = MIN (bounds->x1, tmp_bounds2.x1);
  bounds->x1 = MIN (bounds->x1, tmp_bounds2.x2);

  /* Calculate the maximum x coordinate seen and put in x2. */
  bounds->x2 = MAX (tmp_bounds.x1, tmp_bounds.x2);
  bounds->x2 = MAX (bounds->x2, tmp_bounds2.x1);
  bounds->x2 = MAX (bounds->x2, tmp_bounds2.x2);

  /* Calculate the minimum y coordinate seen and put in y1. */
  bounds->y1 = MIN (tmp_bounds.y1, tmp_bounds.y2);
  bounds->y1 = MIN (bounds->y1, tmp_bounds2.y1);
  bounds->y1 = MIN (bounds->y1, tmp_bounds2.y2);

  /* Calculate the maximum y coordinate seen and put in y2. */
  bounds->y2 = MAX (tmp_bounds.y1, tmp_bounds.y2);
  bounds->y2 = MAX (bounds->y2, tmp_bounds2.y1);
  bounds->y2 = MAX (bounds->y2, tmp_bounds2.y2);
}


/**
 * goo_canvas_item_simple_user_bounds_to_parent:
 * @item: a #GooCanvasItemSimple.
 * @cr: a cairo context.
 * @bounds: the bounds of the item, in the item's coordinate space.
 * 
 * This function is intended to be used by subclasses of #GooCanvasItemSimple,
 * typically in their get_requested_area() method.
 *
 * It converts the item's bounds to a bounding box in its parent's coordinate
 * space. If the item has no transformation matrix set then no conversion is
 * needed.
 **/
void
goo_canvas_item_simple_user_bounds_to_parent (GooCanvasItemSimple *simple,
					      cairo_t             *cr,
					      GooCanvasBounds     *bounds)
{
  cairo_matrix_t *transform = simple->transform;
  GooCanvasBounds tmp_bounds, tmp_bounds2;

  if (!transform)
    return;

  tmp_bounds = tmp_bounds2 = *bounds;

  /* Convert the top-left and bottom-right corners to parent coords. */
  cairo_matrix_transform_point (transform, &tmp_bounds.x1, &tmp_bounds.y1);
  cairo_matrix_transform_point (transform, &tmp_bounds.x2, &tmp_bounds.y2);

  /* Now convert the top-right and bottom-left corners. */
  cairo_matrix_transform_point (transform, &tmp_bounds2.x1, &tmp_bounds2.y2);
  cairo_matrix_transform_point (transform, &tmp_bounds2.x2, &tmp_bounds2.y1);

  /* Calculate the minimum x coordinate seen and put in x1. */
  bounds->x1 = MIN (tmp_bounds.x1, tmp_bounds.x2);
  bounds->x1 = MIN (bounds->x1, tmp_bounds2.x1);
  bounds->x1 = MIN (bounds->x1, tmp_bounds2.x2);

  /* Calculate the maximum x coordinate seen and put in x2. */
  bounds->x2 = MAX (tmp_bounds.x1, tmp_bounds.x2);
  bounds->x2 = MAX (bounds->x2, tmp_bounds2.x1);
  bounds->x2 = MAX (bounds->x2, tmp_bounds2.x2);

  /* Calculate the minimum y coordinate seen and put in y1. */
  bounds->y1 = MIN (tmp_bounds.y1, tmp_bounds.y2);
  bounds->y1 = MIN (bounds->y1, tmp_bounds2.y1);
  bounds->y1 = MIN (bounds->y1, tmp_bounds2.y2);

  /* Calculate the maximum y coordinate seen and put in y2. */
  bounds->y2 = MAX (tmp_bounds.y1, tmp_bounds.y2);
  bounds->y2 = MAX (bounds->y2, tmp_bounds2.y1);
  bounds->y2 = MAX (bounds->y2, tmp_bounds2.y2);
}


/**
 * goo_canvas_item_simple_check_in_path:
 * @item: a #GooCanvasItemSimple.
 * @x: the x coordinate of the point.
 * @y: the y coordinate of the point.
 * @cr: a cairo context.
 * @pointer_events: specifies which parts of the path to check.
 * @add_tolerance: if the line width tolerance setting should be added to
 *  the line width for the check.
 * 
 * This function is intended to be used by subclasses of #GooCanvasItemSimple.
 *
 * It checks if the given point is in the current path, using the item's
 * style settings.
 * 
 * Returns: %TRUE if the given point is in the current path.
 **/
gboolean
goo_canvas_item_simple_check_in_path (GooCanvasItemSimple   *simple,
				      gdouble                x,
				      gdouble                y,
				      cairo_t               *cr,
				      GooCanvasPointerEvents pointer_events,
				      gboolean               add_tolerance)
{
  gboolean do_fill, do_stroke;

  /* Check the filled path, if required. */
  if (pointer_events & GOO_CANVAS_EVENTS_FILL_MASK)
    {
      do_fill = goo_canvas_item_simple_set_fill_options (simple, cr);
      if (!(pointer_events & GOO_CANVAS_EVENTS_PAINTED_MASK) || do_fill)
	{
	  if (cairo_in_fill (cr, x, y))
	    return TRUE;
	}
    }

  /* Check the stroke, if required. */
  if (pointer_events & GOO_CANVAS_EVENTS_STROKE_MASK)
    {
      do_stroke = goo_canvas_item_simple_set_stroke_options (simple, cr, add_tolerance);
      if (!(pointer_events & GOO_CANVAS_EVENTS_PAINTED_MASK) || do_stroke)
	{
	  if (cairo_in_stroke (cr, x, y))
	    return TRUE;
	}
    }

  return FALSE;
}


/**
 * goo_canvas_item_simple_get_line_width:
 * @item: a #GooCanvasItemSimple.
 * 
 * Gets the item's line width.
 * 
 * Returns: the item's line width.
 **/
gdouble
goo_canvas_item_simple_get_line_width (GooCanvasItemSimple   *simple)
{
  if (simple->style && simple->style->line_width >= 0)
    return simple->style->line_width;
  else if (simple->canvas)
    return goo_canvas_get_default_line_width (simple->canvas);
  else
    return 2.0;
}


void
goo_canvas_item_simple_set_style (GooCanvasItemSimple   *simple,
				  GooCanvasStyle        *style)
{
  if (simple->style)
    g_object_unref (simple->style);

  simple->style = style;
  if (style)
    g_object_ref (style);

  goo_canvas_item_simple_changed (simple, TRUE);
}


gboolean
goo_canvas_item_simple_set_stroke_options (GooCanvasItemSimple   *simple,
					   cairo_t               *cr,
					   gboolean		  add_tolerance)
{
  GooCanvasStyle *style = simple->style;
  gdouble line_width, scale;

  /* If no style is set, just reset the source to black and return TRUE so the
     default style will be used. */
  if (!style)
    {
      cairo_set_source_rgb (cr, 0, 0, 0);
      return TRUE;
    }

  if (style->stroke_pattern)
    cairo_set_source (cr, style->stroke_pattern);
  else
    cairo_set_source_rgb (cr, 0, 0, 0);

  if (style->op != CAIRO_OPERATOR_OVER)
    cairo_set_operator (cr, style->op);

  if (style->antialias != CAIRO_ANTIALIAS_GRAY)
    cairo_set_antialias (cr, style->antialias);

  /* Determine the basic line width. */
  if (style->line_width >= 0.0)
    line_width = style->line_width;
  else
    line_width = cairo_get_line_width (cr);

  /* Add on the tolerance, if needed. */
  if (add_tolerance)
    line_width += style->line_width_tolerance;

  /* If the line width is supposed to be unscaled, try to reverse the effects
     of the canvas scale. We use the maximum canvas scale, since being too
     thin is better than being too fat. */
  if (style->line_width_is_unscaled && simple->canvas)
    {
      scale = MAX (simple->canvas->scale_x, simple->canvas->scale_y);

      /* We only want to shrink the lines as the canvas is scaled up.
	 We don't want to affect the line width when the scales are < 1. */
      if (scale > 1.0)
	line_width /= scale;
    }

  /* Set the line width. */
  cairo_set_line_width (cr, line_width);

  if (style->line_cap != CAIRO_LINE_CAP_BUTT)
    cairo_set_line_cap (cr, style->line_cap);

  if (style->line_join != CAIRO_LINE_JOIN_MITER)
    cairo_set_line_join (cr, style->line_join);

  if (style->line_join_miter_limit != 10.0)
    cairo_set_miter_limit (cr, style->line_join_miter_limit);

  if (style->dash)
    cairo_set_dash (cr, style->dash->dashes, style->dash->num_dashes,
		    style->dash->dash_offset);

  /* If the style pattern has been explicitly set to NULL return FALSE, as no
     stroke is wanted. */
  if (style->stroke_pattern_set && !style->stroke_pattern)
    return FALSE;

  return TRUE;
}


gboolean
goo_canvas_item_simple_set_fill_options (GooCanvasItemSimple   *simple,
					 cairo_t               *cr)
{
  GooCanvasStyle *style = simple->style;

  /* If no style is set, just return FALSE as no fill is needed. */
  if (!style)
    return FALSE;

  if (style->fill_pattern)
    cairo_set_source (cr, style->fill_pattern);

  if (style->op != CAIRO_OPERATOR_OVER)
    cairo_set_operator (cr, style->op);

  if (style->antialias != CAIRO_ANTIALIAS_GRAY)
    cairo_set_antialias (cr, style->antialias);

  if (style->fill_rule != CAIRO_FILL_RULE_WINDING)
    cairo_set_fill_rule (cr, style->fill_rule);

  return style->fill_pattern ? TRUE : FALSE;
}


static void
goo_canvas_item_simple_add_child     (GooCanvasItem  *item,
				      GooCanvasItem  *child,
				      gint            position)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  AtkObject *atk_obj, *child_atk_obj;

  g_object_ref (child);

  if (!simple->children)
    simple->children = g_ptr_array_sized_new (8);

  if (position >= 0)
    {
      goo_canvas_util_ptr_array_insert (simple->children, child, position);
    }
  else
    {
      position = simple->children->len;
      g_ptr_array_add (simple->children, child);
    }

  goo_canvas_item_set_parent (child, item);
  goo_canvas_item_set_is_static (child, simple->is_static);

  /* Emit the "children_changed" ATK signal, if ATK is enabled. */
  if (accessibility_enabled)
    {
      atk_obj = atk_gobject_accessible_for_object (G_OBJECT (item));
      if (!ATK_IS_NO_OP_OBJECT (atk_obj))
	{
	  child_atk_obj = atk_gobject_accessible_for_object (G_OBJECT (child));
	  g_signal_emit_by_name (atk_obj, "children_changed::add",
				 position, child_atk_obj);
	}
    }

  goo_canvas_item_request_update (item);
}


static void
goo_canvas_item_simple_move_child    (GooCanvasItem  *item,
				      gint	      old_position,
				      gint            new_position)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  GooCanvasItem *child;
  GooCanvasBounds bounds;

  g_return_if_fail (simple->children != NULL);
  g_return_if_fail (old_position < simple->children->len);
  g_return_if_fail (new_position < simple->children->len);

  /* Request a redraw of the item's bounds. */
  child = simple->children->pdata[old_position];
  if (simple->canvas)
    {
      goo_canvas_item_get_bounds (child, &bounds);
      goo_canvas_request_item_redraw (simple->canvas, &bounds,
				      simple->is_static);
    }

  goo_canvas_util_ptr_array_move (simple->children, old_position, new_position);

  goo_canvas_item_request_update (item);
}


static void
goo_canvas_item_simple_remove_child  (GooCanvasItem  *item,
				      gint            child_num)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  GooCanvasItem *child;
  GooCanvasBounds bounds;
  AtkObject *atk_obj, *child_atk_obj;

  g_return_if_fail (simple->children != NULL);
  g_return_if_fail (child_num < simple->children->len);

  /* Request a redraw of the item's bounds. */
  child = simple->children->pdata[child_num];
  if (simple->canvas)
    {
      goo_canvas_item_get_bounds (child, &bounds);
      goo_canvas_request_item_redraw (simple->canvas, &bounds,
				      simple->is_static);
    }

  /* Emit the "children_changed" ATK signal, if ATK is enabled. */
  if (accessibility_enabled)
    {
      atk_obj = atk_gobject_accessible_for_object (G_OBJECT (item));
      if (!ATK_IS_NO_OP_OBJECT (atk_obj))
	{
	  child_atk_obj = atk_gobject_accessible_for_object (G_OBJECT (child));
	  g_signal_emit_by_name (atk_obj, "children_changed::remove",
				 child_num, child_atk_obj);
	}
    }

  g_ptr_array_remove_index (simple->children, child_num);

  goo_canvas_item_set_parent (child, NULL);
  g_object_unref (child);

  goo_canvas_item_request_update (item);
}


static gint
goo_canvas_item_simple_get_n_children (GooCanvasItem  *item)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;

  return simple->children ? simple->children->len : 0;
}


static GooCanvasItem*
goo_canvas_item_simple_get_child   (GooCanvasItem       *item,
				    gint                 child_num)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;

  if (simple->children && child_num < simple->children->len)
    return simple->children->pdata[child_num];
  return NULL;
}


static void
goo_canvas_item_simple_class_init (GooCanvasItemSimpleClass *klass)
{
  GObjectClass *gobject_class = (GObjectClass*) klass;
  GooCanvasItemClass *item_class = (GooCanvasItemClass*) klass;

  gobject_class->dispose  = goo_canvas_item_simple_dispose;
  gobject_class->finalize = goo_canvas_item_simple_finalize;

  gobject_class->get_property = goo_canvas_item_simple_get_property;
  gobject_class->set_property = goo_canvas_item_simple_set_property;

  item_class->get_canvas         = goo_canvas_item_simple_get_canvas;
  item_class->set_canvas         = goo_canvas_item_simple_set_canvas;

  item_class->add_child          = goo_canvas_item_simple_add_child;
  item_class->move_child         = goo_canvas_item_simple_move_child;
  item_class->remove_child       = goo_canvas_item_simple_remove_child;
  item_class->get_n_children     = goo_canvas_item_simple_get_n_children;
  item_class->get_child          = goo_canvas_item_simple_get_child;

  item_class->get_parent	 = goo_canvas_item_simple_get_parent;
  item_class->set_parent	 = goo_canvas_item_simple_set_parent;
  item_class->get_bounds         = goo_canvas_item_simple_get_bounds;
  item_class->get_items_at	 = goo_canvas_item_simple_get_items_at;
  item_class->request_update     = goo_canvas_item_simple_request_update;
  item_class->update             = goo_canvas_item_simple_update;
  item_class->get_requested_area = goo_canvas_item_simple_get_requested_area;
  item_class->allocate_area      = goo_canvas_item_simple_allocate_area;
  item_class->paint              = goo_canvas_item_simple_paint;

  item_class->get_transform      = goo_canvas_item_simple_get_transform;
  item_class->set_transform      = goo_canvas_item_simple_set_transform;
  item_class->is_visible         = goo_canvas_item_simple_is_visible;
  item_class->get_can_focus	 = goo_canvas_item_simple_get_can_focus;
  item_class->get_is_static	 = goo_canvas_item_simple_get_is_static;
  item_class->set_is_static	 = goo_canvas_item_simple_set_is_static;

  item_class->query_tooltip	 = goo_canvas_item_simple_query_tooltip;

  klass->simple_create_path = goo_canvas_item_simple_default_create_path;
  klass->simple_update      = goo_canvas_item_simple_default_update;
  klass->simple_paint       = goo_canvas_item_simple_default_paint;
  klass->simple_is_item_at  = goo_canvas_item_simple_default_is_item_at;

  /* Register our accessible factory, but only if accessibility is enabled. */
  if (!ATK_IS_NO_OP_OBJECT_FACTORY (atk_registry_get_factory (atk_get_default_registry (), GTK_TYPE_WIDGET)))
    {
      accessibility_enabled = TRUE;
      atk_registry_set_factory_type (atk_get_default_registry (),
				     GOO_TYPE_CANVAS_ITEM_SIMPLE,
				     goo_canvas_item_accessible_factory_get_type ());
    }

  /* Basic drawing properties. */
  g_object_class_install_property (gobject_class, PROP_STROKE_PATTERN,
                                   g_param_spec_boxed ("stroke-pattern",
						       _("Stroke Pattern"),
						       _("The pattern to use to paint the perimeter of the item, or NULL disable painting"),
						       GOO_TYPE_CAIRO_PATTERN,
						       G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_FILL_PATTERN,
                                   g_param_spec_boxed ("fill-pattern",
						       _("Fill Pattern"),
						       _("The pattern to use to paint the interior of the item, or NULL to disable painting"),
						       GOO_TYPE_CAIRO_PATTERN,
						       G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_FILL_RULE,
				   g_param_spec_enum ("fill-rule",
						      _("Fill Rule"),
						      _("The fill rule used to determine which parts of the item are filled"),
						      GOO_TYPE_CAIRO_FILL_RULE,
						      CAIRO_FILL_RULE_WINDING,
						      G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_OPERATOR,
				   g_param_spec_enum ("operator",
						      _("Operator"),
						      _("The compositing operator to use"),
						      GOO_TYPE_CAIRO_OPERATOR,
						      CAIRO_OPERATOR_OVER,
						      G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_ANTIALIAS,
				   g_param_spec_enum ("antialias",
						      _("Antialias"),
						      _("The antialiasing mode to use"),
						      GOO_TYPE_CAIRO_ANTIALIAS,
						      CAIRO_ANTIALIAS_GRAY,
						      G_PARAM_READWRITE));

  /* Line style & width properties. */
  g_object_class_install_property (gobject_class, PROP_LINE_WIDTH,
				   g_param_spec_double ("line-width",
							_("Line Width"),
							_("The line width to use for the item's perimeter, or -1 to use the default line width"),
							-G_MAXDOUBLE,
							G_MAXDOUBLE, -1.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_LINE_WIDTH_TOLERANCE,
				   g_param_spec_double ("line-width-tolerance",
							_("Line Width Tolerance"),
							_("The tolerance added to the line width when testing for mouse events"),
							0.0, G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_LINE_WIDTH_IS_UNSCALED,
				   g_param_spec_boolean ("line-width-is-unscaled",
							 _("Line Width Is Unscaled"),
							 _("If the line width does not grow as the canvas is scaled"),
							 FALSE,
							 G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_LINE_CAP,
				   g_param_spec_enum ("line-cap",
						      _("Line Cap"),
						      _("The line cap style to use"),
						      GOO_TYPE_CAIRO_LINE_CAP,
						      CAIRO_LINE_CAP_BUTT,
						      G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_LINE_JOIN,
				   g_param_spec_enum ("line-join",
						      _("Line Join"),
						      _("The line join style to use"),
						      GOO_TYPE_CAIRO_LINE_JOIN,
						      CAIRO_LINE_JOIN_MITER,
						      G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_LINE_JOIN_MITER_LIMIT,
				   g_param_spec_double ("line-join-miter-limit",
							_("Miter Limit"),
							_("The smallest angle to use with miter joins, in degrees. Bevel joins will be used below this limit"),
							0.0, G_MAXDOUBLE, 10.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_LINE_DASH,
				   g_param_spec_boxed ("line-dash",
						       _("Line Dash"),
						       _("The dash pattern to use"),
						       GOO_TYPE_CANVAS_LINE_DASH,
						       G_PARAM_READWRITE));

  /* Font properties. */
  g_object_class_install_property (gobject_class, PROP_FONT,
				   g_param_spec_string ("font",
							_("Font"),
							_("The base font to use for the text"),
							NULL,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_FONT_DESC,
				   g_param_spec_boxed ("font-desc",
						       _("Font Description"),
						       _("The attributes specifying which font to use"),
						       PANGO_TYPE_FONT_DESCRIPTION,
						       G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_HINT_METRICS,
				   g_param_spec_enum ("hint-metrics",
						      _("Hint Metrics"),
						      _("The hinting to be used for font metrics"),
						      GOO_TYPE_CAIRO_HINT_METRICS,
						      CAIRO_HINT_METRICS_OFF,
						      G_PARAM_READWRITE));

  /* Convenience properties - writable only. */
  g_object_class_install_property (gobject_class, PROP_STROKE_COLOR,
				   g_param_spec_string ("stroke-color",
							_("Stroke Color"),
							_("The color to use for the item's perimeter. To disable painting set the 'stroke-pattern' property to NULL"),
							NULL,
							G_PARAM_WRITABLE));

  g_object_class_install_property (gobject_class, PROP_STROKE_COLOR_RGBA,
				   g_param_spec_uint ("stroke-color-rgba",
						      _("Stroke Color RGBA"),
						      _("The color to use for the item's perimeter, specified as a 32-bit integer value. To disable painting set the 'stroke-pattern' property to NULL"),
						      0, G_MAXUINT, 0,
						      G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_STROKE_PIXBUF,
                                   g_param_spec_object ("stroke-pixbuf",
							_("Stroke Pixbuf"),
							_("The pixbuf to use to draw the item's perimeter. To disable painting set the 'stroke-pattern' property to NULL"),
                                                        GDK_TYPE_PIXBUF,
                                                        G_PARAM_WRITABLE));

  g_object_class_install_property (gobject_class, PROP_FILL_COLOR,
				   g_param_spec_string ("fill-color",
							_("Fill Color"),
							_("The color to use to paint the interior of the item. To disable painting set the 'fill-pattern' property to NULL"),
							NULL,
							G_PARAM_WRITABLE));

  g_object_class_install_property (gobject_class, PROP_FILL_COLOR_RGBA,
				   g_param_spec_uint ("fill-color-rgba",
						      _("Fill Color RGBA"),
						      _("The color to use to paint the interior of the item, specified as a 32-bit integer value. To disable painting set the 'fill-pattern' property to NULL"),
						      0, G_MAXUINT, 0,
						      G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_FILL_PIXBUF,
                                   g_param_spec_object ("fill-pixbuf",
							_("Fill Pixbuf"),
							_("The pixbuf to use to paint the interior of the item. To disable painting set the 'fill-pattern' property to NULL"),
                                                        GDK_TYPE_PIXBUF,
                                                        G_PARAM_WRITABLE));

  /* Other properties. */
  g_object_class_install_property (gobject_class,
				   PROP_PARENT,
				   g_param_spec_object ("parent",
							_("Parent"),
							_("The parent item"),
							GOO_TYPE_CANVAS_ITEM,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class,
				   PROP_VISIBILITY,
				   g_param_spec_enum ("visibility",
						      _("Visibility"),
						      _("When the canvas item is visible"),
						      GOO_TYPE_CANVAS_ITEM_VISIBILITY,
						      GOO_CANVAS_ITEM_VISIBLE,
						      G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class,
				   PROP_VISIBILITY_THRESHOLD,
				   g_param_spec_double ("visibility-threshold",
							_("Visibility Threshold"),
							_("The scale threshold at which the item becomes visible"),
							0.0,
							G_MAXDOUBLE,
							0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class,
				   PROP_TRANSFORM,
				   g_param_spec_boxed ("transform",
						       _("Transform"),
						       _("The transformation matrix of the item"),
						       GOO_TYPE_CAIRO_MATRIX,
						       G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class,
				   PROP_POINTER_EVENTS,
				   g_param_spec_flags ("pointer-events",
						       _("Pointer Events"),
						       _("Specifies when the item receives pointer events"),
						       GOO_TYPE_CANVAS_POINTER_EVENTS,
						       GOO_CANVAS_EVENTS_VISIBLE_PAINTED,
						       G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class,
				   PROP_TITLE,
				   g_param_spec_string ("title",
							_("Title"),
							_("A short context-rich description of the item for use by assistive technologies"),
							NULL,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class,
				   PROP_DESCRIPTION,
				   g_param_spec_string ("description",
							_("Description"),
							_("A description of the item for use by assistive technologies"),
							NULL,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class,
				   PROP_CAN_FOCUS,
				   g_param_spec_boolean ("can-focus",
							 _("Can Focus"),
							 _("If the item can take the keyboard focus"),
							 FALSE,
							 G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class,
				   PROP_TOOLTIP,
				   g_param_spec_string ("tooltip",
							_("Tooltip"),
							_("The tooltip to display for the item"),
							NULL,
							G_PARAM_READWRITE));


  /**
   * GooCanvasItemSimple:clip-path
   *
   * The sequence of commands describing the clip path of the item, specified
   * as a string using the same syntax
   * as in the <ulink url="http://www.w3.org/Graphics/SVG/">Scalable Vector
   * Graphics (SVG)</ulink> path element.
   */
  g_object_class_install_property (gobject_class, PROP_CLIP_PATH,
				   g_param_spec_string ("clip-path",
							_("Clip Path"),
							_("The sequence of path commands specifying the clip path"),
							NULL,
							G_PARAM_WRITABLE));

  g_object_class_install_property (gobject_class, PROP_CLIP_FILL_RULE,
				   g_param_spec_enum ("clip-fill-rule",
						      _("Clip Fill Rule"),
						      _("The fill rule used to determine which parts of the item are clipped"),
						      GOO_TYPE_CAIRO_FILL_RULE,
						      CAIRO_FILL_RULE_WINDING,
						      G_PARAM_READWRITE));
}
