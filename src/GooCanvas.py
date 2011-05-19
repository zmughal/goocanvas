from ..overrides import override
from ..importer import modules

GooCanvas = modules['GooCanvas']._introspection_module

__all__ = []

class CanvasPath(GooCanvas.CanvasPath):

    def __init__(self, parent, path_data, **props):
        GooCanvas.CanvasPath.__init__(self)
        if parent:
            parent.add_child(self, -1)
        if path_data:
            self.props.data = path_data
        for k,v in props.iteritems():
            self.set_property(k,v)

CanvasPath = override(CanvasPath)
__all__.append('CanvasPath')

class CanvasRect(GooCanvas.CanvasRect):

    def __init__(self, parent, x, y, width, height, **props):
        GooCanvas.CanvasPath.__init__(self)
        if parent:
            parent.add_child(self, -1)
        props.update(x=x,y=y,width=width,height=height)
        for k,v in props.iteritems():
            self.set_property(k,v)

    def __new__(cls, *args, **kwds):
        arg_len = len(args)
        kwd_len = len(kwds)
        total_len = arg_len + kwd_len

        def _new(cursor_type):
            return cls.new(cursor_type)

CanvasRect = override(CanvasRect)
__all__.append('CanvasRect')

#We cant change the numner of points once constructed, and boxed types do not allow
#arguments to constructors, so override __new__
class CanvasPoints(GooCanvas.CanvasPoints):

    def __new__(cls, *points):

        print points

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

class CanvasPolyline(GooCanvas.CanvasPolyline):

    def __init__(self, parent, close_path, *points, **props):
        GooCanvas.CanvasPolyline.__init__(self)
        if parent:
            parent.add_child(self, -1)
        props.update(close_path=close_path)
        if points:
            props.update(points=CanvasPoints(*points))
        for k,v in props.iteritems():
            self.set_property(k,v)

    @classmethod
    def new_line(cls, parent, x1, y1, x2, y2, **props):
        return cls(parent, False, *((x1,y1),(x2,y2)), **props)

CanvasPolyline = override(CanvasPolyline)
__all__.append('CanvasPolyline')
