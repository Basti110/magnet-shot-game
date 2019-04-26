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

struct RigidBodyInfo {
    glm::vec3 shape = glm::vec3(1);
    float mass = 0;
    float friction = 0;
    float restitution = 0;
    float linearDamping = 0;
    float angularDamping = 0;
};

class PhysicsManager
{
public:
    PhysicsManager();
    ~PhysicsManager();
    void update(float elapsedSeconds);

    int addCube(const glm::vec3& shape, const glm::mat4& transform, const RigidBodyInfo& info);
    int addPlane(glm::vec3 normal, float planeConstant);
    int addMesh(const std::string &filename);

    bool getTransformation(int index, glm::mat4& transform);
    int pickBody(const glm::vec3& rayFromWorld, const glm::vec3& rayToWorld);

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

