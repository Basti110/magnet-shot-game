#pragma once
#include "physics_node.h"


class BoxNode : public PhysicsNode
{
public:
    BoxNode(const glm::mat4& transform, const glm::vec3& scale, PhysicsManager* physics, short collisionGroup, short collisionMask, float mass);
    void update(float elapsedSeconds) override;

    const glm::vec3& getScale();

private:
    glm::vec3 mScale;
};
