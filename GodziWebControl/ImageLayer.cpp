#include <GodziWebControl/ImageLayer>
#include <GodziWebControl/MapControl>
#include <GodziWebControl/NodeMasks>
#include <GodziWebControl/Utils>
#include <GodziWebControl/TileSourceUtil>

#include <osgEarth/Common>
#include <osgEarthDrivers/tms/TMSOptions>
#include <osgEarthDrivers/wms/WMSOptions>
#include <osgEarthDrivers/arcgis/ArcGISOptions>

#include <osg/io_utils>
#include <osg/ClusterCullingCallback>
#include <string>
#include <sstream>


#include <osgDB/ReadFile>

using namespace osg;
using namespace osgDB;
using namespace GodziWebControl;
using namespace osgEarth;

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

bool RemoveImageLayerCommand::operator ()(GodziWebControl::MapControl *map)
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
    if ("updateImageLayer" == command || "addImageLayer" == command)
    {
        std::string id = args["id"];
        double opacity = as<double>(args["opacity"], 1.0);
        bool visible = args["visible"] != "false";

        return new UpdateImageLayerCommand(id, args, opacity, visible);
    }
    return NULL;
}

bool UpdateImageLayerCommand::operator ()(GodziWebControl::MapControl *map)
{
  if (_id.empty())
    return false;

  osgEarth::ImageLayer* layer = map->getMap()->getImageLayerByName(_id);

  if (!layer)
  {
    osgEarth::TileSourceOptions opt;
    if (TileSourceUtil::createTileSourceOptions(_args, opt))
    {
      layer = new osgEarth::ImageLayer(_id, opt);
      map->getMap()->addImageLayer(layer);
    }
  }

  if (layer)
  {
    layer->setOpacity(_opacity);
    layer->setEnabled(_visible);

    return true;
  }

  return false;
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

bool MoveImageLayerCommand::operator ()(GodziWebControl::MapControl *map)
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