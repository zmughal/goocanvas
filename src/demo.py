from gi.repository import Gtk, GdkPixbuf, GooCanvas

def setup_polyline(c):
    group = c.get_root_item()

    GooCanvas.CanvasRect (group, 0, 0, 600, 450, line_width=4.0)

    GooCanvas.CanvasPolyline.new_line(group, 0, 150, 600, 150, line_width=4.0)

    GooCanvas.CanvasPolyline.new_line(group, 0, 300, 600, 300, line_width=4.0)

    GooCanvas.CanvasPolyline.new_line(group, 200, 0, 200, 450, line_width=4.0)

    GooCanvas.CanvasPolyline.new_line(group, 400, 0, 400, 450, line_width=4.0)

    GooCanvas.CanvasPolyline (group, False,
                       (340.0, 170.0),
                       (340.0, 230.0),
                       (390.0, 230.0),
                       (390.0, 170.0),
                       stroke_color="midnightblue",
                       line_width=3.0,
                       start_arrow=True,
                       end_arrow=True,
                       arrow_tip_length=3.0,
                       arrow_length=4.0,
                       arrow_width=3.5)

    GooCanvas.CanvasPolyline (group, False,
                       (356.0, 180.0),
                       (374.0, 220.0),
                       stroke_color="blue",
                       line_width=1.0,
                       start_arrow=True,
                       end_arrow=True,
                       arrow_tip_length=5.0,
                       arrow_length=6.0,
                       arrow_width=6.0)

    GooCanvas.CanvasPolyline (group, False,
                       (356.0, 220.0),
                       start_arrow=True,
                       end_arrow=True)

def setup_canvas(c):
    root = c.get_root_item()

    #Test the simple commands like moveto and lineto: MmZzLlHhVv. */
    GooCanvas.CanvasPath (root, "M 20 20 L 40 40")
    GooCanvas.CanvasPath (root, "M30 20 l20, 20")
    GooCanvas.CanvasPath (root, "M 60 20 H 80")
    GooCanvas.CanvasPath (root, "M60 40 h20")
    GooCanvas.CanvasPath (root, "M 100,20 V 40")
    GooCanvas.CanvasPath (root, "M 120 20 v 20")

    GooCanvas.CanvasPath (root,
                  "M 180 20 h20 v20 h-20 z m 5,5 h10 v10 h-10 z",
                  fill_color="red",
    #              "fill_rule", CAIRO_FILL_RULE_EVEN_ODD,
                  )

    GooCanvas.CanvasPath (root, "M 220 20 L 260 20 L 240 40 z",
                  fill_color="red",
                  stroke_color="blue",
                  line_width=3.0,
                  )

    #Test the bezier curve commands: CcSsQqTt. */
    GooCanvas.CanvasPath (root,
                  "M20,100 C20,50 100,50 100,100 S180,150 180,100",
                  )

    GooCanvas.CanvasPath (root,
                  "M220,100 c0,-50 80,-50 80,0 s80,50 80,0",
                  )

    GooCanvas.CanvasPath (root,
                  "M20,200 Q60,130 100,200 T180,200",
                  )

    GooCanvas.CanvasPath (root,
                  "M220,200 q40,-70 80,0 t80,0",
                  )

    # Test the elliptical arc commands: Aa. */
    GooCanvas.CanvasPath (root,
                  "M200,500 h-150 a150,150 0 1,0 150,-150 z",
                  fill_color="red",
                  stroke_color="blue",
                  line_width=5.0,
                  )

    GooCanvas.CanvasPath (root,
                  "M175,475 v-150 a150,150 0 0,0 -150,150 z",
                  fill_color="yellow",
                  stroke_color="blue",
                  line_width=5.0,
                  )

    GooCanvas.CanvasPath (root,
                  """M400,600 l 50,-25
                  a25,25 -30 0,1 50,-25 l 50,-25
                  a25,50 -30 0,1 50,-25 l 50,-25
                  a25,75 -30 0,1 50,-25 l 50,-25
                  a25,100 -30 0,1 50,-25 l 50,-25""",
                  stroke_color="red",
                  line_width=5.0,
                  )

    GooCanvas.CanvasPath (root,
                  "M 525,75 a100,50 0 0,0 100,50",
                  stroke_color="red",
                  line_width=5.0,
                  )
    GooCanvas.CanvasPath (root,
                  "M 725,75 a100,50 0 0,1 100,50",
                  stroke_color="red",
                  line_width=5.0,
                  )
    GooCanvas.CanvasPath (root,
                  "M 525,200 a100,50 0 1,0 100,50",
                  stroke_color="red",
                  line_width=5.0,
                  )
    GooCanvas.CanvasPath (root,
                  "M 725,200 a100,50 0 1,1 100,50",
                  stroke_color="red",
                  line_width=5.0,
                  )

def setup_scalability(c):
    N_COLS = 5
    N_ROWS = 20
    PADDING = 10

    vbox = Gtk.VBox (homogeneous=False, spacing=4)
    vbox.set_border_width (4)
    vbox.show()

    table = Gtk.Table (2, 2, False)
    table.set_row_spacings (4)
    table.set_col_spacings (4)
    vbox.pack_start (table, True, True, 0)
    table.show ()

    frame = Gtk.Frame ()
    frame.set_shadow_type (Gtk.ShadowType.IN)
    table.attach (frame,
              0, 1, 0, 1,
              Gtk.AttachOptions.EXPAND | Gtk.AttachOptions.FILL | Gtk.AttachOptions.SHRINK,
              Gtk.AttachOptions.EXPAND | Gtk.AttachOptions.FILL | Gtk.AttachOptions.SHRINK,
              0, 0);
    frame.show()

    if 1:
        pb = GdkPixbuf.Pixbuf.new_from_file("../demo/toroid.png")
        width = pb.get_width()
        height = pb.get_height()
    else:
        pb = None
        width = 37
        height = 19

    c.set_bounds (
            0, 0,
            N_COLS * (width + PADDING),
            N_ROWS * (height + PADDING))
    c.show ();

    scrolled_win = Gtk.ScrolledWindow ()
    scrolled_win.show()
    frame.add(scrolled_win)
    scrolled_win.add(c)

    root = c.get_root_item()
    for i in range(N_COLS):
        for j in range(N_ROWS):
            if pb:
                GooCanvas.CanvasImage (root, pb,
                           i * (width + PADDING),
                           j * (height + PADDING))
            else:
                item = GooCanvas.CanvasRect (root,
                          i * (width + PADDING),
                          j * (height + PADDING),
                          width, height)
                item.props.fill_color = {True:"mediumseagreen",False:"steelblue"}[j % 2 == 0]

    return vbox

def setup_widget(c):
    root = c.get_root_item()

    #Add a few simple items. */
    GooCanvas.CanvasWidget (root, Gtk.Label("Hello World"), 50, 50, 200, 100)
    GooCanvas.CanvasWidget (root, Gtk.Entry(), 50, 250, 200, 50)

    entry = Gtk.Entry ()
    entry.set_text ("Size: -1 x -1")
    GooCanvas.CanvasWidget (root, entry, 50, 300, -1, -1)

    entry = Gtk.Entry ()
    entry.set_text ("Size: 100 x -1")
    GooCanvas.CanvasWidget (root, entry, 50, 350, 100, -1)

def demo_window(setup_func):
    c = GooCanvas.Canvas()
    c.set_size_request(600, 450)
    widget = setup_func(c) or c
    w = Gtk.Window()
    w.set_size_request(600, 450)
    w.add(widget)
    w.show_all()
    w.connect("destroy", Gtk.main_quit)

if __name__ == "__main__":
    demo_window(setup_canvas)
    demo_window(setup_polyline)
    demo_window(setup_scalability)
    demo_window(setup_widget)

    Gtk.main()
