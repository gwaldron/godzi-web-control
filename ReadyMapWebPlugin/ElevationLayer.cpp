#include <ReadyMapWebPlugin/ElevationLayer>
#include <ReadyMapWebPlugin/MapControl>
#include <ReadyMapWebPlugin/NodeMasks>
#include <ReadyMapWebPlugin/Utils>
#include <ReadyMapWebPlugin/TileSourceUtil>

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
using namespace ReadyMapWebPlugin;
using namespace osgEarth;

/***************************************************************************************/

Command* RemoveElevationLayerCommand::Factory::create(const std::string& command, const CommandArguments& args)
{
    if ("removeElevationLayer" == command)
    {
        std::string id = args["id"];
        return new RemoveElevationLayerCommand(id);
    }
    return NULL;
}

bool RemoveElevationLayerCommand::operator ()(ReadyMapWebPlugin::MapControl *map)
{
  osgEarth::ElevationLayer* layer = map->getMap()->getElevationLayerByName(_id);
  if (layer)
  {
    map->getMap()->removeElevationLayer(layer);
  }

  return true;
}

/***************************************************************************************/

Command* AddElevationLayerCommand::Factory::create(const std::string& command, const CommandArguments& args)
{
    if ("addElevationLayer" == command)
    {
        std::string id = args["id"];

        return new AddElevationLayerCommand(id, args);
    }
    return NULL;
}

bool AddElevationLayerCommand::operator ()(ReadyMapWebPlugin::MapControl *map)
{
  if (_id.empty())
    return false;

  osgEarth::ElevationLayer* layer = map->getMap()->getElevationLayerByName(_id);

  if (!layer)
  {
    osgEarth::TileSourceOptions opt;
    if (TileSourceUtil::createTileSourceOptions(_args, opt))
    {
      layer = new osgEarth::ElevationLayer(_id, opt);
      map->getMap()->addElevationLayer(layer);
    }
  }

  return false;
}

/***************************************************************************************/

Command* MoveElevationLayerCommand::Factory::create(const std::string& command, const CommandArguments& args)
{
    if ("moveElevationLayer" == command)
    {
        std::string id = args["id"];
        int index = as<int>(args["index"], -1);

        return new MoveElevationLayerCommand(id, index);
    }
    return NULL;
}

bool MoveElevationLayerCommand::operator ()(ReadyMapWebPlugin::MapControl *map)
{
  if (_index < 0)
    return false;

  osgEarth::ElevationLayer* layer = map->getMap()->getElevationLayerByName(_id);

  if (layer)
  {
    map->getMap()->moveElevationLayer(layer, _index);
    return true;
  }

  return false;
}