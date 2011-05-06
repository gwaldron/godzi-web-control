function ArcGISLayer(name, url, token)
{
	this._id = guid();
	this._name = name;
  this._url = url;
	this._opacity = 1.0;
	this._visible = true;
  this._token = token;
	this._mapCommand = "updateImageLayer";
}

ArcGISLayer.prototype.updateRender = function()
{
  if (this._map != null)
  {
    if (this._mapCommand != undefined)
      this._map.sendCommand(this._mapCommand, {
        type: "arcgis",
        id: this._id,
        name: this._name,
        url: this._url,
        token: this._token,
        opacity: this._opacity,
        visible: this._visible }, false);
  }
}

ArcGISLayer.prototype.getName = function()
{
	return this._name;
}

ArcGISLayer.prototype.setName = function(name)
{
	this._name = name;
	this.updateRender();
}

ArcGISLayer.prototype.getToken = function()
{
	return this._token;
}

ArcGISLayer.prototype.setToken = function(token)
{
	this._token = token;
	this.updateRender();
}

ArcGISLayer.prototype.getOpacity = function()
{
	return this._opacity;
}

ArcGISLayer.prototype.setOpacity = function(opacity)
{
	this._opacity = opacity;
	this.updateRender();
}

ArcGISLayer.prototype.getVisible = function()
{
	return this._visible;
}

ArcGISLayer.prototype.setVisible = function(visible)
{
	this._visible = visible;
	this.updateRender();
}

ArcGISLayer.prototype.getMap = function()
{
  return this._map;
}

ArcGISLayer.prototype.setMap = function(map, command)
{
  this._map = map;

	if (command != undefined)
		this._mapCommand = command;

  this.updateRender();
}

ArcGISLayer.prototype._id;
ArcGISLayer.prototype._name;
ArcGISLayer.prototype._url;
ArcGISLayer.prototype._token;
ArcGISLayer.prototype._opacity;
ArcGISLayer.prototype._visible;
ArcGISLayer.prototype._map;
ArcGISLayer.prototype._mapCommand;
