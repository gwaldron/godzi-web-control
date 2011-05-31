/**
 * Model object
 * @constructor
 */
function Model()
{
  this._id = guid();
  this._scale = new Scale(1,1,1);
  this._location = new Location(0,0,0);
  this._orientation = new Orientation(0,0,0);
  this._url = "";
  this._isAbsolutelyPositioned = false;
  this._visible = true;
}

Model.prototype.updateRender = function()
{
    if (this._map != null)
    {
        this._map.sendCommand( "updateModel", {
            id:         this._id,
            latitude:   this._location.getLatitude(),
            longitude:  this._location.getLongitude(),
            height:     this._location.getAltitude(),
            scaleX:     this._scale.getX(),
            scaleY:     this._scale.getY(),
            scaleZ:     this._scale.getZ(),
            heading:    this._orientation.getHeading(),
            pitch:      this._orientation.getPitch(),
            roll:       this._orientation.getRoll(),
            filename:   this._url,
            absolutelyPositioned: this._isAbsolutelyPositioned }, false );
  }
}

/**
 * Gets the model's parent map
 * @returns {Map}
 */
Model.prototype.getMap = function()
{
  return this._map;
}

/**
 * Sets the model's parent map and adds it to the scene
 * @param {Map} map
 */
Model.prototype.setMap = function(map)
{
  this._map = map;
  this.updateRender();
}

/**
 * Gets the model's location
 * @returns {Location}
 */
Model.prototype.getLocation = function()
{
  return this._location;
}

/**
 * Sets the model's location
 * @param {Location} location
 */
Model.prototype.setLocation = function(location)
{
  this._location = location;
  this.updateRender();
}

/**
 * Gets the model file url
 * @returns {string}
 */
Model.prototype.getURL = function()
{
  return this._url;
}

/**
 * Sets the model file url
 * @param {string} url
 */
Model.prototype.setURL = function(url)
{
  if (this._url == url) return;
  
  this._url = url;
}

/**
 * Gets the model's scale property
 * @returns {Scale}
 */
Model.prototype.getScale = function()
{
  return this._scale;
}

/**
 * Sets the model's scale property
 * @param {Scale}
 */
Model.prototype.setScale = function(scale)
{
  this._scale = scale;
  this.updateRender();
}

/**
 * Gets the model's orientation property
 * @returns {Orientation}
 */
Model.prototype.getOrientation = function()
{
  return this._orientation;
}

/**
 * Sets the model's orientation property
 * @param {Orientation}
 */
Model.prototype.setOrientation = function(orientation)
{
  this._orientation = orientation;
  this.updateRender();
}

/**
 * Gets if the model is absolutely positioned in the scene
 * @returns {bool}
 */
Model.prototype.getIsAbsolutelyPositioned = function()
{
  return this._isAbsolutelyPositioned;
}

/**
 * Sets if the model is absolutely positioned in the scene
 * @param {bool} absolutelyPositioned
 */
Model.prototype.setIsAbsolutelyPositioned = function(absolutelyPositioned)
{
  if (this._isAbsolutelyPositioned != absolutelyPositioned)
  {
    this._isAbsolutelyPositioned =absolutelyPositioned;
    this.updateRender();
  }
}

/**
 * Gets the model's visibility
 * @returns {bool}
 */
Model.prototype.getVisible = function()
{
  return this._visible;
}

/**
 * Sets the model's visibility
 * @param {bool} visible
 */
Model.prototype.setVisible = function(visible)
{
  if (this._visible != visible)
  {
    this._visible = visible;
    this._map.sendCommand( "changeVisibility", 
    {
      id:this._id,
      visible: this._visible
    },
    false);
  }
}
