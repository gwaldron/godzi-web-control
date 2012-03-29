#include <GodziWebControl/ElevationLayer>
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

Command* RemoveElevationLayerCommand::Factory::create(const std::string& command, const CommandArguments& args)
{
    if ("removeElevationLayer" == command)
    {
        int id = as<int>(args["id"], -1);
        return new RemoveElevationLayerCommand(id);
    }
    return NULL;
}

bool RemoveElevationLayerCommand::operator ()(GodziWebControl::MapControl *map)
{
  osgEarth::ElevationLayer* layer = map->getMap()->getElevationLayerByUID(_id);
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
        int id = as<int>(args["id"], -1);
        std::string name = args["name"];

        return new AddElevationLayerCommand(id, name, args);
    }
    return NULL;
}

bool AddElevationLayerCommand::operator ()(GodziWebControl::MapControl *map)
{
  if (_name.empty())
    return false;

  osgEarth::ElevationLayer* layer = map->getMap()->getElevationLayerByUID(_id);

  if (!layer)
  {
    osgEarth::TileSourceOptions opt;
    if (TileSourceUtil::createTileSourceOptions(_args, opt))
    {
      layer = new osgEarth::ElevationLayer(_name, opt);
      map->getMap()->addElevationLayer(layer);
    }

    if (layer)
    {
      osgEarth::Json::Value result;
      result["id"] = layer->getUID();
      osgEarth::Json::FastWriter writer;
      setResult(writer.write(result));

      return true;
    }
  }

  return false;
}

/***************************************************************************************/

Command* MoveElevationLayerCommand::Factory::create(const std::string& command, const CommandArguments& args)
{
    if ("moveElevationLayer" == command)
    {
        int id = as<int>(args["id"], -1);
        int index = as<int>(args["index"], -1);

        return new MoveElevationLayerCommand(id, index);
    }
    return NULL;
}

bool MoveElevationLayerCommand::operator ()(GodziWebControl::MapControl *map)
{
  if (_index < 0)
    return false;

  osgEarth::ElevationLayer* layer = map->getMap()->getElevationLayerByUID(_id);

  if (layer)
  {
    map->getMap()->moveElevationLayer(layer, _index);
    return true;
  }

  return false;
}