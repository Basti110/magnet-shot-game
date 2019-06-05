#pragma once
#include "abstract_node.h"

class PhysicsManager;

class Node : public AbstractNode
{
public:
    Node();
    ~Node();
    void createBuffer() { AbstractNode::createBuffer(); }
    void deleteBuffer() { AbstractNode::deleteBuffer(); }
    void render(glow::UsedProgram& shader, glm::mat4& projection, glm::mat4& view, bool shadowPass) { AbstractNode::render(shader, projection, view, shadowPass); }
    void update(float elapsedSeconds);
    void setRigidBody(int bodyId, PhysicsManager* physics);
    int getPhysicsID() { return mPhysicsID; }

private:
    PhysicsManager* mPhysics;
    int mPhysicsID = -1;
};
