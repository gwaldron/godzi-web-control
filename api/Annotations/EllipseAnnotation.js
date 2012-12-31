
/**
* Constructs a new Ellipse
*/
EllipseAnnotation = function(args) {

    PositionedAnnotation.call(this, args);

    if (args !== undefined) {
        if (args.radiusMajor !== undefined)
            this._radiusMajor = args.radiusMajor;
        if (args.radiusMinor !== undefined)
            this._radiusMinor = args.radiusMinor;
        if (args.rotation !== undefined)
            this._rotation = args.rotation;
    }
};

EllipseAnnotation.prototype = InheritsFromClass(PositionedAnnotation.prototype, {

    _radiusMajor: 20000.0,
    _radiusMinor: 13000.0,
    _rotation: 0.0,

    setMap: function(map) {
        if (map !== null) {
            map.sendCommand("createEllipseNode", {
                id: this.getId(),
                latitude: this.getLocation().getLatitude(),
                longitude: this.getLocation().getLongitude(),
                radiusMajor: this.getRadiusMajor(),
                radiusMinor: this.getRadiusMinor(),
                rotation: this.getRotation(),
                style: this.getStyle(),
                visible: this.getVisible()
            }, false);
            this._map = map;
        }
    },

    getRadiusMajor: function() {
        return this._radiusMajor;
    },
    getRadiusMinor: function() {
        return this._radiusMinor;
    },
    getRotation: function() {
        return this._rotation;
    }
});
