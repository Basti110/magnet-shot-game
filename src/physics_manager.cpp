#include "physics_manager.h"
#include <btBulletDynamicsCommon.h> // bullet physics
#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"
#include "bullet_helper.hh"

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
    mBulletWorld->setGravity(btVector3(0, -10, 0)); // set initial gravity
}

PhysicsManager::~PhysicsManager()
{
    delete mBulletBroadphase;
    delete mBulletCollisionConfig;
    delete mBulletCollisionDispatcher;
    delete mBulletSolver;
    delete mBulletWorld;
}

void PhysicsManager::update(float elapsedSeconds)
{
    // TODO
    for (auto r : mRedMagnets)
    {
        auto pr = r->getCenterOfMassPosition();
        r->clearForces();
        r->setLinearVelocity({0, 0, 0});
        r->setAngularVelocity({0, 0, 0});
        for (auto b : mBlueMagnets)
        {
            auto pb = b->getCenterOfMassPosition();
            // r->setLinearVelocity((pb - pr) * 20);
            glm::vec3 glmB = to_glm(pb);
            glm::vec3 glmR = to_glm(pr);
            float d = glm::distance(glmB, glmR);
            r->applyCentralForce((to_bullet(glm::normalize(glmB - glmR)) * 100) / glm::max(d, 1.0f));
            // r->setLinearVelocity((pb - pr) * 20);
        }
    }
    for (auto b : mBlueMagnets)
    {
        auto pb = b->getCenterOfMassPosition();
        b->clearForces();
        b->setLinearVelocity({0, 0, 0});
        b->setAngularVelocity({0, 0, 0});
        for (auto r : mRedMagnets)
        {
            auto pr = r->getCenterOfMassPosition();
            glm::vec3 glmB = to_glm(pb);
            glm::vec3 glmR = to_glm(pr);
            float d = glm::distance(glmB, glmR);
            // r->setLinearVelocity((pr - pb) * 20);
            b->applyCentralForce((to_bullet(glm::normalize(glmR - glmB)) * 100) / glm::max(d, 1.0f));
            // r->setLinearVelocity((pb - pr) * 20);
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
    // TODO: More Cube info
    btDefaultMotionState* motionState = new btDefaultMotionState(to_bullet(transform));
    btCollisionShape* collisionShape = new btBoxShape(to_bullet(shape));
    // auto const mass = 1.0f;
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

int PhysicsManager::addCapsule(const glm::vec2& shape, const glm::mat4& transform, const RigidBodyInfo& info)
{
    btDefaultMotionState* motionState = new btDefaultMotionState(to_bullet(transform));
    btCollisionShape* collisionShape = new btCapsuleShape(shape.x, shape.y);
    // btCollisionShape* collisionShape = new btBoxShape(to_bullet(shape));
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

void PhysicsManager::addMagnet(btRigidBody* body, bool red) {
    auto& magnets = red ? mRedMagnets : mBlueMagnets;
    magnets.push_back(body);
}

void PhysicsManager::clearMagnets()
{
    mRedMagnets.clear();
    mBlueMagnets.clear();
}

btDiscreteDynamicsWorld* PhysicsManager::getDynamicsWorld()
{
    return mBulletWorld;
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

bool PhysicsManager::bodyWithinReach(const glm::vec3& pos, const glm::vec3& rayToWorld, float distance, btRigidBody* excludeObject)
{
    btVector3 from = to_bullet(pos);
    btVector3 to = to_bullet(rayToWorld);

    btCollisionWorld::AllHitsRayResultCallback res(from, to);
    res.m_flags |= btTriangleRaycastCallback::kF_UseGjkConvexCastRaytest;
    res.m_collisionFilterGroup |= GROUP_STATIC_OBJECTS;
    res.m_collisionFilterGroup |= GROUP_DYNAMIC_OBJECTS;
    mBulletWorld->rayTest(from, to, res);
    for (int i = 0; i < res.m_hitPointWorld.size(); ++i)
    {
        if (res.m_collisionObjects[i] != excludeObject)
        {
            btVector3 a = res.m_hitPointWorld[i];
            glm::vec3 hp(a.x(), a.y(), a.z());

            float d = glm::distance(pos, hp);
            if (d < distance)
                return true;
        }
    }
    return false;
}

btRigidBody* PhysicsManager::getRigidBody(int id)
{
    if (id < 0 || id > mRigidBodies.size())
        return nullptr;
    return mRigidBodies[id];
}

bool PhysicsManager::getTransformation(int idx, glm::mat4& transform)
{
    if (idx < 0 || (size_t)idx >= mRigidBodies.size() || !mRigidBodies[idx])
    {
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
    // mBulletWorld->addRigidBody(body);
    mRigidBodies.push_back(body);
    return mRigidBodies.size() - 1;
}
