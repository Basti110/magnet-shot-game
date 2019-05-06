#include "node.h"
#include "physics_manager.h"
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>


Node::Node()
{
}


Node::~Node()
{
}

void Node::update(float elapsedSeconds) 
{
    if (mPhysicsID >= 0 && mPhysics)
    {
        glm::mat4 transform;
        if (mPhysics->getTransformation(mPhysicsID, transform))
            setGlobalTransformation(transform);
    }
    AbstractNode::update(elapsedSeconds);
}

void Node::setRigidBody(int bodyId, PhysicsManager* physics) 
{
    mPhysicsID = bodyId;
    mPhysics = physics;
}
