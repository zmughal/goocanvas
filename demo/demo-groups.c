#include <config.h>
#include <gtk/gtk.h>
#include <goocanvas.h>


static gboolean dragging = FALSE;
static double drag_x, drag_y;

static gboolean
on_button_press (GooCanvasItem *item,
		 GooCanvasItem *target,
		 GdkEventButton *event,
		 gpointer data)
{
  GooCanvas *canvas;
  GdkCursor *fleur;
  GList *items, *elem;

  g_print ("%p received 'button-press' signal at %g, %g (root: %g, %g)\n",
	   item, event->x, event->y, event->x_root, event->y_root);

  canvas = goo_canvas_item_get_canvas (item);
  items = goo_canvas_get_items_at (canvas, event->x_root, event->y_root,
				   TRUE);
  for (elem = items; elem; elem = elem->next)
    g_print ("  found items: %p\n", elem->data);
  g_list_free (items);

  switch (event->button)
    {
    case 1:
      if (event->state & GDK_SHIFT_MASK)
	{
	  goo_canvas_item_remove (item);
	}
      else
	{
	  drag_x = event->x;
	  drag_y = event->y;

	  fleur = gdk_cursor_new (GDK_FLEUR);
	  goo_canvas_pointer_grab (canvas, item,
				   GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_RELEASE_MASK,
				   fleur,
				   event->time);
	  gdk_cursor_unref (fleur);
	  dragging = TRUE;
	}
      break;

    case 2:
      goo_canvas_item_lower (item, NULL);
      break;

    case 3:
      goo_canvas_item_raise (item, NULL);
      break;

    default:
      break;
    }

  return TRUE;
}


static gboolean
on_motion_notify (GooCanvasItem *item,
		  GooCanvasItem *target,
		  GdkEventMotion *event,
		  gpointer data)
{
#if 0
  g_print ("received 'motion-notify' signal at %g, %g\n",
	   event->x, event->y);
#endif

  if (dragging && (event->state & GDK_BUTTON1_MASK))
    {
      double new_x = event->x;
      double new_y = event->y;

      goo_canvas_item_translate (item, new_x - drag_x, new_y - drag_y);
    }

  return TRUE;
}


static gboolean
on_button_release (GooCanvasItem *item,
		   GooCanvasItem *target,
		   GdkEventButton *event,
		   gpointer data)
{
  GooCanvas *canvas;

#if 0
  g_print ("received 'button-release' signal\n");
#endif

  canvas = goo_canvas_item_get_canvas (item);
  goo_canvas_pointer_ungrab (canvas, item, event->time);
  dragging = FALSE;

  return TRUE;
}


static void
setup_item_signals (GooCanvasItem *item)
{
  g_signal_connect (item, "motion_notify_event",
		    G_CALLBACK (on_motion_notify), NULL);
  g_signal_connect (item, "button_press_event",
		    G_CALLBACK (on_button_press), NULL);
  g_signal_connect (item, "button_release_event",
		    G_CALLBACK (on_button_release), NULL);
}


static void
set_item_id (GooCanvasItem *item,
	     gchar         *group_name,
	     gchar         *item_name)
{
  gchar *id = g_strdup_printf ("%s - %s", group_name, item_name);
  g_object_set_data_full (G_OBJECT (item), "id", id, g_free);
}


static GooCanvasItem*
create_group (GooCanvasItem *parent,
	      gdouble        x,
	      gdouble        y,
	      gdouble        width,
	      gdouble        height,
	      gchar         *clip_path,
	      gchar         *group_name)
{
  GooCanvasItem *group, *child;
  gdouble handle_width = 20, handle_height = 20;
  gdouble half_handle_width = handle_width / 2;
  gdouble half_handle_height = handle_height / 2;

  group = goo_canvas_rect_new (parent, x, y, width, height,
			       "fill-color", "blue",
			       "line-width", 4.0,
			       "clip-path", clip_path,
			       NULL);
  setup_item_signals (group);
  set_item_id (group, group_name, "group");

  child = goo_canvas_rect_new (group,
			       x - half_handle_width,
			       y - half_handle_height,
			       handle_width, handle_height,
			      "fill-color", "orange",
			      NULL);
  setup_item_signals (child);
  set_item_id (child, group_name, "Item1");

  child = goo_canvas_rect_new (group,
			       x + width - half_handle_width,
			       y - half_handle_height,
			       handle_width, handle_height,
			      "fill-color", "orange",
			      NULL);
  setup_item_signals (child);
  set_item_id (child, group_name, "Item2");
#if 1
  goo_canvas_item_rotate (child, 30, x + width, y);
#endif

  child = goo_canvas_rect_new (group,
			       x - half_handle_width,
			       y + height - half_handle_height,
			       handle_width, handle_height,
			      "fill-color", "orange",
			      "tooltip", "Child of Medium Sea Green stippled rectangle",
			      NULL);
  setup_item_signals (child);
  set_item_id (child, group_name, "Item3");

  child = goo_canvas_rect_new (group,
			       x + width - half_handle_width,
			       y + height - half_handle_height,
			       handle_width, handle_height,
			      "fill-color", "orange",
			      "tooltip", "Child of Medium Sea Green stippled rectangle",
			      NULL);
  setup_item_signals (child);
  set_item_id (child, group_name, "Item4");

  return group;
}


static GooCanvasItem*
create_table_group (GooCanvasItem *table,
		    gint           row,
		    gint           column,
		    gchar         *group_name)
{
  GooCanvasItem *group;

  group = create_group (table, 0, 0, 50, 50, NULL, group_name);

  goo_canvas_item_set_child_properties (table, group,
					"row", row,
					"column", column,
					NULL);

  return group;
}


GtkWidget *
create_canvas_groups (void)
{
  GtkWidget *vbox;
  GtkWidget *scrolled_win, *canvas;
  GooCanvasItem *root, *group, *table;

  vbox = gtk_vbox_new (FALSE, 4);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 4);
  gtk_widget_show (vbox);

  scrolled_win = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolled_win),
				       GTK_SHADOW_IN);
  gtk_widget_show (scrolled_win);
  gtk_box_pack_start (GTK_BOX (vbox), scrolled_win, TRUE, TRUE, 0);

  canvas = goo_canvas_new ();
  gtk_widget_set_size_request (canvas, 600, 450);
  goo_canvas_set_bounds (GOO_CANVAS (canvas), 0, 0, 1000, 1000);
  gtk_widget_show (canvas);
  gtk_container_add (GTK_CONTAINER (scrolled_win), canvas);

  root = goo_canvas_get_root_item (GOO_CANVAS (canvas));

#if 1
  group = create_group (root, 100, 100, 50, 50, NULL, NULL);
  group = create_group (root, 300, 100, 80, 40,
			"M300,100 l80,0 l0,20 l-80,20 z", NULL);
  group = create_group (root, 500, 100, 80, 40,
			"M500,100 l80,0 l0,20 l-80,20 z", NULL);
  goo_canvas_item_rotate (group, 45, 540, 120);
#endif

  table = goo_canvas_table_new (root,
#if 1
				"x", 200.0,
				"y", 250.0,
#endif
				"row-spacing", 4.0,
				"column-spacing", 4.0,
				NULL);
  group = create_table_group (table, 0, 0, "Group1");
  group = create_table_group (table, 0, 1, "Group2");
  goo_canvas_item_rotate (group, 45, 0, 0);
  group = create_table_group (table, 0, 2, "Group3");

  group = create_table_group (table, 1, 0, "Group1");
  goo_canvas_item_rotate (group, 45, 0, 0);
  group = create_table_group (table, 1, 1, "Group2");
  group = create_table_group (table, 1, 2, "Group3");
  goo_canvas_item_rotate (group, 45, 0, 0);

  group = create_table_group (table, 2, 0, "Group1");
  group = create_table_group (table, 2, 1, "Group2");
  goo_canvas_item_rotate (group, 45, 0, 0);
  group = create_table_group (table, 2, 2, "Group3");

#if 1
  goo_canvas_item_rotate (table, 45, 300, 350);
#endif

  return vbox;
}
