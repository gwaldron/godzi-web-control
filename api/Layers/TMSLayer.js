function TMSLayer(name, url)
{
  this._id = guid();
  this._name = name;
  this._url = url;
  this._opacity = 1.0;
  this._visible = true;
}

TMSLayer.prototype.updateRender = function()
{
  if (this._map != null)
  {
    this._map.sendCommand("updateImageLayer", {
      type: "tms",
      id: this._id,
      name: this._name,
      url: this._url,
      opacity: this._opacity,
      visible: this._visible }, false);
  }
}

TMSLayer.prototype.getName = function()
{
	return this._name;
}

TMSLayer.prototype.setName = function(name)
{
	this._name = name;
	this.updateRender();
}

TMSLayer.prototype.getOpacity = function()
{
	return this._opacity;
}

TMSLayer.prototype.setOpacity = function(opacity)
{
	this._opacity = opacity;
	this.updateRender();
}

TMSLayer.prototype.getVisible = function()
{
	return this._visible;
}

TMSLayer.prototype.setVisible = function(visible)
{
	this._visible = visible;
	this.updateRender();
}

TMSLayer.prototype.getMap = function()
{
  return this._map;
}

TMSLayer.prototype.setMap = function(map)
{
  this._map = map;
  this.updateRender();
}

TMSLayer.prototype._id;
TMSLayer.prototype._name;
TMSLayer.prototype._url;
TMSLayer.prototype._opacity;
TMSLayer.prototype._visible;
TMSLayer.prototype._map;
