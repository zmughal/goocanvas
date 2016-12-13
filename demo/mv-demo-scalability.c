#include <config.h>
#include <math.h>
#include <gtk/gtk.h>
#include <goocanvas.h>


#define N_COLS 5
#define N_ROWS 20
#define PADDING 10

#if 1
#define USE_PIXMAP 
#endif

GtkWidget *
create_canvas_scalability (void)
{
	GtkWidget *vbox;
	GtkWidget *scrolled_win, *canvas;
	GdkPixbuf *pixbuf;
	GooCanvasItemModel *root, *item G_GNUC_UNUSED;
	int i, j, width, height;

	vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 4);
	gtk_container_set_border_width (GTK_CONTAINER (vbox), 4);
	gtk_widget_show (vbox);

	/* Create the canvas and board */

	pixbuf = gdk_pixbuf_new_from_file("toroid.png", NULL);
#ifdef USE_PIXMAP
	width = gdk_pixbuf_get_width (pixbuf) + 3;
	height = gdk_pixbuf_get_height (pixbuf) +1;
#else
	width = 37;
	height = 19;
#endif
	
	canvas = goo_canvas_new ();
	root = goo_canvas_group_model_new (NULL, NULL);
	goo_canvas_set_root_item_model (GOO_CANVAS (canvas), root);
	g_object_unref (root);

	gtk_widget_set_size_request (canvas, 600, 450);
	goo_canvas_set_bounds (GOO_CANVAS (canvas), 0, 0,
			       N_COLS * (width + PADDING),
			       N_ROWS * (height + PADDING));
	gtk_widget_show (canvas);


	scrolled_win = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolled_win),
					     GTK_SHADOW_IN);
	gtk_widget_show (scrolled_win);
	gtk_box_pack_start (GTK_BOX (vbox), scrolled_win, TRUE, TRUE, 0);

	gtk_container_add (GTK_CONTAINER (scrolled_win), canvas);


	for (i = 0; i < N_COLS; i++) {
		for (j = 0; j < N_ROWS; j++) {
#ifdef USE_PIXMAP
		  item = goo_canvas_image_model_new (root, pixbuf,
						     i * (width + PADDING),
						     j * (height + PADDING),
						     NULL);
#else
		  item = goo_canvas_rect_model_new (root,
						    i * (width + PADDING),
						    j * (height + PADDING),
						    width, height);
		  g_object_set (item,
				"fill_color", (j%2)?"mediumseagreen":"steelblue",
				NULL);
#endif
			
		}
	}

	return vbox;
}
