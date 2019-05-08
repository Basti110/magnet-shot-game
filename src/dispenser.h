#pragma once
#include "mesh_node.h"
#include "sphere_node.h"
#include "physics_manager.h"


class Dispenser : public AbstractNode
{
public:
    Dispenser(const glm::vec3& position, btDynamicsWorld* world) {
        glm::mat4 transform = glm::translate(glm::mat4(1), position);

        MeshNode* top = new MeshNode(
            transform,
            "../../data/meshes/DispenserTop.obj",
            world,
            GROUP_DYNAMIC_OBJECTS,
            GROUP_DYNAMIC_OBJECTS,
            100.0f
        );

        // restrict movement to rotation around the y-axis
        top->getRigidBody()->setLinearFactor(btVector3(0,0,0));
        top->getRigidBody()->setAngularFactor(btVector3(0,1,0));

        MeshNode* base = new MeshNode(
            transform,
            "../../data/meshes/DispenserBase.obj",
            world,
            GROUP_STATIC_OBJECTS,
            GROUP_DYNAMIC_OBJECTS,
            0.0f
        );

        SphereNode* sphere = new SphereNode(
            position + glm::vec3(1.1f, 1.65f,-1.45f),
            0.25f,
            world,
            GROUP_DYNAMIC_OBJECTS,
            GROUP_STATIC_OBJECTS | GROUP_DYNAMIC_OBJECTS,
            1.0f
        );

        addChild(top);
        addChild(base);
        addChild(sphere);
    }
};
