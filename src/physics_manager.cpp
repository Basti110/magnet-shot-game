#include "physics_manager.h"
#include "debug_drawer.h"
#include <btBulletDynamicsCommon.h> // bullet physics
#include <BulletSoftBody/btSoftBody.h>
#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>
#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"
#include "bullet_helper.hh"

#include <polymesh/Mesh.hh>
#include <polymesh/algorithms/properties.hh>
#include <polymesh/fields.hh>
#include <polymesh/formats/obj.hh>

#include <GLFW/glfw3.h>
#include <glad/glad.h>


PhysicsManager::PhysicsManager()
{
    mBulletBroadphase = new btDbvtBroadphase;
    mBulletCollisionConfig = new btDefaultCollisionConfiguration;
    mBulletCollisionDispatcher = new btCollisionDispatcher(mBulletCollisionConfig);
    mBulletSolver = new btSequentialImpulseConstraintSolver;
    mBulletWorld = new btDiscreteDynamicsWorld(mBulletCollisionDispatcher, mBulletBroadphase, mBulletSolver, mBulletCollisionConfig);
    mBulletCoordinateAxes = new btDiscreteDynamicsWorld(mBulletCollisionDispatcher, mBulletBroadphase, mBulletSolver, mBulletCollisionConfig);
    mBulletWorld->setGravity(btVector3(0, -10, 0)); // set initial gravity
    mDebugDrawer = new DebugDrawer();
    mBulletWorld->setDebugDrawer(mDebugDrawer);

    mSoftBodyWorldInfo = new btSoftBodyWorldInfo();
    mSoftBodyWorldInfo->m_dispatcher = mBulletCollisionDispatcher;

    btVector3 worldAabbMin(-1000, -1000, -1000);
    btVector3 worldAabbMax(1000, 1000, 1000);
    btBroadphaseInterface* bP = new btAxisSweep3(worldAabbMin, worldAabbMax, 32766);

    mBulletSoftworld = new btSoftRigidDynamicsWorld(mBulletCollisionDispatcher, bP, mBulletSolver, 0);
    mBulletSoftworld->getDispatchInfo().m_enableSPU = true;


    mSoftBodyWorldInfo->m_broadphase = bP;
    mSoftBodyWorldInfo->m_gravity.setValue(0, -10, 0);
    mSoftBodyWorldInfo->m_sparsesdf.Initialize();
    mSoftBodyWorldInfo->air_density = (btScalar)1.2;
    mSoftBodyWorldInfo->water_density = 0;
    mSoftBodyWorldInfo->water_offset = 0;
    mSoftBodyWorldInfo->water_normal = btVector3(0, 0, 0);
    mSoftBodyWorldInfo->m_gravity.setValue(0, -10, 0);
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
    mBulletSoftworld->stepSimulation(elapsedSeconds, 10);
}

int PhysicsManager::addPlane(glm::vec3 normal, float planeConstant)
{
    btDefaultMotionState* motionState = new btDefaultMotionState();
    btCollisionShape* collisionShape = new btStaticPlaneShape(to_bullet(normal), planeConstant);
    btRigidBody* rigidBody = new btRigidBody(0, motionState, collisionShape);
    return addRigidBody(rigidBody, GROUP_STATIC_OBJECTS, GROUP_DYNAMIC_OBJECTS);
}

btRigidBody* PhysicsManager::addCube(const glm::vec3& shape, const glm::mat4& transform, const RigidBodyInfo& info)
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

    addRigidBody(rigidBody, GROUP_DYNAMIC_OBJECTS, GROUP_STATIC_OBJECTS | GROUP_DYNAMIC_OBJECTS);
    return rigidBody;
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

btRigidBody* PhysicsManager::addCoordinateAxisCone(const glm::vec2& shape, const glm::mat4& transform, const RigidBodyInfo& info)
{
    btDefaultMotionState* motionState = new btDefaultMotionState(to_bullet(transform));
    btCollisionShape* collisionShape = new btConeShape(shape.x, shape.y); // new btCapsuleShape(shape.x, shape.y);
    // btCollisionShape* collisionShape = new btBoxShape(to_bullet(shape));
    btVector3 inertia;
    collisionShape->calculateLocalInertia(info.mass, inertia);
    auto btInfo = btRigidBody::btRigidBodyConstructionInfo(info.mass, motionState, collisionShape, inertia);
    btInfo.m_friction = info.friction;
    btInfo.m_restitution = info.restitution;
    btInfo.m_linearDamping = info.linearDamping;
    btInfo.m_angularDamping = info.angularDamping;
    btRigidBody* rigidBody = new btRigidBody(btInfo);

    //mBulletCoordinateAxes->addRigidBody(rigidBody, GROUP_DYNAMIC_OBJECTS, GROUP_STATIC_OBJECTS | GROUP_DYNAMIC_OBJECTS);

    addRigidBody(rigidBody, GROUP_DYNAMIC_OBJECTS, GROUP_STATIC_OBJECTS | GROUP_DYNAMIC_OBJECTS);
    return rigidBody;
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

void PhysicsManager::deleteId(int id) 
{

}

void PhysicsManager::drawSoftBody(btSoftBody* psb)
{
    const btScalar scl = (btScalar)0.1;
    const btVector3 lcolor = btVector3(0, 0, 0);
    int i;

    for (i = 0; i < psb->m_nodes.size(); ++i)
    {
        const btSoftBody::Node& n = psb->m_nodes[i];
        if (0 == (n.m_material->m_flags & btSoftBody::fMaterial::DebugDraw))
            continue;
        mDebugDrawer->drawLine(n.m_x - btVector3(scl, 0, 0), n.m_x + btVector3(scl, 0, 0), btVector3(1, 0, 0));
        mDebugDrawer->drawLine(n.m_x - btVector3(0, scl, 0), n.m_x + btVector3(0, scl, 0), btVector3(0, 1, 0));
        mDebugDrawer->drawLine(n.m_x - btVector3(0, 0, scl), n.m_x + btVector3(0, 0, scl), btVector3(0, 0, 1));
    }

    for (i = 0; i < psb->m_links.size(); ++i)
    {
        const btSoftBody::Link& l = psb->m_links[i];
        if (0 == (l.m_material->m_flags & btSoftBody::fMaterial::DebugDraw))
            continue;
        mDebugDrawer->drawLine(l.m_n[0]->m_x, l.m_n[1]->m_x, lcolor);
    }
}

void PhysicsManager::renderDebug(glm::mat4& projection, glm::mat4& view, float updateRate)
{
    bool laodNew = (glfwGetTime() - mLastPhysicsUpdate) > updateRate;
    if (laodNew)
    {
        mDebugDrawer->resetLines();
        mBulletWorld->debugDrawWorld();
        for (int i = 0; i < mBulletSoftworld->getSoftBodyArray().size(); i++)
        {
            btSoftBody* psb = (btSoftBody*)mBulletSoftworld->getSoftBodyArray()[i];
            drawSoftBody(psb);
        }
        mLastPhysicsUpdate = glfwGetTime();
    }       
    mDebugDrawer->draw(projection, view, laodNew);
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
    if (id < 0 || (size_t)id > mRigidBodies.size())
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
