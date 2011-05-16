#include <GodziWebControl/Selection>
#include <GodziWebControl/Utils>
#include <GodziWebControl/MapControl>
#include <osg/BlendFunc>
#include <osg/Material>
#include <osgGA/GUIEventHandler>

using namespace GodziWebControl;


#define onNoMatterWhat \
    osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED


struct FlashHandler : public osgGA::GUIEventHandler
{
    FlashHandler() { }

    bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
    {
        if ( ea.getEventType() == osgGA::GUIEventAdapter::FRAME )
        {
            if ( _flashNode.valid() && (ea.getTime() - _t_start_s) >= _duration_s )
            {
                endFlash();
            }
        }
        return false;
    }

    void flash( osg::Node* node, double duration_s )
    {
        endFlash();

        _savedStateSet = node->getStateSet();

        osg::StateSet* newStateSet = node->getStateSet()?
            static_cast<osg::StateSet*>( node->getStateSet()->clone( osg::CopyOp::DEEP_COPY_ALL )) :
            new osg::StateSet();

        // set up a flash material (lighting must be on)
        osg::Material* material = new osg::Material();
        material->setEmission( osg::Material::FRONT_AND_BACK, osg::Vec4f( .5, .5, 1, 1.0f ) );
        newStateSet->setAttributeAndModes( material, onNoMatterWhat );
        newStateSet->setMode( GL_LIGHTING, onNoMatterWhat );    

        _flashNode = node;
        _flashNode->setStateSet( newStateSet );
        _t_start_s = osg::Timer::instance()->time_s();
        _duration_s = duration_s;
    }

    void endFlash()
    {
        if ( _flashNode.valid() )
        {
            _flashNode->setNodeMask( _savedNodeMask );
            _flashNode->setStateSet( _savedStateSet.valid()? _savedStateSet.get() : 0L );
            _flashNode = 0L;
            _savedStateSet = 0L;
        }
    }

    double _t_start_s, _duration_s;
    osg::ref_ptr<osg::Node> _flashNode;
    unsigned int _savedNodeMask;
    osg::ref_ptr<osg::StateSet> _savedStateSet;
};


SelectionSet::SelectionSet( osg::Camera* camera ) :
_highlighted( false ),
_nodeMask( ~0 ),
_camera(camera)
{
    _isolateGroup = new osg::Group();
    _isolateGroup->addDescription( "GodziWebControl::SelectionSet isolation node" );
}

SelectionSet::~SelectionSet()
{
    clear();
}

void
SelectionSet::flash( osg::Node* node, double duration_s )
{
    if ( _camera.valid() )
    {
        if ( !_flashHandler.valid() )
        {
            _flashHandler = new FlashHandler();
            _camera->addEventCallback( static_cast<FlashHandler*>(_flashHandler.get()) );
        }

        static_cast<FlashHandler*>(_flashHandler.get())->flash( node, duration_s );
    }
}

void
SelectionSet::add( osg::Node* node ) 
{
    NodeBackup* backup = new NodeBackup();
    backup->_stateSet = node->getStateSet()? 
        static_cast<osg::StateSet*>(node->getStateSet()->clone( osg::CopyOp::DEEP_COPY_ALL )) :
        NULL;
    backup->_nodeMask = node->getNodeMask();

    _nodes[ node ] = backup;
    _isolateGroup->addChild( node );

    if ( _highlighted )
        setHighlighted( node, true );

    node->setNodeMask( _nodeMask );
}

void
SelectionSet::remove( osg::Node* node )
{
    NodeSet::iterator i = _nodes.find( node );
    if ( i != _nodes.end() )
    {
        reset( node );
        _nodes.erase( i );
        _isolateGroup->removeChild( node );
    }
}

void
SelectionSet::toggle( osg::Node* node, bool selected )
{
    if ( node )
    {
        NodeSet::iterator i = _nodes.find(node);
        if ( selected && i == _nodes.end() )
        {
            add( node );
        }
        else if ( !selected && i != _nodes.end() )
        {
            remove( node );
        }
    }
}
    
void
SelectionSet::clear()
{
    for( NodeSet::iterator i = _nodes.begin(); i != _nodes.end(); i++ )
    {
        reset( i->first.get() );
    }

    _nodes.clear();
    _isolateGroup->removeChildren( 0, _isolateGroup->getNumChildren() );
}

void
SelectionSet::setHighlighted( bool enabled )
{
    if ( _highlighted != enabled )
    {
        for( NodeSet::iterator i = _nodes.begin(); i != _nodes.end(); i++ )
        {
            setHighlighted( i->first.get(), enabled );
        }
    }
}

void
SelectionSet::setNodeMask( unsigned int mask )
{
    for( NodeSet::iterator i = _nodes.begin(); i != _nodes.end(); i++ )
    {
        i->first->setNodeMask( mask );
    }
}

void
SelectionSet::reset( osg::Node* node )
{
    NodeSet::iterator i = _nodes.find( node );
    if ( i != _nodes.end() )
    {
        node->setNodeMask( i->second->_nodeMask );

        node->setStateSet( i->second->_stateSet?
                static_cast<osg::StateSet*>( i->second->_stateSet->clone( osg::CopyOp::DEEP_COPY_ALL ) ) :
                NULL );
    }

    if ( _isolateNodeToReplace.valid() )
    {
        toggleIsolate( _isolateNodeToReplace.get(), false );
    }
}

void
SelectionSet::toggleIsolate( osg::Node* nodeToReplace, bool isolate )
{
    if ( !_isolateNodeToReplace.valid() && isolate )
    {
        _isolateNodeToReplace = nodeToReplace;
        _isolateNodeMask = nodeToReplace->getNodeMask();
        _isolateNodeToReplace->setNodeMask( 0 );

        if ( _isolateNodeToReplace->getNumParents() > 0 )
        {
            _isolateNodeToReplace->getParent(0)->addChild( _isolateGroup.get() );
        }
    }
    else if ( _isolateNodeToReplace.valid() && !isolate )
    {
        if ( _isolateNodeToReplace->getNumParents() > 0 )
        {
            _isolateNodeToReplace->getParent(0)->removeChild( _isolateGroup.get() );
        }
        _isolateNodeToReplace->setNodeMask( _isolateNodeMask );
        _isolateNodeToReplace = 0L;
    }
}

void
SelectionSet::setHighlighted( osg::Node* node, bool enabled )
{
    osg::StateSet* s = node->getOrCreateStateSet();

    if ( enabled )
    {
        // set up a highlighting material (lighting must be on)
        osg::Material* material = new osg::Material();
        material->setEmission( osg::Material::FRONT_AND_BACK, osg::Vec4f( 1, 0, 0, 1.0f ) );
        s->setAttributeAndModes( material, onNoMatterWhat );
        s->setMode( GL_LIGHTING, onNoMatterWhat );
    }
    else
    {
        NodeSet::iterator i = _nodes.find(node);
        if ( i != _nodes.end() )
        {
            node->setStateSet( i->second->_stateSet?
                static_cast<osg::StateSet*>( i->second->_stateSet->clone( osg::CopyOp::DEEP_COPY_ALL ) ) :
                NULL );
        }
    }
}

ToggleObjectSelectedCommand::ToggleObjectSelectedCommand( const CommandArguments& args )
{
    _description = args["description"];
    _name = args["name"];
    _selected = Utils::as<bool>( args["selected"], true );
}

bool
ToggleObjectSelectedCommand::operator()( MapControl* mapControl )
{
    GetObjectInfoVisitor visitor( _name, _description );
    visitor.setNodeMaskOverride( ~0 );
    mapControl->getRoot()->accept( visitor );
    if ( visitor._result.valid() )
    {
        mapControl->getSelectionSet()->toggle( visitor._result.get(), _selected );
        
        osg::notify(osg::NOTICE) << "ToggleObjectSelectedCommand: object [" << _name << "/" << _description << "] toggled"
            << std::endl;
    }
    else
    {
        osg::notify(osg::NOTICE) << "ToggleObjectSelectedCommand: object [" << _name << "/" << _description << "] not found"
            << std::endl;
    }
    return true;
}

ClearSelectionCommand::ClearSelectionCommand( const CommandArguments& args )
{
    //nop
}

bool
ClearSelectionCommand::operator ()( MapControl* mapControl )
{
    mapControl->getSelectionSet()->clear();
    return true;
}

HighlightSelectionCommand::HighlightSelectionCommand( const CommandArguments& args )
{
    _enabled = Utils::as<bool>(args["enabled"], true );
}

bool
HighlightSelectionCommand::operator ()( MapControl* mapControl )
{
    mapControl->getSelectionSet()->setHighlighted( _enabled );
    return true;
}

ToggleIsolateSelectionCommand::ToggleIsolateSelectionCommand( const CommandArguments& args )
{
    _name = args["name"];
    _description = args["description"];
    _isolate = args["isolate"] == "true";
}

bool
ToggleIsolateSelectionCommand::operator()( MapControl* mapControl )
{
    if ( _isolate )
    {
        GetObjectInfoVisitor visitor( _name, _description );
        visitor.setNodeMaskOverride( ~0 );
        mapControl->getRoot()->accept( visitor );
        if ( visitor._result.valid() )
        {
            mapControl->getSelectionSet()->toggleIsolate( visitor._result.get(), true );
        }
    }
    else
    {
        mapControl->getSelectionSet()->toggleIsolate( NULL, false );
    }
    return true;
}

FlashObjectCommand::FlashObjectCommand( const CommandArguments& args )
{
    _tag = args["name"];
    if ( _tag.empty() ) _tag = args["description"];
    if ( _tag.empty() ) _tag = args["tag"];
    _duration_s = Utils::as<double>( args["duration"], 1.0 );
}

bool
FlashObjectCommand::operator()( MapControl* mapControl )
{
    GetObjectInfoVisitor visitor( _tag, _tag );
    visitor.setNodeMaskOverride( ~0 );
    mapControl->getRoot()->accept( visitor );
    if ( visitor._result.valid() )
    {
        mapControl->getSelectionSet()->flash( visitor._result.get(), _duration_s );
    }
    return true;
}
