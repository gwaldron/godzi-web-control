/**
 * Base classes for annotations
 */

//---------------------------------------------------------

Annotation = function(args) {
    this._id = guid();
    this._visible = true;
    this._map = null;
    this._style = null;

    if (args !== undefined) {
        if (args.vislble !== undefined)
            this._visible = args.visible;
        if (args.style !== undefined)
            this._style = args.style;
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
      
    getStyle : function() {
        return this._style;
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
