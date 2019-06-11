#pragma once
#include "mesh_node.h"

#include <glow/objects/Texture2D.hh>
#include <glm/gtc/matrix_transform.hpp>


class Screen : public AbstractNode
{
public:
    Screen(int id, const glm::mat4& transform, PhysicsManager* physics, MessageBus* messageBus) :
        mId(id),
        mIsOn(false),
        mAnimationTimer(0)
    {
        mBase = new MeshNode(
            transform,
            "../../data/meshes/ScreenBase.obj",
            physics,
            GROUP_STATIC_OBJECTS,
            GROUP_DYNAMIC_OBJECTS,
            0.0f
        );

        mTop = new MeshNode(
            transform,
            "../../data/meshes/ScreenTop.obj",
            physics,
            GROUP_NONE,
            GROUP_NONE,
            0.0f
        );
        mTop->setUseTexture(true);
        mTop->setShininess(200);

        addChild(mBase);
        addChild(mTop);

        const std::string idx = std::to_string(id);
        mTextureOn = glow::Texture2D::createFromFile("../../data/textures/ScreenOn" + idx + ".png", glow::ColorSpace::sRGB);
        mTextureOffA = glow::Texture2D::createFromFile("../../data/textures/ScreenOffA" + idx + ".png", glow::ColorSpace::sRGB);
        mTextureOffB = glow::Texture2D::createFromFile("../../data/textures/ScreenOffB" + idx + ".png", glow::ColorSpace::sRGB);

        messageBus->addActivateScreenReceiver([=](ActivateScreenMessage message) {
            this->notifyActivateScreen(message);
        });
    }

    void notifyActivateScreen(ActivateScreenMessage message)
    {
        if (message.screenId == mId) {
            mIsOn = true;
        }
    }

    void update(float elapsedSeconds) override
    {
        mAnimationTimer += elapsedSeconds;
        AbstractNode::update(elapsedSeconds);
    }

    void render(glow::UsedProgram& shader, glm::mat4& projection, glm::mat4& view, bool shadowPass) override
    {
        if (mIsOn) {
            shader.setTexture("uTexture", mTextureOn);
        }
        else {
            if ((int)(mAnimationTimer * 1.5f) % 2 == 0) {
                shader.setTexture("uTexture", mTextureOffA);
            }
            else {
                shader.setTexture("uTexture", mTextureOffB);
            }
        }
        mBase->render(shader, projection, view, shadowPass);
        mTop->render(shader, projection, view, shadowPass);
    }

private:
    int mId;
    bool mIsOn;
    float mAnimationTimer;
    MeshNode* mBase;
    MeshNode* mTop;
    glow::SharedTexture2D mTextureOn;
    glow::SharedTexture2D mTextureOffA;
    glow::SharedTexture2D mTextureOffB;
};
