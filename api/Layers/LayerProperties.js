/**
* Simple layer object
* @param {int} id The layer id
* @param {string} name The layer name
* @constructor
*/
function ImageLayerProperties(layerId) {
  this.id = layerId;
  this.name = null;
  this.opacity = null;
  this.visible = null;
}

/**
 * Gets the layer's id
 * @returns {int}
 */
ImageLayerProperties.prototype.getId = function() {
  return this.id;
}

/**
* Gets the layer's name
* @returns {string}
*/
ImageLayerProperties.prototype.getName = function() {
  return this.name;
}

/**
* Sets the layer's name
* @param {string} name
*/
ImageLayerProperties.prototype.setName = function(name) {
  this.name = name;
}

/**
* Gets the layer's opacity
* @returns {float}
*/
ImageLayerProperties.prototype.getOpacity = function() {
  return this.opacity;
}

/**
* Sets the layer's opacity
* @param {float} opacity
*/
ImageLayerProperties.prototype.setOpacity = function(opacity) {
  this.opacity = opacity;
}

/**
* Gets the layer's visibility
* @returns {bool}
*/
ImageLayerProperties.prototype.getVisible = function() {
  return this.visible;
}

/**
* Sets the layer's visibility
* @param {bool} visible
*/
ImageLayerProperties.prototype.setVisible = function(visible) {
  this.visible = visible;
}

ImageLayerProperties.prototype.id;
ImageLayerProperties.prototype.name;
ImageLayerProperties.prototype.opacity;
ImageLayerProperties.prototype.visible;
