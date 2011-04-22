function GeoRSSLayer(map, feedItems)
{
  this._map = map;
  this._feedItems = feedItems;
}

GeoRSSLayer.prototype.setRenderCallback = function(renderCallback)
{
  if (this._renderCallback != renderCallback)
  {
    this._renderCallback = renderCallback;
	this.render();
  }
}

GeoRSSLayer.prototype.render = function()
{
  //Remove any existing icons from the map
  if (this._icons != null)
  {
    for (var i = 0; i < this._icons.length; i++)
	{
	  if (this._icons[i])
	  {
	    this._map.removeIcon(this._icons[i]);
	  }
	}
  }
  
  //Rerender the feed items
  this._icons = new Array();
  for (var i = 0; i < this._feedItems.length; ++i)
  {
     if (this._renderCallback)
     {	   
	   this._icons[i] = this._renderCallback(this._feedItems[i]);
     }	 
  }
  
  //Add all of the icons to the map
  for (var i = 0; i < this._feedItems.length; ++i)
  {
    if (this._icons[i])
	{
	  this._icons[i].setMap(this._map);
	}
  }
}



GeoRSSLayer.prototype._map;
GeoRSSLayer.prototype._feedItems;
GeoRSSLayer.prototype._icons;
GeoRSSLayer.prototype._renderCallback;