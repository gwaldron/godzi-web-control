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

Model.prototype.getMap = function()
{
  return this._map;
}

Model.prototype.setMap = function(map)
{
  this._map = map;
  this.updateRender();
}

Model.prototype.getLocation = function()
{
  return this._location;
}

Model.prototype.setLocation = function(location)
{
  this._location = location;
  this.updateRender();
}

Model.prototype.getURL = function()
{
  return this._url;
}

Model.prototype.setURL = function(url)
{
  if (this._url == url) return;
  
  this._url = url;
}

Model.prototype.getScale = function()
{
  return this._scale;
}

Model.prototype.setScale = function(scale)
{
  this._scale = scale;
  this.updateRender();
}

Model.prototype.getOrientation = function()
{
  return this._orientation;
}

Model.prototype.setOrientation = function(orientation)
{
  this._orientation = orientation;
  this.updateRender();
}

Model.prototype.getIsAbsolutelyPositioned = function()
{
  return this._isAbsolutelyPositioned;
}

Model.prototype.setIsAbsolutelyPositioned = function(absolutelyPositioned)
{
  if (this._isAbsolutelyPositioned != absolutelyPositioned)
  {
    this._isAbsolutelyPositioned =absolutelyPositioned;
    this.updateRender();
  }
}

Model.prototype.getVisible = function()
{
  return this._visible;
}

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
