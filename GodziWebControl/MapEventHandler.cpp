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
