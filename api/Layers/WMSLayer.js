function WMSLayer(name, url, params)
{
	this._id = guid();
	this._name = name;
  this._url = url;
	this._opacity = 1.0;
	this._visible = true;

	if (params != undefined)
	{
    this._layers = params.layers;
		this._format = params.format;
		this._srs = params.srs;
		this._styles = params.styles;
	}
}

WMSLayer.prototype.updateRender = function()
{
  if (this._map != null)
  {
    this._map.sendCommand("updateImageLayer", {
      type: "wms",
      id: this._id,
      name: this._name,
      url: this._url,
			layers: this._layers,
			format: this._format,
			srs: this._srs,
			styles: this._styles,
      opacity: this._opacity,
      visible: this._visible }, false);
  }
}

WMSLayer.prototype.getName = function()
{
	return this._name;
}

WMSLayer.prototype.setName = function(name)
{
	this._name = name;
	this.updateRender();
}

WMSLayer.prototype.getLayers = function()
{
	return this._layers;
}

WMSLayer.prototype.setLayers = function(layers)
{
	this._layers = layers;
	this.updateRender();
}

WMSLayer.prototype.getFormat = function()
{
	return this._format;
}

WMSLayer.prototype.setFormat = function(format)
{
	this._format = format;
	this.updateRender();
}

WMSLayer.prototype.getSRS = function()
{
	return this._srs;
}

WMSLayer.prototype.setSRS = function(srs)
{
	this._srs = srs;
	this.updateRender();
}

WMSLayer.prototype.getStyles = function()
{
	return this._styles;
}

WMSLayer.prototype.setStyles = function(styles)
{
	this._styles = styles;
	this.updateRender();
}

WMSLayer.prototype.getOpacity = function()
{
	return this._opacity;
}

WMSLayer.prototype.setOpacity = function(opacity)
{
	this._opacity = opacity;
	this.updateRender();
}

WMSLayer.prototype.getVisible = function()
{
	return this._visible;
}

WMSLayer.prototype.setVisible = function(visible)
{
	this._visible = visible;
	this.updateRender();
}

WMSLayer.prototype.getMap = function()
{
  return this._map;
}

WMSLayer.prototype.setMap = function(map)
{
  this._map = map;
  this.updateRender();
}

WMSLayer.prototype._id;
WMSLayer.prototype._name;
WMSLayer.prototype._url;
WMSLayer.prototype._layers;
WMSLayer.prototype._format;
WMSLayer.prototype._srs;
WMSLayer.prototype._styles;
WMSLayer.prototype._opacity;
WMSLayer.prototype._visible;
WMSLayer.prototype._map;
