#include "sphere_node.h"
#include "bullet_helper.hh"

#include <glow-extras/geometry/UVSphere.hh>
#include <glm/gtc/matrix_transform.hpp>


SphereNode::SphereNode(const glm::vec3& position, float radius, btDynamicsWorld* world, short collisionGroup, short collisionMask, float mass) :
    PhysicsNode(world),
    mRadius(radius)
{
    mLocalTransformation = glm::translate(glm::mat4(1), position);
    mGlobalTransformation = mLocalTransformation;
    mVertexArray = glow::geometry::make_uv_sphere();

    btDefaultMotionState* motionState = new btDefaultMotionState(to_bullet(mGlobalTransformation));
    btCollisionShape* collisionShape = new btSphereShape(radius);
    mRigidBody = new btRigidBody(mass, motionState, collisionShape);
    mWorld->addRigidBody(mRigidBody, collisionGroup, collisionMask);
}

void SphereNode::render(const glow::UsedProgram& shader, glm::mat4& projection, glm::mat4& view)
{
    mGlobalTransformation = glm::scale(mGlobalTransformation, glm::vec3(mRadius));
    PhysicsNode::render(shader, projection, view);
}
