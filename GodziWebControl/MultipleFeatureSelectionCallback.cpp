#include <GodziWebControl/MultipleFeatureSelectionCallback>
#include <GodziWebControl/MapControl>

#include <osgEarth/Registry>
#include <osgEarth/Capabilities>
#include <osgEarth/JsonUtils>

#include <osg/Depth>

using namespace osgEarth;
using namespace osgEarth::Features;
using namespace GodziWebControl;


MultipleFeatureSelectionCallback::MultipleFeatureSelectionCallback(MapControl* map, bool multiselect)
: _map(map), _multiselect(multiselect)
{
}

MultipleFeatureSelectionCallback::~MultipleFeatureSelectionCallback()
{
  _map = 0L;
}

void
MultipleFeatureSelectionCallback::onHit( FeatureSourceIndexNode* index, FeatureID fid, const EventArgs& args )
{
    if (!_multiselect)
    {
        clearSelection(false);
    }

    if (!clearSelection(fid))
    {
        OE_WARN << "SELECTING: " << fid << std::endl;

        FeatureDrawSet& drawSet = index->getDrawSet(fid);
        if ( !drawSet.empty() )
        {
            osg::Group* container = 0L;
            osg::Group* group = new osg::Group();
            osg::Geode* geode = 0L;

            OE_DEBUG << "Slices = " << drawSet.slices().size() << std::endl;

            for( FeatureDrawSet::DrawableSlices::iterator d = drawSet.slices().begin(); d != drawSet.slices().end(); ++d )
            {
                FeatureDrawSet::DrawableSlice& slice = *d;
                osg::Geometry* featureGeom = slice.drawable->asGeometry();

                osg::Geometry* highlightGeom = new osg::Geometry( *featureGeom, osg::CopyOp::SHALLOW_COPY );
                osg::Vec4Array* highlightColor = new osg::Vec4Array(1);
                (*highlightColor)[0] = osg::Vec4f(0,1,1,0.5);
                highlightGeom->setColorArray(highlightColor);
                highlightGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
                highlightGeom->setPrimitiveSetList( d->primSets );

                if ( !geode )
                {
                    geode = new osg::Geode();
                    group->addChild( geode );
                }

                geode->addDrawable(highlightGeom);

                if ( !container )
                {
                    // establishes a container for the highlight geometry.
                    osg::Geode* featureGeode = dynamic_cast<osg::Geode*>( featureGeom->getParent(0) );
                    container = featureGeode->getParent(0);
                    if ( featureGeom->getStateSet() )
                        geode->getOrCreateStateSet()->merge( *featureGeom->getStateSet() );
                }
            }

            for( FeatureDrawSet::Nodes::iterator n = drawSet.nodes().begin(); n != drawSet.nodes().end(); ++n )
            {
                group->addChild( *n );
                if ( !container )
                    container = (*n)->getParent(0);
            }

            osg::StateSet* sset = group->getOrCreateStateSet();

            // set up to overwrite the real geometry:
            sset->setAttributeAndModes( new osg::Depth(osg::Depth::LEQUAL,0,1,false), osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE );
            sset->setRenderBinDetails( 42, "DepthSortedBin" );

            // turn off texturing:
            for( int ii = 0; ii < Registry::instance()->getCapabilities().getMaxFFPTextureUnits(); ++ii )
            {
                sset->setTextureMode( ii, GL_TEXTURE_2D, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE );
                sset->setTextureMode( ii, GL_TEXTURE_3D, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE );
                //sset->setTextureMode( ii, GL_TEXTURE_RECTANGLE, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE );
                //sset->setTextureMode( ii, GL_TEXTURE_CUBE_MAP, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE );
            }

            sset->setMode( GL_BLEND,    osg::StateAttribute::ON  | osg::StateAttribute::OVERRIDE );
            sset->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE );

            container->addChild( group );

            Selection selection;
            selection._index     = index;
            selection._fid       = fid;
            selection._group     = group;

            _selections.insert( selection );
        }
    }

    postSelectionEvent();
}

void
MultipleFeatureSelectionCallback::onMiss( const EventArgs& args )
{
    if (!_multiselect)
    {
        clearSelection();
    }
}

void
MultipleFeatureSelectionCallback::clearSelection(bool postEvent)
{
    for( SelectionSet::const_iterator i = _selections.begin(); i != _selections.end(); ++i )
    {
        const Selection& selection = *i;
        osg::ref_ptr<osg::Group> safeGroup = selection._group.get();
        if ( safeGroup.valid() && safeGroup->getNumParents() > 0 )
        {
            osg::Group* parent = safeGroup->getParent(0);
            if ( parent ) 
                parent->removeChild( safeGroup.get() );
        }
    }
    _selections.clear();

    if (postEvent)
        postSelectionEvent();
}

bool
MultipleFeatureSelectionCallback::clearSelection(FeatureID fid)
{
    OE_WARN << "CLEARING: " << fid << std::endl;
    SelectionSet::const_iterator i;
    for( i = _selections.begin(); i != _selections.end(); ++i )
    {
        if (i->_fid == fid)
            break;
    }

    if (i != _selections.end())
    {
        OE_WARN << "CLEARING: " << fid << " FOUND!" << std::endl;
        const Selection& selection = *i;
        osg::ref_ptr<osg::Group> safeGroup = selection._group.get();
        if ( safeGroup.valid() && safeGroup->getNumParents() > 0 )
        {
            osg::Group* parent = safeGroup->getParent(0);
            if ( parent ) 
                parent->removeChild( safeGroup.get() );
        }

        _selections.erase(i);
        return true;
    }

    OE_WARN << "CLEARING: " << fid << " NOT FOUND." << std::endl;
    return false;
}

void
MultipleFeatureSelectionCallback::postSelectionEvent()
{
    if (_map.valid())
    {
        Json::Value root( Json::arrayValue );
        unsigned index = 0;

        for( SelectionSet::const_iterator i = _selections.begin(); i != _selections.end(); ++i )
        {
            const Selection& selection = *i;

            Json::Value selectionRoot;

            std::string fidStr = Stringify() << selection._fid;
            selectionRoot["fid"] = fidStr;

            Json::Value attributes;
            if ( selection._index.valid() && selection._index->getFeatureSource() )
            {
                const Feature* f;
                if ( selection._index->getFeature(selection._fid, f) )
                {
                    const AttributeTable& attrs = f->getAttrs();
                    for( AttributeTable::const_iterator i = attrs.begin(); i != attrs.end(); ++i)
                    {
                        attributes[i->first] = i->second.getString();
                    }
                }
            }    
            selectionRoot["attributes"] = attributes;
            root[index++] = selectionRoot;
        }


        OE_WARN << "POSTING: " << index << " features!" << std::endl;

        Json::FastWriter writer;
        _map->postEvent("", "onfeatureselect", writer.write(root));
    }
}