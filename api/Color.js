function Color(r, g, b, a)
{
  this._r = r;
  this._g = g;
  this._b = b;
  this._a = a;
}

Color.prototype.getR = function()
{
  return this._r;
}

Color.prototype.setR = function(r)
{
  this._r = r;
}

Color.prototype.getG = function()
{
  return this._g;
}

Color.prototype.setG = function(g)
{
  this._g = g;
}

Color.prototype.getB = function()
{
  return this._b;
}

Color.prototype.setB = function(b)
{
  this._b = b;
}

Color.prototype.getA = function()
{
  return this._a;
}

Color.prototype.setA = function(a)
{
  this._a = a;
}








Color.prototype._r;
Color.prototype._g;
Color.prototype._b;
Color.prototype._a;