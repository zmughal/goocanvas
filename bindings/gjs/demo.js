#!/usr/bin/env gjs

const Gtk = imports.gi.Gtk;
const GdkPixbuf = imports.gi.GdkPixbuf;
const GooCanvas = imports.gi.GooCanvas;
const cairo = imports.cairo;


let setup_polyline = function(c) {
    let group = c.get_root_item();
    
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
    
    /* Or any of this */
    /*
    p_points = GooCanvas.CanvasPoints((
                            (340.0, 170.0),
                            (340.0, 230.0),
                            (390.0, 230.0),
                            (390.0, 170.0)))
    GooCanvas.CanvasPolyline (parent=group, close_path=False,
                       points=p_points,
                       stroke_color="midnightblue",
                       line_width=3.0,
                       start_arrow=True,
                       end_arrow=True,
                       arrow_tip_length=3.0,
                       arrow_length=4.0,
                       arrow_width=3.5)

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


let setup_canvas = function(c) {
    let root = c.get_root_item();
    
    // Test the simple commands like moveto and lineto: MmZzLlHhVv
    new GooCanvas.CanvasPath({ parent:root, data:"M 20 20 L 40 40" });
    new GooCanvas.CanvasPath({ parent:root, data:"M30 20 l20, 20"  });
    new GooCanvas.CanvasPath({ parent:root, data:"M 60 20 H 80" });
    new GooCanvas.CanvasPath({ parent:root, data:"M60 40 h20"});
    new GooCanvas.CanvasPath({ parent:root, data:"M 100,20 V 40"});
    
    // Test empty
    let p = new GooCanvas.CanvasPath({ parent: root, data:"" });
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
                  
    let t = new GooCanvas.CanvasText ({
        parent: root,
        text: "Hello World!",
        anchor: GooCanvas.CanvasAnchorType.CENTER
    });
    
};

let setup_scalability = function(c) {
    const N_COLS = 5;
    const N_ROWS = 20;
    const PADDING = 10;
    
    let vbox = new Gtk.VBox({homogeneous: false, spacing: 4});
    vbox.set_border_width(4);
    vbox.show();
    
    let table = Gtk.Table.new(2, 2, false);
    table.set_row_spacings(4);
    table.set_col_spacings(4);
    vbox.pack_start(table, true, true, 0);
    table.show();
    
    let frame = new Gtk.Frame();
    frame.set_shadow_type(Gtk.ShadowType.IN);
    table.attach(frame, 0, 1, 0, 1,
              Gtk.AttachOptions.EXPAND | Gtk.AttachOptions.FILL | Gtk.AttachOptions.SHRINK,
              Gtk.AttachOptions.EXPAND | Gtk.AttachOptions.FILL | Gtk.AttachOptions.SHRINK,
              0, 0);
    frame.show();
    
    let pb;
    let width;
    let height;
    
    if (1) {
        pb = GdkPixbuf.Pixbuf.new_from_file("../demo/toroid.png");
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

    let scrolled_win = new Gtk.ScrolledWindow();
    scrolled_win.show()
    frame.add(scrolled_win)
    scrolled_win.add(c)

    let root = c.get_root_item()
    for (let i=0; i<N_COLS; i++) {
        for (let j=0; j<N_ROWS; j++) {
            if (pb) {
                new GooCanvas.CanvasImage ({
                            parent: root,
                            pixbuf: pb,
                            x: i * (width + PADDING),
                            y: j * (height + PADDING) });
            } else {
                let item = new GooCanvas.CanvasRect ({
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

let setup_widget = function(c) {
    let root = c.get_root_item();
    
    new GooCanvas.CanvasWidget({
        parent: root,
        widget: Gtk.Label.new("Hello World"),
        x: 50, y: 50, width: 200, height: 100
    });
    new GooCanvas.CanvasWidget({
        parent: root,
        widget: new Gtk.Entry(),
        x: 50, y: 250, width: 200, height: 50
    });
    
    let entry = new Gtk.Entry();
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


let demo_window = function(setup_func) {
    let c = new GooCanvas.Canvas();
    c.set_size_request(600, 450);
    let widget = setup_func(c);
    if (!widget) {
        widget = c;
    }
    let w = new Gtk.Window();
    w.set_size_request(600, 450);
    w.add(widget);
    w.show_all();
    w.connect("destroy", Gtk.main_quit);
}

Gtk.init(null, null);

demo_window(setup_canvas);
demo_window(setup_polyline);
demo_window(setup_scalability);
demo_window(setup_widget);

Gtk.main()
