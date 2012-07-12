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
#include <osgEarthAnnotation/AnnotationEditing>
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

#define ANNO_GROUP_NAME        "_annotations"
#define ANNO_EDITOR_GROUP_NAME "_annotationEditors"

namespace
{
    osg::Group* getNamedGroup(MapControl* map, const std::string& name)
    {
        osg::Group* root = map->getRoot();

        FindNamedNodeVisitor nv( name );
        root->accept(nv);

        osg::Group* group = 0L;
        if ( nv._result )
            group = nv._result->asGroup();

        if ( !group )
        {
            group = new osg::Group();
            group->setName( name );
            root->addChild( group );
        }

        return group;
    }

    osg::Group* getAnnotationGroup(MapControl* map)
    {
        return getNamedGroup(map, ANNO_GROUP_NAME);
    }
    
    osg::Group* getAnnotationEditorGroup(MapControl* map)
    {
        return getNamedGroup(map, ANNO_EDITOR_GROUP_NAME);
    }


    GeoPoint getLocation( const CommandArguments& args )
    {
        return GeoPoint(
            SpatialReference::create("wgs84"),
            as<double>( args["longitude"], 0.0 ),
            as<double>( args["latitude"],  0.0 ),
            as<double>( args["altitude"],  0.0 ),
            osgEarth::AltitudeMode::ALTMODE_ABSOLUTE);
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
    map->addCommandFactory( new GetAnnotationNodeBoundsCommand::Factory );
    map->addCommandFactory( new RemoveAnnotationNodeCommand::Factory );
    map->addCommandFactory( new ToggleAnnotationNodeEditorCommand::Factory );
    map->addCommandFactory( new SetAnnotationColorsCommand::Factory );
    map->addCommandFactory( new GetAnnotationColorsCommand::Factory );
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
    if ( "setAnnotationNodeVisibility" == cmd )
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
GetAnnotationNodeBoundsCommand::Factory::create(const std::string& cmd, const CommandArguments& args)
{
    if ( "getAnnotationBounds" == cmd )
    {
        return new GetAnnotationNodeBoundsCommand( args["id"] );
    }
    return 0L;
}


GetAnnotationNodeBoundsCommand::GetAnnotationNodeBoundsCommand(const std::string& id) :
_id( id )
{
    //nop
}


bool
GetAnnotationNodeBoundsCommand::operator ()( MapControl* map )
{
    osg::Group* annoGroup = getAnnotationGroup(map);
    if ( annoGroup )
    {
        osg::Node* node = findNamedNode( _id, annoGroup );
        AnnotationNode* anno = dynamic_cast<AnnotationNode*>( node );
        if ( anno )
        {
            const osg::BoundingSphere& bs = anno->getBound();
            osg::Vec3d center = bs.center();

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


//------------------------------------------------------------------------


#undef  LC
#define LC "[ToggleAnnotationNodeEditorCommand] "

Command*
ToggleAnnotationNodeEditorCommand::Factory::create(const std::string& cmd, const CommandArguments& args)
{
    if ( "toggleAnnotationNodeEditor" == cmd )
    {
        return new ToggleAnnotationNodeEditorCommand( 
            args["id"], 
            as<bool>(args["enabled"], true) );
    }
    return 0L;
}


ToggleAnnotationNodeEditorCommand::ToggleAnnotationNodeEditorCommand(const std::string& id,
                                                                     bool               enabled) :
_id     ( id ),
_enabled( enabled )
{
    //nop
}


bool
ToggleAnnotationNodeEditorCommand::operator ()( MapControl* map )
{
    OE_INFO << LC << "Toggle annotation editor (" << _enabled << ") id=" << _id << std::endl;

    osg::Group* annoEditorGroup = getAnnotationEditorGroup(map);
    if ( annoEditorGroup )
    {
        osg::Node* editor = findNamedNode( _id, annoEditorGroup );
        if ( editor && !_enabled )
        {
            annoEditorGroup->removeChild( editor );
        }
        else if ( !editor && _enabled )
        {
            // find the corresponding annotation:
            osg::Group* annoGroup = getAnnotationGroup(map);
            if ( annoGroup )
            {
                osg::Node* anno = findNamedNode( _id, annoGroup );
                if ( anno )
                {
                    OE_INFO << LC << "Found " << _id << ", creating editor.." << std::endl;

                    if ( dynamic_cast<CircleNode*>(anno) )
                    {
                        editor = new CircleNodeEditor( dynamic_cast<CircleNode*>(anno) );
                    }
                    else if ( dynamic_cast<EllipseNode*>(anno) )
                    {
                        editor = new EllipseNodeEditor( dynamic_cast<EllipseNode*>(anno) );
                    }
                    else if ( dynamic_cast<RectangleNode*>(anno) )
                    {
                        editor = new RectangleNodeEditor( dynamic_cast<RectangleNode*>(anno) );
                    }
                    else if ( dynamic_cast<LocalizedNode*>(anno) )
                    {
                        editor = new LocalizedNodeEditor( dynamic_cast<LocalizedNode*>(anno) );
                    }

                    //TODO: add other types to edit image overlay and featurenode, for example

                    if ( editor )
                    {
                        OE_INFO << LC << "Created editor of type " << typeid(*editor).name() << std::endl;
                        editor->setName( _id );
                        annoEditorGroup->addChild( editor );
                    }
                }   
            }
        }
    }

    return false;
}


//------------------------------------------------------------------------


#undef  LC
#define LC "[SetAnnotationColorsCommand] "

Command*
SetAnnotationColorsCommand::Factory::create(const std::string& cmd, const CommandArguments& args)
{
    if ( "setAnnotationColors" == cmd )
    {
        return new SetAnnotationColorsCommand( 
            args["id"], 
            args["fill"],
            args["stroke"],
            args["opacity"]);
    }
    return 0L;
}


SetAnnotationColorsCommand::SetAnnotationColorsCommand(const std::string& id, const std::string& fill, const std::string& stroke, const std::string& opacity) :
_id( id ),
_fill( fill ),
_stroke( stroke ),
_opacity( opacity )
{
    //nop
}


bool
SetAnnotationColorsCommand::operator ()( MapControl* map )
{
    OE_INFO << LC << "Set annotation colors: fill(" << _fill << ") stroke(" << _stroke << ") opacity(" << _opacity << "), id=" << _id << std::endl;

    if (_fill.length() < 6 && _stroke.length() < 6 && _opacity.length() != 2)
      return false;

    // find the annotation:
    osg::Group* annoGroup = getAnnotationGroup(map);
    if ( annoGroup )
    {
        osg::Node* anno = findNamedNode( _id, annoGroup );
        if ( anno )
        {
            if ( dynamic_cast<CircleNode*>(anno) )
            {
                CircleNode* annoNode = dynamic_cast<CircleNode*>(anno);
                Style newStyle = annoNode->getStyle();

                //Get the current opacity or default to "FF"
                std::string opacity = _opacity;
                if (opacity.length() != 2)
                {
                  opacity = "FF";

                  PolygonSymbol* polySymbol = newStyle.get<PolygonSymbol>();
                  if (polySymbol)
                  {
                    std::string current = osgEarth::Symbology::Color(polySymbol->fill()->color()).toHTML();
                    if (current.length() > 2)
                      opacity = current.substr(current.length() - 2, 2);
                  }
                }

                if (_fill.length() >= 6)
                {
                    newStyle.getOrCreate<PolygonSymbol>()->fill()->color() = osgEarth::Symbology::Color(_fill + opacity);
                }
                else if (_opacity.length() == 2)
                {
                    //If only an opacity was passed in apply it to the current color
                    std::string fill = osgEarth::Symbology::Color(newStyle.getOrCreate<PolygonSymbol>()->fill()->color()).toHTML();
                    fill.replace(fill.length() - 2, 2, _opacity);
                    newStyle.getOrCreate<PolygonSymbol>()->fill()->color() = osgEarth::Symbology::Color(fill);
                }

                if (_stroke.length() >= 6)
                    newStyle.getOrCreate<LineSymbol>()->stroke()->color() = osgEarth::Symbology::Color(_stroke);

                annoNode->setStyle(newStyle);
            }
            else if ( dynamic_cast<EllipseNode*>(anno) )
            {
                EllipseNode* annoNode = dynamic_cast<EllipseNode*>(anno);
                Style newStyle = annoNode->getStyle();

                //Get the current opacity or default to "FF"
                std::string opacity = _opacity;
                if (opacity.length() != 2)
                {
                  opacity = "FF";

                  PolygonSymbol* polySymbol = newStyle.get<PolygonSymbol>();
                  if (polySymbol)
                  {
                    std::string current = osgEarth::Symbology::Color(polySymbol->fill()->color()).toHTML();
                    if (current.length() > 2)
                      opacity = current.substr(current.length() - 2, 2);
                  }
                }

                if (_fill.length() >= 6)
                {
                    newStyle.getOrCreate<PolygonSymbol>()->fill()->color() = osgEarth::Symbology::Color(_fill + opacity);
                }
                else if (_opacity.length() == 2)
                {
                    //If only an opacity was passed in apply it to the current color
                    std::string fill = osgEarth::Symbology::Color(newStyle.getOrCreate<PolygonSymbol>()->fill()->color()).toHTML();
                    fill.replace(fill.length() - 2, 2, _opacity);
                    newStyle.getOrCreate<PolygonSymbol>()->fill()->color() = osgEarth::Symbology::Color(fill);
                }

                if (_stroke.length() >= 6)
                    newStyle.getOrCreate<LineSymbol>()->stroke()->color() = osgEarth::Symbology::Color(_stroke);

                annoNode->setStyle(newStyle);
            }
            else if ( dynamic_cast<RectangleNode*>(anno) )
            {
                RectangleNode* annoNode = dynamic_cast<RectangleNode*>(anno);
                Style newStyle = annoNode->getStyle();

                //Get the current opacity or default to "FF"
                std::string opacity = _opacity;
                if (opacity.length() != 2)
                {
                  opacity = "FF";

                  PolygonSymbol* polySymbol = newStyle.get<PolygonSymbol>();
                  if (polySymbol)
                  {
                    std::string current = osgEarth::Symbology::Color(polySymbol->fill()->color()).toHTML();
                    if (current.length() > 2)
                      opacity = current.substr(current.length() - 2, 2);
                  }
                }

                if (_fill.length() >= 6)
                {
                    newStyle.getOrCreate<PolygonSymbol>()->fill()->color() = osgEarth::Symbology::Color(_fill + opacity);
                }
                else if (_opacity.length() == 2)
                {
                    //If only an opacity was passed in apply it to the current color
                    std::string fill = osgEarth::Symbology::Color(newStyle.getOrCreate<PolygonSymbol>()->fill()->color()).toHTML();
                    fill.replace(fill.length() - 2, 2, _opacity);
                    newStyle.getOrCreate<PolygonSymbol>()->fill()->color() = osgEarth::Symbology::Color(fill);
                }

                if (_stroke.length() >= 6)
                    newStyle.getOrCreate<LineSymbol>()->stroke()->color() = osgEarth::Symbology::Color(_stroke);

                annoNode->setStyle(newStyle);
            } 
            else if ( dynamic_cast<FeatureNode*>(anno) )
            {
                FeatureNode* annoNode = dynamic_cast<FeatureNode*>(anno);
                osgEarth::Features::Feature* annoFeature = const_cast<osgEarth::Features::Feature*>(annoNode->getFeature());

                //Get the current opacity or default to "FF"
                std::string opacity = _opacity;
                if (opacity.length() != 2)
                {
                  opacity = "FF";

                  PolygonSymbol* polySymbol = annoFeature->style()->get<PolygonSymbol>();
                  if (polySymbol)
                  {
                    std::string current = osgEarth::Symbology::Color(polySymbol->fill()->color()).toHTML();
                    if (current.length() > 2)
                      opacity = current.substr(current.length() - 2, 2);
                  }
                }

                if (_fill.length() >= 6)
                {
                    annoFeature->style()->getOrCreate<PolygonSymbol>()->fill()->color() = osgEarth::Symbology::Color(_fill + opacity);
                }
                else if (_opacity.length() == 2)
                {
                    //If only an opacity was passed in apply it to the current color
                    std::string fill = osgEarth::Symbology::Color(annoFeature->style()->getOrCreate<PolygonSymbol>()->fill()->color()).toHTML();
                    fill.replace(fill.length() - 2, 2, _opacity);
                    annoFeature->style()->getOrCreate<PolygonSymbol>()->fill()->color() = osgEarth::Symbology::Color(fill);
                }

                if (_stroke.length() >= 6)
                    annoFeature->style()->getOrCreate<LineSymbol>()->stroke()->color() = osgEarth::Symbology::Color(_stroke);

                annoNode->setFeature(annoFeature);
            }
            //TODO: add other types???

        }   
    }

    return false;
}


//------------------------------------------------------------------------


#undef  LC
#define LC "[GetAnnotationColorsCommand] "

Command*
GetAnnotationColorsCommand::Factory::create(const std::string& cmd, const CommandArguments& args)
{
    if ( "getAnnotationColors" == cmd )
    {
        return new GetAnnotationColorsCommand( 
            args["id"]);
    }
    return 0L;
}


GetAnnotationColorsCommand::GetAnnotationColorsCommand(const std::string& id) :
_id( id )
{
    //nop
}


bool
GetAnnotationColorsCommand::operator ()( MapControl* map )
{
    OE_INFO << LC << "Get annotation colors, id=" << _id << std::endl;

    if (_id.length() <= 0)
      return false;

    // find the annotation:
    osg::Group* annoGroup = getAnnotationGroup(map);
    if ( annoGroup )
    {
        osg::Node* anno = findNamedNode( _id, annoGroup );
        if ( anno )
        {
            OE_INFO << LC << "Found " << _id << std::endl;

            osgEarth::Json::Value result;

            if ( dynamic_cast<CircleNode*>(anno) )
            {
                CircleNode* annoNode = dynamic_cast<CircleNode*>(anno);

                const osgEarth::Symbology::PolygonSymbol* polySymbol = annoNode->getStyle().get<PolygonSymbol>();
                if (polySymbol)
                {
                    std::string color = osgEarth::Symbology::Color(polySymbol->fill()->color()).toHTML();
                    result["fill"] = color.substr(0, 7);
                    result["opacity"] = polySymbol->fill()->color().a();
                }

                const osgEarth::Symbology::LineSymbol* lineSymbol = annoNode->getStyle().get<LineSymbol>();
                if (lineSymbol)
                {
                    std::string color = osgEarth::Symbology::Color(lineSymbol->stroke()->color()).toHTML();
                    result["stroke"] = color.substr(0, 7);
                }
            }
            else if ( dynamic_cast<EllipseNode*>(anno) )
            {
                EllipseNode* annoNode = dynamic_cast<EllipseNode*>(anno);

                const osgEarth::Symbology::PolygonSymbol* polySymbol = annoNode->getStyle().get<PolygonSymbol>();
                if (polySymbol)
                {
                    std::string color = osgEarth::Symbology::Color(polySymbol->fill()->color()).toHTML();
                    result["fill"] = color.substr(0, 7);
                    result["opacity"] = polySymbol->fill()->color().a();
                }

                const osgEarth::Symbology::LineSymbol* lineSymbol = annoNode->getStyle().get<LineSymbol>();
                if (lineSymbol)
                {
                    std::string color = osgEarth::Symbology::Color(lineSymbol->stroke()->color()).toHTML();
                    result["stroke"] = color.substr(0, 7);
                }
            }
            else if ( dynamic_cast<RectangleNode*>(anno) )
            {
                RectangleNode* annoNode = dynamic_cast<RectangleNode*>(anno);

                const osgEarth::Symbology::PolygonSymbol* polySymbol = annoNode->getStyle().get<PolygonSymbol>();
                if (polySymbol)
                {
                    std::string color = osgEarth::Symbology::Color(polySymbol->fill()->color()).toHTML();
                    result["fill"] = color.substr(0, 7);
                    result["opacity"] = polySymbol->fill()->color().a();
                }

                const osgEarth::Symbology::LineSymbol* lineSymbol = annoNode->getStyle().get<LineSymbol>();
                if (lineSymbol)
                {
                    std::string color = osgEarth::Symbology::Color(lineSymbol->stroke()->color()).toHTML();
                    result["stroke"] = color.substr(0, 7);
                }
            } 
            else if ( dynamic_cast<FeatureNode*>(anno) )
            {
                FeatureNode* annoNode = dynamic_cast<FeatureNode*>(anno);
                const osgEarth::Features::Feature* annoFeature = annoNode->getFeature();

                const osgEarth::Symbology::PolygonSymbol* polySymbol = annoFeature->style()->get<PolygonSymbol>();
                if (polySymbol)
                {
                    std::string color = osgEarth::Symbology::Color(polySymbol->fill()->color()).toHTML();
                    result["fill"] = color.substr(0, 7);
                    result["opacity"] = polySymbol->fill()->color().a();
                }

                const osgEarth::Symbology::LineSymbol* lineSymbol = annoFeature->style()->get<LineSymbol>();
                if (lineSymbol)
                {
                    std::string color = osgEarth::Symbology::Color(lineSymbol->stroke()->color()).toHTML();
                    result["stroke"] = color.substr(0, 7);
                }
            }
            //TODO: add other types???


            osgEarth::Json::FastWriter writer;
            setResult(writer.write(result));

            return true;
        }   
    }

    return false;
}
