#pragma once
#include "abstract_node.h"
#include "mesh_node.h"
#include "box_node.h"
#include "scene_manager.h"
#include "physics_manager.h"
#include "bullet_helper.hh"

#include <glm/ext/matrix_transform.hpp>
#include <GLFW/glfw3.h>


class ClockTower : public AbstractNode
{
public:
    ClockTower(const glm::vec3& position, PhysicsManager* physics, MessageBus* messageBus, SceneManager* scene) :
        mPosition(position),
        mScene(scene),
        mMessageBus(messageBus)
    {
        addChild(new MeshNode(
            glm::translate(glm::mat4(1), position),
            "../../data/meshes/ClockTower.ply",
            physics, GROUP_STATIC_OBJECTS, GROUP_DYNAMIC_OBJECTS, 0.0f
        ));

        for (int i = 0; i < 4; i++) {
            MeshNode* hourHand = new MeshNode(
                glm::mat4(1), "../../data/meshes/HourHand.ply",
                physics, GROUP_NONE, GROUP_NONE, 0.0f
            );
            addChild(hourHand);
            mHourHands.push_back(hourHand);

            MeshNode* minuteHand = new MeshNode(
                glm::mat4(1), "../../data/meshes/MinuteHand.ply",
                physics, GROUP_NONE, GROUP_NONE, 0.0f
            );
            addChild(minuteHand);
            mMinuteHands.push_back(minuteHand);
        }

        mRedBox = new BoxNode(
            glm::translate(glm::mat4(1), position + glm::vec3(0.0f, 2.5f, 0.0f)),
            glm::vec3(0.15f, 0.55f, 0.15f),
            physics, GROUP_DYNAMIC_OBJECTS, GROUP_DYNAMIC_OBJECTS, 1.0f
        );
        mRedBox->setColor(red);

        const auto& id = btTransform::getIdentity();
        auto constraint = new btGeneric6DofConstraint(*(mRedBox->getRigidBody()), id, false);
        constraint->setLinearLowerLimit(btVector3(0, -1, 0));
        constraint->setLinearUpperLimit(btVector3(0, 0, 0));
        mRedBox->addConstraint(constraint);
        addChild(mRedBox);

        mWhiteBox = new BoxNode(
            glm::translate(glm::mat4(1), position + glm::vec3(0.0f, 3.8f, 0.0f)),
            glm::vec3(0.05f, 0.75f, 0.05f),
            physics, GROUP_DYNAMIC_OBJECTS, GROUP_DYNAMIC_OBJECTS, 1.0f
        );
        addChild(mWhiteBox);
    }

    void update(float elapsedSeconds) override
    {
        // getSunAngle() == 0 should correspond to 6:00 a.m.
        const float angle = -mScene->getSunAngle() +  glm::half_pi<float>();
        const glm::vec3 offset(1.40395f, 5.21009f, 0.0f);

        for (int i = 0; i < 4; i++) {
            glm::mat4 baseTransform = glm::translate(glm::mat4(1), mPosition);
            baseTransform = glm::rotate(baseTransform, glm::half_pi<float>() * i, glm::vec3(0,1,0));
            baseTransform = glm::translate(baseTransform, offset);

            glm::mat4 hourTransform = glm::rotate(baseTransform, angle * 2, glm::vec3(1,0,0));
            mHourHands[i]->setLocalTransformation(hourTransform);

            glm::mat4 minuteTransform = glm::rotate(baseTransform, angle * 24, glm::vec3(1,0,0));
            mMinuteHands[i]->setLocalTransformation(minuteTransform);
        }

        // pull red box up box
        mRedBox->getRigidBody()->applyCentralImpulse(btVector3(0,1,0));

        // set position of white box
        glm::mat4 transform = to_glm(mRedBox->getRigidBody()->getWorldTransform());
        transform = glm::translate(transform, glm::vec3(0.0f, 1.3f, 0.0f));
        mWhiteBox->getRigidBody()->setWorldTransform(to_bullet(transform));

        // toggle day/night and clear magnets when pulled down
        if (mRedBox->getRigidBody()->getCenterOfMassPosition().y() < mPosition.y + 1.5f) {
            mMessageBus->sendMessage(new MouseClickMessage(0, 0, GLFW_MOUSE_BUTTON_MIDDLE, GLFW_PRESS));
        }
    }

private:
    glm::vec3 mPosition;
    SceneManager* mScene;
    MessageBus* mMessageBus;
    std::vector<MeshNode*> mHourHands;
    std::vector<MeshNode*> mMinuteHands;
    BoxNode* mRedBox;
    BoxNode* mWhiteBox;
};
