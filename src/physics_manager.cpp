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
    for (int i = mBulletWorld->getNumCollisionObjects() - 1; i >= 0; i--)
    {
        btCollisionObject * obj = mBulletWorld->getCollisionObjectArray()[i];
        btRigidBody * body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
            delete body->getMotionState();
        }
        mBulletWorld->removeCollisionObject(obj);
        delete obj;
    }

    for (int j = 0; j < mCollisionShapes.size(); j++)
    {
        btCollisionShape* shape = mCollisionShapes[j];
        mCollisionShapes[j] = nullptr;
        delete shape;
    }

    delete mBulletBroadphase;
    delete mBulletCollisionConfig;
    delete mBulletCollisionDispatcher;
    delete mBulletSolver;
    delete mBulletWorld;
}

void PhysicsManager::update(float elapsedSeconds)
{
    mBulletWorld->stepSimulation(elapsedSeconds, 10);
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
    rigidBody->setUserIndex(mMotionStates.size());
    mBulletWorld->addRigidBody(rigidBody);

    mMotionStates.push_back(motionState);
    mCollisionShapes.push_back(collisionShape);
    mRigidBodies.push_back(rigidBody);

    return mMotionStates.size() - 1;
}

int PhysicsManager::addPlane(glm::vec3 normal, float planeConstant)
{
     
    btDefaultMotionState* motionState = new btDefaultMotionState();
    // TODO
    btCollisionShape* collisionShape = new btStaticPlaneShape(btVector3(0, 1, 0), -0.5);
    btRigidBody* rigidBody = new btRigidBody(0, motionState, collisionShape);
    mBulletWorld->addRigidBody(rigidBody);

    mMotionStates.push_back(motionState);
    mCollisionShapes.push_back(collisionShape);
    mRigidBodies.push_back(rigidBody);
    return mMotionStates.size() - 1;
}

btRigidBody* PhysicsManager::pickBody(const glm::vec3& rayFromWorld, const glm::vec3& rayToWorld)
{
    if (mBulletWorld == nullptr)
        return nullptr;

    btCollisionWorld::ClosestRayResultCallback rayCallback(to_bullet(rayFromWorld), to_bullet(rayToWorld));

    rayCallback.m_flags |= btTriangleRaycastCallback::kF_UseGjkConvexCastRaytest;
    mBulletWorld->rayTest(to_bullet(rayFromWorld), to_bullet(rayToWorld), rayCallback);

    if (rayCallback.hasHit())
    {
        btVector3 pickPos = rayCallback.m_hitPointWorld;
        btRigidBody* body = (btRigidBody*)btRigidBody::upcast(rayCallback.m_collisionObject);

        if (body)
        {
            // other exclusions?
            if (!(body->isStaticObject() || body->isKinematicObject()))
            {
                return body;
            }
        }
    }
    return nullptr;
}

int PhysicsManager::addMesh(const std::string &filename)
{
    btTriangleMesh *triangleMesh = new btTriangleMesh();
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
    auto trimeshShape = new btBvhTriangleMeshShape(triangleMesh, true);
    btRigidBody* rigidBody = new btRigidBody(0, motionState, trimeshShape);
    mBulletWorld->addRigidBody(rigidBody);
    return 0;
}

bool PhysicsManager::getTransformation(int index, glm::mat4& transform)
{
    if (mMotionStates.size() <= index)
        return false;

    btTransform t;
    mMotionStates[index]->getWorldTransform(t);
    transform = to_glm(t);
    return true;
}


