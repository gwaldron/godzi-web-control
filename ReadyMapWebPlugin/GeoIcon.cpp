#include <ReadyMapWebPlugin/GeoIcon>
#include <ReadyMapWebPlugin/MapControl>
#include <ReadyMapWebPlugin/NodeMasks>
#include <ReadyMapWebPlugin/Utils>

#include <osgEarth/Common>

#include <osg/ClusterCullingCallback>
#include <osg/io_utils>
#include <string>
#include <sstream>


#include <osgDB/ReadFile>

#include <osg/Texture2D>

using namespace osg;
using namespace osgEarth;
using namespace ReadyMapWebPlugin;




Geode* createIcon(osg::Image* image,float s,float t)
{
    if (image)
    {
        if (s>0 && t>0)
        {

            float y = t;
            float x = y*(s/t);

            // set up the texture.
            osg::Texture2D* texture = new osg::Texture2D;
            texture->setImage(image);

            // set up the drawstate.
            osg::StateSet* dstate = new osg::StateSet;
            dstate->setMode(GL_CULL_FACE,osg::StateAttribute::OFF);
            dstate->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
            dstate->setTextureAttributeAndModes(0, texture,osg::StateAttribute::ON);

            // set up the geoset.
            Geometry* geom = new Geometry;
            geom->setStateSet(dstate);

            Vec3Array* coords = new Vec3Array(4);
            (*coords)[0].set(-x,y,0.0f);
            (*coords)[1].set(-x,-y,0.0f);
            (*coords)[2].set(x,-y,0.0f);
            (*coords)[3].set(x,y,0.0f);
            geom->setVertexArray(coords);

            Vec2Array* tcoords = new Vec2Array(4);
            (*tcoords)[0].set(0.0f,1.0f);
            (*tcoords)[1].set(0.0f,0.0f);
            (*tcoords)[2].set(1.0f,0.0f);
            (*tcoords)[3].set(1.0f,1.0f);
            geom->setTexCoordArray(0,tcoords);

            osg::Vec4Array* colours = new osg::Vec4Array(1);
            (*colours)[0].set(1.0f,1.0f,1.0,1.0f);
            geom->setColorArray(colours);
            geom->setColorBinding(Geometry::BIND_OVERALL);

            geom->addPrimitiveSet(new DrawArrays(PrimitiveSet::QUADS,0,4));

            // set up the geode.
            osg::Geode* geode = new osg::Geode;
            geode->addDrawable(geom);

            return geode;

        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
}

Geode* createIcon(osg::Image* image)
{
    return createIcon(image,image->s(),image->t());
}



GeoIcon::GeoIcon(const std::string &filename):
_image(0),
_iconWidth(-1),
_iconHeight(-1)
{
    _filename = filename;
    init();
}

void GeoIcon::setFilename(const std::string &filename)
{
    if (_filename != filename)
    {
      _filename = filename;
      _image = 0;
      init();
    }
}

void GeoIcon::setPosition(const osg::Vec3d &position)
{
    _position = position;
}

void GeoIcon::setSize(float w, float h)
{
    _iconWidth = w;
    _iconHeight = h;
    init();
}

void GeoIcon::init()
{
    removeChildren(0, getNumChildren());

    osg::AutoTransform* at = new osg::AutoTransform;
    at->setNodeMask(ICONS);
    at->setAutoRotateMode(AutoTransform::ROTATE_TO_SCREEN);
    at->setAutoScaleToScreen(true);
    at->setPosition(_position);
    //setMinimumScale(0);
    //setMaximumScale(500);
    getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    getOrCreateStateSet()->setRenderBinDetails(11, "RenderBin");
    getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
    if (!_image.valid() && !_filename.empty())
    {
        //Cache images since we will probably be reusing the images across GeoIcons
        osg::ref_ptr<osgDB::ReaderWriter::Options> opt = new osgDB::ReaderWriter::Options;
        opt->setObjectCacheHint(osgDB::ReaderWriter::Options::CACHE_IMAGES);
        _image = osgDB::readImageFile( _filename, opt.get());
    }

    if (_image.valid())
    {
        float s = _iconWidth <= 0 ? _image->s() : _iconWidth;
        float t = _iconHeight <= 0 ? _image->t() : _iconHeight;
        osg::Geode* geode = createIcon(_image.get(), s, t);
        at->addChild(geode);
    }
    addChild(at);
}

/***************************************************************************************/
Command* RemoveIconCommand::Factory::create(const std::string& command, const CommandArguments& args)
{
    if ("removeIcon" == command)
    {
        std::string id = args["id"];
        return new RemoveIconCommand(id);
    }
    return NULL;
}

RemoveIconCommand::RemoveIconCommand(const std::string &id)
{
    _id = id;
}

bool RemoveIconCommand::operator ()(ReadyMapWebPlugin::MapControl *map)
{
    GeoIcon* icon = dynamic_cast<GeoIcon*>(findNamedNode(_id, map->getRoot()));
    if (icon)
    {
        map->getRoot()->removeChild(icon);
    }
    return true;
}


/***************************************************************************************/
Command* UpdateIconCommand::Factory::create(const std::string& command, const CommandArguments& args)
{
    if ("updateIcon" == command)
    {
        std::string id       = args["id"];
        double latitude      = as<double>(args["latitude"], 0.0);
        double longitude     = as<double>(args["longitude"], 0.0);
        double height        = as<double>(args["height"], 0.0);
        std::string filename = args["filename"];
        float iconWidth      = as<float>(args["iconWidth"], 0.0f);
        float iconHeight     = as<float>(args["iconHeight"], 0.0f);

        return new UpdateIconCommand(id,osg::Vec3d(latitude,longitude,height),filename, iconWidth, iconHeight );

    }
    return NULL;
}

UpdateIconCommand::UpdateIconCommand(const std::string &id, const osg::Vec3d &latLonHeight, const std::string &filename,
                                     float iconWidth, float iconHeight)
{
    _id = id;
    _latLonHeight = latLonHeight;
    _filename = filename;
    _iconWidth = iconWidth;
    _iconHeight = iconHeight;
}

bool UpdateIconCommand::operator ()(ReadyMapWebPlugin::MapControl *map)
{
    //Find the icon
    GeoIcon* icon = dynamic_cast<GeoIcon*>(findNamedNode(_id, map->getRoot()));

    bool updating = (icon != 0);

    if (!icon) icon = new GeoIcon(_filename);
    icon->setName(_id);

    osg::Vec3d position;
    map->latLongHeightToXYZ(_latLonHeight.x(), _latLonHeight.y(), _latLonHeight.z(),
                            position.x(), position.y(), position.z());

    icon->setPosition(position);
    icon->setFilename(_filename);
    icon->setSize(_iconWidth, _iconHeight);

    if (map->getCoordinateSystemsNode() && map->getCoordinateSystemsNode()->getEllipsoidModel())
    {
      osg::Vec3d normal = map->getCoordinateSystemsNode()->getEllipsoidModel()->computeLocalUpVector(position.x(), position.y(), position.z());
      icon->setCullCallback(new osg::ClusterCullingCallback(position, normal,0.0));
    }

    if (!updating)
    {
        map->getRoot()->addChild(icon);
    }
    return true;
}