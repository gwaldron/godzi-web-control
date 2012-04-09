
/**
* Constructs a new Circle.
*/
CircleAnnotation = function(args) {

    PositionedAnnotation.call(this, args);

    if (args !== undefined) {
        if (args.radius !== undefined)
            this._radius = args.radius;
    }
};

CircleAnnotation.prototype = InheritsFromClass(PositionedAnnotation.prototype, {

    _radius: 10000.0,
    _draped: true,

    setMap: function(map) {
        if (map !== null) {
            map.sendCommand("createCircleNode", {
                id: this.getId(),
                latitude: this.getLocation().getLatitude(),
                longitude: this.getLocation().getLongitude(),
                radius: this.getRadius(),
                style: this.getStyle(),
                draped: this.getDraped()
            }, false);
            this._map = map;
        }
    },

    getRadius: function() {
        return this._radius;
    },
    getDraped: function() {
        return this._draped;
    }
});
