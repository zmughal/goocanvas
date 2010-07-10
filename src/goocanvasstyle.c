/*
 * GooCanvas. Copyright (C) 2005-2010 Damon Chaplin.
 * Released under the GNU LGPL license. See COPYING for details.
 *
 * goocanvasstyle.c - 
 */
#include <config.h>
#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include "goocanvasstyle.h"
#include "goocanvasprivate.h"
#include "goocanvasenumtypes.h"


G_DEFINE_TYPE (GooCanvasStyle, goo_canvas_style, G_TYPE_OBJECT)


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

  /* The mask of style properties that have been set. */
  PROP_MASK
};


static void
goo_canvas_style_init (GooCanvasStyle *style)
{
  style->stroke_pattern = NULL;
  style->fill_pattern = NULL;

  style->dash = NULL;
  style->font_desc = NULL;

  style->line_width = 2.0;
  style->line_width_tolerance = 0.0;
  style->line_join_miter_limit = 10.0;

  style->mask = 0;

  style->op = CAIRO_OPERATOR_OVER;
  style->antialias = CAIRO_ANTIALIAS_GRAY;
  style->fill_rule = CAIRO_FILL_RULE_WINDING;
  style->line_cap = CAIRO_LINE_CAP_BUTT;
  style->line_join = CAIRO_LINE_JOIN_MITER;
  style->hint_metrics = CAIRO_HINT_METRICS_OFF;
  style->line_width_is_unscaled = FALSE;
}


GooCanvasStyle*
goo_canvas_style_new (void)
{
  return g_object_new (GOO_TYPE_CANVAS_STYLE, NULL);
}


GooCanvasStyle*
goo_canvas_style_copy (GooCanvasStyle* style)
{
  GooCanvasStyle *copy;

  copy = g_object_new (GOO_TYPE_CANVAS_STYLE, NULL);

  if (style->stroke_pattern)
    copy->stroke_pattern = cairo_pattern_reference (style->stroke_pattern);

  if (style->fill_pattern)
    copy->fill_pattern = cairo_pattern_reference (style->fill_pattern);

  if (style->dash)
    copy->dash = goo_canvas_line_dash_ref (style->dash);

  if (style->font_desc)
    copy->font_desc = pango_font_description_copy (style->font_desc);

  copy->line_width = style->line_width;
  copy->line_width_tolerance = style->line_width_tolerance;
  copy->line_join_miter_limit = style->line_join_miter_limit;

  copy->mask = style->mask;

  copy->op = style->op;
  copy->antialias = style->antialias;
  copy->fill_rule = style->fill_rule;
  copy->line_cap = style->line_cap;
  copy->line_join = style->line_join;
  copy->hint_metrics = style->hint_metrics;
  copy->line_width_is_unscaled = style->line_width_is_unscaled;

  return copy;
}


static void
goo_canvas_style_finalize (GObject *object)
{
  GooCanvasStyle *style = (GooCanvasStyle*) object;

  cairo_pattern_destroy (style->stroke_pattern);
  cairo_pattern_destroy (style->fill_pattern);

  if (style->dash)
    {
      goo_canvas_line_dash_unref (style->dash);
      style->dash = NULL;
    }

  if (style->font_desc)
    {
      pango_font_description_free (style->font_desc);
      style->font_desc = NULL;
    }

  G_OBJECT_CLASS (goo_canvas_style_parent_class)->finalize (object);
}


static void
goo_canvas_style_get_property (GObject              *object,
			       guint                 prop_id,
			       GValue               *value,
			       GParamSpec           *pspec)
{
  GooCanvasStyle *style = (GooCanvasStyle*) object;
  gchar *font = NULL;

  switch (prop_id)
    {
      /* Basic drawing properties. */
    case PROP_STROKE_PATTERN:
      g_value_set_boxed (value, style->stroke_pattern);
      break;
    case PROP_FILL_PATTERN:
      g_value_set_boxed (value, style->fill_pattern);
      break;
    case PROP_FILL_RULE:
      g_value_set_enum (value, style->fill_rule);
      break;
    case PROP_OPERATOR:
      g_value_set_enum (value, style->op);
      break;
    case PROP_ANTIALIAS:
      g_value_set_enum (value, style->antialias);
      break;

      /* Line style & width properties. */
    case PROP_LINE_WIDTH:
      g_value_set_double (value, style->line_width);
      break;
    case PROP_LINE_WIDTH_TOLERANCE:
      g_value_set_double (value, style->line_width_tolerance);
      break;
    case PROP_LINE_WIDTH_IS_UNSCALED:
      g_value_set_boolean (value, style->line_width_is_unscaled);
      break;
    case PROP_LINE_CAP:
      g_value_set_enum (value, style->line_cap);
      break;
    case PROP_LINE_JOIN:
      g_value_set_enum (value, style->line_join);
      break;
    case PROP_LINE_JOIN_MITER_LIMIT:
      g_value_set_double (value, style->line_join_miter_limit);
      break;
    case PROP_LINE_DASH:
      g_value_set_boxed (value, style->dash);
      break;

      /* Font properties. */
    case PROP_FONT:
      if (style->font_desc)
	font = pango_font_description_to_string (style->font_desc);
      g_value_set_string (value, font);
      g_free (font);
      break;
    case PROP_FONT_DESC:
      g_value_set_boxed (value, style->font_desc);
      break;
    case PROP_HINT_METRICS:
      g_value_set_enum (value, style->hint_metrics);
      break;

      /* Convenience properties. */
    case PROP_STROKE_COLOR_RGBA:
      goo_canvas_get_rgba_value_from_pattern (style->stroke_pattern, value);
      break;
    case PROP_FILL_COLOR_RGBA:
      goo_canvas_get_rgba_value_from_pattern (style->fill_pattern, value);
      break;

    case PROP_MASK:
      g_value_set_flags (value, style->mask);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


void
goo_canvas_style_set_stroke_pattern (GooCanvasStyle    *style,
				     cairo_pattern_t   *pattern)
{
  cairo_pattern_destroy (style->stroke_pattern);
  style->stroke_pattern = pattern;
  cairo_pattern_reference (style->stroke_pattern);
  style->mask |= GOO_CANVAS_STYLE_STROKE_PATTERN;
}


void
goo_canvas_style_set_fill_pattern (GooCanvasStyle    *style,
				   cairo_pattern_t   *pattern)
{
  cairo_pattern_destroy (style->fill_pattern);
  style->fill_pattern = pattern;
  cairo_pattern_reference (style->fill_pattern);
  style->mask |= GOO_CANVAS_STYLE_FILL_PATTERN;
}


static void
goo_canvas_style_set_property (GObject              *object,
			       guint                 prop_id,
			       const GValue         *value,
			       GParamSpec           *pspec)
{
  GooCanvasStyle *style = (GooCanvasStyle*) object;
  cairo_pattern_t *pattern;
  const char *font_name;
  PangoFontDescription *font_desc = NULL;

  switch (prop_id)
    {
      /* Basic drawing properties. */
    case PROP_STROKE_PATTERN:
      goo_canvas_style_set_stroke_pattern (style, g_value_get_boxed (value));
      break;
    case PROP_FILL_PATTERN:
      goo_canvas_style_set_fill_pattern (style, g_value_get_boxed (value));
      break;
    case PROP_FILL_RULE:
      style->fill_rule = g_value_get_enum (value);
      style->mask |= GOO_CANVAS_STYLE_FILL_RULE;
      break;
    case PROP_OPERATOR:
      style->op = g_value_get_enum (value);
      style->mask |= GOO_CANVAS_STYLE_OPERATOR;
      break;
    case PROP_ANTIALIAS:
      style->antialias = g_value_get_enum (value);
      style->mask |= GOO_CANVAS_STYLE_ANTIALIAS;
      break;

      /* Line style & width properties. */
    case PROP_LINE_WIDTH:
      style->line_width = g_value_get_double (value);
      style->mask |= GOO_CANVAS_STYLE_LINE_WIDTH;
      break;
    case PROP_LINE_WIDTH_TOLERANCE:
      style->line_width_tolerance = g_value_get_double (value);
      style->mask |= GOO_CANVAS_STYLE_LINE_WIDTH_TOLERANCE;
      break;
    case PROP_LINE_WIDTH_IS_UNSCALED:
      style->line_width_is_unscaled = g_value_get_boolean (value);
      style->mask |= GOO_CANVAS_STYLE_LINE_WIDTH_IS_UNSCALED;
      break;
    case PROP_LINE_CAP:
      style->line_cap = g_value_get_enum (value);
      style->mask |= GOO_CANVAS_STYLE_LINE_CAP;
      break;
    case PROP_LINE_JOIN:
      style->line_join = g_value_get_enum (value);
      style->mask |= GOO_CANVAS_STYLE_LINE_JOIN;
      break;
    case PROP_LINE_JOIN_MITER_LIMIT:
      style->line_join_miter_limit = g_value_get_double (value);
      style->mask |= GOO_CANVAS_STYLE_LINE_JOIN_MITER_LIMIT;
      break;
    case PROP_LINE_DASH:
      goo_canvas_line_dash_unref (style->dash);
      style->dash = g_value_get_boxed (value);
      goo_canvas_line_dash_ref (style->dash);
      style->mask |= GOO_CANVAS_STYLE_LINE_DASH;
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
      style->mask |= GOO_CANVAS_STYLE_FONT_DESCRIPTION;
      break;
    case PROP_FONT_DESC:
      if (style->font_desc)
	pango_font_description_free (style->font_desc);
      font_desc = g_value_get_boxed (value);
      if (font_desc)
	style->font_desc = pango_font_description_copy (font_desc);
      else
	style->font_desc = NULL;
      style->mask |= GOO_CANVAS_STYLE_FONT_DESCRIPTION;
      break;
    case PROP_HINT_METRICS:
      style->hint_metrics = g_value_get_enum (value);
      style->mask |= GOO_CANVAS_STYLE_HINT_METRICS;
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

    case PROP_MASK:
      style->mask = g_value_get_flags (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


static void
goo_canvas_style_class_init (GooCanvasStyleClass *klass)
{
  GObjectClass *gobject_class = (GObjectClass*) klass;

  gobject_class->finalize = goo_canvas_style_finalize;

  gobject_class->get_property = goo_canvas_style_get_property;
  gobject_class->set_property = goo_canvas_style_set_property;

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
							_("The line width to use for the item's perimeter"),
							0.0, G_MAXDOUBLE, 2.0,
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

  g_object_class_install_property (gobject_class, PROP_MASK,
				   g_param_spec_flags ("mask",
						       _("Mask"),
						       _("The mask of style properties that are currently set"),
						       GOO_TYPE_CANVAS_STYLE_VALUES_MASK,
						       0,
						       G_PARAM_READWRITE));
}


