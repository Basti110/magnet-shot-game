#pragma once
#include "mesh_node.h"


class Cable : public MeshNode
{
public:
    Cable(int id, PhysicsManager* physics, MessageBus* messageBus) :
        MeshNode(glm::mat4(), getFilename(id), physics, GROUP_NONE, GROUP_NONE, 0.0f),
        mId(id),
        mStartAnimation(false),
        mAnimationTimer(0)
    {
        messageBus->addActivateScreenReceiver([=](ActivateScreenMessage message) {
            this->notifyActivateScreen(message);
        });
    }

    static const std::string getFilename(int id)
    {
        return "../../data/meshes/Cable" + std::to_string(id) + ".obj";
    }

    void notifyActivateScreen(ActivateScreenMessage message)
    {
        if (message.screenId == mId) {
            mStartAnimation = true;
            mAnimationTimer = -0.5f;
        }
    }

    void update(float elapsedSeconds) override
    {
        if (mStartAnimation) {
            mAnimationTimer += elapsedSeconds;
            const float t = glm::clamp(mAnimationTimer, 0.0f, 1.0f);
            setColor((1 - t) * white + t * red);
        }
    }

private:
    int mId;
    bool mStartAnimation;
    float mAnimationTimer;
};
