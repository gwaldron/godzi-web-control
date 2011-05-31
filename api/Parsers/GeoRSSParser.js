/**
 * GeoRSS feed item container object. Properties based on RSS properties defined in http://www.w3schools.com/rss/rss_item.asp
 * @constructor
 */
function FeedItem()
{
}

FeedItem.prototype.guid;
FeedItem.prototype.author;
FeedItem.prototype.category;
FeedItem.prototype.comments;
FeedItem.prototype.title;
FeedItem.prototype.link;
FeedItem.prototype.pubDate;
FeedItem.prototype.description;
FeedItem.prototype.location;
FeedItem.prototype.description;
FeedItem.prototype.location;
FeedItem.prototype.source;

/**
 * GeoRSS parser
 * @constructor
 */
function GeoRSSParser()
{
}

/**
 * Parses the data from a GeoRSS feed and returns an array of feed items
 * @param {string} doc XML content of the GeoRSS feed
 * @returns {array} An array of FeedItem objects
 */
GeoRSSParser.prototype.getItems = function(doc)
{
  //Get the "item" elements
  var items = doc.documentElement.getElementsByTagName("item");
  //If we couldn't get any "items", try using "entry"
  if (items.length == 0)
  { 
    items = doc.documentElement.getElementsByTagName("entry");
  }  

  //Convert the Xml elements to FeedItems  
  var feedItems = new Array();
  for (var i = 0; i < items.length; i++) {
  try {				    
    feedItems[i] = this.createFeedItem(items[i]);
	} catch (e) {
	  alert(e);
    }
  } 
  return feedItems;
}

function getNodeValue(item, name)
{
  try {
    return item.getElementsByTagName(name)[0].childNodes[0].nodeValue;
  }
  catch (e) {}
  return null;
}

GeoRSSParser.prototype.createFeedItem = function(item)
{
    
    var fi = new FeedItem();
	
    //Read the title
    fi.title = item.getElementsByTagName("title")[0].childNodes[0].data;
	

	fi.guid = getNodeValue(item, "guid");
	fi.author = getNodeValue(item, "author");
	fi.category = getNodeValue(item,"category");
	fi.comments = getNodeValue(item, "comments");
	fi.description = getNodeValue(item, "description");
	fi.link = getNodeValue(item,"link");
	fi.pubDate = getNodeValue(item,"pubDate");
	fi.source = getNodeValue(item,"source");

	
	/* namespaces are handled by spec in moz, not in ie */
	var latlng = ""; var lat; var lng;
	if (navigator.userAgent.toLowerCase().indexOf("msie") < 0) {
		try {
			latlng = item.getElementsByTagNameNS("http://www.georss.org/georss","point")[0].childNodes[0].nodeValue;
		} catch (e) { }
		try {
			lat = item.getElementsByTagNameNS("http://www.w3.org/2003/01/geo/wgs84_pos#","lat")[0].childNodes[0].nodeValue;
			lng = item.getElementsByTagNameNS("http://www.w3.org/2003/01/geo/wgs84_pos#","long")[0].childNodes[0].nodeValue;
		} catch (e) {}
	} else {
		try {
			latlng = item.getElementsByTagName("georss:point")[0].childNodes[0].nodeValue;
		} catch (e) {}
		try {
			lat = item.getElementsByTagName("geo:lat")[0].childNodes[0].nodeValue;
			lng = item.getElementsByTagName("geo:long")[0].childNodes[0].nodeValue;
		} catch(e) {}
	}
	if (latlng.length > 0) {
		lat = latlng.split(" ")[0];
		lng = latlng.split(" ")[1];
	}

	if (lat.length > 0 && lng.length > 0) {
	    fi.location = new Location(parseFloat(lat), parseFloat(lng), 0);
		return fi;
	}
	
	return null;
}







