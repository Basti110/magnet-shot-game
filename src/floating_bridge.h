#pragma once
#include "box_node.h"
#include "physics_manager.h"
#include "bullet_helper.hh"

#include <glm/gtc/constants.hpp>


class FloatingBridge : public BoxNode
{
public:
    FloatingBridge(const glm::mat4& transform, PhysicsManager* physics) :
        BoxNode(transform, glm::vec3(2.5f, 0.75f, 1.75f), physics, GROUP_DYNAMIC_OBJECTS, GROUP_STATIC_OBJECTS | GROUP_DYNAMIC_OBJECTS, 10.0f)
    {
        getRigidBody()->setLinearFactor(btVector3(1,0,0));
        getRigidBody()->setAngularFactor(btVector3(0,0,0));
    }

//    void update(float elapsedSeconds) override
//    {
//        mAnimationParam += elapsedSeconds * 1.5f;
//        const float angle = glm::sin(mAnimationParam) * glm::pi<float>() * 0.01f;
//        const glm::vec3 position = to_glm(getRigidBody()->getCenterOfMassPosition());
//        glm::mat4 transform = glm::translate(glm::mat4(1), position);
//        transform = glm::rotate(transform, angle, glm::vec3(1,0,0));
//        getRigidBody()->setCenterOfMassTransform(to_bullet(transform));
//        transform = glm::scale(transform, getScale());
//        setLocalTransformation(transform);
//    }
};
