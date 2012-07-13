#include <GodziWebControl/Model>
#include <GodziWebControl/MapControl>
#include <GodziWebControl/NodeMasks>
#include <GodziWebControl/Utils>

#include <osgEarth/Common>

#include <osg/io_utils>
#include <osg/ClusterCullingCallback>
#include <string>
#include <sstream>


#include <osgDB/ReadFile>

using namespace osg;
using namespace osgDB;
using namespace GodziWebControl;
using namespace osgEarth;

/***************************************************************************************/
Command* RemoveModelCommand::Factory::create(const std::string& command, const CommandArguments& args)
{
    if ("removeModel" == command)
    {
        std::string id = args["id"];
        return new RemoveModelCommand(id);
    }
    return NULL;
}

RemoveModelCommand::RemoveModelCommand(const std::string &id)
{
    _id = id;
}

bool RemoveModelCommand::operator ()(GodziWebControl::MapControl *map)
{   
    osg::Node* node = findNamedNode(_id, map->getRoot());
    if (node)
    {
        map->getRoot()->removeChild(node);
    }
    return true;
}

/***************************************************************************************/

Command* UpdateModelCommand::Factory::create(const std::string& command, const CommandArguments& args)
{
    if ("updateModel" == command)
    {
        std::string id       = args["id"];
        double latitude      = as<double>(args["latitude"], 0.0);
        double longitude     = as<double>(args["longitude"], 0.0);
        double height        = as<double>(args["height"], 0.0);

        double scaleX      = as<double>(args["scaleX"], 1.0);
        double scaleY    = as<double>(args["scaleY"], 1.0);
        double scaleZ        = as<double>(args["scaleZ"], 1.0);
        
        std::string filename = args["filename"];

        double heading = as<double>(args["heading"], 0.0);
        double pitch = as<double>(args["pitch"], 0.0);
        double roll = as<double>(args["roll"], 0.0);

        bool absolutelyPositioned = args["absolutelyPositioned"] == "true";

        return new UpdateModelCommand(id, filename, osg::Vec3d(latitude,longitude,height), osg::Vec3d(scaleX,scaleY,scaleZ),
                                      heading, pitch, roll, absolutelyPositioned);
    }
    return NULL;
}

UpdateModelCommand::UpdateModelCommand(const std::string &id, const std::string &filename, const osg::Vec3d &latLonHeight, const osg::Vec3d &scale,
                                       double heading, double pitch, double roll,
                                       bool absolutelyPositioned)
{
    _id = id;
    _filename = filename;
    _latLonHeight = latLonHeight;
    _scale = scale;
    _heading = heading;
    _pitch = pitch;
    _roll = roll;
    _absolutelyPositioned = absolutelyPositioned;
}

bool UpdateModelCommand::operator ()(GodziWebControl::MapControl *map)
{
    //Find the model
    osg::Group* model = dynamic_cast<osg::Group*>(findNamedNode(_id, map->getRoot()));

    bool updating = (model != 0);

    if (!model)
    {
      model = new osg::Group;
      model->setDataVariance(osg::Object::DYNAMIC);
      model->setName(_id);
      model->setNodeMask(MODELS);
    }


    osg::MatrixTransform* transform = 0;
    if (model->getNumChildren() == 1)
    {
        transform = static_cast<osg::MatrixTransform*>(model->getChild(0));
    }
    else
    {
        transform = new osg::MatrixTransform;
        model->addChild(transform);
    }

    bool loadModel = true;
    if (transform->getNumChildren() > 0)
    {
        std::string currName = transform->getChild(0)->getName();
        loadModel = currName != _filename;
    }

    if (loadModel)
    {
        osg::Node* node = osgDB::readNodeFile(_filename);
        if (node)
        {
            node->setName(_filename);
            model->getDescriptions().push_back(_filename);
            osg::notify(osg::NOTICE) << "Added description " << _filename << std::endl;
            transform->addChild(node);
        }
        else
        {
            osg::notify(osg::NOTICE) << "FAiled to load " << _filename << std::endl;
        }
    }

    osg::Vec3d position;
    map->latLongHeightToXYZ(_latLonHeight.x(), _latLonHeight.y(), _latLonHeight.z(),
                            position.x(), position.y(), position.z());
 
    osg::Matrixd matrix;
    if (!_absolutelyPositioned)
    {
        if (map->getCoordinateSystemsNode() && map->getCoordinateSystemsNode()->getEllipsoidModel())
        {        
            map->getCoordinateSystemsNode()->getEllipsoidModel()->computeLocalToWorldTransformFromXYZ(position.x(), position.y(), position.z(), matrix);
        }
    }
    else
    {
        matrix = osg::Matrix::identity();
    }

    if (!_absolutelyPositioned)
    {
      osg::Vec3d normal = map->getCoordinateSystemsNode()->getEllipsoidModel()->computeLocalUpVector(position.x(), position.y(), position.z());
      model->setCullCallback(new osg::ClusterCullingCallback(position, normal,0.0));
    }

    //Apply the scaling
    matrix.preMult(osg::Matrixd::scale(_scale));

    //Apply the rotation
    osg::Matrix rot_mat;
    rot_mat.makeRotate( 
        osg::DegreesToRadians(_pitch), osg::Vec3(1,0,0),
        osg::DegreesToRadians(_heading), osg::Vec3(0,0,1),
        osg::DegreesToRadians(_roll), osg::Vec3(0,1,0) );

    matrix.preMult(rot_mat);

    transform->setMatrix(matrix);

    if (!updating)
    {
        map->getRoot()->addChild(model);
    }

    // turn on GL_NORMALIZE to prevent problems with scaled normals
    model->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON );
    model->getOrCreateStateSet()->setRenderBinDetails(-100, "RenderBin");

    return true;
}