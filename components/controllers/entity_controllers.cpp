#include <controllers/entity_controllers.hpp>

namespace MFGame
{

EntityController::EntityController(SpatialEntity *entity)
{
    mEntity = entity;
}

void EntityController::setRelativeVelocityVector(MFMath::Vec3 vector)
{
    auto rot = mEntity->getRotation();
    MFMath::Mat4 m = MFMath::rotationMatrix(rot);
    vector = m.preMult(vector);
    mEntity->setVelocity(vector);
}

void CharacterEntityController::instantRotate(MFMath::Quat newRotation)
{
    mEntity->setRotation(newRotation);
    applyCurrentMotion();
}

bool CharacterEntityController::isOnGround()
{
    // Cast a ray downwards and measure the distance.
    double distance = mPhysicsWorld->castRay(mEntity->getPosition(),MFMath::Vec3(0,0,-1));
    return distance < mEntity->getSize().z / 2.0 + 0.1;
}

void CharacterEntityController::applyCurrentMotion()
{
    MFMath::Vec3 movementVec = mMovementVector * mSpeeds[mMovementState];
    movementVec.z = mEntity->getVelocity().z;
    setRelativeVelocityVector(movementVec);
}

CharacterEntityController::CharacterEntityController(SpatialEntity *entity, MFPhysics::PhysicsWorld *physicsWorld): EntityController(entity)
{
    mPhysicsWorld = physicsWorld;
    mMovementState = WALK;
    mMovementVector = MFMath::Vec3(0,0,0);
    setSpeeds(2,5,10);
}

void CharacterEntityController::moveLeft(bool start)
{
    mMovementVector.x = start ? -1 : 0;
    applyCurrentMotion();
}

void CharacterEntityController::moveRight(bool start)
{
    mMovementVector.x = start ? 1 : 0;
    applyCurrentMotion();
}

void CharacterEntityController::moveForward(bool start)
{
    mMovementVector.y = start ? 1 : 0;
    applyCurrentMotion();
}

void CharacterEntityController::moveBackward(bool start)
{
    mMovementVector.y = start ? -1 : 0;
    applyCurrentMotion();
}

void CharacterEntityController::jump()
{
    if (isOnGround())
        mEntity->setVelocity(mEntity->getVelocity() + MFMath::Vec3(0,0,4));
}

class CharacterKeyCallback: public MFInput::KeyInputCallback
{
public:
    CharacterKeyCallback(PlayerEntityController *controller)
    {
        mController = controller;
    }

    virtual void call(bool down, unsigned int keyCode) override
    {
        switch (keyCode)
        {
            case 26: mController->moveForward(down); break;
            case 22: mController->moveBackward(down); break;
            case 4:  mController->moveLeft(down); break;
            case 7:  mController->moveRight(down); break;
            case 44: if (down) mController->jump(); break;
            case 225:
                mController->setMovementState(down
                    ? CharacterEntityController::RUN :
                      CharacterEntityController::WALK);
                break;
            case 224:
                mController->setMovementState(down
                    ? CharacterEntityController::CROUCH :
                      CharacterEntityController::WALK);
                break;
            default: break;
        }
    }

protected:
    CharacterEntityController *mController;
};

void PlayerEntityController::update(double dt)
{
    mCameraController->update(dt);
    MFMath::Vec2 camRot = mCameraController->getRotation();
    instantRotate(MFMath::Vec3(camRot.x - MFMath::PI / 2.0,0,0).toQuat());
}

PlayerEntityController::PlayerEntityController(MFGame::SpatialEntity *playerEntity, MFRender::Renderer *renderer, MFInput::InputManager *inputManager, MFPhysics::PhysicsWorld *physicsWorld):
    CharacterEntityController(playerEntity, physicsWorld)
{
    mRenderer = renderer;
    mInputManager = inputManager;

    mCameraController = new OrbitEntityCameraController(renderer,inputManager,playerEntity,physicsWorld);
    mCameraController->setRelativeOffset(MFMath::Vec3(0,0,1.5));

    std::shared_ptr<CharacterKeyCallback> cb = std::make_shared<CharacterKeyCallback>(this);
    mInputManager->addKeyCallback(cb);
}

PlayerEntityController::~PlayerEntityController()
{
    delete mCameraController;
}

}   // namespace
