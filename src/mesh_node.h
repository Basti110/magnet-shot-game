#pragma once
#include "physics_node.h"


class MeshNode : public PhysicsNode
{
public:
    MeshNode(const glm::mat4& transform, const std::string& filename, btDynamicsWorld* world, short collisionGroup, short collisionMask, float mass);

};
