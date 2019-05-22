#pragma once
#include "physics_node.h"


class SphereNode : public PhysicsNode
{
public:
    SphereNode(const glm::vec3& position, float radius, PhysicsManager* physics, short collisionGroup, short collisionMask, float mass);
    void update(float elapsedSeconds) override;

private:
    float mRadius;
};
