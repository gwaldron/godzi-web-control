#include <GodziWebControl/Kml>
#include <GodziWebControl/MapControl>
//#include <GodziWebControl/NodeMasks>
#include <GodziWebControl/Utils>

#include <osgEarth/Common>
#include <osgEarthDrivers/kml/KML>

#include <osg/io_utils>
#include <string>
#include <sstream>

#include <osgDB/ReadFile>

using namespace osg;
using namespace osgDB;
using namespace GodziWebControl;
using namespace osgEarth;

/***************************************************************************************/

#undef  LC
#define LC "[RemoveKmlCommand] "

Command* RemoveKmlCommand::Factory::create(const std::string& command, const CommandArguments& args)
{
    if ("removeKml" == command)
    {
        std::string id = args["id"];
        return new RemoveKmlCommand(id);
    }
    return NULL;
}

RemoveKmlCommand::RemoveKmlCommand(const std::string &id)
{
    _id = id;
}

bool RemoveKmlCommand::operator ()(GodziWebControl::MapControl *map)
{   
    osg::Node* node = findNamedNode(_id, map->getRoot());
    if (node)
    {
        map->getRoot()->removeChild(node);
    }
    return true;
}

/***************************************************************************************/

#undef  LC
#define LC "[LoadKmlCommand] "

Command* LoadKmlCommand::Factory::create(const std::string& command, const CommandArguments& args)
{
    if ("loadKml" == command)
    {
        std::string id             = args["id"];
        std::string url            = args["url"];
        std::string defaultIconURI = args["defaultIconURI"];
        double iconBaseScale       = as<double>(args["iconBaseScale"], 1.0);
        int iconMaxSize            = as<int>(args["iconMaxSize"], 32);
        bool declutter             = args["declutter"] == "true";

        OE_INFO << LC << "Creating command to load: " << url << std::endl;

        return new LoadKmlCommand(id, url, defaultIconURI, iconBaseScale, iconMaxSize, declutter);
    }
    return NULL;
}

LoadKmlCommand::LoadKmlCommand(const std::string &id, const std::string &url, const std::string &defaultIconURI, double iconBaseScale, int iconMaxSize, bool declutter)
{
    _id = id;
    _url = url;
    _defaultIconURI = defaultIconURI;
    _iconBaseScale = iconBaseScale;
    _iconMaxSize = iconMaxSize;
    _declutter = declutter;
}

bool LoadKmlCommand::operator ()(GodziWebControl::MapControl *map)
{
    if ( !_url.empty() )
    {
        osgEarth::Drivers::KMLOptions kml_options;
        kml_options.defaultIconURI() = _defaultIconURI;
        kml_options.iconBaseScale() = _iconBaseScale;
        kml_options.iconMaxSize() = _iconMaxSize;
        kml_options.declutter() = _declutter;

        OE_INFO << LC << "Loading: " << _url << std::endl;
        
        osg::Node* kml = osgEarth::Drivers::KML::load( URI(_url), map->getMapNode(), kml_options );
        if ( kml )
        {
            OE_INFO << LC << "KML loaded: " << _url << std::endl;
            kml->setName(_id);
            map->getRoot()->addChild( kml );
            return true;
        }

        OE_INFO << LC << "Failed to load: " << _url << std::endl;
    }

    return false;
}