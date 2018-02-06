#ifndef ENTITY_CONTROLLERS_H
#define ENTITY_CONTROLLERS_H

#include <spatial_entity/spatial_entity.hpp>
#include <physics/base_physics_world.hpp>
#include <renderer/base_renderer.hpp>
#include <input/base_input_manager.hpp>
#include <controllers/camera_controllers.hpp>

namespace MFGame
{

class EntityController
{
public:
    EntityController(SpatialEntity *entity);

    /**
      Set the velocity vector relatively to where the entity is facing.
    */

    void setRelativeVelocityVector(MFMath::Vec3 vector);
    SpatialEntity *getEntity()    { return mEntity;   };

protected:
    SpatialEntity *mEntity;
};

class CharacterEntityController: public EntityController
{
public:
    typedef enum
    {
        WALK = 0,
        RUN,
        CROUCH,
    } MovementState;

    CharacterEntityController(SpatialEntity *entity, MFPhysics::PhysicsWorld *physicsWorld);

    void setSpeeds(float crouch, float walk, float run)
    {
        mSpeeds[WALK] = walk;
        mSpeeds[RUN] = run;
        mSpeeds[CROUCH] = crouch;
    }

    void setMovementState(CharacterEntityController::MovementState newState)
    {
        mMovementState = newState;
    }

    void jump();
    bool isOnGround();
    virtual void moveLeft(bool start=true);
    virtual void moveRight(bool start=true);
    virtual void moveForward(bool start=true);
    virtual void moveBackward(bool start=true);

    void instantRotate(MFMath::Quat newRotation);
    void applyCurrentMotion();

protected:
    MFPhysics::PhysicsWorld *mPhysicsWorld;

    MovementState mMovementState;
    MFMath::Vec3 mMovementVector;
    float mSpeeds[3];
};

class PlayerEntityController: public CharacterEntityController
{
public:
    PlayerEntityController(MFGame::SpatialEntity *playerEntity, MFRender::Renderer *renderer, MFInput::InputManager *inputManager, MFPhysics::PhysicsWorld *physicsWorld=0);
    virtual ~PlayerEntityController();
    void update(double dt);

protected:
    OrbitEntityCameraController *mCameraController;
    MFRender::Renderer *mRenderer;
    MFInput::InputManager *mInputManager;
};

}   // namespace

#endif // ENTITY_CONTROLLER_H
