
/**
* Constructs a new PlaceNode Annotation.
*/
PlaceAnnotation = function(args) {

    PositionedAnnotation.call(this, args);

    if (args !== undefined) {
        if (args.text !== undefined)
            this._text = args.text;
        if (args.icon !== undefined)
            this._icon = args.icon;
        if (args.color !== undefined)
            this._color = args.color;
    }
};

PlaceAnnotation.prototype = InheritsFromClass(PositionedAnnotation.prototype, {

    _text: "Label",
    _icon: "",
    _color: "#ffffffff",

    setMap: function(map) {
        if (map !== undefined) {
            map.sendCommand("createPlaceNode", {
                id: this._id,
                latitude: this.getLocation().getLatitude(),
                longitude: this.getLocation().getLongitude(),
                text: this.getText(),
                iconURI: this.getIcon(),
                style: this.getStyle()
            }, false);
            this._map = map;
        }
    },

    getText: function() {
        return this._text;
    },

    getIcon: function() {
        return this._icon;
    },

    getColor: function() {
        return this._color;
    }
});
