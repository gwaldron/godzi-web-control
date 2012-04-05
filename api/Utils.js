function Utils() { }


InheritsFromClass = function(base, extras) {
    function F() { }
    F.prototype = base;
    var obj = new F();
    if (extras) { MixInClass(obj, extras, false); }
    return obj;
};
MixInClass = function(obj, properties, test) {
    for (var key in properties) {
        if (!(test && obj[key])) { obj[key] = properties[key]; }
    }
    return obj;
};


String.prototype.startsWith = function(str) {
    //return (this.match("^"+str)==str)	
	if (this.length >= str.length)
	{
      	var start = this.substring(0, str.length);
		return (start == str);
	}
	return false;
}

// Current Page Reference
// copyright Stephen Chapman, 1st Jan 2005
// you may copy this function but please keep the copyright notice with it
Utils.getPageURI = function()
{
    var uri = new Object();
    uri.dir = location.href.substring(0, location.href.lastIndexOf('\/'));
    uri.prefix = uri.dir + '/';
    uri.dom = uri.dir; if (uri.dom.substr(0,7) == 'http:\/\/') uri.dom = uri.dom.substr(7);
    uri.path = ''; var pos = uri.dom.indexOf('\/'); if (pos > -1) {uri.path = uri.dom.substr(pos+1); uri.dom = uri.dom.substr(0,pos);}
    uri.page = location.href.substring(uri.dir.length+1, location.href.length+1);
    pos = uri.page.indexOf('?');if (pos > -1) {uri.page = uri.page.substring(0, pos);}
    pos = uri.page.indexOf('#');if (pos > -1) {uri.page = uri.page.substring(0, pos);}
    uri.ext = ''; pos = uri.page.indexOf('.');if (pos > -1) {uri.ext =uri.page.substring(pos+1); uri.page = uri.page.substr(0,pos);}
    uri.file = uri.page;
    if (uri.ext != '') uri.file += '.' + uri.ext;
    if (uri.file == '') uri.page = 'index';
    //uri.args = location.search.substr(1).split("?");
    
    // by "arc2000"
    uri.args = {};    
    for(var m, v=location.href.split(/[?&]/), k=v.length-1; k>0; k--) 
        uri.args[(m=v[k].split(/[=#]/))[0].toLowerCase()] = m.length>1?decodeURI(m[1]):"";

    return uri;
}
 
Utils.makeAbsURI = function(reluri)
{
    if( reluri.startsWith("http://") )
        return reluri;
    if( reluri != null && reluri != '' )
        return Utils.getPageURI().prefix + reluri;
    return null;
}
                 