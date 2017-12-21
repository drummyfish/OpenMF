#ifndef OSG_UTILS_H
#define OSG_UTILS_H

#include <fstream>
#include <osg/Transform>
#include <osgGA/FirstPersonManipulator>
#include <math.h>

namespace MFUtil
{

/** Convert quaternion rotation to yaw, pitch, roll (Euler angles), in radians. */

void quatToEuler(osg::Quat q, double &yaw, double &pitch, double &roll)
{
    double qx = q.x(); 
    double qy = q.y(); 
    double qz = q.z(); 
    double qw = q.w(); 

    double sqx = qx * qx; 
    double sqy = qy * qy; 
    double sqz = qz * qz; 
    double sqw = qw * qw; 

    double term1 = 2 * (qx*qy+qw*qz); 
    double term2 = sqw + sqx - sqy - sqz; 
    double term3 = -2 * (qx * qz - qw * qy); 
    double term4 = 2 * (qw * qx + qy * qz); 
    double term5 = sqw - sqx - sqy + sqz; 

    yaw = atan2(term1, term2); 
    pitch = atan2(term4, term5); 
    roll = asin(term3); 
}

/** Convert Euler rotation (yaw, pitch, roll) to quaternion, in radians. */

osg::Quat eulerToQuat(double yaw, double pitch, double roll)
{
    osg::Matrixd mat;

    mat.preMultRotate( osg::Quat(yaw,osg::Vec3f(0,0,1)) );
    mat.preMultRotate( osg::Quat(pitch,osg::Vec3f(1,0,0)) );
    mat.preMultRotate( osg::Quat(roll,osg::Vec3f(0,1,0)) );

    return mat.getRotate(); 
}

class MoveEarthSkyWithEyePointTransform: public osg::Transform
{ 
public: 
    virtual bool computeLocalToWorldMatrix (osg::Matrix & matrix, osg::NodeVisitor * nv) const
    { 
        osgUtil::CullVisitor * cv = dynamic_cast <osgUtil::CullVisitor*> (nv); 

        if (nv) 
        { 
            osg::Vec3 eyePointLocal = cv-> getEyeLocal (); 
            matrix.preMult (osg::Matrix::translate (eyePointLocal)); 
        } 

        return true; 
    }

    virtual bool computeWorldToLocalMatrix (osg :: Matrix & matrix, osg :: NodeVisitor * nv) const
    { 
        osgUtil::CullVisitor * cv = dynamic_cast <osgUtil::CullVisitor*> (nv); 

        if (nv)
        { 
            osg :: Vec3 eyePointLocal = cv-> getEyeLocal (); 
            matrix.postMult (osg :: Matrix :: translate (-eyePointLocal)); 
        }

        return true; 
    } 
};

class WalkManipulator: public osgGA::FirstPersonManipulator
{
public:
    virtual bool handleKeyDown(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us) override;
    virtual bool handleKeyUp(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us) override;
    virtual bool handleFrame(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us) override;

protected:
    double mLastFrameTime;

    int mForw;
    int mRight;
    int mUp;
    bool mShift;
};

bool WalkManipulator::handleKeyDown(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
    FirstPersonManipulator::handleKeyDown(ea,us);

    switch (std::tolower(ea.getKey()))
    {
        case 'w': mForw = 1; break;
        case 's': mForw = -1; break;
        case 'd': mRight = 1; break;
        case 'a': mRight = -1; break;
        case 'e': mUp = 1; break;
        case 'q': mUp = -1; break;
        case 65505: mShift = true; break; 
        default: break;
    }

    return true;
}

bool WalkManipulator::handleKeyUp(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
    FirstPersonManipulator::handleKeyUp(ea,us);

    switch (std::tolower(ea.getKey()))
    {
        case 'w': mForw = 0; break;
        case 's': mForw = 0; break;
        case 'd': mRight = 0; break;
        case 'a': mRight = 0; break;
        case 'e': mUp = 0; break;
        case 'q': mUp = 0; break;
        case 65505: mShift = false; break; 
        default: break;
    }

    return true;
}

bool WalkManipulator::handleFrame(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
    FirstPersonManipulator::handleFrame(ea,us);

    double timeNow = ea.getTime();
    double dt = timeNow - mLastFrameTime;
    double dist = _maxVelocity * dt * (mShift ? 5.0 : 1.0);

    moveForward(mForw * dist);
    moveRight(mRight * dist);
    moveUp(mUp * dist);

    mLastFrameTime = timeNow;
}

}

#endif
