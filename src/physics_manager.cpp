#include "physics_manager.h"
#include <btBulletDynamicsCommon.h> // bullet physics
#include "bullet_helper.hh"
#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"

#include <polymesh/Mesh.hh>
#include <polymesh/algorithms/properties.hh>
#include <polymesh/fields.hh>
#include <polymesh/formats/obj.hh>


PhysicsManager::PhysicsManager()
{
    mBulletBroadphase = new btDbvtBroadphase;
    mBulletCollisionConfig = new btDefaultCollisionConfiguration;
    mBulletCollisionDispatcher = new btCollisionDispatcher(mBulletCollisionConfig);
    mBulletSolver = new btSequentialImpulseConstraintSolver;
    mBulletWorld = new btDiscreteDynamicsWorld(mBulletCollisionDispatcher, mBulletBroadphase, mBulletSolver, mBulletCollisionConfig);
    mBulletWorld->setGravity(btVector3(0, -20, 0)); // set initial gravity
}

PhysicsManager::~PhysicsManager()
{
    for (size_t i = 0; i < mRigidBodies.size(); ++i) {
        removeRigidBody(i);
    }
    for (auto c : mMagnetConstraints) {
        mBulletWorld->removeConstraint(c);
        delete c;
    }

    delete mBulletBroadphase;
    delete mBulletCollisionConfig;
    delete mBulletCollisionDispatcher;
    delete mBulletSolver;
    delete mBulletWorld;
}

void PhysicsManager::update(float elapsedSeconds)
{
    // TODO
    for (auto r : mRedMagnets) {
        auto pr = r->getCenterOfMassPosition();
        for (auto b : mBlueMagnets) {
            auto pb = b->getCenterOfMassPosition();
            r->applyCentralForce(pb - pr);
        }
    }
    for (auto b : mBlueMagnets) {
        auto pb = b->getCenterOfMassPosition();
        for (auto r : mRedMagnets) {
            auto pr = r->getCenterOfMassPosition();
            b->applyCentralForce(pr - pb);
        }
    }

    mBulletWorld->stepSimulation(elapsedSeconds, 10);
}

int PhysicsManager::addPlane(glm::vec3 normal, float planeConstant)
{
    btDefaultMotionState* motionState = new btDefaultMotionState();
    btCollisionShape* collisionShape = new btStaticPlaneShape(to_bullet(normal), planeConstant);
    btRigidBody* rigidBody = new btRigidBody(0, motionState, collisionShape);
    return addRigidBody(rigidBody, GROUP_STATIC_OBJECTS, GROUP_DYNAMIC_OBJECTS);
}

int PhysicsManager::addCube(const glm::vec3& shape, const glm::mat4& transform, const RigidBodyInfo& info)
{
    //TODO: More Cube info
    btDefaultMotionState* motionState = new btDefaultMotionState(to_bullet(transform));
    btCollisionShape* collisionShape = new btBoxShape(to_bullet(shape));
    //auto const mass = 1.0f;
    btVector3 inertia;
    collisionShape->calculateLocalInertia(info.mass, inertia);
    auto btInfo = btRigidBody::btRigidBodyConstructionInfo(info.mass, motionState, collisionShape, inertia);
    btInfo.m_friction = info.friction;
    btInfo.m_restitution = info.restitution;
    btInfo.m_linearDamping = info.linearDamping;
    btInfo.m_angularDamping = info.angularDamping;
    btRigidBody* rigidBody = new btRigidBody(btInfo);

    return addRigidBody(rigidBody, GROUP_DYNAMIC_OBJECTS, GROUP_STATIC_OBJECTS | GROUP_DYNAMIC_OBJECTS);
}

int PhysicsManager::addMesh(const std::string &filename)
{
    btTriangleMesh* triangleMesh = new btTriangleMesh();
    pm::Mesh m;
    pm::obj_reader<float> obj_reader(filename, m);
    auto pos = obj_reader.get_positions().map([](std::array<float, 4> const& t) { return btVector3(t[0], t[1], t[2]); });

    for (auto f : m.faces()) {
        std::vector<btVector3> verts;
        for (auto v : f.vertices()) {
            verts.push_back(pos[v]);
        }
        triangleMesh->addTriangle(verts[0], verts[1], verts[2]);
    }

    btDefaultMotionState* motionState = new btDefaultMotionState();
    btCollisionShape* collisionShape = new btBvhTriangleMeshShape(triangleMesh, true);
    btRigidBody* rigidBody = new btRigidBody(0, motionState, collisionShape);

    return addRigidBody(rigidBody, GROUP_STATIC_OBJECTS, GROUP_DYNAMIC_OBJECTS);
}

int PhysicsManager::addMagnet(const glm::mat4& transform, float radius, int parent, bool red)
{
    btRigidBody* parentRigidBody = mRigidBodies[parent];
    const float mass = parentRigidBody->isStaticObject() ? 0.0f : 0.000001f;

    btDefaultMotionState* motionState = new btDefaultMotionState(to_bullet(transform));
    btCollisionShape* collisionShape = new btSphereShape(radius);
    btRigidBody* magnetRigidBody = new btRigidBody(mass, motionState, collisionShape);

    auto& magnets = red ? mRedMagnets : mBlueMagnets;
    magnets.push_back(magnetRigidBody);

    if (!parentRigidBody->isStaticObject()) {
        auto magnetCenter = magnetRigidBody->getCenterOfMassPosition();
        auto pivotInParent = parentRigidBody->getWorldTransform().inverse() * magnetCenter;
        auto pivotInMagnet = btVector3(0,0,0);

        auto* constraint = new btPoint2PointConstraint(*parentRigidBody, *magnetRigidBody, pivotInParent, pivotInMagnet);
        mBulletWorld->addConstraint(constraint, true);
        mMagnetConstraints.push_back(constraint);
    }

    return addRigidBody(magnetRigidBody, GROUP_NONE, GROUP_NONE);
}

void PhysicsManager::clearMagnets()
{
    for (auto c : mMagnetConstraints) {
        mBulletWorld->removeConstraint(c);
        delete c;
    }
    mMagnetConstraints.clear();

    for (auto m : mRedMagnets) {
        removeRigidBody(m->getUserIndex());
    }
    mRedMagnets.clear();

    for (auto m : mBlueMagnets) {
        removeRigidBody(m->getUserIndex());
    }
    mBlueMagnets.clear();
}

btRigidBody* PhysicsManager::pickBody(const glm::vec3& rayFromWorld, const glm::vec3& rayToWorld)
{
    glm::vec3 dummy;
    return pickBody(rayFromWorld, rayToWorld, dummy);
}


btRigidBody* PhysicsManager::pickBody(const glm::vec3& rayFromWorld, const glm::vec3& rayToWorld, glm::vec3& isect)
{
    if (mBulletWorld == nullptr)
        return nullptr;

    btCollisionWorld::ClosestRayResultCallback rayCallback(to_bullet(rayFromWorld), to_bullet(rayToWorld));
    rayCallback.m_flags |= btTriangleRaycastCallback::kF_UseGjkConvexCastRaytest;
    rayCallback.m_collisionFilterGroup |= GROUP_STATIC_OBJECTS;
    rayCallback.m_collisionFilterGroup |= GROUP_DYNAMIC_OBJECTS;
    mBulletWorld->rayTest(to_bullet(rayFromWorld), to_bullet(rayToWorld), rayCallback);

    if (rayCallback.hasHit())
    {
        btVector3 pickPos = rayCallback.m_hitPointWorld;
        btRigidBody* body = (btRigidBody*)btRigidBody::upcast(rayCallback.m_collisionObject);

        if (body && !body->isKinematicObject())
        {
            isect = to_glm(pickPos);
            return body;
        }
    }
    return nullptr;
}

bool PhysicsManager::getTransformation(int idx, glm::mat4& transform)
{
    if (idx < 0 || (size_t)idx >= mRigidBodies.size() || !mRigidBodies[idx]) {
        return false;
    }

    btTransform t;
    mRigidBodies[idx]->getMotionState()->getWorldTransform(t);
    transform = to_glm(t);
    return true;
}

int PhysicsManager::addRigidBody(btRigidBody* body, short group, short mask)
{
    body->setUserIndex(mRigidBodies.size());
    mBulletWorld->addRigidBody(body, group, mask);
    mRigidBodies.push_back(body);
    return mRigidBodies.size() - 1;
}

void PhysicsManager::removeRigidBody(btRigidBody* body)
{
    removeRigidBody(body->getUserIndex());
}

void PhysicsManager::removeRigidBody(int idx)
{
    if (idx < 0 || (size_t)idx >= mRigidBodies.size() || !mRigidBodies[idx]) {
        return;
    }

    btRigidBody* body = mRigidBodies[idx];
    mBulletWorld->removeRigidBody(body);
    delete body->getMotionState();
    delete body->getCollisionShape();
    delete body;
    mRigidBodies[idx] = nullptr;
}

