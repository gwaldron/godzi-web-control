/**
* Simple layer object
* @param {int} id The layer id
* @param {string} name The layer name
* @constructor
*/
function ImageLayerProperties(layerId) {
  this._id = layerId;
  this._name = null;
  this._opacity = null;
  this._visible = null;
}

/**
 * Gets the layer's id
 * @returns {int}
 */
ImageLayerProperties.prototype.getId = function() {
  return this._id;
}

/**
* Gets the layer's name
* @returns {string}
*/
ImageLayerProperties.prototype.getName = function() {
  return this._name;
}

/**
* Sets the layer's name
* @param {string} name
*/
ImageLayerProperties.prototype.setName = function(name) {
  this._name = name;
}

/**
* Gets the layer's opacity
* @returns {float}
*/
ImageLayerProperties.prototype.getOpacity = function() {
  return this._opacity;
}

/**
* Sets the layer's opacity
* @param {float} opacity
*/
ImageLayerProperties.prototype.setOpacity = function(opacity) {
  this._opacity = opacity;
}

/**
* Gets the layer's visibility
* @returns {bool}
*/
ImageLayerProperties.prototype.getVisible = function() {
  return this._visible;
}

/**
* Sets the layer's visibility
* @param {bool} visible
*/
ImageLayerProperties.prototype.setVisible = function(visible) {
  this._visible = visible;
}

ImageLayerProperties.prototype._id;
ImageLayerProperties.prototype._name;
ImageLayerProperties.prototype._opacity;
ImageLayerProperties.prototype._visible;
