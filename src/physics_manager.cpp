#include "physics_manager.h"
#include <btBulletDynamicsCommon.h> // bullet physics
#include "bullet_helper.hh"


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

int PhysicsManager::addCube(glm::vec3 shape, float mass, const glm::mat4& transform)
{
    btDefaultMotionState* motionState = new btDefaultMotionState(to_bullet(transform));
    btCollisionShape* collisionShape = new btBoxShape(btVector3(1, 1, 1));
    //auto const mass = 1.0f;
    btVector3 inertia;
    collisionShape->calculateLocalInertia(mass, inertia);
    auto info = btRigidBody::btRigidBodyConstructionInfo(mass, motionState, collisionShape, inertia);
    info.m_friction = 0.1f;
    info.m_restitution = 0.0f;
    info.m_linearDamping = 0.01f;
    info.m_angularDamping = 0.01f;
    btRigidBody* rigidBody = new btRigidBody(info);
    mBulletWorld->addRigidBody(rigidBody);

    mMotionStates.push_back(motionState);
    mCollisionShapes.push_back(collisionShape);
    mRigidBodies.push_back(rigidBody);

    return mMotionStates.size() - 1;
}

int PhysicsManager::addPlane(glm::vec3 normal, float planeConstant)
{
    btDefaultMotionState* motionState = new btDefaultMotionState();
    btCollisionShape* collisionShape = new btStaticPlaneShape(btVector3(0, 1, 0), -3);
    btRigidBody* rigidBody = new btRigidBody(0, motionState, collisionShape);
    mBulletWorld->addRigidBody(rigidBody);

    mMotionStates.push_back(motionState);
    mCollisionShapes.push_back(collisionShape);
    mRigidBodies.push_back(rigidBody);
    return mMotionStates.size() - 1;
}

bool PhysicsManager::getTransformation(int index, glm::mat4& transform)
{
    if (mMotionStates.size() >= index)
        return false;

    btTransform t;
    mMotionStates[index]->getWorldTransform(t);
    transform = to_glm(t);
    return true;
}


