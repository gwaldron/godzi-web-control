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

Map.embed = function(div_id, map_id, type, classid ) {

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
Map.prototype.sendCommand = function( command, args, hasResult )
{
    var json_args = $.toJSON( args );
    if ( hasResult )
    {
        var result = this._plugin.sendCommand( command, json_args, hasResult );
        return jQuery.parseJSON( result );
    }
    else
    {
        this._plugin.sendCommand( command, json_args, hasResult );
    }
}

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

Map.prototype.getBackColor = function()
{
    var result = this.sendCommand( "getBackColor", {}, true );
    return new Color( result.r, result.g, result.b, result.a );
}

Map.prototype.setBackColor = function(color)
{
    this.sendCommand( "setBackColor", { "r": color.getR(), "g": color.getG(), "b": color.getB(), "a": color.getA() }, false );
}

Map.prototype.toggleStats = function()
{
    this.sendCommand( "toggleStats", {}, false );
}

Map.prototype.cyclePolygonMode = function()
{
    this.sendCommand( "cyclePolygonMode", {}, false );
}

Map.prototype.addTextLabel = function(label)
{
  label.setMap(this);
}

Map.prototype.removeTextLabel = function(label)
{
  this.sendCommand("removeTextLabel", { id: label._id }, false);
}

Map.prototype.addIcon = function(icon)
{
  icon.setMap(this);
}

Map.prototype.removeIcon = function(icon)
{
  this.sendCommand("removeIcon", { id: icon._id }, false);
}

Map.prototype.addModel = function(model)
{
    model.setMap(this);
}

Map.prototype.removeModel = function(model)
{
  this.sendCommand("removeModel", { id: model._id }, false);
}

Map.prototype.addImageLayer = function(layer)
{
	layer.setMap(this, "updateImageLayer");
}

Map.prototype.moveImageLayer = function(layer, newIndex)
{
	this.sendCommand("moveImageLayer", { id: layer._id, index: newIndex }, false);
}

Map.prototype.removeImageLayer = function(layer)
{
	this.sendCommand("removeImageLayer", { id: layer._id }, false);
}

Map.prototype.addElevationLayer = function(layer)
{
	layer.setMap(this, "addElevationLayer");
}

Map.prototype.moveElevationLayer = function(layer, newIndex)
{
	this.sendCommand("moveElevationLayer", { id: layer._id, index: newIndex }, false);
}

Map.prototype.removeElevationLayer = function(layer)
{
	this.sendCommand("removeElevationLayer", { id: layer._id }, false);
}

Map.prototype.setMapFile = function(mapFile)
{
    var args = $.toJSON( { "filename": mapFile } );
    this._plugin.sendCommand("setMap", args, false);
}

Map.prototype.getIntersection = function(x,y)
{
    var result = this.sendCommand("getIntersection", { x: x, y: y }, true);

    if (result.valid == 1)
    {
        return new Location(result.latitude,result.longitude,result.height);
    }

    return null;                                                         
}

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

Map.prototype.getViewpoint = function()
{
    var args = $.toJSON( { "dummy": 0 } );
    var result = jQuery.parseJSON(this._plugin.sendCommand("getViewpoint", args, true));
    return result;
}

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

Map.prototype.home = function()
{
    var args = $.toJSON( { "dummy": 0 } );
    this._plugin.sendCommand("home", args, false);
}

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
