
/**
* Constructs a new localized geometry node.
*/
LocalGeometryAnnotation = function(args) {

    PositionedAnnotation.call(this, args);

    if (args !== undefined) {
        if (args.geomWKT !== undefined)
            this._geomWKT = args.geomWKT;
        if (args.draped !== undefined)
            this._draped = args.draped;
    }
};

LocalGeometryAnnotation.prototype = InheritsFromClass(PositionedAnnotation.prototype, {

    _geomWKT: null,
    _draped: false,

    setMap: function(map) {
        if (map !== null) {
            map.sendCommand("createLocalGeometryNode", {
                id: this.getId(),
                latitude: this.getLocation().getLatitude(),
                longitude: this.getLocation().getLongitude(),
                altitude: this.getLocation().getAltitude(),
                geomWKT: this.getGeomWKT(),
                style: this.getStyle(),
                draped: this.getDraped(),
                visible: this.getVisible()
            }, false);
            this._map = map;
        }
    },

    getGeomWKT: function() {
        return this._geomWKT;
    },
    getDraped: function() {
        return this._draped;
    }
});
