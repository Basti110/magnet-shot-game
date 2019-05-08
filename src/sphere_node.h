#pragma once
#include "physics_node.h"


class SphereNode : public PhysicsNode
{
public:
    SphereNode(const glm::vec3& position, float radius, btDynamicsWorld* world, short collisionGroup, short collisionMask, float mass);
    void render(const glow::UsedProgram& shader, glm::mat4& projection, glm::mat4& view) override;

private:
    float mRadius;
};
