/**
 * GeoRSS parser
 * @constructor
 */
function GPXParser()
{
}

/**
 * Parses the GPX XML data and returns an array of items
 * @param {string} doc GPX XML data
 * @returns {array} An array of items with properties {string} name and {Location} location
 */
GPXParser.prototype.getItems = function(doc)
{
  //Just get points for right now
  var points = doc.documentElement.getElementsByTagName("wpt");  

  var items = new Array;
  for (var i = 0; i < points.length; i++)
  { 
    var point = points[i];
    var name = point.getElementsByTagName("name")[0].childNodes[0].data;
	var lat = point.getAttribute("lat");
	var lon = point.getAttribute("lon");
	
	items[i] = {"name" : name, "location" : new Location(parseFloat(lat), parseFloat(lon), 0)};
  }  
  return items;
}