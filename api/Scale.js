function Scale(x, y, z)
{
  this._x = x;
  this._y = y;
  this._z = z;
}

Scale.prototype.clone = function()
{
  return new Scale(this._x, this._y, this._z);
}

Scale.prototype.getX = function()
{
  return this._x;
}

Scale.prototype.setX = function(x)
{
  this._x = x;
}

Scale.prototype.getY = function()
{
  return this._y;
}

Scale.prototype.setY = function(y)
{
  this._y = y;
}

Scale.prototype.getZ = function()
{
  return this._z;
}

Scale.prototype.setZ = function(z)
{
  this._z = z;
}

Scale.prototype.set = function(x,y,z)
{
  this._x = x;
  this._y = y;
  this._z = z;
}


Scale.prototype._x;
Scale.prototype._y;
Scale.prototype._z;