#include <GodziWebControl/TileSourceUtil>

#include <osgEarthDrivers/arcgis/ArcGISOptions>
#include <osgEarthDrivers/tms/TMSOptions>
#include <osgEarthDrivers/wms/WMSOptions>

#include <osgDB/FileNameUtils>

using namespace GodziWebControl;


namespace
{
	std::string extractBetween(const std::string& str, const std::string &lhs, const std::string &rhs)
	{
			std::string result;
			std::string::size_type start = str.find(lhs);
			if (start != std::string::npos)
			{
					start += lhs.length();
					std::string::size_type count = str.size() - start;
					std::string::size_type end = str.find(rhs, start); 
					if (end != std::string::npos) count = end-start;
					result = str.substr(start, count);
			}
			return result;
	}
}

/***************************************************************************************/


bool TileSourceUtil::createTileSourceOptions(const CommandArguments& args, osgEarth::TileSourceOptions& out_opt)
{
  std::string type = args["type"];
  std::string url = args["url"];
  //std::string name = args["name"];

  if (url.empty())
    return false;

  if (type == "tms")
  {
    out_opt = osgEarth::Drivers::TMSOptions(url);
    return true;
  }
  else if (type == "wms")
  {
    std::string layers = args["layers"];
    std::string format = args["format"];
    std::string srs = args["srs"];
    std::string styles = args["styles"];

    osgEarth::Drivers::WMSOptions wmsOpt;
    wmsOpt.url() = url.substr(0, url.find("?"));

    std::string lower = osgDB::convertToLowerCase( url );

    if (!layers.empty())
      wmsOpt.layers() = layers;
    else if (lower.find("layers=", 0) != std::string::npos)
		  wmsOpt.layers() = extractBetween(lower, "layers=", "&");

    if (!format.empty())
      wmsOpt.format() = format;
    else if (lower.find("srs=", 0) != std::string::npos)
      wmsOpt.format() = extractBetween(lower, "format=image/", "&");

    if (!srs.empty())
      wmsOpt.srs() = srs;
    else if (lower.find("format=image/", 0) != std::string::npos)
      wmsOpt.srs() = extractBetween(lower, "srs=", "&");

    if (!styles.empty())
      wmsOpt.style() = styles;
    else if (lower.find("styles=", 0) != std::string::npos)
      wmsOpt.style() = extractBetween(lower, "styles=", "&");

    out_opt = wmsOpt;
    return true;
  }
  else if (type == "arcgis")
  {
    std::string token = args["token"];

    osgEarth::Drivers::ArcGISOptions arcOpt;
    arcOpt.url() = url;

    if (!token.empty())
      arcOpt.token() = token;

    out_opt = arcOpt;
    return true;
  }

  return false;
}