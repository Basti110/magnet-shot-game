#include "physics_node.h"
#include "bullet_helper.hh"


PhysicsNode::PhysicsNode(btDynamicsWorld* world) :
    mWorld(world),
    mRigidBody(nullptr),
    mColor(glm::vec3(0.5))
{

}

PhysicsNode::~PhysicsNode()
{
    for (auto c : mConstraints) {
        mWorld->removeConstraint(c);
    }
    if (mRigidBody) {
        mWorld->removeRigidBody(mRigidBody);
        delete mRigidBody->getMotionState();
        delete mRigidBody->getCollisionShape();
        delete mRigidBody;
    }
}

void PhysicsNode::update(float elapsedSeconds)
{
    if (mRigidBody) {
        btTransform transform;
        mRigidBody->getMotionState()->getWorldTransform(transform);
        mGlobalTransformation = to_glm(transform);
    }
    AbstractNode::update(elapsedSeconds);
}

void PhysicsNode::render(const glow::UsedProgram& shader, glm::mat4& projection, glm::mat4& view)
{
    shader.setUniform("model", mGlobalTransformation);
    shader.setUniform("colorRatio", 1.0f);
    shader.setUniform("color", mColor);
    mVertexArray->bind().draw();
    AbstractNode::render(shader, projection, view);
}
