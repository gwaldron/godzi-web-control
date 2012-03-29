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
        int id = as<int>(args["id"], -1);
        return new RemoveImageLayerCommand(id);
    }
    return NULL;
}

bool RemoveImageLayerCommand::operator ()(GodziWebControl::MapControl *map)
{
  osgEarth::ImageLayer* layer = map->getMap()->getImageLayerByUID(_id);
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
        int id = as<int>(args["id"], -1);
        std::string name = args["name"];
        double opacity = as<double>(args["opacity"], -1.0);
        bool visible = args["visible"] != "false";
        bool visibleSet = args["visible"].length() > 0;

        return new UpdateImageLayerCommand(id, name, args, opacity, visible, visibleSet);
    }
    return NULL;
}

bool UpdateImageLayerCommand::operator ()(GodziWebControl::MapControl *map)
{
  osgEarth::ImageLayer* layer = _id != -1 ? map->getMap()->getImageLayerByUID(_id) : 0L;

  if (!layer)
  {
    osgEarth::TileSourceOptions opt;
    if (TileSourceUtil::createTileSourceOptions(_args, opt))
    {
      layer = new osgEarth::ImageLayer(_name, opt);
      map->getMap()->addImageLayer(layer);
    }
  }

  if (layer)
  {
    if (_opacity >= 0.0)
      layer->setOpacity(_opacity);

    if (_visibleSet)
      layer->setVisible(_visible);

    osgEarth::Json::Value result;
    result["id"] = layer->getUID();
    osgEarth::Json::FastWriter writer;
    setResult(writer.write(result));

    return true;
  }

  return false;
}

/***************************************************************************************/

Command* MoveImageLayerCommand::Factory::create(const std::string& command, const CommandArguments& args)
{
    if ("moveImageLayer" == command)
    {
        int id = as<int>(args["id"], -1);
        int index = as<int>(args["index"], -1);

        return new MoveImageLayerCommand(id, index);
    }
    return NULL;
}

bool MoveImageLayerCommand::operator ()(GodziWebControl::MapControl *map)
{
  if (_index < 0)
    return false;

  osgEarth::ImageLayer* layer = map->getMap()->getImageLayerByUID(_id);

  if (layer)
  {
    map->getMap()->moveImageLayer(layer, _index);
    return true;
  }

  return false;
}

/***************************************************************************************/

Command* GetImageLayersCommand::Factory::create(const std::string& command, const CommandArguments& args)
{
    if ("getImageLayers" == command)
    {
        return new GetImageLayersCommand();
    }
    return NULL;
}

bool GetImageLayersCommand::operator ()(GodziWebControl::MapControl *map)
{
  osgEarth::ImageLayerVector layers;
  map->getMap()->getImageLayers(layers);

  std::stringstream idBuf;
  std::stringstream nameBuf;
  for (osgEarth::ImageLayerVector::const_iterator it = layers.begin(); it != layers.end(); ++it)
  {
    idBuf << (*it)->getUID() << ";";
    nameBuf << (*it)->getName() << ";";
  }

  osgEarth::Json::Value result;
  result["ids"] = idBuf.str();
  result["names"] = nameBuf.str();
  osgEarth::Json::FastWriter writer;
  setResult(writer.write(result));

  return true;
}

/***************************************************************************************/

Command* GetImageLayerPropertiesCommand::Factory::create(const std::string& command, const CommandArguments& args)
{
    if ("getImageLayerProperties" == command)
    {
        int id = as<int>(args["id"], -1);
        return new GetImageLayerPropertiesCommand(id);
    }
    return NULL;
}

bool GetImageLayerPropertiesCommand::operator ()(GodziWebControl::MapControl *map)
{
    osgEarth::ImageLayer* layer = map->getMap()->getImageLayerByUID(_id);
    if (layer)
    {
        osgEarth::Json::Value result;
        result["name"] = layer->getName();
        result["opacity"] = layer->getOpacity();
        result["visible"] = layer->getVisible();
        osgEarth::Json::FastWriter writer;
        setResult(writer.write(result));

        return true;
    }

    return false;
}