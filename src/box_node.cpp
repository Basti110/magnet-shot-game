#include "box_node.h"
#include "bullet_helper.hh"
#include "load_mesh.h"

#include <glm/gtc/matrix_transform.hpp>


BoxNode::BoxNode(const glm::mat4& transform, const glm::vec3& scale, PhysicsManager* physics, short collisionGroup, short collisionMask, float mass) :
    PhysicsNode(physics),
    mScale(scale)
{
    setLocalTransformation(transform);
    mVertexArray = load_mesh_from_obj("../../data/meshes/UnitCube.obj", false);

    btDefaultMotionState* motionState = new btDefaultMotionState(to_bullet(transform));
    btCollisionShape* collisionShape = new btBoxShape(to_bullet(scale));
    mRigidBody = new btRigidBody(mass, motionState, collisionShape);
    mWorld->addRigidBody(mRigidBody, collisionGroup, collisionMask);
}

void BoxNode::update(float elapsedSeconds)
{
    PhysicsNode::update(elapsedSeconds);
    mGlobalTransformation = glm::scale(mGlobalTransformation, mScale);
}

const glm::vec3& BoxNode::getScale()
{
    return mScale;
}
