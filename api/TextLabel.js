var TextLabelAlignment = { LEFT_TOP : 0,
                           LEFT_CENTER : 1,
                           LEFT_BOTTOM : 2,
						   CENTER_TOP  : 3,
						   CENTER_CENTER : 4,
						   CENTER_BOTTOM : 5,
						   RIGHT_TOP : 6,
						   RIGHT_CENTER : 7,
						   RIGHT_BOTTOM : 8,
						   LEFT_BASE_LINE : 9,
						   CENTER_BASE_LINE : 10,
						   RIGHT_BASE_LINE : 11,
						   LEFT_BOTTOM_BASE_LINE : 12,
						   CENTER_BOTTOM_BASE_LINE : 13,
						   RIGHT_BOTTOM_BASE_LINE : 14}

function TextLabel(text, location)
{
  this._id = guid();
  this._map = null;
  this._text = text;
  this._location = location;
  this._color = new Color(1,1,1,1);
  this._outlineColor = new Color(0,0,0,1);
  this._size = 24;
  this._alignment = TextLabelAlignment.LEFT_BASE_LINE;
  this._visible = true;
  this._outlineVisible = true;
}


TextLabel.prototype.getLocation = function()
{
  return this._location;
}

TextLabel.prototype.getText = function()
{
  return this._text;
}

TextLabel.prototype.setText = function(text)
{
  this._text = text;
  this.updateRender();
}

TextLabel.prototype.getSize = function()
{
  return this._size;
}

TextLabel.prototype.setSize = function(size)
{
  this._size = size;
  this.updateRender();
}

TextLabel.prototype.getColor = function()
{
  return this._color;
}

TextLabel.prototype.setColor = function(color)
{
  this._color = color;
  this.updateRender();
}

TextLabel.prototype.setOutlineColor = function(outlineColor)
{
  this._outlineColor = outlineColor;
  this.updateRender();
}

TextLabel.prototype.getOutlineVisible = function()
{
  return this._outlineVisible;
}

TextLabel.prototype.setOutlineVisible = function(outlineVisible)
{
  if (this._outlineVisible != outlineVisible)
  {
    this._outlineVisible = outlineVisible;
	this.updateRender();
  }
}

TextLabel.prototype.setLocation = function(location)
{
  this._location = location;
  this.updateRender();
}

TextLabel.prototype.setAlignment = function(alignment)
{
  this._alignment = alignment;
  this.updateRender();
}

TextLabel.prototype.getVisible = function()
{
  return this._visible;
}

TextLabel.prototype.setVisible = function(visible)
{
  if (this._visible != visible)
  {
    this._visible = visible;
	
	if (this._map != null)
    {
      var o = this._map.getObject();
	  if (o != null)
	  {
        o.setVisibility(this._id, this._visible);	  
	  }
    }	
  }
}


TextLabel.prototype.getMap = function()
{
  return this._map;
}

TextLabel.prototype.setMap = function(map)
{
  this._map = map;
  this.updateRender();
}

TextLabel.prototype.updateRender = function()
{
  if (this._map != null)
  {
    
	  this._map.sendCommand("updateTextLabel",
	                {
	                "id" :this._id, 
					"latitude" : this._location.getLatitude(),
					"longitude" : this._location.getLongitude(),
					"height" : this._location.getAltitude(),
					"colorR" : this._color.getR(),
					"colorG" : this._color.getG(),
					"colorB" : this._color.getB(),
					"colorA" : this._color.getA(),
					"outlineColorR" : this._outlineColor.getR(),
					"outlineColorG" : this._outlineColor.getG(),
					"outlineColorB" : this._outlineColor.getB(),
					"outlineColorA" : this._outlineColor.getA(),
					"outlineVisible" : this._outlineVisible,
					"size" : this._size,
					"alignment" : this._alignment,
					"text" : this._text
					},
					false);					      
  }
}