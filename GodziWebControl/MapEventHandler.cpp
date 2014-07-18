#include <GodziWebControl/MapEventHandler>

#include <osgEarth/JsonUtils>

#include <sstream>

using namespace osg;
using namespace osgGA;
using namespace osgEarth;
using namespace GodziWebControl;

MapEventHandler::MapEventHandler(GodziWebControl::MapControl* map):
_map(map)
{
}

bool
MapEventHandler::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
    switch (ea.getEventType())
    {
    case GUIEventAdapter::PUSH:
        {
            _xMouseDown = ea.getX();
            _yMouseDown = ea.getY();
            Json::Value root;
            root["x"] = ea.getX();
            root["y"] = ea.getY();
            root["button"] = ea.getButton();
            Json::FastWriter writer;
            _map->postEvent("", "onmousedown",writer.write(root));
            return false;
        }
    case GUIEventAdapter::RELEASE:
        {
            Json::Value root;
            root["x"] = ea.getX();
            root["y"] = ea.getY();
            root["button"] = ea.getButton();
            Json::FastWriter writer;
            std::string json = writer.write(root);
            _map->postEvent("", "onmouseup", json);

            if ( ::fabs( _xMouseDown - ea.getX() ) <= 3.0f && ::fabs( _yMouseDown - ea.getY() ) <= 3.0f )
            {
                _map->postEvent("", "onclick", json);
            }

            return false;
        }
    case GUIEventAdapter::MOVE:
    case GUIEventAdapter::DRAG:
        {
            Json::Value root;
            root["x"] = ea.getX();
            root["y"] = ea.getY();
            root["button"] = ea.getButton();
            Json::FastWriter writer;
            _map->postEvent("", "onmousemove",writer.write(root));
            return false;
        }
    case GUIEventAdapter::DOUBLECLICK:
        {
            Json::Value root;
            root["x"] = ea.getX();
            root["y"] = ea.getY();
            root["button"] = ea.getButton();
            Json::FastWriter writer;
            _map->postEvent("", "ondoubleclick",writer.write(root));
            return false;
        }
    case GUIEventAdapter::FRAME:
        {
            /*Json::Value root;
            root["frameTime"] = ea.getTime();           
            Json::FastWriter writer;
            _map->postEvent("", "onframeend",writer.write(root));
            return false;*/
        }
        break;
    case GUIEventAdapter::RESIZE:
        {
            _map->updateOverviewMap(true);
            return false;
        }
    default:
        return false;
    }

    return false;
}

void
MapEventHandler::onHit( FeatureSourceIndexNode* index, FeatureID fid, const EventArgs& args )
{
    Json::Value root;
    std::string fidStr = Stringify() << fid;
    root["fid"] = fidStr;

    Json::Value attributes;
    if ( index && index->getFeatureSource() )
    {
        const Feature* f;
        if ( index->getFeature(fid, f) )
        {
            const AttributeTable& attrs = f->getAttrs();
            for( AttributeTable::const_iterator i = attrs.begin(); i != attrs.end(); ++i)
            {
                attributes[i->first] = i->second.getString();
            }
        }
    }    
    root["attributes"] = attributes;

    Json::FastWriter writer;
    _map->postEvent("", "onfeatureselect", writer.write(root));
}

void
MapEventHandler::onMiss( const EventArgs& args )
{
  OE_WARN << "FEATURE MISS" << std::endl;
  _map->postEvent("", "onfeatureselect", "");
}