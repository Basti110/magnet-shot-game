#pragma once
#include "mesh_node.h"
#include "sphere_node.h"
#include "physics_manager.h"


class Dispenser : public AbstractNode
{
public:
    Dispenser(const glm::vec3& position, PhysicsManager* physics) {
        glm::mat4 transform = glm::translate(glm::mat4(1), position);

        MeshNode* top = new MeshNode(
            transform,
            "../../data/meshes/DispenserTop.obj",
            physics,
            GROUP_DYNAMIC_OBJECTS,
            GROUP_DYNAMIC_OBJECTS,
            10.0f
        );
        top->setColor(glm::vec3(234/255.0f, 22/255.0f, 22/255.0f));

        // restrict movement to rotation around the y-axis
        top->getRigidBody()->setLinearFactor(btVector3(0,0,0));
        top->getRigidBody()->setAngularFactor(btVector3(0,1,0));

        MeshNode* base = new MeshNode(
            transform,
            "../../data/meshes/DispenserBase.obj",
            physics,
            GROUP_STATIC_OBJECTS,
            GROUP_DYNAMIC_OBJECTS,
            0.0f
        );

        SphereNode* sphere = new SphereNode(
            position + glm::vec3(1.1f, 1.65f,-1.45f),
            0.25f,
            physics,
            GROUP_DYNAMIC_OBJECTS,
            GROUP_STATIC_OBJECTS | GROUP_DYNAMIC_OBJECTS,
            1.0f
        );
        sphere->setColor(glm::vec3(189/255.0f, 51/255.0f, 208/255.0f));

        addChild(top);
        addChild(base);
        addChild(sphere);
    }
};
