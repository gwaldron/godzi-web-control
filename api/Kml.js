/**
* Loads a KML file
*/

//---------------------------------------------------------

Kml = function(args) {
  this._id = guid();
  this._visible = true;
  this._map = null;
  this._name = 'KML [' + this._id + ']';
  this._url = '';
  this._defaultIconURI = '';
  this._iconBaseScale = 1.0;
  this._iconMaxSize = 32;
  this._declutter = true;

  if (args !== undefined) {
    if (args.name !== undefined)
      this._name = args.name;
    if (args.vislble !== undefined)
      this._visible = args.visible;
    if (args.url !== undefined)
      this._url = args.url;
    if (args.defaultIconURI !== undefined)
      this._defaultIconURI = args.defaultIconURI;
    if (args.iconBaseScale !== undefined)
      this._iconBaseScale = args.iconBaseScale;
    if (args.iconMaxSize !== undefined)
      this._iconMaxSize = args.iconMaxSize;
    if (args.declutter !== undefined)
      this._declutter = args.declutter;
  }
};

Kml.prototype = {

  getId: function() {
    return this._id;
  },

  getMap: function() {
    return this._map;
  },

  getName: function() {
    return this._name;
  },

  setName: function(name) {
    this._name = name;
  },
  
  getVisible: function() {
    return this._visible;
  },
  
//  setVisible: function(value) {
//    if (value != this._visible && this._map !== null) {
//      this._visible = value;
//      this._map.sendCommand("setAnnotationNodeVisibility", {
//        id: this._id,
//        visible: this._visible
//      });
//    }
//  },

  getUrl: function() {
    return this._url;
  },
  
  setUrl: function(url) {
    this._url = url;
  },

//  getBounds: function() {
//    return this._map.sendCommand("getAnnotationBounds", {
//      id: this._id
//    }, true);
//  },

  setMap: function(map) {
      if (map !== null) {
            map.sendCommand("loadKml", {
                id: this.getId(),
                url: this._url,
                defaultIconURI: this._defaultIconURI,
                iconBaseScale: this._iconBaseScale,
                iconMaxSize: this._iconMaxSize,
                declutter: this._declutter
            }, false);
            this._map = map;
        }
    }
};