#!/usr/bin/env seed

const Gtk = imports.gi.Gtk;
const GdkPixbuf = imports.gi.GdkPixbuf;
const GooCanvas = imports.gi.GooCanvas;
const cairo = imports.cairo;


var setup_polyline = function(c) {
    var group = c.get_root_item();
    
    new GooCanvas.CanvasRect ({
        parent: group,
        x: 0,
        y: 0,
        width: 600,
        height: 450,
        line_width: 4.0
    });

    /* No way to do this in JS yet */
    /*
    GooCanvas.CanvasPolyline.new_line(group, 0, 150, 600, 150, line_width=4.0)

    GooCanvas.CanvasPolyline.new_line(group, 0, 300, 600, 300, line_width=4.0)

    GooCanvas.CanvasPolyline.new_line(group, 200, 0, 200, 450, line_width=4.0)

    GooCanvas.CanvasPolyline.new_line(group, 400, 0, 400, 450, line_width=4.0)
    */
    
    /* This can be done in Seed but not GJS; very longwinded though */
    var p_points = new GooCanvas.CanvasPoints.c_new(4);
    p_points.set_point(0, 340.0, 170.0);
    p_points.set_point(1, 340.0, 230.0);
    p_points.set_point(2, 390.0, 230.0);
    p_points.set_point(3, 390.0, 170.0);
    
    new GooCanvas.CanvasPolyline({
        parent: group,
        points: p_points,
        stroke_color: "midnightblue",
        start_arrow: true,
        end_arrow: true,
        arrow_tip_length: 3.0,
        arrow_length: 4.0,
        arrow_width: 3.5
    });
    
    /* etc...
    p_points = GooCanvas.CanvasPoints((
                            (356.0, 180.0),
                            (374.0, 220.0)))
    GooCanvas.CanvasPolyline (parent=group, close_path=False,
                       points=p_points,
                       stroke_color="blue",
                       line_width=1.0,
                       start_arrow=True,
                       end_arrow=True,
                       arrow_tip_length=5.0,
                       arrow_length=6.0,
                       arrow_width=6.0)

    GooCanvas.CanvasPolyline (parent=group, close_path=False,
                       points=GooCanvas.CanvasPoints(((356.0, 220.0),)),
                       start_arrow=True,
                       end_arrow=True)
    */
};


var setup_canvas = function(c) {
    var root = c.get_root_item();
    
    // Test the simple commands like moveto and lineto: MmZzLlHhVv
    new GooCanvas.CanvasPath({ parent:root, data:"M 20 20 L 40 40" });
    new GooCanvas.CanvasPath({ parent:root, data:"M30 20 l20, 20"  });
    new GooCanvas.CanvasPath({ parent:root, data:"M 60 20 H 80" });
    new GooCanvas.CanvasPath({ parent:root, data:"M60 40 h20"});
    new GooCanvas.CanvasPath({ parent:root, data:"M 100,20 V 40"});
    
    // Test empty
    var p = new GooCanvas.CanvasPath({ parent: root, data:"" });
    p.data = "M 120 20 v 20";
    
    new GooCanvas.CanvasPath({
        parent: root,
        data: "M 180 20 h20 v20 h-20 z m 5,5 h10 v10 h-10 z",
        fill_color: "red",
        //fill_rule: cairo.FILL_RULE_EVEN_ODD,
    });
    
    new GooCanvas.CanvasPath({
        parent:root,
        data: "M 220 20 L 260 20 L 240 40 z",
        fill_color: "red",
        stroke_color: "blue",
        line_width: 3.0
    });
    
    //Test the bezier curve commands: CcSsQqTt.   
    new GooCanvas.CanvasPath({
        parent: root,
        data: "M20,100 C20,50 100,50 100,100 S180,150 180,100"
    });
    
    new GooCanvas.CanvasPath({
        parent: root,
        data: "M220,100 c0,-50 80,-50 80,0 s80,50 80,0"
    });
    
    new GooCanvas.CanvasPath({
        parent: root,
        data: "M20,200 Q60,130 100,200 T180,200"
    });
    
    new GooCanvas.CanvasPath({
        parent: root,
        data: "M220,200 q40,-70 80,0 t80,0"
    });
    
    // Test the elliptical arc commands: Aa.
    new GooCanvas.CanvasPath ({
        parent: root,
        data: "M200,500 h-150 a150,150 0 1,0 150,-150 z",
        fill_color: "red",
        stroke_color: "blue",
        line_width: 5.0,
    });
    
    new GooCanvas.CanvasPath({
        parent: root,
        data: "M175,475 v-150 a150,150 0 0,0 -150,150 z",
        fill_color: "yellow",
        stroke_color: "blue",
        line_width: 5.0
    });

    new GooCanvas.CanvasPath({
        parent: root,
        data: "M400,600 l 50,-25 " +
              "a25,25 -30 0,1 50,-25 l 50,-25" +
              "a25,50 -30 0,1 50,-25 l 50,-25" +
              "a25,75 -30 0,1 50,-25 l 50,-25" +
              "a25,100 -30 0,1 50,-25 l 50,-25",
        stroke_color: "red",
        line_width: 5.0,
    });

    new GooCanvas.CanvasPath({
        parent: root,
        data: "M 725,75 a100,50 0 0,1 100,50",
        stroke_color: "red",
        line_width: 5.0,
    });
    new GooCanvas.CanvasPath({
        parent: root,
        data: "M 525,200 a100,50 0 1,0 100,50",
        stroke_color: "red",
        line_width: 5.0,
    });
    new GooCanvas.CanvasPath({
        parent: root,
        data: "M 725,200 a100,50 0 1,1 100,50",
        stroke_color: "red",
        line_width: 5.0,
    });
                  
    var t = new GooCanvas.CanvasText ({
        parent: root,
        text: "Hello World!",
        anchor: GooCanvas.CanvasAnchorType.CENTER
    });
};

var setup_scalability = function(c) {
    const N_COLS = 5;
    const N_ROWS = 20;
    const PADDING = 10;
    
    var vbox = new Gtk.VBox({homogeneous: false, spacing: 4});
    vbox.set_border_width(4);
    vbox.show();
    
    var table = new Gtk.Table.c_new(2, 2, false);
    table.set_row_spacings(4);
    table.set_col_spacings(4);
    vbox.pack_start(table, true, true, 0);
    table.show();
    
    var frame = new Gtk.Frame();
    frame.set_shadow_type(Gtk.ShadowType.IN);
    table.attach(frame, 0, 1, 0, 1,
              Gtk.AttachOptions.EXPAND | Gtk.AttachOptions.FILL | Gtk.AttachOptions.SHRINK,
              Gtk.AttachOptions.EXPAND | Gtk.AttachOptions.FILL | Gtk.AttachOptions.SHRINK,
              0, 0);
    frame.show();
    
    var pb;
    var width;
    var height;
    
    if (1) {
        pb = new GdkPixbuf.Pixbuf.from_file("../demo/toroid.png");
        width = pb.get_width();
        height = pb.get_height();
    } else {
        pb = null;
        width = 37;
        height = 19;
    }
    
    c.set_bounds (
            0, 0,
            N_COLS * (width + PADDING),
            N_ROWS * (height + PADDING));
    c.show ();

    var scrolled_win = new Gtk.ScrolledWindow();
    scrolled_win.show()
    frame.add(scrolled_win)
    scrolled_win.add(c)

    var root = c.get_root_item()
    for (var i=0; i<N_COLS; i++) {
        for (var j=0; j<N_ROWS; j++) {
            if (pb) {
                new GooCanvas.CanvasImage ({
                            parent: root,
                            pixbuf: pb,
                            x: i * (width + PADDING),
                            y: j * (height + PADDING) });
            } else {
                var item = new GooCanvas.CanvasRect ({
                            parent: root,
                            x: i * (width + PADDING),
                            y: j * (height + PADDING),
                            width: width,
                            height: height});
                item.fill_color = (j % 2 === 0) ? "mediumseagreen" : "steelblue";
            }
        }           
    }

    return vbox;
}

var setup_widget = function(c) {
    var root = c.get_root_item();
    
    new GooCanvas.CanvasWidget({
        parent: root,
        widget: new Gtk.Label.c_new("Hello World"),
        x: 50, y: 50, width: 200, height: 100
    });
    new GooCanvas.CanvasWidget({
        parent: root,
        widget: new Gtk.Entry(),
        x: 50, y: 250, width: 200, height: 50
    });
    
    var entry = new Gtk.Entry();
    entry.set_text("Size: -1 x -1");
    new GooCanvas.CanvasWidget({
        parent: root,
        widget: entry,
        x: 50, y: 300, width: -1, height: -1
    });
    
    entry = new Gtk.Entry();
    entry.set_text("Size: 100 x -1");
    new GooCanvas.CanvasWidget({
        parent: root,
        widget: entry,
        x:50, y: 350, width: 100, height: -1
    });
}


var demo_window = function(setup_func) {
    var c = new GooCanvas.Canvas();
    c.set_size_request(600, 450);
    var widget = setup_func(c);
    if (!widget) {
        widget = c;
    }
    var w = new Gtk.Window();
    w.set_size_request(600, 450);
    w.add(widget);
    w.show_all();
    w.signal.destroy.connect(Gtk.main_quit);
}

Gtk.init(null, null);

demo_window(setup_canvas);
demo_window(setup_polyline);
demo_window(setup_scalability);
demo_window(setup_widget);

Gtk.main()
