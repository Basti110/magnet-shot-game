#pragma once
#include "mesh_node.h"
#include "sphere_node.h"
#include "physics_manager.h"


class Dispenser : public AbstractNode
{
public:
    Dispenser(const glm::mat4& transform, PhysicsManager* physics) {
        MeshNode* base = new MeshNode(
            transform,
            "../../data/meshes/DispenserBase.obj",
            physics,
            GROUP_STATIC_OBJECTS,
            GROUP_DYNAMIC_OBJECTS,
            0.0f
        );

        btCompoundShape* compoundShape = new btCompoundShape();
        btBoxShape* horizontalBox = new btBoxShape(btVector3(0.790345f, 0.081555f, 0.081555f));
        btBoxShape* diagonalBox = new btBoxShape(btVector3(0.56805f, 0.081555f, 0.0795825f));
        btBoxShape* verticalBox = new btBoxShape(btVector3(0.099503f, 0.41545f, 0.081555f));

        btTransform t;
        t.setIdentity();
        t.setOrigin(btVector3(0.0f, 2.57134f, 0.0f));
        compoundShape->addChildShape(t, horizontalBox);

        t.setIdentity();
        t.setRotation(btQuaternion(0.0f, 0.0f, glm::radians(11.6f)));
        t.setOrigin(btVector3(-1.33076f, 2.46037f, 0.0f));
        compoundShape->addChildShape(t, diagonalBox);

        t.setIdentity();
        t.setRotation(btQuaternion(0.0f, 0.0f, glm::radians(-11.6f)));
        t.setOrigin(btVector3(1.33076f, 2.46037f, 0.0f));
        compoundShape->addChildShape(t, diagonalBox);

        t.setIdentity();
        t.setOrigin(btVector3(-1.80358f, 2.00898f, 0.0f));
        compoundShape->addChildShape(t, verticalBox);

        t.setIdentity();
        t.setOrigin(btVector3(1.80358f, 2.00898f, 0.0f));
        compoundShape->addChildShape(t, verticalBox);

        MeshNode* top = new MeshNode(
            transform,
            "../../data/meshes/DispenserTop.obj",
            physics,
            compoundShape,
            GROUP_DYNAMIC_OBJECTS,
            GROUP_DYNAMIC_OBJECTS,
            10.0f
        );
        top->setColor(glm::vec3(234/255.0f, 22/255.0f, 22/255.0f));

        // restrict movement to rotation around the y-axis
        top->getRigidBody()->setLinearFactor(btVector3(0,0,0));
        top->getRigidBody()->setAngularFactor(btVector3(0,1,0));

        SphereNode* sphere = new SphereNode(
            glm::vec3(transform * glm::vec4(1.1f, 1.65f,-1.45f, 1.0f)),
            0.25f,
            physics,
            GROUP_DYNAMIC_OBJECTS,
            GROUP_STATIC_OBJECTS | GROUP_DYNAMIC_OBJECTS,
            1.0f
        );
        sphere->setColor(glm::vec3(189/255.0f, 51/255.0f, 208/255.0f));

        addChild(base);
        addChild(top);
        addChild(sphere);
    }
};
