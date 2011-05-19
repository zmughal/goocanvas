from ..overrides import override
from ..importer import modules

GooCanvas = modules['GooCanvas']._introspection_module

__all__ = []

class _GooCanvasItem:
    def __init__(self, parent, **props):
        if parent:
            parent.add_child(self, -1)
        for k,v in props.iteritems():
            self.set_property(k,v)

class CanvasPath(GooCanvas.CanvasPath, _GooCanvasItem):

    def __init__(self, parent, path_data, **props):
        GooCanvas.CanvasPath.__init__(self)
        _GooCanvasItem.__init__(self, parent, **props)
        if path_data:
            self.props.data = path_data

CanvasPath = override(CanvasPath)
__all__.append('CanvasPath')

class CanvasRect(GooCanvas.CanvasRect, _GooCanvasItem):

    def __init__(self, parent, x, y, width, height, **props):
        GooCanvas.CanvasPath.__init__(self)
        props.update(x=x,y=y,width=width,height=height)
        _GooCanvasItem.__init__(self, parent, **props)

CanvasRect = override(CanvasRect)
__all__.append('CanvasRect')

#We cant change the numner of points once constructed, and boxed types do not allow
#arguments to constructors, so override __new__
class CanvasPoints(GooCanvas.CanvasPoints):

    def __new__(cls, *points):

        assert len(points)
        assert len(points[0])

        obj = cls.new(len(points))
        i = 0
        for p in points:
            obj.set_point(i, p[0],p[1])
            i += 1
        return obj

CanvasPoints = override(CanvasPoints)
__all__.append('CanvasPoints')

class CanvasPolyline(GooCanvas.CanvasPolyline, _GooCanvasItem):

    def __init__(self, parent, close_path, *points, **props):
        GooCanvas.CanvasPolyline.__init__(self)
        props.update(close_path=close_path)
        if points:
            props.update(points=CanvasPoints(*points))
        _GooCanvasItem.__init__(self, parent, **props)

    @classmethod
    def new_line(cls, parent, x1, y1, x2, y2, **props):
        return cls(parent, False, *((x1,y1),(x2,y2)), **props)

CanvasPolyline = override(CanvasPolyline)
__all__.append('CanvasPolyline')

class CanvasImage(GooCanvas.CanvasImage, _GooCanvasItem):

    def __init__(self, parent, pixbuf, x, y, **props):
        GooCanvas.CanvasImage.__init__(self)
        props.update(pixbuf=pixbuf,x=x,y=y)
        _GooCanvasItem.__init__(self, parent, **props)

CanvasImage = override(CanvasImage)
__all__.append('CanvasImage')

class CanvasWidget(GooCanvas.CanvasWidget, _GooCanvasItem):

    def __init__(self, parent, widget, x, y, width, height, **props):
        GooCanvas.CanvasWidget.__init__(self)
        #taken from the C constructor
        #g_object_set_data (G_OBJECT (witem->widget), "goo-canvas-item", witem);
        widget.show()
        props.update(widget=widget,x=x,y=y,width=width,height=height)
        _GooCanvasItem.__init__(self, parent, **props)

CanvasWidget = override(CanvasWidget)
__all__.append('CanvasWidget')
