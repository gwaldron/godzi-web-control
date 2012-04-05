#include <GodziWebControl/Annotations>
#include <GodziWebControl/MapControl>
#include <GodziWebControl/NodeMasks>
#include <GodziWebControl/Utils>

#include <osgEarth/Common>
#include <osgEarthAnnotation/LabelNode>
#include <osgEarthAnnotation/PlaceNode>
#include <osgEarthAnnotation/CircleNode>
#include <osgEarthAnnotation/EllipseNode>
#include <osgEarthAnnotation/RectangleNode>
#include <osgEarthAnnotation/FeatureNode>
#include <osgEarthAnnotation/LocalGeometryNode>
#include <osgEarthSymbology/Geometry>
#include <osgEarthFeatures/GeometryUtils>
#include <osgEarthFeatures/Feature>

#include <osg/io_utils>
#include <osg/ClusterCullingCallback>
#include <string>
#include <sstream>


#include <osgDB/ReadFile>

using namespace GodziWebControl;
using namespace osgEarth;
using namespace osgEarth::Annotation;
using namespace osgEarth::Symbology;
using namespace osgEarth::Features;


//------------------------------------------------------------------------

#define ANNO_GROUP_NAME "_annotations"

namespace
{
    osg::Group* getAnnotationGroup(MapControl* map)
    {
        osg::Group* root = map->getRoot();

        FindNamedNodeVisitor nv( ANNO_GROUP_NAME );
        root->accept(nv);

        osg::Group* annos = 0L;
        if ( nv._result )
            annos = nv._result->asGroup();

        if ( !annos )
        {
            annos = new osg::Group();
            annos->setName( ANNO_GROUP_NAME );
            Decluttering::setEnabled( annos->getOrCreateStateSet(), true );
            root->addChild( annos );
        }

        return annos;
    }
}


//------------------------------------------------------------------------


void
AnnotationCommands::registerAll( MapControl* map )
{
    map->addCommandFactory( new CreateLabelNodeCommand::Factory );
    map->addCommandFactory( new CreatePlaceNodeCommand::Factory );
    map->addCommandFactory( new CreateCircleNodeCommand::Factory );
    map->addCommandFactory( new CreateEllipseNodeCommand::Factory );
    map->addCommandFactory( new CreateRectangleNodeCommand::Factory );
    map->addCommandFactory( new CreateFeatureNodeCommand::Factory );
    map->addCommandFactory( new CreateLocalGeometryNodeCommand::Factory );
    map->addCommandFactory( new SetAnnotationNodePositionCommand::Factory );
    map->addCommandFactory( new SetAnnotationNodeVisibilityCommand::Factory );
    map->addCommandFactory( new RemoveAnnotationNodeCommand::Factory );
}


//------------------------------------------------------------------------


Command*
CreateLabelNodeCommand::Factory::create(const std::string& cmd, const CommandArguments& args)
{
    if ( "createLabelNode" == cmd )
    {
        return new CreateLabelNodeCommand(
            args["id"],
            as<double>( args["lat"],  0.0 ),
            as<double>( args["long"], 0.0 ),
            as<double>( args["alt"],  0.0 ),
            args["text"] );
    }
    return 0L;
}


CreateLabelNodeCommand::CreateLabelNodeCommand(const std::string& id,
                                               double             lat_degrees,
                                               double             lon_degrees,
                                               double             alt_m,
                                               const std::string& text ) :
_id  ( id ),
_lat ( lat_degrees ),
_lon ( lon_degrees ),
_alt ( alt_m ),
_text( text )
{
    //nop
}

bool
CreateLabelNodeCommand::operator ()( MapControl* map )
{
    osg::Group* annoGroup = getAnnotationGroup(map);
    if ( annoGroup )
    {
        annoGroup->addChild( new LabelNode(
            map->getMapNode(),
            GeoPoint( SpatialReference::create("wgs84"), _lon, _lat, _alt ),
            _text ) );
    }
    return annoGroup != 0L;
}


//------------------------------------------------------------------------


Command*
CreatePlaceNodeCommand::Factory::create(const std::string& cmd, const CommandArguments& args)
{
    if ( "createPlaceNode" == cmd )
    {
        return new CreatePlaceNodeCommand(
            args["id"],
            as<double>( args["lat"],  0.0 ),
            as<double>( args["long"], 0.0 ),
            as<double>( args["alt"],  0.0 ),
            args["text"],
            args["iconURI"] );
    }
    return 0L;
}


CreatePlaceNodeCommand::CreatePlaceNodeCommand(const std::string& id,
                                               double             lat_degrees,
                                               double             lon_degrees,
                                               double             alt_m,
                                               const std::string& text,
                                               const std::string& iconURI) :
_id     ( id ),
_lat    ( lat_degrees ),
_lon    ( lon_degrees ),
_alt    ( alt_m ),
_text   ( text ),
_iconURI( iconURI )
{
    //nop
}

bool
CreatePlaceNodeCommand::operator ()( MapControl* map )
{
    osg::Group* annoGroup = getAnnotationGroup(map);
    if ( annoGroup )
    {
        osg::Image* icon = URI(_iconURI).getImage();

        annoGroup->addChild( new PlaceNode(
            map->getMapNode(),
            GeoPoint( SpatialReference::create("wgs84"), _lon, _lat, _alt ),
            icon,
            _text ) );
    }
    return annoGroup != 0L;
}


//------------------------------------------------------------------------

Command*
CreateCircleNodeCommand::Factory::create(const std::string& cmd, const CommandArguments& args)
{
    if ( "createCircleNode" == cmd )
    {
        osg::Vec4f color(
            as<float>(args["colorR"],1.0f),
            as<float>(args["colorG"],1.0f),
            as<float>(args["colorB"],1.0f),
            as<float>(args["colorA"],1.0f) );

        return new CreateCircleNodeCommand(
            args["id"],
            as<double>( args["lat"],    0.0 ),
            as<double>( args["long"],   0.0 ),
            as<double>( args["radius"], 0.0 ),
            color );
    }
    return 0L;
}


CreateCircleNodeCommand::CreateCircleNodeCommand(const std::string& id,
                                                 double             lat_degrees,
                                                 double             lon_degrees,
                                                 double             radius_m,
                                                 const osg::Vec4f&  color) :
_id    ( id ),
_lat   ( lat_degrees ),
_lon   ( lon_degrees ),
_radius( radius_m ),
_color ( color )
{
    //nop
}

bool
CreateCircleNodeCommand::operator ()( MapControl* map )
{
    osg::Group* annoGroup = getAnnotationGroup(map);
    if ( annoGroup )
    {
        Style style;
        style.getOrCreate<TextSymbol>()->fill()->color() = _color;

        annoGroup->addChild( new CircleNode(
            map->getMapNode(),
            GeoPoint( SpatialReference::create("wgs84"), _lon, _lat, _alt ),
            Distance(_radius, Units::METERS),
            style ) );
    }
    return annoGroup != 0L;
}


//------------------------------------------------------------------------


Command*
CreateEllipseNodeCommand::Factory::create(const std::string& cmd, const CommandArguments& args)
{
    if ( "createEllipseNode" == cmd )
    {
        osg::Vec4f color(
            as<float>(args["colorR"],1.0f),
            as<float>(args["colorG"],1.0f),
            as<float>(args["colorB"],1.0f),
            as<float>(args["colorA"],1.0f) );

        return new CreateEllipseNodeCommand(
            args["id"],
            as<double>( args["lat"],            0.0 ),
            as<double>( args["long"],           0.0 ),
            as<double>( args["radiusMajor"], 1000.0 ),
            as<double>( args["radiusMinor"], 2000.0 ),
            as<double>( args["rotation"],       0.0 ),
            color );
    }
    return 0L;
}


CreateEllipseNodeCommand::CreateEllipseNodeCommand(const std::string& id,
                                                   double             lat_degrees,
                                                   double             lon_degrees,
                                                   double             radiusMajor_m,
                                                   double             radiusMinor_m,
                                                   double             rotation_degrees,
                                                   const osg::Vec4f&  color) :
_id         ( id ),
_lat        ( lat_degrees ),
_lon        ( lon_degrees ),
_radiusMajor( radiusMajor_m ),
_radiusMinor( radiusMinor_m ),
_rotation   ( rotation_degrees ),
_color      ( color )
{
    //nop
}


bool
CreateEllipseNodeCommand::operator ()( MapControl* map )
{
    osg::Group* annoGroup = getAnnotationGroup(map);
    if ( annoGroup )
    {
        Style style;
        style.getOrCreate<TextSymbol>()->fill()->color() = _color;

        annoGroup->addChild( new EllipseNode(
            map->getMapNode(),
            GeoPoint( SpatialReference::create("wgs84"), _lon, _lat, _alt ),
            Distance(_radiusMajor, Units::METERS),
            Distance(_radiusMinor, Units::METERS),
            Angle   (_rotation,    Units::DEGREES),
            style ) );
    }
    return annoGroup != 0L;
}


//------------------------------------------------------------------------


Command*
CreateRectangleNodeCommand::Factory::create(const std::string& cmd, const CommandArguments& args)
{
    if ( "createRectangleNode" == cmd )
    {
        osg::Vec4f color(
            as<float>(args["colorR"],1.0f),
            as<float>(args["colorG"],1.0f),
            as<float>(args["colorB"],1.0f),
            as<float>(args["colorA"],1.0f) );

        return new CreateRectangleNodeCommand(
            args["id"],
            as<double>( args["lat"],            0.0 ),
            as<double>( args["long"],           0.0 ),
            as<double>( args["width"],       1000.0 ),
            as<double>( args["height"],      2000.0 ),
            color );
    }
    return 0L;
}


CreateRectangleNodeCommand::CreateRectangleNodeCommand(const std::string& id,
                                                       double             lat_degrees,
                                                       double             lon_degrees,
                                                       double             width_m,
                                                       double             height_m,
                                                       const osg::Vec4f&  color) :
_id         ( id ),
_lat        ( lat_degrees ),
_lon        ( lon_degrees ),
_width      ( width_m ),
_height     ( height_m ),
_color      ( color )
{
    //nop
}


bool
CreateRectangleNodeCommand::operator ()( MapControl* map )
{
    osg::Group* annoGroup = getAnnotationGroup(map);
    if ( annoGroup )
    {
        Style style;
        style.getOrCreate<TextSymbol>()->fill()->color() = _color;

        annoGroup->addChild( new RectangleNode(
            map->getMapNode(),
            GeoPoint( SpatialReference::create("wgs84"), _lon, _lat, _alt ),
            Distance(_width, Units::METERS),
            Distance(_height, Units::METERS),
            style ) );
    }
    return annoGroup != 0L;
}


//------------------------------------------------------------------------



Command*
CreateFeatureNodeCommand::Factory::create(const std::string& cmd, const CommandArguments& args)
{
    if ( "createFeatureNode" == cmd )
    {
        osg::Vec4f color(
            as<float>(args["colorR"],1.0f),
            as<float>(args["colorG"],1.0f),
            as<float>(args["colorB"],1.0f),
            as<float>(args["colorA"],1.0f) );

        return new CreateFeatureNodeCommand(
            args["id"],
            args["geomWKT"],
            color,
            as<bool>( args["draped"], true ) );
    }
    return 0L;
}


CreateFeatureNodeCommand::CreateFeatureNodeCommand(const std::string& id,
                                                   const std::string& geomWKT,
                                                   const osg::Vec4f&  color,
                                                   bool               draped ) :
_id         ( id ),
_geomWKT    ( geomWKT ),
_color      ( color ),
_draped     ( draped )
{
    //nop
}


bool
CreateFeatureNodeCommand::operator ()( MapControl* map )
{
    osg::Group* annoGroup = getAnnotationGroup(map);
    if ( annoGroup )
    {
        Style style;
        style.getOrCreate<TextSymbol>()->fill()->color() = _color;

        osg::ref_ptr<Geometry> geom = GeometryUtils::geometryFromWKT( _geomWKT );

        if ( geom.valid() )
        {
            osg::ref_ptr<Feature> feature = new Feature(
                geom.get(),
                SpatialReference::create("wgs84"),
                style );

            annoGroup->addChild( new FeatureNode(
                map->getMapNode(),
                feature.get(),
                _draped ) );
        }
    }
    return annoGroup != 0L;
}


//------------------------------------------------------------------------



Command*
CreateLocalGeometryNodeCommand::Factory::create(const std::string& cmd, const CommandArguments& args)
{
    if ( "createLocalGeometryNode" == cmd )
    {
        osg::Vec4f color(
            as<float>(args["colorR"],1.0f),
            as<float>(args["colorG"],1.0f),
            as<float>(args["colorB"],1.0f),
            as<float>(args["colorA"],1.0f) );

        return new CreateLocalGeometryNodeCommand(
            args["id"],
            as<double>( args["lat"],  0.0 ),
            as<double>( args["long"], 0.0 ),
            as<double>( args["alt"],  0.0 ),
            args["geomWKT"],
            color,
            as<bool>( args["draped"], true ) );
    }
    return 0L;
}


CreateLocalGeometryNodeCommand::CreateLocalGeometryNodeCommand(const std::string& id,
                                                               double             lat_degrees,
                                                               double             lon_degrees,
                                                               double             alt_m,
                                                               const std::string& geomWKT,
                                                               const osg::Vec4f&  color,
                                                               bool               draped ) :
_id         ( id ),
_lat        ( lat_degrees ),
_lon        ( lon_degrees ),
_alt        ( alt_m ),
_geomWKT    ( geomWKT ),
_draped     ( draped ),
_color      ( color )
{
    //nop
}


bool
CreateLocalGeometryNodeCommand::operator ()( MapControl* map )
{
    osg::Group* annoGroup = getAnnotationGroup(map);
    if ( annoGroup )
    {
        Style style;
        style.getOrCreate<TextSymbol>()->fill()->color() = _color;

        osg::ref_ptr<Geometry> geom = GeometryUtils::geometryFromWKT( _geomWKT );

        if ( geom.valid() )
        {
            annoGroup->addChild( new LocalGeometryNode(
                map->getMapNode(),
                geom.get(),
                style,
                _draped ) );
        }
    }
    return annoGroup != 0L;
}


//------------------------------------------------------------------------


Command*
SetAnnotationNodePositionCommand::Factory::create(const std::string& cmd, const CommandArguments& args)
{
    if ( "setAnnotationNodePosition" == cmd )
    {
        return new SetAnnotationNodePositionCommand(
            args["id"],
            as<double>( args["lat"],  0.0 ),
            as<double>( args["long"], 0.0 ),
            as<double>( args["alt"],  0.0 ) );
    }
    return 0L;
}


SetAnnotationNodePositionCommand::SetAnnotationNodePositionCommand(const std::string& id,
                                                                   double             lat_degrees,
                                                                   double             lon_degrees,
                                                                   double             alt_m ) :
_id         ( id ),
_lat        ( lat_degrees ),
_lon        ( lon_degrees ),
_alt        ( alt_m )
{
    //nop
}


bool
SetAnnotationNodePositionCommand::operator ()( MapControl* map )
{
    osg::Group* annoGroup = getAnnotationGroup(map);
    if ( annoGroup )
    {
        osg::Node* node = findNamedNode( _id, annoGroup );
        PositionedAnnotationNode* pn = dynamic_cast<PositionedAnnotationNode*>( node );
        if ( pn )
        {
            pn->setPosition( GeoPoint(SpatialReference::create("wgs84"), _lon, _lat, _alt) );
            return true;
        }
    }

    return false;
}


//------------------------------------------------------------------------


Command*
SetAnnotationNodeVisibilityCommand::Factory::create(const std::string& cmd, const CommandArguments& args)
{
    if ( "setAnnotationNodePosition" == cmd )
    {
        return new SetAnnotationNodeVisibilityCommand(
            args["id"],
            as<bool>( args["visible"], true ) );
    }
    return 0L;
}


SetAnnotationNodeVisibilityCommand::SetAnnotationNodeVisibilityCommand(const std::string& id,
                                                                       bool               visible) :
_id     ( id ),
_visible( visible )
{
    //nop
}


bool
SetAnnotationNodeVisibilityCommand::operator ()( MapControl* map )
{
    osg::Group* annoGroup = getAnnotationGroup(map);
    if ( annoGroup )
    {
        osg::Node* node = findNamedNode( _id, annoGroup );
        AnnotationNode* anno = dynamic_cast<AnnotationNode*>( node );
        if ( anno )
        {
            anno->setNodeMask( _visible ? ~0 : 0 );
            return true;
        }
    }

    return false;
}


//------------------------------------------------------------------------



Command*
RemoveAnnotationNodeCommand::Factory::create(const std::string& cmd, const CommandArguments& args)
{
    if ( "removeAnnotationNode" == cmd )
    {
        return new RemoveAnnotationNodeCommand( args["id"] );
    }
    return 0L;
}


RemoveAnnotationNodeCommand::RemoveAnnotationNodeCommand(const std::string& id) :
_id ( id )
{
    //nop
}


bool
RemoveAnnotationNodeCommand::operator ()( MapControl* map )
{
    osg::Group* annoGroup = getAnnotationGroup(map);
    if ( annoGroup )
    {
        osg::Node* node = findNamedNode( _id, annoGroup );
        if ( node )
        {
            annoGroup->removeChild( node );
            return true;
        }
    }

    return false;
}
