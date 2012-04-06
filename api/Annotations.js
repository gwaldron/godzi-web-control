/**
 * Annotations API.
 */

//---------------------------------------------------------

Annotation = function(args) {
    this._id = guid();
    this._visible = true;
    this._map = null;

    if (args !== undefined) {
        if (args.vislble !== undefined)
            this._visible = args.visible;
    }
};

Annotation.prototype = {

    getId : function() {
        return this._id;
    },    
    
    getMap : function() {
        return this._map;
    },    
    
    getVisible : function() {
        return this._visible;
    },    
    
    setVisible : function(value) {
        if ( value != this._visible && this._map !== null ) {
            this._map.sendCommand( "setAnnotationNodeVisibility", {
                visible: this._visible
            } );
        }
    }
};

//---------------------------------------------------------

PositionedAnnotation = function(args) {

    Annotation.call(this, args);
    
    if ( args !== undefined ) {
        if (args.location !== undefined) 
            this._location = args.location;
    }
};
    
PositionedAnnotation.prototype = InheritsFromClass( Annotation.prototype, {

    _location: new Location(0,0,0),

    getLocation : function() {
        return this._location;
    },
    
    setLocation : function( location ) {
        if ( this._map !== null ) {
            this._map.sendCommand("setAnnotationNodePosition", {
                id: this._id,
                latitude: this.getLocation().getLatitude(),
                longitude: this.getLocation().getLongitude(),
                altitude: this.getLocation().getAltitude()
            } );
            this._location = location;
        }
    }  
});

//---------------------------------------------------------

/**
 * Constructs a new Label Annotation.
 */
LabelAnnotation = function(args) {

    PositionedAnnotation.call(this, args);

    if (args !== undefined) {
        if (args.text !== undefined )
            this._text = args.text;
        if (args.color !== undefined )
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
                color: this.getColor()
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

//---------------------------------------------------------

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
                color: this.getColor()
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

//---------------------------------------------------------

/**
 * Constructs a new Circle.
 */
CircleAnnotation = function(args) {

    PositionedAnnotation.call(this, args);

    if (args !== undefined) {
        if (args.radius !== undefined)
            this._radius = args.radius;
        if (args.color !== undefined)
            this._color = args.color;
    }
};

CircleAnnotation.prototype = InheritsFromClass(PositionedAnnotation.prototype, {

    _radius: 10000.0,
    _color: "#ffff00ff",
    
    setMap: function(map) {
        if (map !== null) {
            map.sendCommand("createCircleNode", {
                id: this.getId(),
                latitude: this.getLocation().getLatitude(),
                longitude: this.getLocation().getLongitude(),
                radius: this.getRadius(),
                color: this.getColor()
            }, false);
            this._map = map;
        }
    },
    
    getRadius: function() {
        return this._radius;
    },
    
    getColor: function() {
        return this._color;
    }
});
