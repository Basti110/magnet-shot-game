#pragma once
#include "mesh_node.h"
#include "camera.h"
#include "physics_manager.h"

#include <glm/ext/matrix_transform.hpp>


class MagnetGun : public MeshNode
{
public:
    MagnetGun(const glm::vec3& position, PhysicsManager* physics, MessageBus* messageBus, Camera* camera) :
        MeshNode(glm::translate(glm::mat4(1), position), "../../data/meshes/MagnetGun.ply", physics, GROUP_NONE, GROUP_NONE, 0.0f),
        mCamera(camera),
        mAttatchToCamera(false),
        mInitialPosition(position),
        mRotationParam(0),
        mPositionParam(0),
        mRecoilParam(0)
    {
        messageBus->addMouseClickReceiver([=](MouseClickMessage message) {
            if (mAttatchToCamera) this->notifyMouseClickInput(message);
        });
        messageBus->addGameModeReceiver([=](GameModeMessage message) {
            if (mAttatchToCamera) this->notifyGameModeChange(message);
        });
        messageBus->addLocationEventReceiver([=](LocationEventMessage message) {
            if (!mAttatchToCamera) this->notifyLocationEvent(message);
        });
    }

    void update(float elapsedSeconds) override
    {
        if (mAttatchToCamera) {
            const glm::vec3 front = mCamera->getCameraFront();
            const glm::vec3 right = mCamera->getCameraRight();
            const glm::vec3 up = mCamera->getCameraUp();
            const glm::vec3 position = mCamera->getPos() - 0.05 * front + 0.2 * right - 0.26 * up;
            mGlobalTransformation = glm::translate(glm::mat4(1), position);
            mGlobalTransformation = mGlobalTransformation * glm::mat4(glm::mat3(right, up, -front));
            mGlobalTransformation = glm::rotate(mGlobalTransformation, 0.0125f, glm::vec3(0,1,0));
            mGlobalTransformation = glm::rotate(mGlobalTransformation, 0.01f, glm::vec3(1,0,0));

            // recoil animation
            if (mRecoilParam > 0) {
                const float recoil = glm::sin(mRecoilParam) * 0.05f;
                mGlobalTransformation = glm::translate(mGlobalTransformation, glm::vec3(0, 0, recoil));
                mRecoilParam -= elapsedSeconds * 20.0f;
            }
            else {
                mRecoilParam = 0;
            }
        }
        else {
            mRotationParam += elapsedSeconds * 2.0f;
            mPositionParam += elapsedSeconds * 2.5f;
            glm::vec3 position = mInitialPosition;
            position.y += glm::sin(mPositionParam) * 0.1;
            mGlobalTransformation = glm::translate(glm::mat4(1), position);
            mGlobalTransformation = glm::rotate(mGlobalTransformation, mRotationParam, glm::vec3(0,1,0));
        }
    }

    void notifyMouseClickInput(MouseClickMessage message)
    {
        if ((message.getAction() == GLFW_PRESS && message.getInput() == GLFW_MOUSE_BUTTON_LEFT) ||
            (message.getAction() == GLFW_PRESS && message.getInput() == GLFW_MOUSE_BUTTON_RIGHT))
        {
            mRecoilParam = glm::pi<float>();
        }
    }

    void notifyGameModeChange(GameModeMessage message)
    {
        setVisible(message.mode == GameMode::Gameplay);
    }

    void notifyLocationEvent(LocationEventMessage message)
    {
        if (message.eventId == LocationEventId::MagnetGunPickUp &&
            message.eventType == LocationEventType::Enter)
        {
            mAttatchToCamera = true;
            setDisableShadows(true);
        }
    }

private:
    Camera* mCamera;
    bool mAttatchToCamera;
    glm::vec3 mInitialPosition;
    float mRotationParam;
    float mPositionParam;
    float mRecoilParam;
};
