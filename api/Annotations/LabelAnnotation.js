
/**
* Constructs a new Label Annotation.
*/
LabelAnnotation = function(args) {

    PositionedAnnotation.call(this, args);

    if (args !== undefined) {
        if (args.text !== undefined)
            this._text = args.text;
        if (args.color !== undefined)
            this._color = args.color;
    }
};

LabelAnnotation.prototype = InheritsFromClass(PositionedAnnotation.prototype, {

    _text: "Label",
    _color: "#ffffffff",

    setMap: function(map) {
        if (map !== undefined) {
            map.sendCommand("createLabelNode", {
                id: this._id,
                latitude: this.getLocation().getLatitude(),
                longitude: this.getLocation().getLongitude(),
                text: this.getText(),
                style: this.getStyle()
            }, false);
            this._map = map;
        }
    },

    getText: function() {
        return this._text;
    },

    getColor: function() {
        return this._color;
    }
});
