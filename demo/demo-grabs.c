#include <config.h>
#include <string.h>
#include <gtk/gtk.h>
#include <goocanvas.h>


static gboolean
on_widget_draw (GtkWidget *widget,
		  cairo_t *cr,
		  char *item_id)
{
  GtkAllocation allocation;

  g_print ("%s received 'draw' signal\n", item_id);

  gtk_widget_get_allocation (widget, &allocation);

  cairo_rectangle (cr, 0, 0,
		   allocation.width, allocation.height);
  cairo_stroke (cr);

  return FALSE;
}


static gboolean
on_widget_enter_notify (GtkWidget *widget,
			GdkEventCrossing *event,
			char *item_id)
{
  g_print ("%s received 'enter-notify' signal\n", item_id);
  return TRUE;
}


static gboolean
on_widget_leave_notify (GtkWidget *widget,
			GdkEventCrossing *event,
			char *item_id)
{
  g_print ("%s received 'leave-notify' signal\n", item_id);
  return TRUE;
}


static gboolean
on_widget_motion_notify (GtkWidget *widget,
			 GdkEventMotion *event,
			 char *item_id)
{
  GdkDevice *device = gdk_event_get_device ((GdkEvent*) event);

  g_print ("%s received 'motion-notify' signal (window: %p)\n", item_id,
	   event->window);

  if (event->is_hint && device)
    gdk_window_get_device_position (event->window, device, NULL, NULL, NULL);

  return TRUE;
}


static gboolean
on_widget_button_press (GtkWidget *widget,
			GdkEventButton *event,
			char *item_id)
{
  GdkDevice *device = gdk_event_get_device ((GdkEvent*) event);

  g_print ("%s received 'button-press' signal\n", item_id);

  if (device && strstr (item_id, "explicit"))
    {
      GdkGrabStatus status;

#if GTK_CHECK_VERSION(3,20,0)
      status = gdk_seat_grab (gdk_device_get_seat (device),
			      gtk_widget_get_window (widget),
			      GDK_SEAT_CAPABILITY_ALL_POINTING,
			      FALSE, NULL, (GdkEvent*) event, NULL, NULL);
#else
      GdkEventMask mask = GDK_BUTTON_PRESS_MASK
	| GDK_BUTTON_RELEASE_MASK
	| GDK_POINTER_MOTION_MASK
	| GDK_POINTER_MOTION_HINT_MASK
	| GDK_ENTER_NOTIFY_MASK
	| GDK_LEAVE_NOTIFY_MASK;

      status = gdk_device_grab (device, gtk_widget_get_window (widget),
				GDK_OWNERSHIP_NONE, FALSE, mask,
				NULL, event->time);
#endif

      if (status == GDK_GRAB_SUCCESS)
	g_print ("grabbed pointer\n");
      else
	g_print ("pointer grab failed\n");
    }

  return TRUE;
}


static gboolean
on_widget_button_release (GtkWidget *widget,
			  GdkEventButton *event,
			  char *item_id)
{
  GdkDevice *device = gdk_event_get_device ((GdkEvent*) event);

  g_print ("%s received 'button-release' signal\n", item_id);

  if (device && strstr (item_id, "explicit"))
    {
#if GTK_CHECK_VERSION(3,20,0)
      gdk_seat_ungrab (gdk_device_get_seat (device));
#else
      gdk_device_ungrab (device, event->time);
#endif
      g_print ("released pointer grab\n");
    }

  return TRUE;
}




static gboolean
on_enter_notify (GooCanvasItem *item,
		 GooCanvasItem *target,
		 GdkEventCrossing *event,
		 gpointer data)
{
  char *item_id = g_object_get_data (G_OBJECT (item), "id");

  g_print ("%s received 'enter-notify' signal\n", item_id);
  return FALSE;
}


static gboolean
on_leave_notify (GooCanvasItem *item,
		 GooCanvasItem *target,
		 GdkEventCrossing *event,
		 gpointer data)
{
  char *item_id = g_object_get_data (G_OBJECT (item), "id");

  g_print ("%s received 'leave-notify' signal\n", item_id);
  return FALSE;
}


static gboolean
on_motion_notify (GooCanvasItem *item,
		  GooCanvasItem *target,
		  GdkEventMotion *event,
		  gpointer data)
{
  char *item_id = g_object_get_data (G_OBJECT (item), "id");

  g_print ("%s received 'motion-notify' signal\n", item_id);
  return FALSE;
}


static gboolean
on_button_press (GooCanvasItem *item,
		 GooCanvasItem *target,
		 GdkEventButton *event,
		 gpointer data)
{
  char *item_id = g_object_get_data (G_OBJECT (item), "id");

  g_print ("%s received 'button-press' signal\n", item_id);

  if (strstr (item_id, "explicit"))
    {
      GooCanvas *canvas;
      GdkGrabStatus status;
      GdkEventMask mask = GDK_BUTTON_PRESS_MASK
	| GDK_BUTTON_RELEASE_MASK
	| GDK_POINTER_MOTION_MASK
	| GDK_POINTER_MOTION_HINT_MASK
	| GDK_ENTER_NOTIFY_MASK
	| GDK_LEAVE_NOTIFY_MASK;

      canvas = goo_canvas_item_get_canvas (item);
      status = goo_canvas_pointer_grab (canvas, item, mask, NULL, event->time);
      if (status == GDK_GRAB_SUCCESS)
	g_print ("grabbed pointer\n");
      else
	g_print ("pointer grab failed\n");
    }

  return FALSE;
}


static gboolean
on_button_release (GooCanvasItem *item,
		   GooCanvasItem *target,
		   GdkEventButton *event,
		   gpointer data)
{
  char *item_id = g_object_get_data (G_OBJECT (item), "id");

  g_print ("%s received 'button-release' signal\n", item_id);

  if (strstr (item_id, "explicit"))
    {
      GooCanvas *canvas;

      canvas = goo_canvas_item_get_canvas (item);
      goo_canvas_pointer_ungrab (canvas, item, event->time);
      g_print ("released pointer grab\n");
    }

  return FALSE;
}


static void
create_fixed (GtkGrid *grid, gint row, gchar *text, gchar *id)
{
  GtkWidget *label, *fixed, *drawing_area;
  char *view_id;

  label = gtk_label_new (text);
  gtk_grid_attach (grid, label, 0, row, 1, 1);
  gtk_widget_show (label);

  fixed = gtk_fixed_new ();
  gtk_widget_set_has_window (fixed, TRUE);
  gtk_widget_set_events (fixed,
			 GDK_EXPOSURE_MASK
			 | GDK_BUTTON_PRESS_MASK
			 | GDK_BUTTON_RELEASE_MASK
			 | GDK_POINTER_MOTION_MASK
			 | GDK_POINTER_MOTION_HINT_MASK
			 | GDK_KEY_PRESS_MASK
			 | GDK_KEY_RELEASE_MASK
			 | GDK_ENTER_NOTIFY_MASK
			 | GDK_LEAVE_NOTIFY_MASK
			 | GDK_FOCUS_CHANGE_MASK);
  gtk_widget_set_size_request (fixed, 200, 100);
  gtk_grid_attach (GTK_GRID (grid), fixed, 1, row, 1, 1);
  g_object_set (fixed, "expand", FALSE, NULL);
  gtk_widget_show (fixed);

  view_id = g_strdup_printf ("%s-background", id);
  g_signal_connect (fixed, "draw",
		    G_CALLBACK (on_widget_draw), view_id);

  g_signal_connect (fixed, "enter_notify_event",
		    G_CALLBACK (on_widget_enter_notify), view_id);
  g_signal_connect (fixed, "leave_notify_event",
		    G_CALLBACK (on_widget_leave_notify), view_id);
  g_signal_connect (fixed, "motion_notify_event",
		    G_CALLBACK (on_widget_motion_notify), view_id);
  g_signal_connect (fixed, "button_press_event",
		    G_CALLBACK (on_widget_button_press), view_id);
  g_signal_connect (fixed, "button_release_event",
		    G_CALLBACK (on_widget_button_release), view_id);


  drawing_area = gtk_drawing_area_new ();
  gtk_widget_set_events (drawing_area,
			 GDK_EXPOSURE_MASK
			 | GDK_BUTTON_PRESS_MASK
			 | GDK_BUTTON_RELEASE_MASK
			 | GDK_POINTER_MOTION_MASK
			 | GDK_POINTER_MOTION_HINT_MASK
			 | GDK_KEY_PRESS_MASK
			 | GDK_KEY_RELEASE_MASK
			 | GDK_ENTER_NOTIFY_MASK
			 | GDK_LEAVE_NOTIFY_MASK
			 | GDK_FOCUS_CHANGE_MASK);

  gtk_widget_set_size_request (drawing_area, 60, 60);
  gtk_fixed_put (GTK_FIXED (fixed), drawing_area, 20, 20);
  gtk_widget_show (drawing_area);

  view_id = g_strdup_printf ("%s-left", id);
  g_signal_connect (drawing_area, "draw",
		    G_CALLBACK (on_widget_draw), view_id);

  g_signal_connect (drawing_area, "enter_notify_event",
		    G_CALLBACK (on_widget_enter_notify), view_id);
  g_signal_connect (drawing_area, "leave_notify_event",
		    G_CALLBACK (on_widget_leave_notify), view_id);
  g_signal_connect (drawing_area, "motion_notify_event",
		    G_CALLBACK (on_widget_motion_notify), view_id);
  g_signal_connect (drawing_area, "button_press_event",
		    G_CALLBACK (on_widget_button_press), view_id);
  g_signal_connect (drawing_area, "button_release_event",
		    G_CALLBACK (on_widget_button_release), view_id);


  drawing_area = gtk_drawing_area_new ();
  gtk_widget_set_events (drawing_area,
			 GDK_EXPOSURE_MASK
			 | GDK_BUTTON_PRESS_MASK
			 | GDK_BUTTON_RELEASE_MASK
			 | GDK_POINTER_MOTION_MASK
			 | GDK_POINTER_MOTION_HINT_MASK
			 | GDK_KEY_PRESS_MASK
			 | GDK_KEY_RELEASE_MASK
			 | GDK_ENTER_NOTIFY_MASK
			 | GDK_LEAVE_NOTIFY_MASK
			 | GDK_FOCUS_CHANGE_MASK);

  gtk_widget_set_size_request (drawing_area, 60, 60);
  gtk_fixed_put (GTK_FIXED (fixed), drawing_area, 120, 20);
  gtk_widget_show (drawing_area);

  view_id = g_strdup_printf ("%s-right", id);
  g_signal_connect (drawing_area, "draw",
		    G_CALLBACK (on_widget_draw), view_id);

  g_signal_connect (drawing_area, "enter_notify_event",
		    G_CALLBACK (on_widget_enter_notify), view_id);
  g_signal_connect (drawing_area, "leave_notify_event",
		    G_CALLBACK (on_widget_leave_notify), view_id);
  g_signal_connect (drawing_area, "motion_notify_event",
		    G_CALLBACK (on_widget_motion_notify), view_id);
  g_signal_connect (drawing_area, "button_press_event",
		    G_CALLBACK (on_widget_button_press), view_id);
  g_signal_connect (drawing_area, "button_release_event",
		    G_CALLBACK (on_widget_button_release), view_id);
}


static void
setup_item_signals (GooCanvasItem *item)
{
  g_signal_connect (item, "enter_notify_event",
		    G_CALLBACK (on_enter_notify), NULL);
  g_signal_connect (item, "leave_notify_event",
		    G_CALLBACK (on_leave_notify), NULL);
  g_signal_connect (item, "motion_notify_event",
		    G_CALLBACK (on_motion_notify), NULL);
  g_signal_connect (item, "button_press_event",
		    G_CALLBACK (on_button_press), NULL);
  g_signal_connect (item, "button_release_event",
		    G_CALLBACK (on_button_release), NULL);
}


static void
create_canvas (GtkGrid *grid, gint row, gchar *text, gchar *id)
{
  GtkWidget *label, *canvas;
  GooCanvasItem *root, *rect;
  char *view_id;

  label = gtk_label_new (text);
  gtk_grid_attach (grid, label, 0, row, 1, 1);
  gtk_widget_show (label);

  canvas = goo_canvas_new ();

  gtk_widget_set_size_request (canvas, 200, 100);
  goo_canvas_set_bounds (GOO_CANVAS (canvas), 0, 0, 200, 100);
  gtk_grid_attach (grid, canvas, 1, row, 1, 1);
  gtk_widget_show (canvas);

  root = goo_canvas_get_root_item (GOO_CANVAS (canvas));

  rect = goo_canvas_rect_new (root, 0, 0, 200, 100,
			      "stroke-pattern", NULL,
			      "fill-color", "yellow",
			      NULL);
  view_id = g_strdup_printf ("%s-yellow", id);
  g_object_set_data_full (G_OBJECT (rect), "id", view_id, g_free);
  setup_item_signals (rect);

  rect = goo_canvas_rect_new (root, 20, 20, 60, 60,
			      "stroke-pattern", NULL,
			      "fill-color", "blue",
			      NULL);
  view_id = g_strdup_printf ("%s-blue", id);
  g_object_set_data_full (G_OBJECT (rect), "id", view_id, g_free);
  setup_item_signals (rect);

  rect = goo_canvas_rect_new (root, 120, 20, 60, 60,
			      "stroke-pattern", NULL,
			      "fill-color", "red",
			      NULL);
  view_id = g_strdup_printf ("%s-red", id);
  g_object_set_data_full (G_OBJECT (rect), "id", view_id, g_free);
  setup_item_signals (rect);
}


GtkWidget *
create_grabs_page (void)
{
  GtkWidget *vbox, *grid, *label;

  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 4);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 4);
  gtk_widget_show (vbox);

  label = gtk_label_new ("Move the mouse over the widgets and canvas items on the right to see what events they receive.\nClick buttons to start explicit or implicit pointer grabs and see what events they receive now.\n(They should all receive the same events.)");
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
  gtk_widget_show (label);

  grid = gtk_grid_new ();
  gtk_container_set_border_width (GTK_CONTAINER (grid), 12);
  gtk_grid_set_row_spacing (GTK_GRID (grid), 12);
  gtk_grid_set_column_spacing (GTK_GRID (grid), 12);
  gtk_box_pack_start (GTK_BOX (vbox), grid, FALSE, FALSE, 0);
  g_object_set (grid, "halign", GTK_ALIGN_CENTER, NULL);
  gtk_widget_show (grid);

  /* Drawing area with explicit grabs. */
  create_fixed (GTK_GRID (grid), 1,
		"Widget with Explicit Grabs:",
		"widget-explicit");

  /* Drawing area with implicit grabs. */
  create_fixed (GTK_GRID (grid), 2,
		"Widget with Implicit Grabs:",
		"widget-implicit");

  /* Canvas with explicit grabs. */
  create_canvas (GTK_GRID (grid), 3,
		 "Canvas with Explicit Grabs:",
		 "canvas-explicit");

  /* Canvas with implicit grabs. */
  create_canvas (GTK_GRID (grid), 4,
		 "Canvas with Implicit Grabs:",
		 "canvas-implicit");

  return vbox;
}
