#include <GodziWebControl/ModelLayer>
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

Command* RemoveModelLayerCommand::Factory::create(const std::string& command, const CommandArguments& args)
{
    if ("removeModelLayer" == command)
    {
        int id = as<int>(args["id"], -1);
        return new RemoveModelLayerCommand(id);
    }
    return NULL;
}

bool RemoveModelLayerCommand::operator ()(GodziWebControl::MapControl *map)
{
  osgEarth::ModelLayer* layer = map->getMap()->getModelLayerByUID(_id);
  if (layer)
  {
    map->getMap()->removeModelLayer(layer);
  }

  return true;
}

/***************************************************************************************/

Command* AddModelLayerCommand::Factory::create(const std::string& command, const CommandArguments& args)
{
    if ("addModelLayer" == command)
    {
        int id = as<int>(args["id"], -1);
        std::string name = args["name"];

        return new AddModelLayerCommand(id, name, args);
    }
    return NULL;
}

bool AddModelLayerCommand::operator ()(GodziWebControl::MapControl *map)
{
  if (_name.empty())
    return false;

  osgEarth::ModelLayer* layer = map->getMap()->getModelLayerByUID(_id);

  if (!layer)
  {
    osgEarth::TileSourceOptions opt;
    if (TileSourceUtil::createTileSourceOptions(_args, opt))
    {
      layer = new osgEarth::ModelLayer(_name, opt);
      map->getMap()->addModelLayer(layer);
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

Command* MoveModelLayerCommand::Factory::create(const std::string& command, const CommandArguments& args)
{
    if ("moveModelLayer" == command)
    {
        int id = as<int>(args["id"], -1);
        int index = as<int>(args["index"], -1);

        return new MoveModelLayerCommand(id, index);
    }
    return NULL;
}

bool MoveModelLayerCommand::operator ()(GodziWebControl::MapControl *map)
{
  if (_index < 0)
    return false;

  osgEarth::ModelLayer* layer = map->getMap()->getModelLayerByUID(_id);

  if (layer)
  {
    map->getMap()->moveModelLayer(layer, _index);
    return true;
  }

  return false;
}

/***************************************************************************************/

Command* GetModelLayersCommand::Factory::create(const std::string& command, const CommandArguments& args)
{
    if ("getModelLayers" == command)
    {
        return new GetModelLayersCommand();
    }
    return NULL;
}

bool GetModelLayersCommand::operator ()(GodziWebControl::MapControl *map)
{
  osgEarth::ModelLayerVector layers;
  map->getMap()->getModelLayers(layers);

  std::stringstream idBuf;
  std::stringstream nameBuf;
  for (osgEarth::ModelLayerVector::const_iterator it = layers.begin(); it != layers.end(); ++it)
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

Command* ToggleModelLayerCommand::Factory::create(const std::string& command, const CommandArguments& args)
{
    if ("toggleModelLayer" == command)
    {
        int id = as<int>(args["id"], -1);
        bool visible = as<bool>(args["visible"], true);

        return new ToggleModelLayerCommand(id, visible);
    }
    return NULL;
}

bool ToggleModelLayerCommand::operator ()(GodziWebControl::MapControl *map)
{
  osgEarth::ModelLayer* layer = map->getMap()->getModelLayerByUID(_id);

  if (layer && layer->getVisible() != _visible)
  {
    layer->setVisible(_visible);
    return true;
  }

  return false;
}

/***************************************************************************************/

Command* GetModelLayerBoundsCommand::Factory::create(const std::string& command, const CommandArguments& args)
{
    if ("getModelLayerBounds" == command)
    {
        int id = as<int>(args["id"], -1);

        return new GetModelLayerBoundsCommand(id);
    }
    return NULL;
}

bool GetModelLayerBoundsCommand::operator ()(GodziWebControl::MapControl *map)
{
  if (_id == -1)
    return false;

  osgEarth::ModelLayer* layer = map->getMap()->getModelLayerByUID(_id);

  if (layer)
  {
    osg::ref_ptr<osg::Node> temp = layer->createSceneGraph( map->getMap(), map->getMap()->getDBOptions(), 0L );
    if (temp.valid())
    {
      osg::NodePathList nodePaths = temp->getParentalNodePaths();
      if (!nodePaths.empty())
      {
        osg::NodePath path = nodePaths[0];

        osg::Matrixd localToWorld = osg::computeLocalToWorld( path );
        osg::Vec3d center = osg::Vec3d(0,0,0) * localToWorld;

        const osg::BoundingSphere& bs = temp->getBound();

        // if the tether node is a MT, we are set. If it's not, we need to get the
        // local bound and add its translation to the localToWorld.
        if ( !dynamic_cast<osg::MatrixTransform*>( temp.get() ) )
          center += bs.center();

        GeoPoint output;
        output.fromWorld(map->getMap()->getSRS(), center);

        osgEarth::Json::Value result;
        result["id"] = _id;
        result["latitude"] = output.y();
        result["longitude"] = output.x();
        result["radius"] = bs.radius();
        osgEarth::Json::FastWriter writer;
        setResult(writer.write(result));

        return true;
      }
    }
  }

  return false;
}

/***************************************************************************************/

Command* GetModelLayerPropertiesCommand::Factory::create(const std::string& command, const CommandArguments& args)
{
    if ("getModelLayerProperties" == command)
    {
        int id = as<int>(args["id"], -1);
        return new GetModelLayerPropertiesCommand(id);
    }
    return NULL;
}

bool GetModelLayerPropertiesCommand::operator ()(GodziWebControl::MapControl *map)
{
    osgEarth::ModelLayer* layer = map->getMap()->getModelLayerByUID(_id);
    if (layer)
    {
        osgEarth::Json::Value result;
        result["name"] = layer->getName();
        result["overlay"] = layer->getOverlay();
        result["visible"] = layer->getVisible();
        osgEarth::Json::FastWriter writer;
        setResult(writer.write(result));

        return true;
    }

    return false;
}