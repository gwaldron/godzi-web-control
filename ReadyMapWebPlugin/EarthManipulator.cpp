//#include <ReadyMapWebPlugin/EarthManipulator>
//
//#include <osgUtil/IntersectionVisitor>
//#include <osgUtil/LineSegmentIntersector>
//
//#include <osgViewer/View>
//
//#include <osg/io_utils>
//
//using namespace osg;
//using namespace osgGA;
//using namespace ReadyMapWebPlugin;
//
////Spherical linear interpolate between two angles
//double slerp(double interp, double from, double to)
//{
//    if (interp <= 0) return from;
//    if (interp >= 1) return to;
//
//    //Use the quaternion slerp method to compute rotation around the X axis
//    osg::Quat q;
//    osg::Vec3d axis(1,0,0);
//
//    q.slerp(interp, osg::Quat(from, axis), osg::Quat(to, axis));
//
//    //Get the rotation around the X axis
//    double x = atan2( 2 * (q.y()*q.z() + q.w()*q.x()), (q.w()*q.w() - q.x()*q.x() - q.y()*q.y() + q.z() * q.z()));
//    return x;
//}
//
////Linear interpolation between two values
//double lerp(double interp, double from, double to)
//{
//    if (interp <= 0) return from;
//    if (interp >= 1) return to;
//
//    return (1.0 - interp) * from + interp * to;
//}
//
//
//template<class T>
//class FindTopMostNodeOfTypeVisitor : public osg::NodeVisitor
//{
//public:
//    FindTopMostNodeOfTypeVisitor():
//      osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN),
//          _foundNode(0)
//      {}
//
//      void apply(osg::Node& node)
//      {
//          T* result = dynamic_cast<T*>(&node);
//          if (result)
//          {
//              _foundNode = result;
//          }
//          else
//          {
//              traverse(node);
//          }
//      }
//
//      T* _foundNode;
//};
//
//template<class T>
//T* findTopMostNodeOfType(osg::Node* node)
//{
//    if (!node) return 0;
//
//    FindTopMostNodeOfTypeVisitor<T> fnotv;
//    node->accept(fnotv);
//
//    return fnotv._foundNode;
//}
//
//EarthManipulator::EarthManipulator()
//{
//    setDistance(1.0, false);
//    _distanceSmoothing = 0.05;
//
//    setCenter(osg::Vec3d(0,0,0), false);
//    _centerSmoothing = 0.05;
//
//    _wheelZoomRatio = 0.1;
//
//    flushMouseEventStack();
//
//    _currTime = 0.0;
//    _jumpTime = 2.0;
//    _previousTime = 0.0;
//
//    _jumping = false;
//
//    setPitch(0.0, false);
//    _pitchSmoothing = 0.05;
//
//    setHeading(0.0, false);
//    _headingSmoothing = 0.05;
//
//    _smoothingEnabled = true;
//
//    _reverseMouse = false;
//
//    _rangeBoost = 0;
//
//    _doubleClickZoomRatio = 0.5;
//    _doubleClickJumpTime = 2.0;
//
//    //Note, we don't want the HAT to try to read in missing database tiles, so set it readcallback to NULL
//    _hat.setDatabaseCacheReadCallback( 0 );
//}
//
//EarthManipulator::~EarthManipulator()
//{
//}
//
//osg::Node* EarthManipulator::getNode()
//{
//    return _node.get();
//}
//
//const osg::Node* EarthManipulator::getNode() const
//{
//    return _node.get();
//}
//
//void EarthManipulator::flushMouseEventStack()
//{
//    _ga_t1 = NULL;
//    _ga_t0 = NULL;
//}
//
//
//void EarthManipulator::addMouseEvent(const GUIEventAdapter& ea)
//{
//    _ga_t1 = _ga_t0;
//    _ga_t0 = &ea;
//}
//
//void EarthManipulator::setNode(osg::Node* node)
//{
//    _node = node;
//
//    if (_node.valid())
//    {
//        //Find the coordinate systems node
//        _csn = findTopMostNodeOfType<osg::CoordinateSystemNode>(_node.get());
//        if (!_csn.valid())
//        {
//            osg::notify(osg::NOTICE) << "Warning:  Could not find valid CoordinateSystemsNode" << std::endl;
//        }
//        else
//        {
//            osg::notify(osg::NOTICE) << "EarthManipulator found CoordinateSystemsNode" << std::endl;
//        }
//        _maximumDistance = _node->getBound().radius() * 10;
//    }
//}
//
//void EarthManipulator::jumpTo(const osg::Vec3d &center, double distance, double heading, double pitch, double time)
//{
//    _startCenter = _center;
//    _goalCenter = center;
//
//    _goalDistance = osg::clampBetween(distance, _minimumDistance, _maximumDistance);
//    _startDistance = _distance;
//
//    _goalPitch = osg::clampBetween(pitch, -osg::PI_2, 0.0);
//    _startPitch = _pitch;
//
//    _goalHeading = heading;
//    _startHeading = _heading;
//
//    //Compute the "range boost" between the start and end points
//    if (_csn.valid() && _csn->getEllipsoidModel())
//    {
//        double startLat, startLon, startHeight;
//        _csn->getEllipsoidModel()->convertXYZToLatLongHeight(_startCenter.x(), _startCenter.y(), _startCenter.z(), startLat, startLon, startHeight);
//
//        double goalLat, goalLon, goalHeight;
//        _csn->getEllipsoidModel()->convertXYZToLatLongHeight(_goalCenter.x(), _goalCenter.y(), _goalCenter.z(), goalLat, goalLon, goalHeight);
//
//        double dlat = osg::RadiansToDegrees(startLat - goalLat);
//        double dlon = osg::RadiansToDegrees(startLon - goalLon);
//        _rangeBoost = sqrt(dlat * dlat + dlon * dlon) * 111000;
//    }
//    else
//    {
//        _rangeBoost = (_goalCenter - _startCenter).length();
//    }
//
//    _currTime = 0.0;
//    _jumpTime = time;
//    _jumping = true;
//}
//
//void EarthManipulator::cancelJump()
//{
//    if (_jumping)
//    {
//        _jumping = false;
//        _goalCenter = _center;
//        _goalDistance = _distance;
//    }
//}
//
//void EarthManipulator::setDistance(const double &distance, bool smooth)
//{
//    double dist = osg::clampBetween(distance, _minimumDistance, _maximumDistance);
//    //If not smoothing going from the current distance to the next, just directly set both the goal distance and the distance
//    if (!smooth)
//    {
//        _distance = _startDistance = _goalDistance = dist;
//    }
//    else
//    {
//        //Set the start distance and the goal distance
//        _startDistance = _distance;
//        _goalDistance = dist;
//    }
//}
//
//
//void EarthManipulator::setPitchSmoothing(const double &smoothing)
//{
//    _pitchSmoothing = osg::clampBetween(smoothing, 0.0, 1.0);
//}
//
//void EarthManipulator::setHeadingSmoothing(const double &smoothing)
//{
//    _headingSmoothing = osg::clampBetween(smoothing, 0.0, 1.0);
//}
//
//void EarthManipulator::setCenterSmoothing(const double &smoothing)
//{
//    _centerSmoothing = osg::clampBetween(smoothing, 0.0, 1.0);
//}
//
//void EarthManipulator::setDoubleClickZoomRatio(const double &ratio)
//{
//    _doubleClickZoomRatio = ratio;
//}
//
//
//void EarthManipulator::setCenter(const osg::Vec3d &center, bool smooth)
//{
//    if (!smooth)
//    {
//        _center = _startCenter = _goalCenter = center;
//    }
//    else
//    {
//        _goalCenter = center;
//    }
//}
//
//void EarthManipulator::setHeading(const double &heading, bool smooth)
//{
//    if (!smooth)
//    {
//        _heading = _startHeading = _goalHeading = heading;
//    }
//    else
//    {
//        _goalHeading = heading;
//    }
//}
//
//void EarthManipulator::setPitch(const double &pitch, bool smooth)
//{
//    _goalPitch = osg::clampBetween(pitch, -osg::PI_2, 0.0);
//    if (!smooth)
//    {
//        _pitch =  _startPitch = _goalPitch;
//    }
//}
//
//bool EarthManipulator::getIntersectionAtLatLon(const double &latRadians, const double &lonRadians, osg::Vec3d &hit)
//{
//    if ( _csn.valid() && _csn->getEllipsoidModel())
//    {
//        osg::Vec3d loc;
//        _csn->getEllipsoidModel()->convertLatLongHeightToXYZ(latRadians, lonRadians, 0, loc.x(), loc.y(), loc.z());
//        return getIntersectionAtLocation(loc, hit);
//    }
//
//    osg::notify(osg::NOTICE) << "getIntersectionAtLatLon not valid for non-geocentric databases" << std::endl;
//    return false;
//}
//
//bool EarthManipulator::getIntersectionAtLocation(const osg::Vec3d &loc, osg::Vec3d& hit)
//{
//    if (_node.valid())
//    {
//        double distance = _node->getBound().radius() * 0.2;
//        CoordinateFrame eyePointCoordFrame = getCoordinateFrame( loc );
//        osg::Vec3d ip;
//        if (intersect(loc+getUpVector(eyePointCoordFrame)*distance,
//            loc-getUpVector(eyePointCoordFrame)*distance,
//            ip))
//        {
//            hit = ip;
//            return true;
//        }
//    }
//    return false;
//}
//
//void EarthManipulator::setByMatrix(const osg::Matrixd& matrix)
//{
//    osg::Vec3d lookVector(- matrix(2,0),-matrix(2,1),-matrix(2,2));
//    osg::Vec3d eye(matrix(3,0),matrix(3,1),matrix(3,2));
//    computePosition(eye, lookVector, osg::Vec3d(0, 1, 0));    
//}
//
//osg::Matrixd EarthManipulator::getMatrix() const
//{
//    return osg::Matrixd::inverse(getInverseMatrix());
//}
//
//osg::Matrixd EarthManipulator::getInverseMatrix() const
//{
//    return 
//        getRotation(_center) *
//        osg::Matrixd(osg::Quat(_heading, osg::Vec3d(0, 0, 1)) * osg::Quat(_pitch, osg::Vec3d(1, 0, 0))) *
//        osg::Matrixd::translate(0.0, 0.0, -_distance);
//}
//
//osg::Matrixd EarthManipulator::getRotation(const osg::Vec3d &center) const
//{
//    //The look vector will be going directly from the eye point to the point on the earth,
//    //so the look vector is simply the up vector at the center point
//    osg::CoordinateFrame cf = getCoordinateFrame(center);
//    osg::Vec3d lookVector = -getUpVector(cf);
//
//    osg::Vec3d side;
//
//    //Force the side vector to be orthogonal to north
//    osg::Vec3d worldUp(0,0,1);
//
//    double dot = osg::absolute(worldUp * lookVector);
//    if (osg::equivalent(dot, 1.0))
//    {
//        //We are looking nearly straight down the up vector, so use the Y vector for world up instead
//        worldUp = osg::Vec3d(0, 1, 0);
//    }
//
//    side = lookVector ^ worldUp;
//
//    osg::Vec3d up = side ^ lookVector;
//    up.normalize();
//
//    //We want a very slight offset
//    double offset = 1e-6;
//    return osg::Matrixd::lookAt(center - (lookVector * offset),center,up);
//}
//
//
//void EarthManipulator::home(const GUIEventAdapter& ,GUIActionAdapter& us)
//{
//    if (getAutoComputeHomePosition()) computeHomePosition();
//
//    computePosition(_homeEye, _homeCenter, _homeUp);
//    us.requestRedraw();
//}
//
//void EarthManipulator::home(double /*currentTime*/)
//{
//    if (getAutoComputeHomePosition()) computeHomePosition();
//    computePosition(_homeEye, _homeCenter, _homeUp);
//}
//
//void EarthManipulator::computeHomePosition()
//{
//    if(getNode())
//    {
//        const osg::BoundingSphere& boundingSphere=getNode()->getBound();
//        osg::Vec3d center = boundingSphere.center();
//        double radius = boundingSphere.radius();
//
//        //If we are dealing with a whole earth database, it is possible that the back side could be culled due to a ClusterCullingCallback, which will
//        //make the bounds and center not truly at the center of the node.  So, if we encounter a whole earth database, override the center and radius
//        if (_csn.valid() && _csn->getEllipsoidModel())
//        {
//            center = osg::Vec3d(0,0,0);
//            radius = _csn->getEllipsoidModel()->getRadiusEquator();
//        }
//
//        setHomePosition(center+osg::Vec3( 0.0,-5.0f * radius,0.0f),
//            center,
//            osg::Vec3(0.0f,0.0f,1.0f),
//            _autoComputeHomePosition);
//    }
//}
//
//double EarthManipulator::getRadius()
//{
//    if (_csn.valid() && _csn->getEllipsoidModel())
//    {
//        return _csn->getEllipsoidModel()->getRadiusEquator();
//    }
//    else if (_node.valid())
//    {
//        return _node->getBound().radius();
//    }
//    return 0.0;
//}
//
//
//void EarthManipulator::init(const GUIEventAdapter& ,GUIActionAdapter& )
//{
//    flushMouseEventStack();
//}
//
//void EarthManipulator::goNorthUp()
//{
//    setHeading(0, _smoothingEnabled);
//}
//
//void EarthManipulator::lookAtXY(const double &x, const double &y)
//{
//    if (!_node) return;
//
//    osg::Vec3d targetCenter(x, y, 0);
//    if (_csn.valid() && _csn->getEllipsoidModel())
//    {
//        //If we have a geocentric position, assume the user passed in lon, lat in radians.
//        _csn->getEllipsoidModel()->convertLatLongHeightToXYZ(y, x, 0, targetCenter.x(), targetCenter.y(), targetCenter.z());
//    }
//
//    osg::Vec3d newCenter(targetCenter);
//    if (getIntersectionAtLocation(targetCenter, newCenter))
//    {
//        jumpTo(newCenter, _distance, _heading, _pitch, _doubleClickJumpTime);
//    }
//}
//
//
//
///** handle events, return true if handled, false otherwise.*/
//bool EarthManipulator::handle(const GUIEventAdapter& ea,GUIActionAdapter& us)
//{
//    //Get the intersection point
//    osgViewer::View* view = dynamic_cast<osgViewer::View*>(&us);
//    if (!view) return false;
//
//    //osg::notify(osg::NOTICE) << "CurrTime: " << currTime << " " << "PrevTime: " << _previousTime << " " << "dt " << dt << std::endl;
//
//    switch (ea.getEventType())
//    {
//    case osgGA::GUIEventAdapter::PUSH:
//        {
//            flushMouseEventStack();
//            addMouseEvent(ea);
//            return true;
//        }
//        break;
//
//    case osgGA::GUIEventAdapter::DRAG:
//        {
//            addMouseEvent(ea);
//
//            unsigned int buttonMask = ea.getButtonMask();
//            if (buttonMask == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
//            {
//                if (_ga_t0.valid() && _ga_t1.valid())
//                {
//                    if ((_ga_t0->getX() != _ga_t1->getX()) ||
//                        (_ga_t0->getY() != _ga_t1->getY()))
//                    {
//                        if ((_ga_t0->getX() >= 0 && _ga_t0->getX() < ea.getWindowWidth()) &&
//                            (_ga_t1->getX() >= 0 && _ga_t1->getX() < ea.getWindowWidth()) &&
//                            (_ga_t0->getY() >= 0 && _ga_t0->getY() < ea.getWindowHeight()) &&
//                            (_ga_t1->getY() >= 0 && _ga_t1->getY() < ea.getWindowHeight()))
//                        {
//                            osg::Vec3d hit, prevHit;
//
//                            osgUtil::LineSegmentIntersector::Intersections intersections;
//
//                            bool gotPrevHit = false;
//                            if (view->computeIntersections(_ga_t0->getX(), _ga_t0->getY(), intersections, this->_intersectTraversalMask))
//                            {
//                                prevHit = intersections.begin()->getWorldIntersectPoint();
//                                gotPrevHit = true;
//                            }
//
//                            bool gotHit = false;
//                            if (view->computeIntersections(_ga_t1->getX(), _ga_t1->getY(), intersections, this->_intersectTraversalMask))
//                            {
//                                hit = intersections.begin()->getWorldIntersectPoint();
//                                gotHit = true;
//                            }
//
//                            if (gotPrevHit && gotHit)
//                            {
//                                osg::Vec3d newCenter;
//                                bool gotNewCenter = false;
//                                if (_csn.valid() && _csn->getEllipsoidModel())
//                                {
//                                    double hitLat, hitLon, hitHeight;
//                                    _csn->getEllipsoidModel()->convertXYZToLatLongHeight(hit.x(), hit.y(), hit.z(),hitLat, hitLon, hitHeight);
//
//                                    double prevLon, prevLat, prevHeight;
//                                    _csn->getEllipsoidModel()->convertXYZToLatLongHeight(prevHit.x(), prevHit.y(), prevHit.z(),prevLat, prevLon, prevHeight);
//
//                                    double lat, lon, height;
//                                    _csn->getEllipsoidModel()->convertXYZToLatLongHeight(_goalCenter.x(), _goalCenter.y(), _goalCenter.z(),lat,lon,height);
//
//                                    lat += (hitLat - prevLat);
//                                    lon += (hitLon - prevLon);
//
//                                    gotNewCenter = getIntersectionAtLatLon(lat, lon, newCenter);
//                                }
//                                else
//                                {
//                                    double x = _goalCenter.x() + hit.x() - prevHit.x();
//                                    double y = _goalCenter.y() + hit.y() - prevHit.y();
//                                    gotNewCenter = getIntersectionAtLocation(osg::Vec3d(x, y, 0), newCenter);
//                                }
//                                if (gotNewCenter)
//                                {
//                                    cancelJump();
//                                    setCenter(newCenter, _smoothingEnabled);
//                                }
//                            }
//                        }
//                    }
//                }
//            }
//            else if (buttonMask == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
//            {
//                if (_ga_t0.valid() && _ga_t1.valid())
//                {
//                    double dx = _ga_t1->getXnormalized() - _ga_t0->getXnormalized();
//                    double dy = _ga_t1->getYnormalized() - _ga_t0->getYnormalized();
//
//                    cancelJump();
//                    setPitch( _goalPitch + dy, _smoothingEnabled );
//                    setHeading( _goalHeading - dx, _smoothingEnabled );
//                }
//            }
//            else if (buttonMask == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
//            {
//                if (_ga_t0.valid() && _ga_t1.valid())
//                {
//                    double dy = _ga_t1->getYnormalized() - _ga_t0->getYnormalized();
//                    cancelJump();
//                    setDistance( _goalDistance * (1.0 - dy), _smoothingEnabled );
//                }
//            }
//
//
//            return true;
//        }
//        break;
//
//    case osgGA::GUIEventAdapter::RELEASE:
//        {
//            flushMouseEventStack();
//            addMouseEvent(ea);
//        }
//        break;
//    case osgGA::GUIEventAdapter::DOUBLECLICK:
//        {
//            flushMouseEventStack();
//            int buttonMask = ea.getButton();
//
//            if ((buttonMask == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) ||
//                (buttonMask == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON))
//            {
//                double ratio = (buttonMask == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) ? _doubleClickZoomRatio : 1.0/_doubleClickZoomRatio;
//                osgUtil::LineSegmentIntersector::Intersections intersections;
//                if (view->computeIntersections(ea.getX(), ea.getY(), intersections, this->_intersectTraversalMask))
//                {
//                    //Center on the view
//                    osg::Vec3d hit = intersections.begin()->getWorldIntersectPoint();
//                    jumpTo(hit, _distance * ratio, _heading, _pitch, _doubleClickJumpTime);
//                    return true;
//                }
//            }
//        }
//        break;
//
//    case osgGA::GUIEventAdapter::KEYDOWN:
//        {
//            switch (ea.getKey())
//            {
//                //Go home
//            case osgGA::GUIEventAdapter::KEY_Space:
//                flushMouseEventStack();
//                home(ea, us);
//                us.requestRedraw();
//                break;
//            //Go to North Up
//            case 'n':
//                flushMouseEventStack();
//                goNorthUp();
//                break;
//            case 'i':
//                setSmoothingEnabled(!_smoothingEnabled);
//                break;
//            }
//        }
//        break;
//
//    case osgGA::GUIEventAdapter::SCROLL:
//        {
//            flushMouseEventStack();
//            float delta = ea.getScrollingMotion() == osgGA::GUIEventAdapter::SCROLL_DOWN? _wheelZoomRatio : -_wheelZoomRatio;
//            if (_reverseMouse) delta = -delta;
//            cancelJump();
//            setDistance( _goalDistance * (1.0 + delta), _smoothingEnabled );
//            us.requestRedraw();
//            return true;
//        }
//        break;
//
//    case osgGA::GUIEventAdapter::FRAME:
//        {
//            double currTime = ea.getTime();
//            double dt = (currTime - _previousTime);
//            _previousTime = currTime;
//
//            //Update the view if necessary
//            if (!update(dt))
//            {
//                us.requestRedraw();
//            }           
//            return false;
//        }
//        break;
//    default:
//        return false;
//    }
//
//    return false;
//}
//
//bool EarthManipulator::intersect(const osg::Vec3d& start, const osg::Vec3d& end, osg::Vec3d& intersection) const
//{
//    osg::ref_ptr<osgUtil::LineSegmentIntersector> lsi = new osgUtil::LineSegmentIntersector(start,end);
//
//    osgUtil::IntersectionVisitor iv(lsi.get());
//    iv.setTraversalMask(_intersectTraversalMask);
//
//    _node->accept(iv);
//
//    if (lsi->containsIntersections())
//    {
//        intersection = lsi->getIntersections().begin()->getWorldIntersectPoint();
//        return true;
//    }
//    return false;
//}
//
//void EarthManipulator::computePosition(const osg::Vec3d& eye,const osg::Vec3d& center,const osg::Vec3d& up)
//{
//    if (!_node) return;
//
//    cancelJump();
//
//    osg::Vec3d lv(center-eye);
//    setDistance( lv.length(), false );
//    setCenter(center, false);
//    setPitch(0, false);
//    setHeading(0, false);
//    
//    osg::Vec3d surface = eye;
//    if (_csn.valid() && _csn->getEllipsoidModel())
//    {
//        //Try to determine a point that would be close to the surface of the node along the look vector
//        osg::Vec3d surfaceUp = getUpVector(getCoordinateFrame(eye));
//        surface = center + surfaceUp * getRadius();
//        setCenter(surface, false);
//    }
//
//    //Try to get a hit at the surface point.
//    osg::Vec3d hit;
//    if (getIntersectionAtLocation(surface, hit))
//    {
//        setCenter(hit, false);
//    }
//    setDistance((eye-_center).length(), false);
//}
//
//Viewpoint EarthManipulator::getCurrentViewpoint()
//{
//    double x = _center.x();
//    double y = _center.y();
//
//    //If the model is geocentric, the X and Y of the viewpoint is the lon and lat in radians
//    if (_csn.valid() && _csn->getEllipsoidModel())
//    {
//        double height;
//        _csn->getEllipsoidModel()->convertXYZToLatLongHeight(_center.x(), _center.y(), _center.z(), y, x, height);
//    }
//    return Viewpoint(x,y,_distance, _heading, _pitch);
//}
//
//void EarthManipulator::setViewpoint(const Viewpoint& viewpoint, double time /*= 0.0*/)
//{
//    osg::Vec3d newCenter;
//    //If the model is geocentric, the X and Y of the viewpoint is the lon and lat in radians
//    if (_csn.valid() && _csn->getEllipsoidModel())
//    {
//        _csn->getEllipsoidModel()->convertLatLongHeightToXYZ(viewpoint._y, viewpoint._x, 0, newCenter.x(), newCenter.y(), newCenter.z());
//    }
//    else
//    {
//        newCenter.x() = viewpoint._x;
//        newCenter.y() = viewpoint._y;
//        newCenter.z() = 0;
//    }
//
//    osg::Vec3d hit;
//    if (getIntersectionAtLocation(newCenter, hit))
//    {
//        newCenter = hit;
//    }
//    else
//    {
//        osg::notify(osg::NOTICE) << "Warning:  Could not intersect scene at " << newCenter << std::endl;
//    }
//
//    jumpTo(newCenter, viewpoint._range, viewpoint._heading, viewpoint._pitch, time);
//}
//
//bool EarthManipulator::update(double dt)
//{
//    bool finished = true;
//
//    _currTime += dt;
//
//    double ratio = osg::minimum((_currTime / _jumpTime), 1.0);
//
//    if ((_goalDistance != _distance) || _rangeBoost != 0)
//    {
//        if (_jumping)
//        {
//            if (ratio < 1)
//            {
//                //If we are jumping to a location, we want to smoothly go there over time
//                double dist = (_goalDistance - _startDistance) * ratio;
//                if (_rangeBoost > 0)
//                {
//                    dist += _rangeBoost * sin(ratio * osg::PI);
//                }
//                _distance = osg::clampBetween(_startDistance + dist, _minimumDistance, _maximumDistance);
//            }
//            else
//            {
//                _rangeBoost = 0.0;
//                _distance = _goalDistance;
//            }
//        }
//        else
//        {
//            //If we are not jumping, but the goal distance is different than the distance, we want
//            //some type of dampening to occur
//            _distance = osg::clampBetween((1 - _distanceSmoothing) * _distance + _distanceSmoothing * _goalDistance, _minimumDistance, _maximumDistance);
//        }
//        finished = false;
//    }
//
//    //Interpolate between the current center and the target center
//    double dist = (_goalCenter - _center).length();
//    double eps = 0.0001;
//    if (dist > eps)
//    {
//        osg::Vec3d start = _jumping ? _startCenter : _center;
//        double interp = _jumping ? ratio : _centerSmoothing;
//        if (_csn.valid() && _csn->getEllipsoidModel())
//        {
//            //Interpolate between the current and goal center
//            double lat, lon, height;
//            _csn->getEllipsoidModel()->convertXYZToLatLongHeight(start.x(), start.y(), start.z(),lat,lon,height);
//
//            double goalLat, goalLon;
//            _csn->getEllipsoidModel()->convertXYZToLatLongHeight(_goalCenter.x(), _goalCenter.y(), _goalCenter.z(),goalLat,goalLon,height);
//
//            //Compute the new lat/lon
//            lat = slerp(interp, lat, goalLat);
//            lon = slerp(interp, lon, goalLon);
//
//            //Update the center to be the terrain intersection at the new lat lon
//            osg::Vec3d newCenter;
//            if (getIntersectionAtLatLon(lat, lon, newCenter))
//            {
//                _center = newCenter;
//            }
//        }
//        else
//        {
//            double x = lerp(interp, start.x(), _goalCenter.x());
//            double y = lerp(interp, start.y(), _goalCenter.y());
//            double z = lerp(interp, start.z(), _goalCenter.z());
//            osg::Vec3d newCenter;
//            if (getIntersectionAtLocation(osg::Vec3d(x,y,z), newCenter))
//            {
//                _center = newCenter;
//            }
//        }
//    }
//
//    //Interpolate pitch if necessary
//    if (!osg::equivalent(_goalPitch, _pitch))
//    {
//        if (_jumping)
//        {
//          _pitch = slerp(ratio, _startPitch, _goalPitch);
//        }
//        else
//        {
//          _pitch = slerp(_pitchSmoothing, _pitch, _goalPitch);
//        }
//    }
//
//    //Interpolate heading if necessary
//    if (!osg::equivalent(_goalHeading, _heading))
//    {
//        if (_jumping)
//        {
//            _heading = slerp(ratio, _startHeading, _goalHeading);
//        }
//        else
//        {
//          _heading = slerp(_headingSmoothing, _heading, _goalHeading);
//        }
//    }
//
//    if (finished)
//    {
//        _currTime = 0.0;
//        _jumping = false;
//    }
//
//    return finished;
//}
//
//double EarthManipulator::getHeightAboveTerrain()
//{
//    if (_csn.valid())
//    {
//        osg::Vec3d eye, center, up;
//        osg::Matrixd mat = getInverseMatrix();
//        mat.getLookAt(eye, center, up);
//        _hat.clear();
//        _hat.addPoint(eye);
//
//        _hat.computeIntersections(_csn.get(), _intersectTraversalMask);
//        return _hat.getHeightAboveTerrain(0);
//    }
//    return 0.0;
//}
