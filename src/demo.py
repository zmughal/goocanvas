from gi.repository import Gtk, GooCanvas

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

def demo_polyline():
    c = GooCanvas.Canvas()
    c.set_size_request(600, 450)

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
				       
#  setup_item_signals (polyline1);

    w = Gtk.Window()
    w.add(c)
    w.show_all()
    w.connect("destroy", Gtk.main_quit)

def demo_canvas():
    c = GooCanvas.Canvas()
    c.set_size_request(600, 450)

    setup_canvas(c)

    w = Gtk.Window()
    w.add(c)
    w.show_all()
    w.connect("destroy", Gtk.main_quit)

demo_canvas()
demo_polyline()

Gtk.main()
