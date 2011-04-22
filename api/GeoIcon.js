function GeoIcon(location, filename)
{
  this._id = guid();
  this._location = location;
  this._filename = filename;
  this._iconWidth = -1;
  this._iconHeight = -1;
  this._visible = true;
}

GeoIcon.prototype.getId = function()
{
  return this._id;
}

GeoIcon.prototype.clone = function()
{
  var clone = new GeoIcon(this.location, this._filename);
  clone._iconWidth = this._iconWidth;
  clone._iconHeight = this._iconHeight;
  return clone;
}

GeoIcon.prototype.getIconWidth = function()
{
  return this._iconWidth;
}

GeoIcon.prototype.getIconHeight = function()
{
  return this._iconHeight;
}

GeoIcon.prototype.setIconSize = function(w, h)
{
  this._iconWidth = w;
  this._iconHeight = h;
  this.updateRender();
}

GeoIcon.prototype.getLocation = function()
{
  return this._location;
}

GeoIcon.prototype.setLocation = function(location)
{
  this._location = location;
  this.updateRender();
}

GeoIcon.prototype.getMap = function()
{
  return this._map;
}

GeoIcon.prototype.setMap = function(map)
{
  this._map = map;
  this.updateRender();
}

GeoIcon.prototype.updateRender = function()
{
  if (this._map != null) {
	  this._map.sendCommand("updateIcon",
	                {
	                "id" : this._id,
					"latitude" : this._location.getLatitude(),
					"longitude" : this._location.getLongitude(),
					"height" : this._location.getAltitude(),
					"iconWidth" : this._iconWidth,
					"iconHeight" : this._iconHeight,
					"filename" : this._filename
					},
					false);					
  }
}

GeoIcon.prototype.getVisible = function()
{
  return this._visible;
}

GeoIcon.prototype.setVisible = function(visible)
{
  if (this._visible != visible)
  {
    this._visible = visible;
	
    if (this._map != null)
    {
      this._map.changeVisibility(this._id, this._visible);	  
    }	
  }
}

GeoIcon.prototype._map;
GeoIcon.prototype._id;
GeoIcon.prototype._location;
GeoIcon.prototype._filename;
GeoIcon.prototype._iconWidth;
GeoIcon.prototype._iconHeight;
