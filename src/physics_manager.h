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
class DebugDrawer;


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

    void addMagnet(btRigidBody* body, bool red);
    void clearMagnets();
    void deleteId(int id);
    void renderDebug(glm::mat4& projection, glm::mat4& view, float updateRate);

    btDiscreteDynamicsWorld* getDynamicsWorld();
    btRigidBody* pickBody(const glm::vec3& rayFromWorld, const glm::vec3& rayToWorld);
    btRigidBody* pickBody(const glm::vec3& rayFromWorld, const glm::vec3& rayToWorld, glm::vec3& isect);
    bool bodyWithinReach(const glm::vec3& pos, const glm::vec3& rayToWorld, float distance, btRigidBody* excludeObject);

    int addPlane(glm::vec3 normal, float planeConstant);
    btRigidBody* addCube(const glm::vec3& shape, const glm::mat4& transform, const RigidBodyInfo& info);
    int addCapsule(const glm::vec2& shape, const glm::mat4& transform, const RigidBodyInfo& info);
    btRigidBody* addCoordinateAxisCone(const glm::vec2& shape, const glm::mat4& transform, const RigidBodyInfo& info);
    int addRigidBody(btRigidBody* body, short group = GROUP_NONE, short mask = GROUP_NONE);
    bool getTransformation(int idx, glm::mat4& transform);
    btRigidBody* getRigidBody(int id);

private:
    btBroadphaseInterface* mBulletBroadphase = nullptr;
    btDefaultCollisionConfiguration* mBulletCollisionConfig = nullptr;
    btCollisionDispatcher* mBulletCollisionDispatcher = nullptr;
    btSequentialImpulseConstraintSolver* mBulletSolver = nullptr;
    btDiscreteDynamicsWorld* mBulletWorld = nullptr;
    btDiscreteDynamicsWorld* mBulletCoordinateAxes = nullptr;
    DebugDrawer* mDebugDrawer = nullptr;

    std::vector<btRigidBody*> mRigidBodies;
    std::vector<btRigidBody*> mRedMagnets;
    std::vector<btRigidBody*> mBlueMagnets;

    float mLastPhysicsUpdate = 0;
};
