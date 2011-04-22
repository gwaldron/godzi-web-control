#include <ReadyMapWebPlugin/GetObjectInfoCommand>

#include <ReadyMapWebPlugin/MapCommands>
#include <ReadyMapWebPlugin/Utils>

#include <osgEarth/Common>
#include <osgEarth/JsonUtils>
#include <osgEarth/MapNode>

#include <osgUtil/IntersectionVisitor>
#include <osgUtil/LineSegmentIntersector>
#include <osgSim/ShapeAttribute>

#include <iomanip>

using namespace ReadyMapWebPlugin;


Command*
GetObjectInfoCommand::Factory::create(const std::string& cmd, const CommandArguments& args)
{
    return "getObjectInfo" == cmd? new GetObjectInfoCommand( args ) : NULL;
}

GetObjectInfoCommand::GetObjectInfoCommand(const CommandArguments& args)
{
    _nodeName = args["name"];
    _description = args["description"];
    _mouseX = Utils::as<int>( args["mouseX"], -1 );
    _mouseY = Utils::as<int>( args["mouseY"], -1 );
    _searchNodeName = args["searchNode"];
}

bool
GetObjectInfoCommand::operator ()( ReadyMapWebPlugin::MapControl* mapControl )
{
    osg::notify(osg::NOTICE) << "GetObjectInfoCommand "
        << "name=" << _nodeName
        << ", description=" << _description
        << ", mouseX=" << _mouseX
        << ", mouseY=" << _mouseY
        << std::endl;

    osg::ref_ptr<osg::Node> resultNode;
    osg::Matrixd resultMatrix;
    osg::BoundingBox resultBBox;

    osg::ref_ptr<osg::Node> searchNode = mapControl->getRoot();

    if ( !_searchNodeName.empty() )
    {
        GetObjectInfoVisitor v( _nodeName, _description );
        // override node masks in order to traverse invisible nodes:
        v.setNodeMaskOverride( ~0 );
        mapControl->getRoot()->accept( v );
        if ( v._result.valid() )
            searchNode = v._result.get();
    }

    if ( _nodeName.empty() && _description.empty() )
    {
        osg::ref_ptr<osgUtil::LineSegmentIntersector> picker = new osgUtil::LineSegmentIntersector(
            osgUtil::Intersector::WINDOW, _mouseX, _mouseY );

        osgUtil::IntersectionVisitor iv( picker.get() );
        mapControl->getView()->getCamera()->accept( iv );
        //searchNode->accept( iv );
        if ( picker->containsIntersections() )
        {
            for( osgUtil::LineSegmentIntersector::Intersections::const_iterator k = picker->getIntersections().begin(); k != picker->getIntersections().end(); k++ )
            {
                std::string desc = k->nodePath.back()->getNumDescriptions() > 0?
                    k->nodePath.back()->getDescription(0) :
                    "";

                osg::notify(osg::NOTICE)
                    << "    Name=" << k->nodePath.back()->getName()
                    << ", Description=" << desc
                    << std::endl;

                // We only accept the first node with a description!!
                if ( !desc.empty() )
                {
                    resultNode = k->nodePath.back();
                    resultMatrix = *(k->matrix.get());
                    break;
                }
            }
        }
    }
    else
    {
        GetObjectInfoVisitor v( _nodeName, _description );
        // override node masks in order to traverse invisible nodes:
        v.setNodeMaskOverride( ~0 );
        //mapControl->getRoot()->accept( v );
        searchNode->accept( v );
        resultNode = v._result.get();
        resultMatrix = v._resultMatrix;
        resultBBox = v._bb;
    }

    // encode the results:
    osgEarth::Json::Value result;
    if ( resultNode.valid() )
    {
        result["name"] = resultNode->getName();
        if ( resultNode->getDescriptions().size() > 0 )
            result["description"] = resultNode->getDescription(0);

        osg::Vec3d centroid = resultNode->getBound().center() * resultMatrix;
        //osg::Vec3d centroid = v._bb.center();

        result["x"] = centroid.x();
        result["y"] = centroid.y();
        result["z"] = centroid.z();
        result["radius"] = resultNode->getBound().radius();

        //osg::Vec3d centroid2 = resultBBox.center();
        //result["bb_x"] = centroid2.x();
        //result["bb_y"] = centroid2.y();
        //result["bb_z"] = centroid2.z();

        // shape attributes, if present:
        if ( resultNode->getUserData() )
        {
            osgSim::ShapeAttributeList* list = dynamic_cast<osgSim::ShapeAttributeList*>( resultNode->getUserData() );
            if ( list )
            {
                osgEarth::Json::Value a = osgEarth::Json::Value( osgEarth::Json::arrayValue );
                a.resize( list->size() );
                for( int m=0; m < list->size(); m++ )
                {
                    a[m]["Name"] = (*list)[m].getName();
                    a[m]["Value"] =
                        (*list)[m].getType() == osgSim::ShapeAttribute::STRING? (*list)[m].getString() :
                        (*list)[m].getType() == osgSim::ShapeAttribute::DOUBLE? Utils::toString( (*list)[m].getDouble() ) :
                        (*list)[m].getType() == osgSim::ShapeAttribute::INTEGER? Utils::toString( (*list)[m].getInt() ) :
                        "";
                }
                result["attrs"] = a;
            }
        }

        osgEarth::MapNode* mapNode = findTopMostNodeOfType<osgEarth::MapNode>( mapControl->getRoot() );
        if (mapNode)
        {
            double lat, lon, h;  

            mapNode->getMap()->getProfile()->getSRS()->getEllipsoid()->convertXYZToLatLongHeight(
                centroid.x(), centroid.y(), centroid.z(), lat, lon, h );

            result["latitude"]  = osg::RadiansToDegrees( lat );
            result["longitude"] = osg::RadiansToDegrees( lon );
            result["altitude"]  = h;

            //osg::notify(osg::NOTICE)
            //    << std::setprecision(10)
            //    << "    Lat/Long/Alt = "
            //    << osg::RadiansToDegrees( lat ) << ", "
            //    << osg::RadiansToDegrees( lon ) << ", "
            //    << h << std::endl;
        }
    }

    osgEarth::Json::FastWriter writer;
    setResult(writer.write(result));

    return true;
}
