#undef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0502

#include <ReadyMapWebPlugin/MapControl>
#include <ReadyMapWebPlugin/NodeMasks>
#include <ReadyMapWebPlugin/Utils>
#include <ReadyMapWebPlugin/MapCommands>
#include <ReadyMapWebPlugin/GetObjectInfoCommand>
#include <ReadyMapWebPlugin/MapEventHandler>

#include <osgEarth/Registry>
#include <osgEarthUtil/EarthManipulator>

#include <osg/io_utils>
#include <osg/Version>

#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>
#include <osgGA/TrackballManipulator>

#include <osg/GLObjects>

#include <osgViewer/api/Win32/GraphicsWindowWin32>

#include <sstream>
#include <iostream>

#include <windows.h>
#include <windowsx.h>

#include <string.h>
#include <atlconv.h>


//http://www.codeproject.com/KB/DLL/DLLModuleFileName.aspx
EXTERN_C IMAGE_DOS_HEADER __ImageBase;

using namespace ReadyMapWebPlugin;

#define WM_READYMAP_EVENT (WM_USER + 0)

class EventData
{
public:
    EventData(const std::string &target, const std::string &eventName, const std::string &data):
      _target(target),
          _event(eventName),
          _data(data)
      {
      }
      std::string _data;
      std::string _target;
      std::string _event;
};


class MapInit
{
public:
    MapInit():
      _log(0)
      {

          //Redirect cout and cerr to the log file
          char *appData = getenv("APPDATA");

          //Only enable logging if an env var is set.
          if (getenv("READYMAP_LOGGING") != 0)
          {
              std::string logPath = std::string(appData) + "\\ReadyMapWeb3D\\";
              bool gotFilename = false;         
              std::string filename;
              int i =0; 
              while (!gotFilename)
              {
                  std::stringstream ss;
                  ss << logPath << "readymap_log" << i << ".txt" << std::flush;
                  filename = ss.str();
                  gotFilename = !osgDB::fileExists(filename);
                  i++;
              }
              _log = new std::ofstream( filename.c_str() );
              std::cout.rdbuf( _log->rdbuf() );
              std::cerr.rdbuf( _log->rdbuf() );
          }



		  std::string cachePath = std::string(appData) + "\\ReadyMapWeb3D\\cache";
		  osgDB::makeDirectory( cachePath );
		  //Set up a global cache for all maps to use

		  osgEarth::TMSCacheOptions cacheOpt;
		  cacheOpt.setPath(cachePath);

		  osgEarth::Cache* globalCache = new osgEarth::TMSCache(cacheOpt);
		  osgEarth::Registry::instance()->setCacheOverride( globalCache );

#if (OPENSCENEGRAPH_MAJOR_VERSION == 2 && OPENSCENEGRAPH_MINOR_VERSION >= 8)
          int numThreads = OpenThreads::GetNumberOfProcessors();
          osg::notify(osg::NOTICE) << "Setting number of database threads to " << numThreads << std::endl;
          osg::DisplaySettings::instance()->setNumOfDatabaseThreadsHint( numThreads );
#endif


          LPCH lpszCurDir = new CHAR[_MAX_PATH];
          ::GetModuleFileNameA((HINSTANCE)&__ImageBase, lpszCurDir, _MAX_PATH);

          char* lastSlash = strrchr(lpszCurDir, '\\');
          if (lastSlash)
              *(lastSlash + 1) = '\0'; 
          osgDB::getLibraryFilePathList().push_front(std::string(lpszCurDir));
          SetDllDirectoryA(lpszCurDir);
      }

      ~MapInit()
      {

          osgEarth::Registry::instance()->setCacheOverride( 0 );
          if (_log)
          {
              _log->close();
              delete _log;
          }
      }

      std::ofstream* _log;
};

static MapInit s_mapInit;


static LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    MapControl* map = (MapControl*)GetWindowLong(hWnd, GWL_USERDATA);
    return map->handleNativeWindowingEvent(hWnd, msg, wParam, lParam);
}

MapControl::MapControl():
_eventCallback(0)
{
    _commandQueue = new CommandQueue;

    addCommandFactory(new SetMapCommand::Factory());
    addCommandFactory(new GetBackColorCommand::Factory());
    addCommandFactory(new SetBackColorCommand::Factory());
    addCommandFactory(new ChangeVisibilityCommand::Factory());
    addCommandFactory(new GetViewpointCommand::Factory());
    addCommandFactory(new SetViewpointCommand::Factory());
    addCommandFactory(new ToggleStatsCommand::Factory());
    addCommandFactory(new CyclePolygonModeCommand::Factory());
    addCommandFactory(new HomeCommand::Factory());
    addCommandFactory(new FadeMapCommand::Factory());

    addCommandFactory(new UpdateIconCommand::Factory());
    addCommandFactory(new RemoveIconCommand::Factory());

    addCommandFactory(new UpdateModelCommand::Factory());
    addCommandFactory(new RemoveModelCommand::Factory());

    addCommandFactory(new UpdateTextLabelCommand::Factory());
    addCommandFactory(new RemoveTextLabelCommand::Factory());

    addCommandFactory(new GetNamesCommand::Factory());
    addCommandFactory(new GetIntersectionCommand::Factory());   
    addCommandFactory(new GetDescriptionsCommand::Factory());

    addCommandFactory(new GetObjectInfoCommand::Factory());

    addCommandFactory(new SelectionCommandFactory() );
}

void MapControl::init(void* window)
{
    //Create a new viewer
    _viewer = new osgViewer::Viewer;
    //_viewer->setThreadingModel(osgViewer::ViewerBase::SingleThreaded);

    _viewer->setKeyEventSetsDone(0);

    _hwnd = (HWND)window;

    osg::ref_ptr<osg::GraphicsContext::Traits> t = new osg::GraphicsContext::Traits();
    t->inheritedWindowData = new osgViewer::GraphicsWindowWin32::WindowData( _hwnd );
    t->setInheritedWindowPixelFormat = true;
    t->doubleBuffer = true;
    t->windowDecoration = false;
    t->sharedContext = NULL;
    t->supportsResize = false;

    RECT rect;
    ::GetWindowRect( (HWND)_hwnd, &rect );
    t->x = 0;
    t->y = 0;
    t->width = rect.right - rect.left;
    t->height = rect.bottom - rect.top;

    if (t->width == 0) t->width = 1;
    if (t->height == 0) t->height = 1;

    osg::notify(osg::NOTICE) << "Creating window " << t->x << ", " << t->y << ", " << t->width << ", " << t->height << std::endl;

    osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext( t.get() );  

    //There is an issue somewhere in OSG that has problems with the reuse of context ID's.  To reproduce:
    //1)Open up the main example
    //2)Switch to a different map using the radio buttons
    //3)Open a new, empty tab
    //4)Close the original map tab
    //5)Open a new example tab.  The new viewer created tries to reuse the original context ID and either crashes or textures,geometry become wacky 

    //Doing this prevents context ID reuse
    osg::GraphicsContext::incrementContextIDUsageCount(gc->getState()->getContextID());

    _viewer->getCamera()->setGraphicsContext( gc.get() );
    _viewer->getCamera()->setViewport( new osg::Viewport( 0, 0, t->width, t->height ) );
    _viewer->getCamera()->setProjectionMatrixAsPerspective(30.0f, (double)(t->width)/(double)(t->height), 1.0f, 10000.0f);
    _viewer->getCamera()->setClearColor(_clearColor);

    _root = new osg::Group;

	osgEarth::Util::EarthManipulator* manip = new osgEarth::Util::EarthManipulator;
    //manip->setSmoothingEnabled(false);
    manip->setIntersectTraversalMask(TERRAIN);
    _viewer->setCameraManipulator( manip );

    //_root->addChild(osgDB::readNodeFile("c:/dev/OpenSceneGraph-Data/cow.osg"));

    /*//Set up a skylight rather than using the traditional headlamp
    _viewer->setLightingMode(osg::View::SKY_LIGHT);
    osg::Light* light = _viewer->getLight();
    light->setAmbient( osg::Vec4( .4, .4, .4, 1 ) );
    light->setDiffuse( osg::Vec4( 1, 1, .8, 1 ) );*/
    //_root->getOrCreateStateSet()->setMode(GL_LIGHTING , osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
    
    _viewer->setSceneData( _root.get() );

    _statsHandler = new osgViewer::StatsHandler();
    _viewer->addEventHandler( _statsHandler.get() );
    _viewer->addEventHandler( new MapEventHandler(this) );

    _stateSetManipulator = new osgGA::StateSetManipulator(_viewer->getCamera()->getOrCreateStateSet());
    _viewer->addEventHandler( _stateSetManipulator.get() );

    _viewer->setDone( false );

    _previousWindowProcedure = SubclassWindow(_hwnd, (WNDPROC)WinProc);

	SetWindowLong(_hwnd, GWL_USERDATA, (LONG)this);

	_viewer->realize();

    _selectionSet = new SelectionSet( _viewer->getCamera() );

    _viewer->home();
}

void MapControl::run()
{
  if (_viewer.valid())
  {
    while (!_viewer->done())
    {
      {
        //Execute any pending commands
        _commandQueue->execute(this);

        //Position the light to be at the eye point
        //osg::Vec3d eye, center, up;
        //_viewer->getCamera()->getViewMatrixAsLookAt(eye, center, up);
        //_viewer->getLight()->setPosition(osg::Vec4(eye, 1));
        //osg::notify(osg::NOTICE) << "Eye " << eye << std::endl;
        _viewer->frame();
      }
      microSleep(10);
    }
  }
}

int MapControl::cancel()
{
    int contextID = _viewer->getCamera()->getGraphicsContext()->getState()->getContextID();

    int result = 0;

    osg::notify(osg::NOTICE) << "Doing MapControl::cancel " << std::endl;

    if (isRunning())
    {
        _viewer->setDone( true);
        while (isRunning())
        {
            OpenThreads::Thread::YieldCurrentThread();
            osg::notify(osg::NOTICE) << "  Waiting for thread to cancel " << std::endl;
        }
    }

    //Subclass it back
    SubclassWindow(_hwnd, _previousWindowProcedure);

    //osg::discardAllDeletedGLObjects(contextID);
    //osg::Texture::discardAllDeletedTextureObjects(contextID);

    //Delete the viewer
    _viewer = 0;

    osg::notify(osg::NOTICE) << "  Thread cancelled" << std::endl;
    return result;
}

void MapControl::toggleStats()
{
    int key = _statsHandler->getKeyEventTogglesOnScreenStats();
    _viewer->getEventQueue()->keyPress(key, _viewer->getEventQueue()->getTime());
}

void MapControl::cyclePolygonMode()
{
    int key = _stateSetManipulator->getKeyEventCyclePolygonMode();
    _viewer->getEventQueue()->keyPress( key, _viewer->getEventQueue()->getTime() );
}

float MapControl::getVerticalScale()
{
    if (_terrain.valid())
    {
        return _terrain->getVerticalScale();
    }
    return -1.0f;
}

void MapControl::setVerticalScale(float verticalScale)
{
    if (_terrain.valid())
    {
        _terrain->setVerticalScale(verticalScale);
    }
}

float MapControl::getSampleRatio()
{
    if (_terrain.valid())
    {
        return _terrain->getSampleRatio();
    }
    return -1.0f;
}

void MapControl::setSampleRatio(float sampleRatio)
{
    if (_terrain.valid())
    {
        _terrain->setSampleRatio(sampleRatio);
    }
}



void MapControl::setMapFile(const std::string &mapFile)
{
    osg::notify(osg::NOTICE) << "MapControl::setMapFile " << mapFile << std::endl;

    bool wasMapValid = _mapNode.valid();
    if (_mapNode.valid())
    {
        //Remove the current map node
		    _root->removeChild( _mapNode.get() );
        _mapNode->releaseGLObjects();
        osg::flushAllDeletedGLObjects(_viewer->getCamera()->getGraphicsContext()->getState()->getContextID());
        _mapNode = 0;
        _csn = new osg::CoordinateSystemNode();
        _csn->setEllipsoidModel(new osg::EllipsoidModel());
        _terrain = 0;
    }

    //_viewer->getDatabasePager()->clear();

    //If the mapfile is pointing to a URL, prepend a "server:" to allow the URL to pass through to the Earth plugin rather
    //than being downloaded by the CURL plugin.
    std::string filename = mapFile;
    /*if (osgDB::containsServerAddress(filename))
    {
        filename = "server:" + filename;
    }*/


    //Read the file and add it to the scene
    osg::ref_ptr<osg::Node> loaded = osgDB::readNodeFile( filename );
    if ( loaded )
        _mapNode = osgEarth::MapNode::findMapNode( loaded.get() );

    if (_mapNode.valid())
    {
        _mapNode->setName( "map" );
        _mapNode->setNodeMask( TERRAIN );

        osg::notify(osg::NOTICE) << "Warning:  Couldn't find CSN" << std::endl;
        _csn = findTopMostNodeOfType<osg::CoordinateSystemNode>(_mapNode.get());
        if (_csn.valid())
        {
            osg::notify(osg::NOTICE) << "Found CSN using visitor " << std::endl;
        }
        else
        {
            osg::notify(osg::NOTICE) << "Warning:  Couldn't find CSN" << std::endl;
        }

        _terrain = findTopMostNodeOfType<osgTerrain::Terrain>(_mapNode);
        osgEarth::MapNode* mapNode = findTopMostNodeOfType<osgEarth::MapNode>(_mapNode);
        if (mapNode)
        {
            _root->addChild(_mapNode.get());

			      for (unsigned int i = 0; i < mapNode->getMap()->getNumImageLayers(); ++i)
            {
				        mapNode->getMap()->getImageLayerAt(i)->setOpacity( 1.0f );
            }
        }
    }
    _viewer->getDatabasePager()->registerPagedLODs(_root.get());
    _viewer->computeActiveCoordinateSystemNodePath();
    _viewer->getCameraManipulator()->setNode(_mapNode.get());

    //Go home if we are going from an invalid map to a valid one
    if (!wasMapValid && _mapNode.valid())
    {
        _viewer->home();
    }
}

LRESULT MapControl::handleNativeWindowingEvent( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch (msg) {
        case WM_READYMAP_EVENT:
            {
                EventData *ed = (EventData*)wParam;
                if (ed)
                {
                    if (_eventCallback) _eventCallback->handleEvent(ed->_target, ed->_event, ed->_data);
                    delete ed;
                }
                return 0;
            }
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
            {
                ::SetFocus ( hwnd );
                break;
            }
        case WM_MOUSEWHEEL:
            {
                getView()->getEventQueue()->mouseScroll(GET_WHEEL_DELTA_WPARAM(wParam)<0 ? osgGA::GUIEventAdapter::SCROLL_DOWN : osgGA::GUIEventAdapter::SCROLL_UP);
                return 0;
            }
    }
    return ::CallWindowProc(_previousWindowProcedure, hwnd, msg, wParam, lParam);
}

void MapControl::latLongHeightToXYZ(double lat, double lon, double height, double &x, double &y, double &z)
{
    //TODO:  Rework so that we are using the CSN again.  Some of the elements like GeoIcons expect the map to always have a consistent CSN
    /*if (_csn.valid() && _csn->getEllipsoidModel())
    {
        _csn->getEllipsoidModel()->convertLatLongHeightToXYZ(osg::DegreesToRadians(lat), osg::DegreesToRadians(lon), height, x, y, z);
    }
    else
    {
        x = lon;
        y = lat;
        z = height;
    }*/

    //osg::ref_ptr<osg::EllipsoidModel> em = new osg::EllipsoidModel();
    //em->convertLatLongHeightToXYZ(osg::DegreesToRadians(lat), osg::DegreesToRadians(lon), height, x, y, z);

    _mapNode->getMap()->getProfile()->getSRS()->getEllipsoid()->convertLatLongHeightToXYZ(
        osg::DegreesToRadians(lat), osg::DegreesToRadians(lon), height, x, y, z);
}

void MapControl::xyzToLatLongHeight(double x, double y, double z, double &lat, double &lon, double &height)
{
    _mapNode->getMap()->getProfile()->getSRS()->getEllipsoid()->convertXYZToLatLongHeight(
        x, y, z, lat, lon, height );
    lon = osg::RadiansToDegrees( lon );
    lat = osg::RadiansToDegrees( lat );

    //TODO:  Rework so that we are using the CSN again.  Some of the elements like GeoIcons expect the map to always have a consistent CSN
    //osg::ref_ptr<osg::EllipsoidModel> em = new osg::EllipsoidModel();
    //em->convertXYZToLatLongHeight(x, y, z, lat, lon, height);
    //lon = osg::RadiansToDegrees(lon);
    //lat = osg::RadiansToDegrees(lat);
    /*if (_csn.valid() && _csn->getEllipsoidModel())
    {
        _csn->getEllipsoidModel()->convertXYZToLatLongHeight(x, y, z, lat, lon, height);
        lat = osg::RadiansToDegrees(lat);
        lon = osg::RadiansToDegrees(lon);
    }
    else
    {
        lon = x;
        lat = y;
        height = z;
    }*/
}

osgEarth::Util::Viewpoint
MapControl::getViewpoint()
{
	osgEarth::Util::EarthManipulator* em = dynamic_cast<osgEarth::Util::EarthManipulator*>(getView()->getCameraManipulator());
    if (em)
    {
        return em->getViewpoint();
        //return em->getCurrentViewpoint();
    }
	return osgEarth::Util::Viewpoint();
}

void
MapControl::addCommandFactory(CommandFactory* factory)
{
    _commandFactories.push_back(factory);
}

std::string
MapControl::handleCommand(const std::string &command, const std::string &args, bool blocking)
{
    static std::string EMPTY_STRING;

    CommandArguments arguments(args);

    osg::notify(osg::INFO) << "MapControl::handleCommand " << command << "(" << args << ")" << std::endl;

    bool handled = false;
    for (CommandFactoryList::iterator itr = _commandFactories.begin(); itr != _commandFactories.end(); ++itr)
    {
        osg::ref_ptr<Command> cmd = itr->get()->create(command, arguments);
        if (cmd.valid())
        {
            osg::ref_ptr<osg::RefBlock> block;

            if (blocking)
            {
                block = new osg::RefBlock;
                cmd->setBlock(block);
            }

            _commandQueue->add(cmd.get());

            if (blocking)
            {
                block->block();
                return cmd->getResult();
            }
            handled = true;
            break;
        }
    }
    if (!handled)
    {
        osg::notify(osg::NOTICE) << "Could not create command for " << command << "(" << args << ")" << std::endl;
    }
    return EMPTY_STRING;
}

void
MapControl::postEvent(const std::string &target, const std::string &eventName, const std::string &data)
{
    EventData* ed = new EventData(target, eventName, data);
    PostMessage(_hwnd, WM_READYMAP_EVENT, (WPARAM)ed, 0L);
}
