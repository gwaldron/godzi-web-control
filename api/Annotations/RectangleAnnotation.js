
/**
* Constructs a new axis-aligned Rectangle.
*/
RectangleAnnotation = function(args) {

    PositionedAnnotation.call(this, args);

    if (args !== undefined) {
        if (args.width !== undefined)
            this._width = args.width;
        if (args.height !== undefined)
            this._height = args.height;
        if (args.draped !== undefined)
            this._draped = args.draped;            
    }
};

RectangleAnnotation.prototype = InheritsFromClass(PositionedAnnotation.prototype, {

    _width: 10000,
    _height: 5000,
    _draped: true,

    setMap: function(map) {
        if (map !== null) {
            map.sendCommand("createRectangleNode", {
                id: this.getId(),
                latitude: this.getLocation().getLatitude(),
                longitude: this.getLocation().getLongitude(),
                width: this.getWidth(),
                height: this.getHeight(),
                style: this.getStyle(),
                draped: this.getDraped()
            }, false);
            this._map = map;
        }
    },

    getWidth: function() {
        return this._width;
    },
    getHeight: function() {
        return this._height;
    },
    getDraped: function() {
        return this._draped;
    }
});
