#include "FirstPersonManipulator"
#include <cassert>

#include <osgEarth/Notify>

using namespace osg;
using namespace osgGA;
using namespace GodziWebControl;

int FirstPersonManipulator::_accelerationFlagIndex = allocateRelativeFlag();
int FirstPersonManipulator::_maxVelocityFlagIndex = allocateRelativeFlag();
int FirstPersonManipulator::_wheelMovementFlagIndex = allocateRelativeFlag();


/// Constructor.
FirstPersonManipulator::FirstPersonManipulator( int flags )
   : inherited( flags ),
     _velocity( 0. ),
     _strafing(0.),
     _walking(0.),
     _floating(0.),
     _maxVelocity(1000.)
{
   setWheelMovement( 500.0, false );

   if( _flags & SET_CENTER_ON_WHEEL_FORWARD_MOVEMENT )
      setAnimationTime( 0.2 );
}


/// Constructor.
FirstPersonManipulator::FirstPersonManipulator( const FirstPersonManipulator& fpm, const CopyOp& copyOp )
   : inherited( fpm, copyOp ),
     _eye( fpm._eye ),
     _rotation( fpm._rotation ),
     _velocity( fpm._velocity ),
     _acceleration( fpm._acceleration ),
     _maxVelocity( fpm._maxVelocity ),
     _wheelMovement( fpm._wheelMovement ),
     _strafing(fpm._strafing),
     _walking(fpm._walking),
     _floating(fpm._floating)
{
}


/** Set the position of the manipulator using a 4x4 matrix.*/
void FirstPersonManipulator::setByMatrix( const Matrixd& matrix )
{
   // set variables
   _eye = matrix.getTrans();
   _rotation = matrix.getRotate();

   // fix current rotation
   if( getVerticalAxisFixed() )
      fixVerticalAxis( _eye, _rotation, true );
}


/** Set the position of the manipulator using a 4x4 matrix.*/
void FirstPersonManipulator::setByInverseMatrix( const Matrixd& matrix )
{
   setByMatrix( Matrixd::inverse( matrix ) );
}


/** Get the position of the manipulator as 4x4 matrix.*/
Matrixd FirstPersonManipulator::getMatrix() const
{
   return Matrixd::rotate( _rotation ) * Matrixd::translate( _eye );
}


/** Get the position of the manipulator as a inverse matrix of the manipulator,
    typically used as a model view matrix.*/
Matrixd FirstPersonManipulator::getInverseMatrix() const
{
   return Matrixd::translate( -_eye ) * Matrixd::rotate( _rotation.inverse() );
}


// doc in parent
void FirstPersonManipulator::setTransformation( const osg::Vec3d& eye, const osg::Quat& rotation )
{
   // set variables
   _eye = eye;
   _rotation = rotation;

   // fix current rotation
   if( getVerticalAxisFixed() )
      fixVerticalAxis( _eye, _rotation, true );
}


// doc in parent
void FirstPersonManipulator::getTransformation( osg::Vec3d& eye, osg::Quat& rotation ) const
{
   eye = _eye;
   rotation = _rotation;
}


// doc in parent
void FirstPersonManipulator::setTransformation( const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up )
{
   // set variables
   osg::Matrixd m( osg::Matrixd::lookAt( eye, center, up ) );
   _eye = eye;
   _rotation = m.getRotate().inverse();

   // fix current rotation
   if( getVerticalAxisFixed() )
      fixVerticalAxis( _eye, _rotation, true );
}


// doc in parent
void FirstPersonManipulator::getTransformation( osg::Vec3d& eye, osg::Vec3d& center, osg::Vec3d& up ) const
{
   center = _eye + _rotation * osg::Vec3d( 0.,0.,-1. );
   eye = _eye;
   up = _rotation * osg::Vec3d( 0.,1.,0. );
}


/** Sets velocity.
 *
 *  There are no checks for maximum velocity applied.
 */
void FirstPersonManipulator::setVelocity( const double& velocity )
{
   _velocity = velocity;
}


/** Sets acceleration.
 *
 *  If acceleration effect is unwanted, it can be set to DBL_MAX.
 *  Then, there will be no acceleration and object will reach its
 *  maximum velocity immediately.
 */
void FirstPersonManipulator::setAcceleration( const double& acceleration, bool relativeToModelSize )
{
   _acceleration = acceleration;
   setRelativeFlag( _accelerationFlagIndex, relativeToModelSize );
}


/// Returns acceleration speed.
double FirstPersonManipulator::getAcceleration( bool *relativeToModelSize ) const
{
   if( relativeToModelSize )
      *relativeToModelSize = getRelativeFlag( _accelerationFlagIndex );

   return _acceleration;
}


/** Sets maximum velocity.
 *
 *  If acceleration is set to DBL_MAX, there is no speeding up.
 *  Instead, maximum velocity is used for velocity at once without acceleration.
 */
void FirstPersonManipulator::setMaxVelocity( const double& maxVelocity, bool relativeToModelSize )
{
   _maxVelocity = maxVelocity;
   setRelativeFlag( _maxVelocityFlagIndex, relativeToModelSize );
}


/// Returns maximum velocity.
double FirstPersonManipulator::getMaxVelocity( bool *relativeToModelSize ) const
{
   if( relativeToModelSize )
      *relativeToModelSize = getRelativeFlag( _maxVelocityFlagIndex );

   return _maxVelocity;
}


/// Sets movement size on single wheel step.
void FirstPersonManipulator::setWheelMovement( const double& wheelMovement, bool relativeToModelSize )
{
   _wheelMovement = wheelMovement;
   setRelativeFlag( _wheelMovementFlagIndex, relativeToModelSize );
}


/// Returns movement size on single wheel step.
double FirstPersonManipulator::getWheelMovement( bool *relativeToModelSize ) const
{
   if( relativeToModelSize )
      *relativeToModelSize = getRelativeFlag( _wheelMovementFlagIndex );

   return _wheelMovement;
}


void FirstPersonManipulator::home( double currentTime )
{
   inherited::home( currentTime );
   _velocity = 0.;
   _strafing = 0.;
   _walking = 0.;
}


void FirstPersonManipulator::home( const GUIEventAdapter& ea, GUIActionAdapter& us )
{
   inherited::home( ea, us );
   _velocity = 0.;
   _strafing = 0.;
   _walking = 0.;
}


void FirstPersonManipulator::init( const GUIEventAdapter& ea, GUIActionAdapter& us )
{
   inherited::init( ea, us );

   // stop movement
   _velocity = 0.;
   _strafing = 0.;
   _walking = 0.;
}

// need this to force handling of arrow keys
bool FirstPersonManipulator::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
  bool handled = false;

  switch(ea.getEventType())
  {
    case(osgGA::GUIEventAdapter::KEYUP):
      handled = handleKeyUp( ea, us );
      break;

    case(osgGA::GUIEventAdapter::KEYDOWN):
      handled = handleKeyDown( ea, us );
      break;

    default:
      handled = inherited::handle( ea, us );
  }

  return handled;
}


// doc in parent
bool FirstPersonManipulator::handleMouseWheel( const GUIEventAdapter& ea, GUIActionAdapter& us )
{
    osgGA::GUIEventAdapter::ScrollingMotion sm = ea.getScrollingMotion();

    // handle centering
    if( _flags & SET_CENTER_ON_WHEEL_FORWARD_MOVEMENT )
    {

        if( ((sm == GUIEventAdapter::SCROLL_DOWN) && (_wheelMovement > 0.)) ||
            ((sm == GUIEventAdapter::SCROLL_UP)   && (_wheelMovement < 0.)) )
        {

            // stop thrown animation
            _thrown = false;

            if( getAnimationTime() <= 0. )

                // center by mouse intersection (no animation)
                setCenterByMousePointerIntersection( ea, us );

            else {

                // start new animation only if there is no animation in progress
                if( !isAnimating() )
                    startAnimationByMousePointerIntersection( ea, us );

            }
        }
    }

    switch( sm )
    {

        // mouse scroll up event
        case GUIEventAdapter::SCROLL_UP:
        {
            // move forward
            moveForward( isAnimating() ? dynamic_cast< FirstPersonAnimationData* >( _animationData.get() )->_targetRot : _rotation,
                         -_wheelMovement * (getRelativeFlag( _wheelMovementFlagIndex ) ? _modelSize : 1. ));
            us.requestRedraw();
            us.requestContinuousUpdate( isAnimating() || _thrown );
            return true;
        }

        // mouse scroll down event
        case GUIEventAdapter::SCROLL_DOWN:
        {
            // move backward
            moveForward( _wheelMovement * (getRelativeFlag( _wheelMovementFlagIndex ) ? _modelSize : 1. ));
            _thrown = false;
            us.requestRedraw();
            us.requestContinuousUpdate( isAnimating() || _thrown );
            return true;
        }

        // unhandled mouse scrolling motion
        default:
            return false;
    }
}

bool FirstPersonManipulator::handleKeyDown( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
  if ( ea.getKey() == 'f' )
  {
    _strafing = -1.0 * _maxVelocity;
    us.requestContinuousUpdate( true );
    us.requestRedraw();
    return true;
  }
  else if ( ea.getKey() == 'h' )
  {
    _strafing = _maxVelocity;
    us.requestContinuousUpdate( true );
    us.requestRedraw();
    return true;
  }
  else if ( ea.getKey() == 't' )
  {
    _walking = _maxVelocity;
    us.requestContinuousUpdate( true );
    us.requestRedraw();
    return true;
  }
  else if ( ea.getKey() == 'g' )
  {
    _walking = _maxVelocity * -1.0;
    us.requestContinuousUpdate( true );
    us.requestRedraw();
    return true;
  }
  else if ( ea.getKey() == 'y' )
  {
    _floating = _maxVelocity;
    us.requestContinuousUpdate( true );
    us.requestRedraw();
    return true;
  }
  else if ( ea.getKey() == 'n' )
  {
    _floating = _maxVelocity * -1.0;
    us.requestContinuousUpdate( true );
    us.requestRedraw();
    return true;
  }

  return inherited::handleKeyDown( ea, us );
}

bool FirstPersonManipulator::handleKeyUp( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
  if( ea.getKey() == 'f' || ea.getKey() == 'h' )
  {
    _strafing = 0.0;
    us.requestContinuousUpdate( false );
    return true;
  }
  else if ( ea.getKey()== 't' || ea.getKey() == 'g' )
  {
    _walking = 0.0;
    us.requestContinuousUpdate( false );
    return true;
  }
  else if ( ea.getKey()== 'y' || ea.getKey() == 'n' )
  {
    _floating = 0.0;
    us.requestContinuousUpdate( false );
    return true;
  }

  return inherited::handleKeyUp( ea, us );
}

bool FirstPersonManipulator::handleFrame( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
  inherited::handleKeyDown( ea, us );

  return performFrameMovement();
}

bool FirstPersonManipulator::performFrameMovement()
{
  if (_delta_frame_time < 0.0)
  {
    OSG_WARN << "Manipulator warning: _delta_frame_time = " << _delta_frame_time << std::endl;
    return false;
  }

  if (_strafing != 0.0)
  {
    osg::Matrixd rotation;
    rotation.makeRotate(_rotation);
    osg::Vec3d sideVector = getSideVector(rotation);

    _eye += sideVector * (_strafing * _delta_frame_time);

    return true;
  }

  if (_walking != 0.0)
  {
    moveForward(_walking * _delta_frame_time);
  }

  if (_floating != 0.0)
  {
    moveUp(_floating * _delta_frame_time);
  }

  return false;
}

// doc in parent
bool FirstPersonManipulator::performMovementLeftMouseButton( const double eventTimeDelta, const double dx, const double dy )
{
   // world up vector
   CoordinateFrame coordinateFrame = getCoordinateFrame( _eye );
   Vec3d localUp = getUpVector( coordinateFrame );

   rotateYawPitch( _rotation, dx, dy, localUp );

   return true;
}


bool FirstPersonManipulator::performMouseDeltaMovement( const float dx, const float dy )
{
   // rotate camera
   if( getVerticalAxisFixed() ) {

      // world up vector
      CoordinateFrame coordinateFrame = getCoordinateFrame( _eye );
      Vec3d localUp = getUpVector( coordinateFrame );

      rotateYawPitch( _rotation, dx, dy, localUp );

   } else

      rotateYawPitch( _rotation, dx, dy );

   return true;
}


/// Move camera forward by distance parameter.
void FirstPersonManipulator::moveForward( const double distance )
{
   moveForward( _rotation, distance );
}


/// Move camera forward by distance parameter.
void FirstPersonManipulator::moveForward( const Quat& rotation, const double distance )
{
   _eye += rotation * Vec3d( 0., 0., -distance );
}


/// Move camera right by distance parameter.
void FirstPersonManipulator::moveRight( const double distance )
{
   _eye += _rotation * Vec3d( distance, 0., 0. );
}


/// Move camera up by distance parameter.
void FirstPersonManipulator::moveUp( const double distance )
{
   _eye += _rotation * Vec3d( 0., distance, 0. );
}


void FirstPersonManipulator::applyAnimationStep( const double currentProgress, const double prevProgress )
{
   FirstPersonAnimationData *ad = dynamic_cast< FirstPersonAnimationData* >( _animationData.get() );
   assert( ad );

   // compute new rotation
   _rotation.slerp( currentProgress, ad->_startRot, ad->_targetRot );

   // fix vertical axis
   if( getVerticalAxisFixed() )
      fixVerticalAxis( _eye, _rotation, false );
}


// doc in parent
bool FirstPersonManipulator::startAnimationByMousePointerIntersection(
      const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
   // get current transformation
   osg::Vec3d prevEye;
   osg::Quat prevRot;
   getTransformation( prevEye, prevRot );

   // center by mouse intersection
   if( !setCenterByMousePointerIntersection( ea, us ) )
      return false;

   FirstPersonAnimationData *ad = dynamic_cast< FirstPersonAnimationData*>( _animationData.get() );
   assert( ad );

   // setup animation data and restore original transformation
   ad->start( prevRot, _rotation, ea.getTime() );
   setTransformation( _eye, prevRot );

   return true;
}


void FirstPersonManipulator::FirstPersonAnimationData::start( const Quat& startRotation, const Quat& targetRotation,
                                                              const double startTime )
{
   AnimationData::start( startTime );

   _startRot = startRotation;
   _targetRot = targetRotation;
}
