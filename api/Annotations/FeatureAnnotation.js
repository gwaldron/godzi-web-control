
/**
* Constructs a new Feature node.
*/
FeatureAnnotation = function(args) {

    Annotation.call(this, args);

    if (args !== undefined) {
        if (args.geomWKT !== undefined)
            this._geomWKT = args.geomWKT;
    }
};

FeatureAnnotation.prototype = InheritsFromClass(Annotation.prototype, {

    _geomWKT: null,

    setMap: function(map) {
        if (map !== null) {
            map.sendCommand("createFeatureNode", {
                id: this.getId(),
                geomWKT: this.getGeomWKT(),
                style: this.getStyle(),
                visible: this.getVisible()
            }, false);
            this._map = map;
        }
    },

    getGeomWKT: function() {
        return this._geomWKT;
    }
});
