#include <iostream>
#include <engine/engine.hpp>
#include <controllers/entity_controllers.hpp>
#include <cxxopts.hpp>

class OpenMFEngine: public MFGame::Engine
{
public:
    OpenMFEngine(MFGame::Engine::EngineSettings settings): MFGame::Engine(settings)
    {
        mPlayerEntity = mSpatialEntityManager->getEntityById(mSpatialEntityFactory->createCapsuleEntity("tommy.4ds"));
        mPlayerEntityController = new MFGame::PlayerEntityController(mPlayerEntity,mRenderer,mInputManager,mPhysicsWorld);
        mSpatialEntityFactory->setDebugMode(false);
    };

    virtual ~OpenMFEngine()
    {
        delete mPlayerEntityController;
    };

    virtual void frame() override
    {
        mPlayerEntityController->update(mEngineSettings.mUpdatePeriod);
    };

    void setPlayerPosition(MFMath::Vec3 pos)
    {
        mPlayerEntity->setPosition(pos);
    };

protected:
    MFGame::SpatialEntity *mPlayerEntity;
    MFGame::PlayerEntityController *mPlayerEntityController;
};

int main(int argc, char** argv)
{
    MFGame::Engine::EngineSettings settings;
    settings.mInitWindowWidth = 1024;
    settings.mInitWindowHeight = 768;
    OpenMFEngine engine(settings);

    cxxopts::Options options("OpenMF","OpenMF game.");

    options.add_options()
        ("h,help","Display help and exit.")
        ("s,scenario","Set test game scenario number.",cxxopts::value<unsigned int>());

    auto arguments = options.parse(argc,argv);

    if (arguments.count("h") > 0)
    {
        std::cout << options.help() << std::endl;
        return 0;
    }

    unsigned int scenario = 0;

    if (arguments.count("s") > 0)
        scenario = arguments["s"].as<unsigned int>();

    switch (scenario)
    {
        case 1:
            engine.loadMission("freeride");
            engine.setPlayerPosition(MFMath::Vec3(-164.49,-74.50,19.91));
            break;   
        
        case 2:
            engine.loadMission("mise11-vila");
            engine.setPlayerPosition(MFMath::Vec3(104.65,172.62,-4.22));
            break;   
 
        case 3:
            engine.loadMission("mise20-galery");
            engine.setPlayerPosition(MFMath::Vec3(21.00,-9.39,7.78));
            break;   

        case 4:
            engine.loadMission("mise02-saliery");
            engine.setPlayerPosition(MFMath::Vec3(-1774.71,31.45,-2.52));
            break;
 
        default:
            engine.loadMission("tutorial");
            engine.setPlayerPosition(MFMath::Vec3(126.02,320.43,3.67));
            break;
    }

    engine.getRenderer()->setCameraParameters(true,90,0,0.25,2000);
    engine.run();

    return 0;
}
