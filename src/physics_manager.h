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
class btTypedConstraint;


enum CollisionGroup
{
    GROUP_NONE            = 0,
    GROUP_PLAYER          = 1,
    GROUP_STATIC_OBJECTS  = 1 << 1,
    GROUP_DYNAMIC_OBJECTS = 1 << 2,
};

struct RigidBodyInfo
{
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

    int addPlane(glm::vec3 normal, float planeConstant);
    int addCube(const glm::vec3& shape, const glm::mat4& transform, const RigidBodyInfo& info);
    int addMesh(const std::string &filename);
    int addMagnet(const glm::mat4& transform, float radius, int parent, bool red);
    int addConstraint(btTypedConstraint* constraint);

    void clearMagnets();

    btRigidBody* pickBody(const glm::vec3& rayFromWorld, const glm::vec3& rayToWorld);
    btRigidBody* pickBody(const glm::vec3& rayFromWorld, const glm::vec3& rayToWorld, glm::vec3& isect);
    btRigidBody* getRigidBody(int id);

    bool getTransformation(int idx, glm::mat4& transform);

private:
    int addRigidBody(btRigidBody* body, short group = GROUP_NONE, short mask = GROUP_NONE);
    void removeRigidBody(btRigidBody* body);
    void removeRigidBody(int idx);

private:
    btBroadphaseInterface* mBulletBroadphase = nullptr;
    btDefaultCollisionConfiguration* mBulletCollisionConfig = nullptr;
    btCollisionDispatcher* mBulletCollisionDispatcher = nullptr;
    btSequentialImpulseConstraintSolver* mBulletSolver = nullptr;
    btDiscreteDynamicsWorld* mBulletWorld = nullptr;

    std::vector<btRigidBody*> mRigidBodies;
    std::vector<btTypedConstraint*> mConstraints;

    std::vector<btRigidBody*> mRedMagnets;
    std::vector<btRigidBody*> mBlueMagnets;
    std::vector<btTypedConstraint*> mMagnetConstraints;
};

