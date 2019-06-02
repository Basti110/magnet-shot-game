#pragma once
#include "mesh_node.h"
#include "physics_manager.h"
#include "bullet_helper.hh"

#include <glm/gtc/matrix_transform.hpp>


class WindTurbine : public AbstractNode
{
public:
    WindTurbine(const glm::mat4& transform, float bladesAngle, PhysicsManager* physics) :
        mBladesAngle(bladesAngle)
    {
        MeshNode* base = new MeshNode(
            transform,
            "../../data/meshes/WindTurbineBase.obj",
            physics,
            GROUP_STATIC_OBJECTS,
            GROUP_DYNAMIC_OBJECTS,
            0.0f
        );

        const glm::vec3 bladesOffset(0.72530f, 8.73904f, 0.0f);
        mBladesTransform = glm::translate(transform, bladesOffset);

        mBlades = new MeshNode(
            mBladesTransform,
            "../../data/meshes/WindTurbineBlades.obj",
            physics,
            GROUP_STATIC_OBJECTS,
            GROUP_DYNAMIC_OBJECTS,
            0.0f
        );

        addChild(base);
        addChild(mBlades);
    }

    void update(float elapsedSeconds)
    {
        mBladesAngle += elapsedSeconds * 0.75f;
        const glm::mat4 rotation = glm::rotate(glm::mat4(1), mBladesAngle, glm::vec3(1,0,0));
        mBlades->getRigidBody()->setCenterOfMassTransform(to_bullet(mBladesTransform * rotation));
        mBlades->setLocalTransformation(mBladesTransform * rotation);
    }

private:
    MeshNode* mBlades;
    glm::mat4 mBladesTransform;
    float mBladesAngle;
};
