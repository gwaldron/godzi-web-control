/**
 * WMS layer object
 * @param {string} name The layer name
 * @param {string} url The layer url
 * @param {object} params Object containing optional parameters (layers, format, srs, and styles) as properties
 * @constructor
 */
function WMSLayer(name, url, params)
{
  this._id = -1; //guid();
	this._name = name;
  this._url = url;
	this._opacity = 1.0;
	this._visible = true;
	this._mapCommand = "updateImageLayer";

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
    if (this._mapCommand != undefined)
    {
      var rslt = this._map.sendCommand(this._mapCommand, {
        type: "wms",
        id: this._id,
        name: this._name,
        url: this._url,
        layers: this._layers,
        format: this._format,
        srs: this._srs,
        styles: this._styles,
        opacity: this._opacity,
        visible: this._visible }, true);
        
      this._id = rslt.id;
    }
  }
}

/**
 * Gets the layer's name
 * @returns {string}
 */
WMSLayer.prototype.getName = function()
{
	return this._name;
}

/**
 * Sets the layer's name
 * @param {string} name
 */
WMSLayer.prototype.setName = function(name)
{
	this._name = name;
	this.updateRender();
}

/**
 * Gets the list of displayed WMS layers from this source
 * @returns {string} Comma-separated list of layers
 */
WMSLayer.prototype.getLayers = function()
{
	return this._layers;
}

/**
 * Sets the list of displayed WMS layers from this source
 * @param {string} layers Comma-separated list of layers
 */
WMSLayer.prototype.setLayers = function(layers)
{
	this._layers = layers;
	this.updateRender();
}

/**
 * Gets the optional WMS format parameter
 * @returns {string}
 */
WMSLayer.prototype.getFormat = function()
{
	return this._format;
}

/**
 * Sets the optional WMS format paramter
 * @param {string} format
 */
WMSLayer.prototype.setFormat = function(format)
{
	this._format = format;
	this.updateRender();
}

/**
 * Gets the optional WMS SRS parameter
 * @returns {string}
 */
WMSLayer.prototype.getSRS = function()
{
	return this._srs;
}

/**
 * Sets the optional WMS SRS paramter
 * @param {string} srs
 */
WMSLayer.prototype.setSRS = function(srs)
{
	this._srs = srs;
	this.updateRender();
}

/**
 * Gets the optional WMS styles parameter
 * @returns {string}
 */
WMSLayer.prototype.getStyles = function()
{
	return this._styles;
}

/**
 * Sets the optional WMS styles paramter
 * @param {string} styles
 */
WMSLayer.prototype.setStyles = function(styles)
{
	this._styles = styles;
	this.updateRender();
}

/**
 * Gets the layer's opacity
 * @returns {float}
 */
WMSLayer.prototype.getOpacity = function()
{
	return this._opacity;
}

/**
 * Sets the layer's opacity
 * @param {float} opacity
 */
WMSLayer.prototype.setOpacity = function(opacity)
{
	this._opacity = opacity;
	this.updateRender();
}

/**
 * Gets the layer's visibility
 * @returns {bool}
 */
WMSLayer.prototype.getVisible = function()
{
	return this._visible;
}

/**
 * Sets the layer's visibility
 * @param {bool} visible
 */
WMSLayer.prototype.setVisible = function(visible)
{
	this._visible = visible;
	this.updateRender();
}

/**
 * Gets the layer's parent map
 * @returns {Map}
 */
WMSLayer.prototype.getMap = function()
{
  return this._map;
}

WMSLayer.prototype.setMap = function(map, command)
{
  this._map = map;

	if (command != undefined)
		this._mapCommand = command;

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
WMSLayer.prototype._mapCommand;
