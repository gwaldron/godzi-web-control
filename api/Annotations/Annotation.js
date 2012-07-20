/**
 * Base classes for annotations
 */

//---------------------------------------------------------

Annotation = function(args) {
    this._id = guid();
    this._visible = true;
    this._map = null;
    this._style = null;
    this._name = 'Annotation [' + this._id + ']';

    if (args !== undefined) {
        if (args.name !== undefined)
            this._name = args.name;
        if (args.visible !== undefined)
            this._visible = args.visible;
        if (args.style !== undefined)
            this._style = args.style;
    }
};

Annotation.prototype = {

    getId: function() {
        return this._id;
    },

    getMap: function() {
        return this._map;
    },

    getVisible: function() {
        return this._visible;
    },

    getStyle: function() {
        return this._style;
    },
    
    getName: function() {
        return this._name;
    },
    
    setName: function (name) {
        this._name = name;
    },

    setVisible: function(value) {
        if (value != this._visible && this._map !== null) {
            this._visible = value;
            this._map.sendCommand("setAnnotationNodeVisibility", {
                id: this._id,
                visible: this._visible
            });
        }
      },

    setFill: function(color) {
        this._map.sendCommand("setAnnotationColors", {
            id: this._id,
            fill: color
        });
    },

    setStroke: function(color) {
        this._map.sendCommand("setAnnotationColors", {
          id: this._id,
          stroke: color
        });
    },
    
    setOpacity: function(alpha) {
        this._map.sendCommand("setAnnotationColors", {
          id: this._id,
          opacity: alpha
        });
    },

    enableEditor: function(value) {
        this._map.sendCommand("toggleAnnotationNodeEditor", {
            id: this._id,
            enabled: value
        });
    },

    getBounds: function() {
        return this._map.sendCommand("getAnnotationBounds", {
            id: this._id
        }, true);
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
