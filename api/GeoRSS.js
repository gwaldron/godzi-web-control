//Based on  http://brainoff.com/gmaps/mgeorss.html

function GeoRSSFeed(map)
{
  this.map = map;
  this.rssurl = false;
  this.icons = new Array();
}

GeoRSSFeed.prototype.setImage = function(image)
{
  this.image = image;
}

GeoRSSFeed.prototype.load=function(url, proxy)
{
 	this.rssurl = url;
	
	var actualURL = proxy + this.rssurl;
	
	var feed = this;

    new Ajax.Request(actualURL,
    {
	  method:'get',
	  
	  onSuccess: function(transport){
	    var response = transport.responseText;		

		var xmlDoc = loadXMLText(response);
		var items = xmlDoc.documentElement.getElementsByTagName("item");
		if (items.length == 0)
		{
		  items = xmlDoc.documentElement.getElementsByTagName("entry");
		}
		
		for (var i = 0; i < items.length; i++) {
			try {				    
				feed.icons[i] = feed.createIcon(items[i]);
				feed.map.addIcon(feed.icons[i]);
			} catch (e) {alert("Failed" + e)
			}
		}
	  },
	  
	  onFailure: function(){ alert("Error loading feed")},
	  onException: function(a,b) { alert("Error loading feed " + b)}
    });
}

GeoRSSFeed.prototype.createIcon = function(item) {
	var title = item.getElementsByTagName("title")[0].childNodes[0].data;
	var description;
	try {
	 description = item.getElementsByTagName("description")[0].childNodes[0].nodeValue;
	} catch (e) {}
	var link;
	try {
	 link = item.getElementsByTagName("link")[0].childNodes[0].nodeValue;
	} catch (e) {}

	/* namespaces are handled by spec in moz, not in ie */
	var latlng = ""; var lat; var lng; var box=""; var img="";
	if (navigator.userAgent.toLowerCase().indexOf("msie") < 0) {
		try {
			latlng = item.getElementsByTagNameNS("http://www.georss.org/georss","point")[0].childNodes[0].nodeValue;
		} catch (e) { }
		try {
			lat = item.getElementsByTagNameNS("http://www.w3.org/2003/01/geo/wgs84_pos#","lat")[0].childNodes[0].nodeValue;
			lng = item.getElementsByTagNameNS("http://www.w3.org/2003/01/geo/wgs84_pos#","long")[0].childNodes[0].nodeValue;
		} catch (e) {}
		try {
			var rel = item.getElementsByTagNameNS("http://www.georss.org/georss","relationshiptag")[0].childNodes[0].nodeValue;
			if (rel == "image-extent") {
				box = item.getElementsByTagNameNS("http://www.georss.org/georss","box")[0].childNodes[0].nodeValue;
				img = item.getElementsByTagNameNS("http://search.yahoo.com/mrss","content")[0].getAttribute("url");
			}
		} catch (e) { }
	} else {
		try {
			latlng = item.getElementsByTagName("georss:point")[0].childNodes[0].nodeValue;
		} catch (e) {}
		try {
			lat = item.getElementsByTagName("geo:lat")[0].childNodes[0].nodeValue;
			lng = item.getElementsByTagName("geo:long")[0].childNodes[0].nodeValue;
		} catch(e) {}
		try {
			box = item.getElementsByTagName("georss:box")[0].childNodes[0].nodeValue;
		} catch (e) {}
		try {
			var rel = item.getElementsByTagName("georss:box")[0].getAttribute("relationshiptag");
			if (rel == "image-extent") {
				box = item.getElementsByTagName("georss:box")[0].childNodes[0].nodeValue;
				img = item.getElementsByTagName("media:content")[0].getAttribute("url");
			}
		} catch (e) {}
	}
	if (latlng.length > 0) {
		lat = latlng.split(" ")[0];
		lng = latlng.split(" ")[1];
	}
	
	if (lat.length > 0 && lng.length > 0) {
		var icon = new GeoIcon(new Location(parseFloat(lat), parseFloat(lng), 0), this.image);
		//icon.setIconSize(16,16);
		return icon;
	}
}

GeoRSSFeed.prototype.image;
GeoRSSFeed.prototype.map;
GeoRSSFeed.prototype.rssurl;
GeoRSSFeed.prototype.icons;
