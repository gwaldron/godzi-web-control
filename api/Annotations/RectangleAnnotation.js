
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
    }
};

RectangleAnnotation.prototype = InheritsFromClass(PositionedAnnotation.prototype, {

    _width: 10000,
    _height: 5000,

    setMap: function(map) {
        if (map !== null) {
            map.sendCommand("createRectangleNode", {
                id: this.getId(),
                latitude: this.getLocation().getLatitude(),
                longitude: this.getLocation().getLongitude(),
                width: this.getWidth(),
                height: this.getHeight(),
                style: this.getStyle(),
                visible: this.getVisible()
            }, false);
            this._map = map;
        }
    },

    getWidth: function() {
        return this._width;
    },
    getHeight: function() {
        return this._height;
    }
});
