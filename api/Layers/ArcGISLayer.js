/**
 * ArcGIS layer object
 * @param {string} name The layer name
 * @param {string} url The layer url
 * @param {string} token Optional ArcGIS authentication token
 * @constructor
 */
function ArcGISLayer(name, url, token)
{
  this._id = -1; //guid();
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
    {
      var rslt = this._map.sendCommand(this._mapCommand, {
        type: "arcgis",
        id: this._id,
        name: this._name,
        url: this._url,
        token: this._token,
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
ArcGISLayer.prototype.getName = function()
{
	return this._name;
}

/**
 * Sets the layer's name
 * @param {string} name
 */
ArcGISLayer.prototype.setName = function(name)
{
	this._name = name;
	this.updateRender();
}

/**
 * Gets the optional ArcGIS authentication token
 * @returns {string}
 */
ArcGISLayer.prototype.getToken = function()
{
	return this._token;
}

/**
 * Sets the ArcGIS authentication token
 * @param {string} token
 */
ArcGISLayer.prototype.setToken = function(token)
{
	this._token = token;
	this.updateRender();
}

/**
 * Gets the layer's opacity
 * @returns {float}
 */
ArcGISLayer.prototype.getOpacity = function()
{
	return this._opacity;
}

/**
 * Sets the layer's opacity
 * @param {float} opacity
 */
ArcGISLayer.prototype.setOpacity = function(opacity)
{
	this._opacity = opacity;
	this.updateRender();
}

/**
 * Gets the layer's visibility
 * @returns {bool}
 */
ArcGISLayer.prototype.getVisible = function()
{
	return this._visible;
}

/**
 * Sets the layer's visibility
 * @param {bool} visible
 */
ArcGISLayer.prototype.setVisible = function(visible)
{
	this._visible = visible;
	this.updateRender();
}

/**
 * Gets the layer's parent map
 * @returns {Map}
 */
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
