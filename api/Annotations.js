/**
 * Annotations API.
 */

//---------------------------------------------------------

Annotation = function(map, args) {
    this._id = guid();
    this._map = map;
    this._visible = true;
    
    if ( args !== undefined ) {
        if ( args.vislble !== undefined )
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

PositionedAnnotation = function(map, args) {
    Annotation.call(map, args);
    
    this._location = new Location(0.0,0.0,0.0);
    
    if ( args !== undefined ) {
        if (args.location !== undefined) 
            this._location = args.location;
    }
};
    
PositionedAnnotation.prototype = InheritsFromClass( Annotation, {

    getLocation : function() {
        return this._location;
    },
    
    setLocation : function( location ) {
        if ( this._map !== null ) {
            this._map.sendCommand("setAnnotationNodePosition", {
                id: this._id,
                latitude: this._location.getLatitude(),
                longitude: this._location.getLongitude(),
                altitude: this._location.getAltitude()
            } );
            this._location = location;
        }
    }        
});

//---------------------------------------------------------

/**
 * Constructs a new Label Annotation.
 */
LabelAnnotation = function(map, args) {

    PositionedAnnotation.call(map, args);
    
    this._text = "Label";
    this._color = "#ffffffff";

    if (args !== undefined) {
        if (args.text !== undefined )
            this._text = args.text;
        if (args.color !== undefined )
            this._color = args.color;
    }

    if (map !== null) {
        map.sendCommand("createLabelNode", {
            id: this._id,
            latitude: this._location.getLatitude(),
            longitude: this._location.getLongitude(),
            text: this._text,
            color: this._color
        });
    }
};

CircleAnnotation.prototype = InheritsFromClass(PositionedAnnotation, {
    getRadius : function() {
        return this._radius;
    },
    getColor : function() {
        return this._color;
    }
});

//---------------------------------------------------------

/**
 * Constructs a new Circle.
 */
CircleAnnotation = function(map, args) {

    PositionedAnnotation.call(map, args);
    
    this._radius = 10000.0;
    this._color = "#ffff00ff";

    if (args !== undefined) {
        if (args.radius !== undefined)
            this._radius = args.radius;
        if (args.color !== undefined)
            this._color = args.color;
    }

    if (map !== null) {
        map.sendCommand("createCircleNode", {
            id: this.getId(),
            latitude: this.getLocation().getLatitude(),
            longitude: this.getLocation().getLongitude(),
            radius: this._radius,
            color: this._color
        });
    }
};

CircleAnnotation.prototype = InheritsFromClass(PositionedAnnotation, {
    getRadius : function() {
        return this._radius;
    },
    getColor : function() {
        return this._color;
    }
});
