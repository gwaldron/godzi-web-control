#include <ReadyMapWebPlugin/ImageLayer>
#include <ReadyMapWebPlugin/MapControl>
#include <ReadyMapWebPlugin/NodeMasks>
#include <ReadyMapWebPlugin/Utils>

#include <osgEarth/Common>
#include <osgEarthDrivers/tms/TMSOptions>
#include <osgEarthDrivers/wms/WMSOptions>
#include <osgEarthDrivers/arcgis/ArcGISOptions>

#include <osg/io_utils>
#include <osg/ClusterCullingCallback>
#include <osgDB/FileNameUtils>
#include <string>
#include <sstream>


#include <osgDB/ReadFile>

using namespace osg;
using namespace osgDB;
using namespace ReadyMapWebPlugin;
using namespace osgEarth;

/***************************************************************************************/

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

Command* RemoveImageLayerCommand::Factory::create(const std::string& command, const CommandArguments& args)
{
    if ("removeImageLayer" == command)
    {
        std::string id = args["id"];
        return new RemoveImageLayerCommand(id);
    }
    return NULL;
}

bool RemoveImageLayerCommand::operator ()(ReadyMapWebPlugin::MapControl *map)
{
  osgEarth::ImageLayer* layer = map->getMap()->getImageLayerByName(_id);
  if (layer)
  {
    map->getMap()->removeImageLayer(layer);
  }

  return true;
}

/***************************************************************************************/

Command* UpdateImageLayerCommand::Factory::create(const std::string& command, const CommandArguments& args)
{
    if ("updateImageLayer" == command)
    {
        std::string id = args["id"];
        double opacity = as<double>(args["opacity"], 1.0);
        bool visible = args["visible"] != "false";

        return new UpdateImageLayerCommand(id, args, opacity, visible);
    }
    return NULL;
}

bool UpdateImageLayerCommand::operator ()(ReadyMapWebPlugin::MapControl *map)
{
  osgEarth::ImageLayer* layer = map->getMap()->getImageLayerByName(_id);

  if (!layer)
  {
    layer = createImageLayer();
    map->getMap()->addImageLayer(layer);
  }

  if (layer)
  {
    layer->setOpacity(_opacity);
    layer->setEnabled(_visible);

    return true;
  }

  return false;
}

osgEarth::ImageLayer* UpdateImageLayerCommand::createImageLayer()
{
  std::string type = _args["type"];
  std::string url = _args["url"];
  std::string name = _args["name"];

  if (url.empty() || _id.empty())
    return 0;

  if (type == "tms")
  {
    return new osgEarth::ImageLayer(_id, osgEarth::Drivers::TMSOptions(url));
  }
  else if (type == "wms")
  {
    std::string layers = _args["layers"];
    std::string format = _args["format"];
    std::string srs = _args["srs"];
    std::string styles = _args["styles"];

    osgEarth::Drivers::WMSOptions opt;
    opt.url() = url.substr(0, url.find("?"));

    std::string lower = osgDB::convertToLowerCase( url );

    if (!layers.empty())
      opt.layers() = layers;
    else if (lower.find("layers=", 0) != std::string::npos)
		  opt.layers() = extractBetween(lower, "layers=", "&");

    if (!format.empty())
      opt.format() = format;
    else if (lower.find("srs=", 0) != std::string::npos)
      opt.format() = extractBetween(lower, "format=image/", "&");

    if (!srs.empty())
      opt.srs() = srs;
    else if (lower.find("format=image/", 0) != std::string::npos)
      opt.srs() = extractBetween(lower, "srs=", "&");

    if (!styles.empty())
      opt.style() = styles;
    else if (lower.find("styles=", 0) != std::string::npos)
      opt.style() = extractBetween(lower, "styles=", "&");

    return new osgEarth::ImageLayer(_id, opt);
  }
  else if (type == "arcgis")
  {
    std::string token = _args["token"];

    osgEarth::Drivers::ArcGISOptions opt;
    opt.url() = url;

    if (!token.empty())
      opt.token() = token;

    return new osgEarth::ImageLayer(_id, opt);
  }

  return 0;
}

/***************************************************************************************/

Command* MoveImageLayerCommand::Factory::create(const std::string& command, const CommandArguments& args)
{
    if ("moveImageLayer" == command)
    {
        std::string id = args["id"];
        int index = as<int>(args["index"], -1);

        return new MoveImageLayerCommand(id, index);
    }
    return NULL;
}

bool MoveImageLayerCommand::operator ()(ReadyMapWebPlugin::MapControl *map)
{
  if (_index < 0)
    return false;

  osgEarth::ImageLayer* layer = map->getMap()->getImageLayerByName(_id);

  if (layer)
  {
    map->getMap()->moveImageLayer(layer, _index);
    return true;
  }

  return false;
}