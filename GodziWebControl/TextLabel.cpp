#include <GodziWebControl/TextLabel>
#include <GodziWebControl/MapControl>
#include <GodziWebControl/NodeMasks>
#include <GodziWebControl/Utils>
#include <osgEarth/Common>

#include <osg/ClusterCullingCallback>

#include <sstream>

using namespace osgEarth;
using namespace GodziWebControl;


Command* UpdateTextLabelCommand::Factory::create(const std::string& command, const CommandArguments& args)
{
    if ("updateTextLabel" == command)
    {
        std::string id       = args["id"];
        double latitude      = as<double>(args["latitude"], 0.0);
        double longitude     = as<double>(args["longitude"], 0.0);
        double height        = as<double>(args["height"], 0.0);

        float colorR    = as<float>(args["colorR"],1.0f);
        float colorG    = as<float>(args["colorG"],1.0f);
        float colorB    = as<float>(args["colorB"],1.0f);
        float colorA    = as<float>(args["colorA"],1.0f);

        float outlineColorR    = as<float>(args["outlineColorR"],1.0f);
        float outlineColorG    = as<float>(args["outlineColorG"],1.0f);
        float outlineColorB    = as<float>(args["outlineColorB"],1.0f);
        float outlineColorA    = as<float>(args["outlineColorA"],1.0f);

        bool outlineVisible = args["outlineVisible"] == "true";

        float size = as<float>(args["size"], 1.0f);
        int alignment = as<int>(args["alignment"],0);

        std::string text = args["text"];

        return new UpdateTextLabelCommand(id, latitude, longitude, height,
            osg::Vec4(colorR, colorG, colorB, colorA),
            osg::Vec4(outlineColorR, outlineColorG, outlineColorB, outlineColorA),
            outlineVisible,
            size,
            alignment,
            text);
    }
    return NULL;
}

UpdateTextLabelCommand::UpdateTextLabelCommand(const std::string &id, double latitude, double longitude, double height,
                               const osg::Vec4& color,
                               const osg::Vec4& outlineColor,
                               bool  outlineVisible,
                               const float &size,
                               const int &alignment,
                               const std::string &text)
{
    _id = id;
    _text = text;
    _longitude = longitude;
    _latitude = latitude;
    _height = height;
    _color = color;
    _outlineColor = outlineColor;
    _outlineVisible = outlineVisible;
    _size = size;
    _alignment = alignment;
}

bool UpdateTextLabelCommand::operator()(MapControl* map)
{
    //Try to get the existing label
    osg::MatrixTransform* xform = dynamic_cast<osg::MatrixTransform*>( findNamedNode( _id, map->getRoot() ) );
    bool updating = (xform != 0L);

    TextLabel* label = 0L;
    if ( updating ) {
        label = static_cast<TextLabel*>( xform->getChild(0) );
    }
    else {
        label = new TextLabel( _id );
        xform = new osg::MatrixTransform();
        xform->addChild( label );
    }

    //TextLabel* label = dynamic_cast<TextLabel*>(findNamedNode(_id, map->getRoot()));
    //if (!label) label = new TextLabel(_id);

    xform->getOrCreateStateSet()->setRenderBinDetails(11, "RenderBin");
    xform->setNodeMask( TEXT_LABELS );
    label->setText(_text);

    osg::Vec3d position;
    map->latLongHeightToXYZ(_latitude, _longitude, _height,
                            position.x(), position.y(), position.z());

    osg::Vec3d normal = map->getCoordinateSystemsNode()->getEllipsoidModel()->computeLocalUpVector(position.x(), position.y(), position.z());
    label->_drawable->setCullCallback(new osg::ClusterCullingCallback(osg::Vec3d(0,0,0), normal, 0.0));
    //label->_drawable->setPosition(position);

    xform->setMatrix( osg::Matrixd::translate( position ) );

    label->_drawable->setColor(_color);
    label->_drawable->setBackdropColor(_outlineColor);
    if (_outlineVisible)
    {
        label->_drawable->setBackdropType(osgText::Text::OUTLINE);
    }
    else
    {
        label->_drawable->setBackdropType(osgText::Text::NONE);
    }
    label->_drawable->setAlignment((osgText::TextBase::AlignmentType)_alignment);
    label->_drawable->setCharacterSize(_size);

    if (!updating)
    {
        map->getRoot()->addChild( xform );
    }
    return true;
}

/***************************************************************************************/
Command* RemoveTextLabelCommand::Factory::create(const std::string& command, const CommandArguments& args)
{
    if ("removeTextLabel" == command)
    {
        std::string id = args["id"];
        return new RemoveTextLabelCommand(id);
    }
    return NULL;
}


RemoveTextLabelCommand::RemoveTextLabelCommand(const std::string &id)
{
    _id = id;
}

bool RemoveTextLabelCommand::operator()(MapControl* map)
{
    osg::Node* node = findNamedNode(_id, map->getRoot());
    if (node)
    {
        map->getRoot()->removeChild(node);
    }
    return true;
}

/***************************************************************************************/

TextLabel::TextLabel(std::string id):
osg::Geode()
{
    _id = id;
    _drawable = new osgText::Text;

    osgText::Font* font = osgText::readFontFile("fonts/arial.ttf");
    _drawable->setFont(font);

    _drawable->setAutoRotateToScreen(true);
    _drawable->setCharacterSize(50);
    _drawable->setCharacterSizeMode(osgText::TextBase::SCREEN_COORDS);

    setName(_id);
    addDrawable(_drawable.get());
    setDataVariance( osg::Object::DYNAMIC );
    getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
}

void TextLabel::setText(const std::string &value)
{
    _text = value;
    _drawable->setText(value);
}
