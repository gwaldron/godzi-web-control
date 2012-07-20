
/**
* Constructs a new Feature node.
*/
FeatureAnnotation = function(args) {

    Annotation.call(this, args);

    if (args !== undefined) {
        if (args.geomWKT !== undefined)
            this._geomWKT = args.geomWKT;
        if (args.draped !== undefined)
            this._draped = args.draped;
    }
};

FeatureAnnotation.prototype = InheritsFromClass(Annotation.prototype, {

    _geomWKT: null,
    _draped: false,

    setMap: function(map) {
        if (map !== null) {
            map.sendCommand("createFeatureNode", {
                id: this.getId(),
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
