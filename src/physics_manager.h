#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>

class btCollisionShape;
class btRigidBody;
struct btDefaultMotionState;
class btBroadphaseInterface;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btDiscreteDynamicsWorld;
class btSequentialImpulseConstraintSolver;


class PhysicsManager
{
public:
    PhysicsManager();
    ~PhysicsManager();
    void update(float elapsedSeconds);

    int addCube(glm::vec3 shape, float mass, const glm::mat4& transform);
    int addPlane(glm::vec3 normal, float planeConstant);
    int addMesh(const std::string &filename);

    bool getTransformation(int index, glm::mat4& transform);

private: // bullet physics
    btBroadphaseInterface* mBulletBroadphase = nullptr;
    btDefaultCollisionConfiguration* mBulletCollisionConfig = nullptr;
    btCollisionDispatcher* mBulletCollisionDispatcher = nullptr;
    btSequentialImpulseConstraintSolver* mBulletSolver = nullptr;
    btDiscreteDynamicsWorld* mBulletWorld = nullptr;

    std::vector<btCollisionShape*> mCollisionShapes;
    std::vector<btRigidBody*> mRigidBodies;
    std::vector<btDefaultMotionState*> mMotionStates;
};

