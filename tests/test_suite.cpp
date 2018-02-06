/*
  These are tests that DO NOT require the original game data.
*/

#include <tests_general.hpp>

#include <utils/math.hpp>
#include <engine/engine.hpp>

bool testMath()
{
    printSubHeader("Math");

    auto testAngle = MFMath::Vec3(0.1,0.2,0.3);
    auto toQuatAndBack = testAngle.toQuat().toEuler();
    const double err = 0.01;

    ass(std::abs(testAngle.x - toQuatAndBack.x) <= err && std::abs(testAngle.y - toQuatAndBack.y) <= err && std::abs(testAngle.z - toQuatAndBack.z) <= err);
    ass(std::abs(MFMath::length(MFMath::Vec3(-1,1,2)) - 2.4494) <= 0.001);

    return getNumErrors() == 0;
}

bool testEngine()
{
    printSubHeader("Engine");

    message("Create engine.");
    MFGame::Engine::EngineSettings settings;
    MFGame::Engine *testEngine = new MFGame::Engine(settings);

    auto entityManager = testEngine->getSpatialEntityManager();
    auto entityFactory = testEngine->getSpatialEntityFactory();

    ass(entityManager->getEntityById(10) == 0);

    message("Create test entities.");
    MFGame::SpatialEntity::Id entity1Id = entityFactory->createTestBallEntity();
    MFGame::SpatialEntity::Id entity2Id = entityFactory->createTestBoxEntity();
    MFGame::SpatialEntity::Id entity3Id = entityFactory->createTestBoxEntity();

    ass(entityManager->getEntityById(entity1Id) != 0);

    auto entity1 = entityManager->getEntityById(entity1Id);
    auto entity2 = entityManager->getEntityById(entity2Id);
    auto entity3 = entityManager->getEntityById(entity3Id);

    ass(entityManager->getEntityById(MFGame::SpatialEntity::NullId) == 0);

    // Put one rigid box above another, static box.
    MFMath::Vec3 staticInitPos = MFMath::Vec3(100,10,10);
    entity2->setPosition(staticInitPos);
    entity2->setPhysicsBehavior(MFGame::SpatialEntity::STATIC);
    entity3->setPosition(staticInitPos + MFMath::Vec3(0,0,20));
    entity3->setPhysicsBehavior(MFGame::SpatialEntity::RIGID);

    MFMath::Vec3 initPos = entity1->getPosition();
    message("entity position: " + initPos.str());

    static unsigned int steps = 100;

    message("Step " + std::to_string(steps) + " frames.");

    for (unsigned int i = 0; i < steps; ++i)
        testEngine->step(0.1);

    // Entity1 should have fallen down.
    MFMath::Vec3 endPos = entity1->getPosition();
    ass(std::abs(initPos.z - endPos.z) > 10.0);   // Did it fall?
    message("entity position: " + endPos.str());

    // Entity2 box shouldn't have moved (is static) and entity3 box should be above (on top of it).
    ass(MFMath::distance(staticInitPos,entity2->getPosition()) < 0.001);
    ass(entity2->getPosition().z < entity3->getPosition().z);

    message("Delete engine.");
    delete testEngine;

    return getNumErrors() == 0;
}

int main()
{
    printHeader("OPENMF TEST SUITE");   

    testMath();
    testEngine();

    printHeader("TEST RESULTS");
    message("errors: " + std::to_string(getNumErrors()));

    return getNumErrors() == 0 ? 0 : 1;
}
