#pragma once
#include "box_node.h"
#include "physics_manager.h"
#include "bullet_helper.hh"

#include <glm/gtc/matrix_transform.hpp>


class Stairs : public AbstractNode
{
public:
    Stairs(const glm::mat4& transform, PhysicsManager* physics, MessageBus* messageBus) :
        mAnimationState(AnimationState::Default),
        mAnimationTimer(0)
    {
        const int nSteps = 15;
        const float stepWidth = 12.0f / nSteps;
        const float stepHeight = 4.0f / nSteps;
        const glm::vec3 scale(1.5f, stepHeight/2, stepWidth/2);

        for (int i = 0; i < 15; i++) {
            glm::mat4 boxTransform = glm::translate(transform, glm::vec3(0, -(i * stepHeight), i * stepWidth));
            BoxNode* box = new BoxNode(boxTransform, scale, physics,GROUP_STATIC_OBJECTS, GROUP_DYNAMIC_OBJECTS, 0.0f);
            box->setSpecular(glm::vec3(0));
            box->setDisableShadows(true);
            mBoxes.push_back(box);
            mTargetTransforms.push_back(boxTransform);
            addChild(box);
        }
        hideBoxes();

        messageBus->addLocationEventReceiver([=](LocationEventMessage message) {
            this->notifyLocationEvent(message);
        });
    }

    void update(float elapsedSeconds)
    {
        if (mAnimationState == AnimationState::Default) {
            return;
        }

        const float targetAlpha = 0.7f;
        const float durationFadeOut = 1.0f;

        const float speed = 3.5f;
        const float delay = 0.1f;

        mAnimationTimer += elapsedSeconds;

        if (mAnimationState == AnimationState::FadeIn ||
            mAnimationState == AnimationState::FadeInOut)
        {
            for (size_t i = 0; i < mBoxes.size(); i++) {
                float t = (mAnimationTimer - i * delay) * speed;
                t = glm::clamp(t, 0.0f, glm::half_pi<float>());
                const glm::vec3 offset(0, 1.0f + glm::cos(t), 0);
                const glm::mat4 transform = glm::translate(mTargetTransforms[i], offset);
                mBoxes[i]->setLocalTransformation(glm::scale(transform, mBoxes[i]->getScale()));
                mBoxes[i]->getRigidBody()->setCenterOfMassTransform(to_bullet(transform));
                const float alpha = t / (glm::half_pi<float>());
                mBoxes[i]->setAlpha(alpha * alpha * targetAlpha);
            }
            if (mAnimationTimer > glm::half_pi<float>() / speed + (mBoxes.size() - 1) * delay) {
                if (mAnimationState == AnimationState::FadeIn) {
                    mAnimationState = AnimationState::Default;
                }
                if (mAnimationState == AnimationState::FadeInOut) {
                    mAnimationState = AnimationState::FadeOut;
                }
                mAnimationTimer = 0.0f;
            }
        }

        if (mAnimationState == AnimationState::FadeOut) {
            for (auto box : mBoxes) {
                box->setAlpha((1.0f - (mAnimationTimer / durationFadeOut)) * targetAlpha);
            }
            if (mAnimationTimer > durationFadeOut) {
                mAnimationState = AnimationState::Default;
                mAnimationTimer = 0.0f;
                hideBoxes();
            }
        }
    }

    void activate()
    {
        if (mAnimationState != AnimationState::FadeIn &&
            mAnimationState != AnimationState::FadeInOut)
        {
            mAnimationState = AnimationState::FadeIn;
            mAnimationTimer = 0.0f;
        }
    }

    void deactivate()
    {
        if (mAnimationState == AnimationState::FadeIn) {
            mAnimationState = AnimationState::FadeInOut;
        }
        else {
            mAnimationState = AnimationState::FadeOut;
            mAnimationTimer = 0.0f;
        }
    }

private:
    enum class AnimationState {
        Default,
        FadeIn,
        FadeOut,
        FadeInOut
    };

    void hideBoxes()
    {
        const glm::mat4 transform = glm::translate(glm::mat4(1), glm::vec3(0, -1000, 0));
        for (auto box : mBoxes) {
            box->setLocalTransformation(transform);
            box->getRigidBody()->setCenterOfMassTransform(to_bullet(transform));
        }
    }

    void notifyLocationEvent(LocationEventMessage message)
    {
        if (message.eventId == LocationEventId::ActivateStairs &&
            message.eventType == LocationEventType::Enter)
        {
            activate();
        }

        if (message.eventId == LocationEventId::DeactivateStairs &&
            message.eventType == LocationEventType::Exit)
        {
            deactivate();
        }
    }

private:
    std::vector<BoxNode*> mBoxes;
    std::vector<glm::mat4> mTargetTransforms;
    AnimationState mAnimationState;
    float mAnimationTimer;
};
