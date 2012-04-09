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
#include <osgEarthSymbology/SLD>
#include <osgEarthSymbology/CssUtils>

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
            root->addChild( annos );
        }

        return annos;
    }


    GeoPoint getLocation( const CommandArguments& args )
    {
        return GeoPoint(
            SpatialReference::create("wgs84"),
            as<double>( args["longitude"], 0.0 ),
            as<double>( args["latitude"],  0.0 ),
            as<double>( args["altitude"],  0.0 ) );
    }


    void getProp( const Config& conf, std::ostream& buf )
    {
        const std::string& name = conf.key();
        const std::string& val  = conf.value();

        if ( name.size() >= 2 && name[0] == '\"' && name[name.size()-1] == '\"' )
            buf << name.substr(1, name.size()-2);
        else
            buf << name;

        buf << ":";

        if ( val.size() >= 2 && val[0] == '\"' && val[val.size()-1] == '\"' )
            buf << val.substr(1, val.size()-2);
        else
            buf << val;

        buf << ";";
    }


    Style getStyle( const CommandArguments& args )
    {
        // convert from JSON-encoded style to CSS style. Subtley different.
        Config conf;
        conf.fromJSON( args["style"] );

        std::stringstream buf;
        buf << "default { ";

        if ( conf.children().size() > 0 )
        {
            for( ConfigSet::const_iterator i = conf.children().begin(); i != conf.children().end(); ++i )
            {
                getProp( *i, buf );
            }
        }
        else
        {
            getProp( conf, buf );
        }
        buf << " }";
        std::string css;
        css = buf.str();

        // now parse the CSS.
        ConfigSet styleSet;
        CssUtils::readConfig( css, "", styleSet );
        Style result;
        SLDReader::readStyleFromCSSParams( styleSet.front(), result );

        return result;
    }        


    Geometry* getGeometry( const CommandArguments& args )
    {
        return GeometryUtils::geometryFromWKT( args["geomWKT"] );
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

#undef  LC
#define LC "[CreateLabelNodeCommand] "

Command*
CreateLabelNodeCommand::Factory::create(const std::string& cmd, const CommandArguments& args)
{
    if ( "createLabelNode" == cmd )
    {
        return new CreateLabelNodeCommand(
            args["id"],
            getLocation( args ),
            args["text"],
            getStyle( args ) );
    }
    return 0L;
}


CreateLabelNodeCommand::CreateLabelNodeCommand(const std::string& id,
                                               const GeoPoint&    location,
                                               const std::string& text,
                                               const Style&       style ) :
_id      ( id ),
_location( location ),
_text    ( text ),
_style   ( style )
{
    //nop
}

bool
CreateLabelNodeCommand::operator ()( MapControl* map )
{
    osg::Group* annoGroup = getAnnotationGroup(map);
    if ( annoGroup )
    {
        AnnotationNode* anno = new LabelNode(
            map->getMapNode(),
            _location,
            _text,
            _style );

        anno->setName( _id );
        Decluttering::setEnabled( anno->getOrCreateStateSet(), true );
        annoGroup->addChild( anno );
    }
    return annoGroup != 0L;
}


//------------------------------------------------------------------------

#undef  LC
#define LC "[CreatePlaceNodeCommand] "

Command*
CreatePlaceNodeCommand::Factory::create(const std::string& cmd, const CommandArguments& args)
{
    if ( "createPlaceNode" == cmd )
    {
        return new CreatePlaceNodeCommand(
            args["id"],
            getLocation( args ),
            args["text"],
            args["iconURI"],
            getStyle( args ) );
    }
    return 0L;
}


CreatePlaceNodeCommand::CreatePlaceNodeCommand(const std::string& id,
                                               const GeoPoint&    location,
                                               const std::string& text,
                                               const std::string& iconURI,
                                               const Style&       style ) :
_id      ( id ),
_location( location ),
_text    ( text ),
_iconURI ( iconURI ),
_style   ( style )
{
    //nop
}

bool
CreatePlaceNodeCommand::operator ()( MapControl* map )
{
    osg::Group* annoGroup = getAnnotationGroup(map);
    if ( annoGroup )
    {
        ReadResult r = URI(_iconURI).readImage();
        osg::Image* icon = r.getImage();

        if ( r.failed() )
        {
            OE_WARN << LC << "Loading icon [" << _iconURI << "] error: " << r.getResultCodeString(r.code()) << std::endl;
        }

        AnnotationNode* anno = new PlaceNode(map->getMapNode(), _location, icon, _text, _style);

        anno->setName( _id );
        Decluttering::setEnabled( anno->getOrCreateStateSet(), true );
        annoGroup->addChild( anno );
    }
    return annoGroup != 0L;
}


//------------------------------------------------------------------------

#undef  LC
#define LC "[CreateCircleNodeCommand] "

Command*
CreateCircleNodeCommand::Factory::create(const std::string& cmd, const CommandArguments& args)
{
    if ( "createCircleNode" == cmd )
    {
        return new CreateCircleNodeCommand(
            args["id"],
            getLocation( args ),
            Distance( as<double>(args["radius"], 10000.0), Units::METERS ),
            getStyle( args ) );
    }
    return 0L;
}


CreateCircleNodeCommand::CreateCircleNodeCommand(const std::string& id,
                                                 const GeoPoint&    location,
                                                 const Distance&    radius,
                                                 const Style&       style ) :
_id      ( id ),
_location( location ),
_radius  ( radius ),
_style   ( style )
{
    //nop
}

bool
CreateCircleNodeCommand::operator ()( MapControl* map )
{
    osg::Group* annoGroup = getAnnotationGroup(map);
    if ( annoGroup )
    {
        //OE_DEBUG << LC << "Creating circle node, lat = " << _lat << ", lon = " << _lon << ", _alt = " << alt << ", radius = " << _radius << std::endl;

        AnnotationNode* anno = new CircleNode(map->getMapNode(), _location, _radius, _style);

        anno->setName( _id );
        annoGroup->addChild( anno );
    }
    return annoGroup != 0L;
}


//------------------------------------------------------------------------


Command*
CreateEllipseNodeCommand::Factory::create(const std::string& cmd, const CommandArguments& args)
{
    if ( "createEllipseNode" == cmd )
    {
        return new CreateEllipseNodeCommand(
            args["id"],
            getLocation( args ),
            Distance( as<double>(args["radiusMajor"], 1000.0), Units::METERS ),
            Distance( as<double>(args["radiusMinor"], 2000.0), Units::METERS ),
            Angle   ( as<double>(args["rotation"],       0.0), Units::DEGREES ),
            getStyle( args ) );
        
    }
    return 0L;
}


CreateEllipseNodeCommand::CreateEllipseNodeCommand(const std::string& id,
                                                   const GeoPoint&    location,
                                                   const Distance&    radiusMajor,
                                                   const Distance&    radiusMinor,
                                                   const Angle&       rotation,
                                                   const Style&       style ) :
_id         ( id ),
_location   ( location ),
_radiusMajor( radiusMajor ),
_radiusMinor( radiusMinor ),
_rotation   ( rotation ),
_style      ( style )
{
    //nop
}


bool
CreateEllipseNodeCommand::operator ()( MapControl* map )
{
    osg::Group* annoGroup = getAnnotationGroup(map);
    if ( annoGroup )
    {
        AnnotationNode* anno = new EllipseNode(
            map->getMapNode(),
            _location,
            _radiusMajor,
            _radiusMinor,
            _rotation,
            _style );

        anno->setName( _id );
        annoGroup->addChild( anno );
    }
    return annoGroup != 0L;
}


//------------------------------------------------------------------------


Command*
CreateRectangleNodeCommand::Factory::create(const std::string& cmd, const CommandArguments& args)
{
    if ( "createRectangleNode" == cmd )
    {
        return new CreateRectangleNodeCommand(
            args["id"],
            getLocation( args ),
            Distance( as<double>( args["width"],       10000.0 ), Units::METERS ),
            Distance( as<double>( args["height"],      20000.0 ), Units::METERS ),
            getStyle( args ),
            as<bool>(args["draped"], true) );
    }
    return 0L;
}


CreateRectangleNodeCommand::CreateRectangleNodeCommand(const std::string& id,
                                                       const GeoPoint&    location,
                                                       const Distance&    width,
                                                       const Distance&    height,
                                                       const Style&       style,
                                                       bool               draped) :
_id         ( id ),
_location   ( location ),
_width      ( width ),
_height     ( height ),
_style      ( style ),
_draped     ( draped )
{
    //nop
}


bool
CreateRectangleNodeCommand::operator ()( MapControl* map )
{
    osg::Group* annoGroup = getAnnotationGroup(map);
    if ( annoGroup )
    {
        AnnotationNode* anno = new RectangleNode(
            map->getMapNode(),
            _location,
            _width,
            _height,
            _style,
            _draped );

        anno->setName( _id );
        annoGroup->addChild( anno );
    }
    return annoGroup != 0L;
}


//------------------------------------------------------------------------



Command*
CreateFeatureNodeCommand::Factory::create(const std::string& cmd, const CommandArguments& args)
{
    if ( "createFeatureNode" == cmd )
    {
        return new CreateFeatureNodeCommand(
            args["id"],
            getGeometry( args ),
            getStyle( args ),
            as<bool>( args["draped"], true ) );
    }
    return 0L;
}


CreateFeatureNodeCommand::CreateFeatureNodeCommand(const std::string& id,
                                                   Geometry*          geom,
                                                   const Style&       style,
                                                   bool               draped ) :
_id         ( id ),
_geom       ( geom ),
_style      ( style ),
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
        if ( _geom.valid() )
        {
            osg::ref_ptr<Feature> feature = new Feature(
                _geom.get(),
                SpatialReference::create("wgs84"),
                _style );

            AnnotationNode* anno = new FeatureNode(
                map->getMapNode(),
                feature.get(),
                _draped );

            anno->setName( _id );
            annoGroup->addChild( anno );
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
        return new CreateLocalGeometryNodeCommand(
            args["id"],
            getLocation( args ),
            getGeometry( args ),
            getStyle( args ),
            as<bool>( args["draped"], true ) );
    }
    return 0L;
}


CreateLocalGeometryNodeCommand::CreateLocalGeometryNodeCommand(const std::string& id,
                                                               const GeoPoint&    location,
                                                               Geometry*          geom,
                                                               const Style&       style,
                                                               bool               draped ) :
_id         ( id ),
_location   ( location ),
_geom       ( geom ),
_style      ( style ),
_draped     ( draped )
{
    //nop
}


bool
CreateLocalGeometryNodeCommand::operator ()( MapControl* map )
{
    osg::Group* annoGroup = getAnnotationGroup(map);
    if ( annoGroup )
    {
        if ( _geom.valid() )
        {
            AnnotationNode* anno = new LocalGeometryNode(
                map->getMapNode(),
                _geom.get(),
                _style,
                _draped );

            anno->setName( _id );
            annoGroup->addChild( anno );
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
            getLocation( args ) );
    }
    return 0L;
}


SetAnnotationNodePositionCommand::SetAnnotationNodePositionCommand(const std::string& id,
                                                                   const GeoPoint&    location ) :
_id         ( id ),
_location   ( location )
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
            pn->setPosition( _location );
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
