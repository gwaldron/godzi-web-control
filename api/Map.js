/**
 * The primary Map object
 * @param {string} id An id that corresponds to the id passed to the page's Map.embed() call
 * @constructor
 */
function Map( id )
{  
  this._id = id;  
  this._plugin = document.getElementById(this._id);

   var map = this;
   
   var handleEvent = function(target,eventName,data)
	{
	  //If the target is empty, that means that the target is the Map
	  if (target.length == 0)
          {
            //Find the list of event callbacks by the given name.
	    var callbackArray = map._eventCallbacks[eventName];
	    if (callbackArray)
	    {
              //Try to parse any data passed in.
              var obj = null;
	      try
	      {
                obj = jQuery.parseJSON(data);
	      }
	      catch(e)
	      {
                //alert(e + "Data=" + data);
              }

	      //Call each callback
	      for (var i in callbackArray)
                callbackArray[i](obj);
	    }
            //else
	    //{
	    //  alert("Couldn't find event " + eventName);
	    //}
	  }
	}
  
  //FF
  if (BrowserDetect.browser == "Firefox")
  {
    this._plugin.godziEventHandler = handleEvent;
  }
  else if (BrowserDetect.browser == "Explorer")
  {
    this._plugin.attachEvent("godzievent", handleEvent);
  }  

}

/**
 * Method used to embed the map control into page.
 * @param {string} div_id ID of the parent div
 * @param {string} map_id ID of the map control object
 * @param {string} type MIME type, should be "application/godzi" unless chaned in the plugin code
 * @param {string} classid Windows Class ID, should be "5F41CFDD-D29D-4CA8-B37D-6F64F28E12EA" unless changed in the plugin code
 */
Map.embed = function(div_id, map_id, type, classid )
{
    //Firefox uses the type attribute to determine what plugin to use
    if (BrowserDetect.browser == "Firefox")
    {
	      if (detectGodzi())
	      {
	          objString = '<object id="' + map_id + '" type="' + type + '" width="100%" height="100%" border="0" frameborder="no"/>';
	     }
	     else
       {
	         objString = '<iframe src="http://demo.pelicanmapping.com/rmweb/plugintest.html" width="100%" frameborder="0" height="100%"></iframe>';
	     }
    }
    else if (BrowserDetect.browser == "Explorer")
    {
	      if (detectGodzi())
	      {
	          objString = '<object id="' + map_id + '" height="100%" width="100%" classid="' + classid + '"/>';
	      }
	      else
	      {
	          objString = '<iframe src="http://demo.pelicanmapping.com/rmweb/plugintest.html" width="100%" frameborder="0" height="100%"></iframe>';
	      }
    }
    else
    {
        alert("Unsupported browser" + BrowserDetect.browser);
    }

    document.getElementById(div_id).innerHTML = objString;
}

Map.prototype._id;

Map.prototype.getId = function() {
  return this._id;
}

Map.prototype._eventCallbacks = {};

/**
 * Adds an callback for the specified event
 * @param {string} type The event to register for
 * @param {function} callback The callback function
 */
Map.prototype.addEvent = function( type, callback ) {
  //Registers callbacks for events by type
  //Valid types include:
  //  onmousedown, onmouseup, onmousemove, onframeend, onclick, ondoubleclick

  //Make sure the given callback is indeed a function.
  if ( typeof callback != "function" )
    throw new TypeError();

  //If a collection of callbacks for the given event type already extists,
  //add the callback to the collection. If not, start a new collection.
  if ( type in this._eventCallbacks )
  {
    this._eventCallbacks[type].push(callback);
  }
  else
  {
    this._eventCallbacks[type] = [callback];
  }
}

// convenience: json-ifies input and output
Map.prototype.sendCommand = function(command, args, hasResult) {

    if (hasResult === undefined)
        hasResult = false;
        
    var json_args = $.toJSON(args);
    if (hasResult) {
        var result = this._plugin.sendCommand(command, json_args, hasResult);
        return jQuery.parseJSON(result);
    }
    else {
        this._plugin.sendCommand(command, json_args, hasResult);
    }
}

/**
 * Changes the visibility of an object in the scene (model, icon, label, etc.)
 * @param {string} id The object's ID
 * @param {bool} visible The desired visibility
 */
Map.prototype.changeVisibility = function( id, visible )
{
    this.sendCommand( "changeVisibility", { id: id, visible: visible }, false );
}

//Map.prototype.getObjectInfoByName = function( name )
//{
//    return this.sendCommand( "getObjectInfo", { name: name }, true );
//}

//Map.prototype.getObjectInfoByDescription = function( description )
//{
//    return this.sendCommand( "getObjectInfo", { description: description }, true );
//}

//Map.prototype.getObjectInfoAtMouse = function( mx, my )
//{
//    return this.sendCommand( "getObjectInfo", { mouseX: mx, mouseY: my }, true );
//}    

/**
 * Gets the map's background color (not valid if the sky is displayed)
 * @returns {Color} A Color object representing the background color
 */
Map.prototype.getBackColor = function()
{
    var result = this.sendCommand( "getBackColor", {}, true );
    return new Color( result.r, result.g, result.b, result.a );
}

/**
 * Sets the map's background color (not valid if the sky is displayed)
 * @param {Color} color Color object defining the new background color
 */
Map.prototype.setBackColor = function(color)
{
    this.sendCommand( "setBackColor", { "r": color.getR(), "g": color.getG(), "b": color.getB(), "a": color.getA() }, false );
}

/**
 * Cycles through the stats overlay displays. Equivalent of pressing the 's' key.
 */
Map.prototype.toggleStats = function()
{
    this.sendCommand( "toggleStats", {}, false );
}

/**
 * Toggles wireframe/vertex display modes. Equivalent of pressing the 'w' key.
 */
Map.prototype.cyclePolygonMode = function()
{
    this.sendCommand( "cyclePolygonMode", {}, false );
}

/**
 * Adds a text label to the scene.
 * @param {TextLabel} label The label to add
 */
Map.prototype.addTextLabel = function(label)
{
  label.setMap(this);
}

/**
 * Removes a text label from the scene
 * @param {TextLabel} label The label to remove
 */
Map.prototype.removeTextLabel = function(label)
{
  this.sendCommand("removeTextLabel", { id: label._id }, false);
}

/**
 * Adds a GeoIcon to the scene
 * @param {GeoIcon} icon The icon to add
 */
Map.prototype.addIcon = function(icon)
{
  icon.setMap(this);
}

/**
 * Removes a GeoIcon from the scene
 * @param {GeoIcon} icon The icon to remove
 */
Map.prototype.removeIcon = function(icon)
{
  this.sendCommand("removeIcon", { id: icon._id }, false);
}

/**
 * Adds a model to the scene
 * @param {Model} model The model to add
 */
Map.prototype.addModel = function(model)
{
    model.setMap(this);
}

/**
 * Removes a model from the scene
 * @param {Model} model The model to remove
 */
Map.prototype.removeModel = function(model)
{
  this.sendCommand("removeModel", { id: model._id }, false);
}

/**
 * Adds an image layer to the map
 * @param {supported layer type} layer The layer to add
 */
Map.prototype.addImageLayer = function(layer)
{
	layer.setMap(this, "updateImageLayer");}

/**
 * Updates an image layer
 * @param {ImageLayerProperties} updated layer properties
 */
Map.prototype.updateImageLayer = function(layer)
{
  this.sendCommand("updateImageLayer", { 'id': layer._id, 'name': layer._name, 'visible': layer._visible, 'opacity': layer._opacity }, false);
}

/**
 * Gets an image layer's properties
 * @param {integer} layer id
 */
Map.prototype.getImageLayerProperties = function(id)
{
  return this.sendCommand("getImageLayerProperties", { "id" : id }, true);
}

/**
* Moves an image layer to the given index
* @param {supported layer type} layer The layer to move
* @param {integer} newIndex The (valid) new index of the layer
*/
Map.prototype.moveImageLayer = function(layer, newIndex)
{
  //alert('Moving layer ' + layer._id + ' to index ' + newIndex);
	this.sendCommand("moveImageLayer", { id: layer._id, index: newIndex }, false);
}

/**
 * Removes an image layer from the map
 * @param {supported layer type} layer The layer to remove
 */
Map.prototype.removeImageLayer = function(layer)
{
	this.sendCommand("removeImageLayer", { id: layer._id }, false);}

/**
 * Gets a list of image layers from the map
 */
Map.prototype.getImageLayers = function()
{
  var result = this.sendCommand("getImageLayers", { }, true);

  var ids = result.ids;
  if ((ids != null) && (ids.length > 0))
  {
    ids = ids.split(";");
  }
  
  var names = result.names;
  if ((names != null) && (names.length > 0))
  {
    names = names.split(";");
  }

  return { "ids" : ids, "names" : names };
}

/**
* Adds an elevation layer to the map
* @param {supported layer type} layer The layer to add
*/
Map.prototype.addElevationLayer = function(layer)
{
	layer.setMap(this, "addElevationLayer");
}

/**
 * Moves an elevation layer to the given index
 * @param {supported layer type} layer The layer to move
 * @param {integer} newIndex The (valid) new index of the layer
 */
Map.prototype.moveElevationLayer = function(layer, newIndex)
{
	this.sendCommand("moveElevationLayer", { id: layer._id, index: newIndex }, false);
}

/**
 * Removes an elevation layer from the map
 * @param {supported layer type} layer The layer to remove
 */
Map.prototype.removeElevationLayer = function(layer)
{
	this.sendCommand("removeElevationLayer", { id: layer._id }, false);
}

/**
 * Sets and loads the specified map (.earth) file
 * @param {string} mapFile The map file URL
 */
Map.prototype.setMapFile = function(mapFile)
{
    var args = $.toJSON( { "filename": mapFile } );
    this._plugin.sendCommand("setMap", args, false);
}

/**
 * Gets the intersection point with the scene based on mouse x/y coordinates
 * @param {float} x The mouse X coordinate
 * @param {float} y The mouse Y coordinate
 * @returns {Location} a Location object containing the intersect coordinates (lat/lon/alt) or null if none found
 */
Map.prototype.getIntersection = function(x,y)
{
    var result = this.sendCommand("getIntersection", { x: x, y: y }, true);

    if (result.valid == 1)
    {
        return new Location(result.latitude,result.longitude,result.height);
    }

    return null;                                                         
}

/**
 * Gets the names of objects in the scene at a given location based on mouse x/y coordinates
 * @param {float} x The mouse x coordinate
 * @param {float} y The mouse y coordinate
 * @returns {object} Array of strings containing the names
 */
Map.prototype.getNames = function(x,y)
{
    var result = this.sendCommand("getNames", { x: x, y: y }, true);

    var names = result.names;	
    if ((names != null) && (names.length > 0))
    {
      names = names.split( ";" );
    }

    return names;
}

/**
 * Gets the descriptions of objects in the scene at a given location based on mouse x/y coordinates
 * @param {float} x The mouse x coordinate
 * @param {float} y The mouse y coordinate
 * @returns {object} Array of strings containing the descriptions
 */
Map.prototype.getDescriptions = function(x,y)
{
    var result = this.sendCommand("getDescriptions", { x: x, y: y }, true);

    var descriptions = result.descriptions;	
    if ((descriptions != null) && (descriptions.length > 0))
    {
      descriptions = descriptions.split( ";" );
    }

    return descriptions;
}

/**
 * Gets the current viewpoint
 * @returns {object} Object containing viewpoint properties: latitude, longitude, altitude, range, heading, and pitch
 */
Map.prototype.getViewpoint = function()
{
    var args = $.toJSON( { "dummy": 0 } );
    var result = jQuery.parseJSON(this._plugin.sendCommand("getViewpoint", args, true));
    return result;
}

/**
 * Sets the viewpoint
 * @param {object} viewpoint Object containing viewpoint properties: latitude, longitude, altitude, range, heading, and pitch
 * @param {float} transitionTime Length of camera transition from current viewpoint to the new one
 */
Map.prototype.setViewpoint = function(viewpoint, transitionTime)
{
    var vp = jQuery.extend({}, viewpoint);
    vp.transitionTime = transitionTime; // just append a property

    if (!vp.pitch)
    {
      var current = this.getViewpoint();
      vp.pitch = current.pitch;
    }

    this.sendCommand( "setViewpoint", vp, false );
}

/**
 * Enables the sky display
 */
Map.prototype.showSky = function()
{
	this.sendCommand("showSky", { }, false);
}

/**
 * Sets the date and time to be respresented by the sky
 * @param {integer} year The year
 * @param {integer} month The month
 * @param {integer} day The day
 * @param {float} timeUTC The UTC time
 */
Map.prototype.setSkyDateTime = function (year, month, day, timeUTC)
{
	this.sendCommand("setSkyDateTime", { year: year, month: month, day: day, time:timeUTC }, false);
}

/**
 * Resets the view. Equivalent of pressing the spacebar.
 */
Map.prototype.home = function()
{
    var args = $.toJSON( { "dummy": 0 } );
    this._plugin.sendCommand("home", args, false);
}

/**
 * Sets the opacity of all image layers
 * @param {float} opacity The opacity (0.0 - 1.0)
 */
Map.prototype.setFade = function(opacity)
{
    this.sendCommand("fadeMap", { "opacity": opacity }, false);
}


//Map.prototype.toggleObjectSelected = function( tag, selected )
//{
//    this.sendCommand( "toggleObjectSelected", 
//        { "name": tag, "description": tag, "selected": selected },
//        false );
//}

//Map.prototype.clearSelection = function()
//{
//    this.sendCommand( "clearSelection", { }, false );
//}

//Map.prototype.highlightSelection = function( enabled )
//{
//    this.sendCommand( "highlightSelection", { "enabled": enabled }, false );
//}

//Map.prototype.toggleIsolateSelection = function( tagOfNodeToHide, isolate )
//{
//    this.sendCommand( "toggleIsolateSelection",
//        { "name": tagOfNodeToHide, "description": tagOfNodeToHide, "isolate":isolate },
//        false );
//}

Map.prototype.flashObject = function( tag, duration_s )
{
    this.sendCommand( "flashObject", { "tag": tag, "duration": duration_s }, false );
}
