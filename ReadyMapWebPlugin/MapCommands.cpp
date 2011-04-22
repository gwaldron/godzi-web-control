#include <ReadyMapWebPlugin/MapCommands>
#include <ReadyMapWebPlugin/Utils>

#include <osgEarth/Common>
#include <osgEarth/JsonUtils>
#include <osgEarth/MapNode>
#include <osgEarthUtil/EarthManipulator>

#include <osg/ComputeBoundsVisitor>
#include <osgUtil/IntersectionVisitor>
#include <osgUtil/LineSegmentIntersector>
#include <osgSim/ShapeAttribute>

#include <iomanip>

using namespace ReadyMapWebPlugin;


Command*
SetMapCommand::Factory::create(const std::string& command, const CommandArguments& args)
{
    if ("setMap" == command)
    {
        std::string filename = args["filename"];
        return new SetMapCommand(filename);
    }
    return NULL;
}

SetMapCommand::SetMapCommand(const std::string &mapFile):
_mapFile(mapFile)
{
}

bool SetMapCommand::operator ()(MapControl* map)
{
    map->setMapFile( _mapFile );
    return false;
}



/**************************************************************************************************/

Command* GetBackColorCommand::Factory::create(const std::string& command, const CommandArguments& args)
{
    if ("getBackColor" == command)
    {
        return new GetBackColorCommand();
    }
    return NULL;
}

GetBackColorCommand::GetBackColorCommand()
{        
}

bool GetBackColorCommand::operator ()(MapControl* map)
{
    osg::Vec4f color = map->getView()->getCamera()->getClearColor();
    osgEarth::Json::Value result;
    result["r"] = color.r();
    result["g"] = color.g();
    result["b"] = color.b();
    result["a"] = color.a();
    osgEarth::Json::FastWriter writer;
    setResult(writer.write(result));
    return true;
}

/**************************************************************************************************/

Command* ChangeVisibilityCommand::Factory::create(const std::string& command, const CommandArguments& args)
{
    if ("changeVisibility" == command)
    {
        std::string id = args["id"];
        bool visible   = args["visible"] == "true";
        return new ChangeVisibilityCommand(id, visible);
    }
    return NULL;
}

ChangeVisibilityCommand::ChangeVisibilityCommand(const std::string &id, bool visible):
_id(id),
_visible(visible)
{
}

bool ChangeVisibilityCommand::operator()(MapControl* map)
{
    osg::Node* node = findNamedNode(_id, map->getRoot());
    if (node)
    {
        node->setNodeMask( _visible ? ~0 : 0 );
    }
    else
    {
        osg::notify(osg::NOTICE) << "Couldn't find node with id " << _id << std::endl;
    }
    return true;
}

/**************************************************************************************************/
Command* GetViewpointCommand::Factory::create(const std::string& command, const CommandArguments& args)
{
    if ("getViewpoint" == command)
    {
        return new GetViewpointCommand();
    }
    return NULL;
}

GetViewpointCommand::GetViewpointCommand()
{        
}

bool GetViewpointCommand::operator ()(MapControl* map)
{
	osgEarth::Util::Viewpoint vp = map->getViewpoint();
    osgEarth::Json::Value result;
    result["valid"] = vp.isValid();
    if ( vp.isValid() )
    {
        result["latitude"]  = vp.getFocalPoint().y(); //osg::RadiansToDegrees(vp.getFocalPoint().y() );
        result["longitude"] = vp.getFocalPoint().x(); //osg::RadiansToDegrees(vp._x);
        result["altitude"]  = vp.getFocalPoint().z();
        result["range"]     = vp.getRange(); //_range;
        result["heading"]   = vp.getHeading(); //osg::RadiansToDegrees(vp._heading);
        result["pitch"]     = vp.getPitch(); //osg::RadiansToDegrees(vp._pitch);
    }
    osgEarth::Json::FastWriter writer;
    setResult(writer.write(result));
    return true;
}


/**************************************************************************************************/

Command* SetViewpointCommand::Factory::create(const std::string& command, const CommandArguments& args)
{
    if ("setViewpoint" == command)
    {
        double longitude      = osgEarth::as<double>(args["longitude"], 0.0);
        double latitude       = osgEarth::as<double>(args["latitude"], 0.0);
        double altitude       = osgEarth::as<double>(args["altitude"], 0.0);
        double range          = osgEarth::as<double>(args["range"], 0.0);
        double heading        = osgEarth::as<double>(args["heading"], 0.0);
        double pitch          = osgEarth::as<double>(args["pitch"], 0.0);
        double transitionTime = osgEarth::as<double>(args["transitionTime"], 0.0);

		return new SetViewpointCommand( osgEarth::Util::Viewpoint(
            osg::Vec3d( longitude, latitude, altitude ),
            heading, pitch, range ),
            transitionTime );
            //Viewpoint(osg::DegreesToRadians(longitude), 
            //osg::DegreesToRadians(latitude), 
            //range, 
            //osg::DegreesToRadians(heading),
            //osg::DegreesToRadians(pitch)),
            //transitionTime);

    }
    return NULL;
}

SetViewpointCommand::SetViewpointCommand(const osgEarth::Util::Viewpoint& viewpoint, double transitionTime)
{
    _viewpoint = viewpoint;
    _transitionTime = transitionTime;
}

bool SetViewpointCommand::operator()(MapControl* map)
{
	osgEarth::Util::EarthManipulator* em = dynamic_cast<osgEarth::Util::EarthManipulator*>(
        map->getView()->getCameraManipulator() );

    if (em)
    {
        em->setViewpoint(_viewpoint, _transitionTime);
        //em->setViewpoint( _viewpoint );
    }
    return true;
}


/**************************************************************************************************/
Command* SetBackColorCommand::Factory::create(const std::string& command, const CommandArguments& args)
{
    if ("setBackColor" == command)
    {
        float r = osgEarth::as<float>(args["r"], 0.0f);
        float g = osgEarth::as<float>(args["g"], 0.0f);
        float b = osgEarth::as<float>(args["b"], 0.0f);
        float a = osgEarth::as<float>(args["a"], 0.0f);
        return new SetBackColorCommand(osg::Vec4(r,g,b,a));
    }
    return NULL;
}

SetBackColorCommand::SetBackColorCommand(const osg::Vec4f &color):
_color(color)
{        
}

bool SetBackColorCommand::operator ()(MapControl* map)
{
    map->getView()->getCamera()->setClearColor(_color);
    return true;
}

/**************************************************************************************************/
Command* ToggleStatsCommand::Factory::create(const std::string &command, const CommandArguments &args)
{
    if ("toggleStats" == command)
    {
        return new ToggleStatsCommand();
    }
    return NULL;
}

ToggleStatsCommand::ToggleStatsCommand()
{
}

bool ToggleStatsCommand::operator ()(ReadyMapWebPlugin::MapControl *map)
{
    map->toggleStats();
    return true;
}

/**************************************************************************************************/

Command* CyclePolygonModeCommand::Factory::create(const std::string &command, const CommandArguments &args)
{
    if ("cyclePolygonMode" == command)
    {
        return new CyclePolygonModeCommand();
    }
    return NULL;
}

CyclePolygonModeCommand::CyclePolygonModeCommand()
{
}

bool CyclePolygonModeCommand::operator ()(ReadyMapWebPlugin::MapControl *map)
{
    map->cyclePolygonMode();
    return true;
}

/**************************************************************************************************/
Command* HomeCommand::Factory::create(const std::string &command, const CommandArguments &args)
{
    if ("home" == command)
    {
        return new HomeCommand();
    }
    return NULL;
}

HomeCommand::HomeCommand()
{
}

bool HomeCommand::operator ()(ReadyMapWebPlugin::MapControl *map)
{
    map->getView()->home();
    return true;
}


/**************************************************************************************************/
Command* GetNamesCommand::Factory::create(const std::string &command, const CommandArguments &args)
{
    if ("getNames" == command)
    {
        int x = osgEarth::as<int>(args["x"], 0);
        int y = osgEarth::as<int>(args["y"], 0);
        return new GetNamesCommand(x,y);
    }
    return NULL;
}

GetNamesCommand::GetNamesCommand(int x, int y):
_x(x),
_y(y)
{
}

bool GetNamesCommand::operator()(MapControl* map)
{
    osg::notify(osg::INFO) << "Executing GetNamesCommand () " << std::endl;

    //Use the PolytopeIntersector to select objects
    int w = 3;
    int h = 3;
    osgUtil::PolytopeIntersector* picker = new osgUtil::PolytopeIntersector(osgUtil::Intersector::WINDOW, _x-w, _y-h, _x + w, _y + h);  
    osgUtil::IntersectionVisitor iv(picker);
    map->getView()->getCamera()->accept( iv);

    std::vector<std::string> names;

    if (picker->containsIntersections())
    {
        //Copy any named nodes into the names list
        const osg::NodePath& nodePath = picker->getFirstIntersection().nodePath;
        for (osg::NodePath::const_iterator itr = nodePath.begin(); itr != nodePath.end(); ++itr)
        {
            if (!(*itr)->getName().empty())
            {
                names.push_back((*itr)->getName());
            }
        }
    }

    std::stringstream buf;
    for (unsigned int i = 0; i < names.size(); ++i)
    {
        buf << names[i] << ";";
    }

    osgEarth::Json::Value result;
    result["names"] = buf.str();
    osgEarth::Json::FastWriter writer;
    setResult(writer.write(result));
    return true;
}

/**************************************************************************************************/
Command* GetIntersectionCommand::Factory::create(const std::string &command, const CommandArguments &args)
{
    if ("getIntersection" == command)
    {
        int x = osgEarth::as<int>(args["x"], 0);
        int y = osgEarth::as<int>(args["y"], 0);
        return new GetIntersectionCommand(x,y);
    }
    return NULL;
}

GetIntersectionCommand::GetIntersectionCommand(int x, int y):
_x(x),
_y(y)
{
}

bool GetIntersectionCommand::operator()(MapControl* map)
{
    //Invert the Y coordinate
    osg::Viewport* viewport = map->getView()->getCamera()->getViewport();
    //int invY = viewport->height()-_y-1;

    bool valid = false;
    
    osg::Vec3d intersection;
    //Compute intersections with the viewer
    osgUtil::LineSegmentIntersector::Intersections intersections;
    if (map->getView()->computeIntersections(_x, _y, intersections))
    {
        intersection = intersections.begin()->getWorldIntersectPoint();
        valid = true;
    }
    

    double latitude = 0;
    double longitude = 0;
    double height = 0;

    if (valid)
    {
        map->xyzToLatLongHeight(intersection.x(), intersection.y(), intersection.z(), latitude,longitude,height);
    }

    osgEarth::Json::Value result;
    result["latitude"] = latitude;
    result["longitude"] = longitude;
    result["height"] = height;
    result["valid"] = valid;
    osgEarth::Json::FastWriter writer;
    osg::notify(osg::NOTICE) << "Intersection " << writer.write(result);
    setResult(writer.write(result));
    return true;
}


/**************************************************************************************************/
Command* GetDescriptionsCommand::Factory::create(const std::string &command, const CommandArguments &args)
{
    if ("getDescriptions" == command)
    {
        int x = osgEarth::as<int>(args["x"], 0);
        int y = osgEarth::as<int>(args["y"], 0);
        return new GetDescriptionsCommand(x,y);
    }
    return NULL;
}

GetDescriptionsCommand::GetDescriptionsCommand(int x, int y):
_x(x),
_y(y)
{
}

bool GetDescriptionsCommand::operator()(MapControl* map)
{
    osg::notify(osg::INFO) << "Executing GetDescriptionsCommand () " << std::endl;

    //Use the PolytopeIntersector to select objects
    int w = 3;
    int h = 3;
    osgUtil::PolytopeIntersector* picker = new osgUtil::PolytopeIntersector(osgUtil::Intersector::WINDOW, _x-w, _y-h, _x + w, _y + h);  
    osgUtil::IntersectionVisitor iv(picker);
    map->getView()->getCamera()->accept( iv);

    std::vector<std::string> descriptions;

    if (picker->containsIntersections())
    {
        //Copy any named nodes into the names list
        const osg::NodePath& nodePath = picker->getFirstIntersection().nodePath;
        for (osg::NodePath::const_iterator itr = nodePath.begin(); itr != nodePath.end(); ++itr)
        {   
            //osg::notify(osg::NOTICE) << "Name: " << (*itr)->getName() << " descriptions=" << (*itr)->getNumDescriptions() << std::endl;
            if ((*itr)->getNumDescriptions() > 0)
            {
                for (osg::Node::DescriptionList::const_iterator descItr = (*itr)->getDescriptions().begin();
                    descItr != (*itr)->getDescriptions().end();
                    ++descItr)
                {
                    descriptions.push_back(*descItr);
                }
            }
        }
    }

    std::stringstream buf;
    for (unsigned int i = 0; i < descriptions.size(); ++i)
    {
        buf << descriptions[i] << ";";
    }

    osg::notify(osg::NOTICE) << "Got descriptions " << buf.str() << std::endl;

    osgEarth::Json::Value result;
    result["descriptions"] = buf.str();
    osgEarth::Json::FastWriter writer;
    setResult(writer.write(result));
    return true;
}

/**************************************************************************************************/
Command* FadeMapCommand::Factory::create(const std::string &command, const CommandArguments &args)
{
    if ("fadeMap" == command)
    {
        float opacity = osgEarth::as<float>(args["opacity"], 0.0f);
        return new FadeMapCommand(opacity);
    }
    return NULL;
}

FadeMapCommand::FadeMapCommand(float opacity):
_opacity(opacity)
{   
}

bool FadeMapCommand::operator ()(ReadyMapWebPlugin::MapControl *map)
{
  osg::notify(osg::NOTICE) << "FadeMapCommand " << _opacity << std::endl;
	osgEarth::Map* oeMap = map->getMap();
  if (oeMap)
  {
		unsigned int numLayers = oeMap->getNumImageLayers();
    for (unsigned int i = 0; i < numLayers; ++i)
    {
			oeMap->getImageLayerAt(i)->setOpacity( _opacity );
    }
  }
  return true;
}


/*****************************************************************************/
