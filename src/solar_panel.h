#pragma once
#include "bullet_helper.hh"
#include "mesh_node.h"
#include "physics_manager.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>


class SolarPanel : public AbstractNode
{
public:
    SolarPanel(const glm::mat4& transform, float panelAngle, PhysicsManager* physics, MessageBus* messageBus)
      : mMessageBus(messageBus), mMessageSent(false)
    {
        mBase = new MeshNode(transform, "../../data/meshes/SolarPanelBase.obj", physics, GROUP_STATIC_OBJECTS, GROUP_DYNAMIC_OBJECTS, 0.0f);

        btCompoundShape* compoundShape = new btCompoundShape();
        btCylinderShape* pylon = new btCylinderShape(btVector3(0.101403f, 1.59142f, 0.101403f));
        btBoxShape* panel = new btBoxShape(btVector3(1.8f, 0.04f, 1.125f));

        const glm::vec3 pylonPosition(0.0f, 1.71907, 0.0f);

        btTransform t;
        t.setIdentity();
        t.setOrigin(to_bullet(pylonPosition));
        compoundShape->addChildShape(t, pylon);

        t.setIdentity();
        t.setRotation(btQuaternion(0.0f, glm::radians(45.0f), 0.0f));
        t.setOrigin(btVector3(0.0f, 3.25f, 0.0f));
        compoundShape->addChildShape(t, panel);

        mTop = new MeshNode(glm::rotate(transform, panelAngle, glm::vec3(0, 1, 0)), "../../data/meshes/SolarPanelTop.ply", physics, compoundShape,
                            GROUP_DYNAMIC_OBJECTS, GROUP_STATIC_OBJECTS | GROUP_DYNAMIC_OBJECTS, 10.0f);

        // restrict movement to rotation around the y-axis
        mTop->getRigidBody()->setLinearFactor(btVector3(0, 0, 0));
        mTop->getRigidBody()->setAngularFactor(btVector3(0, 1, 0));
        mTop->setColor(glm::vec3(0));

            addChild(mBase);
        addChild(mTop);

        mOrientation = to_bullet(glm::quat_cast(transform));
        mLockedInHeight = mTop->getRigidBody()->getCenterOfMassPosition().y() - 0.12f;
    }

    void update(float elapsedSeconds)
    {
        mBase->update(elapsedSeconds);
        mTop->update(elapsedSeconds);

        if (mTop->getRigidBody()->getOrientation().angleShortestPath(mOrientation) < 0.01)
        {
            mTop->getRigidBody()->setLinearFactor(btVector3(0, 1, 0));
            mTop->getRigidBody()->setAngularFactor(btVector3(0, 0, 0));
            mTop->getRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
        }

        if (!mMessageSent && mTop->getRigidBody()->getCenterOfMassPosition().y() < mLockedInHeight)
        {
            mMessageBus->sendMessage(new ActivateScreenMessage(1));
            mMessageSent = true;
        }
    }

private:
    MessageBus* mMessageBus;
    MeshNode* mBase;
    MeshNode* mTop;
    btQuaternion mOrientation;
    float mLockedInHeight;
    bool mMessageSent;
};
