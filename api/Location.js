Location = function(lat, lon, alt) {
    this._latitude = lat !== undefined ? lat : 0.0;
    this._longitude = lon !== undefined ? lon : 0.0;
    this._altitude = alt != undefined ? alt : 0.0;
}

Location.prototype.clone = function()
{
  return new Location(this._latitude, this._longitude, this._altitude);
}

Location.prototype.getLatitude = function()
{
  return this._latitude;
}

Location.prototype.getLongitude = function()
{
  return this._longitude;
}

Location.prototype.getAltitude = function()
{
  return this._altitude;
}

Location.prototype.setLatitude = function(latitude)
{
  this._latitude = latitude;
}

Location.prototype.setLongitude = function(longitude)
{
  this._longitude = longitude;
}

Location.prototype.setAltitude = function(altitude)
{
  this._altitude = altitude;
}

Location.prototype.setLatLngAlt = function(lat,lng,alt)
{
  this._longitude = lng;
  this._latitude = lat;
  this._altitude = alt;
}

Location.prototype.toString = function()
{
  return this._latitude + ", " + this._longitude + ", " + this._altitude;
}

Location.prototype._latitude;
Location.prototype._longitude;
Location.prototype._altitude;