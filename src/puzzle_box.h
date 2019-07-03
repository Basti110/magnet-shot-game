#pragma once
#include "physics_node.h"
#include "message_bus.h"
#include "bullet_helper.hh"
#include "load_mesh.h"

#include <glow/objects/Texture2D.hh>


class PuzzleBox : public PhysicsNode
{
public:
    PuzzleBox(const glm::mat4& transform, PhysicsManager* physics, int x, int y) :
        PhysicsNode(physics),
        mScale(0.49f),
        mTexOffset((y*6 + x) / 48.0f, 0.0f)
    {
        const short collisionGroup = GROUP_DYNAMIC_OBJECTS;
        const short collisionMask = GROUP_STATIC_OBJECTS | GROUP_DYNAMIC_OBJECTS;
        const float mass = 5.0f;

        setLocalTransformation(transform);
        mVertexArray = load_mesh_from_obj("../../data/meshes/PuzzleBox.obj", false);

        btDefaultMotionState* motionState = new btDefaultMotionState(to_bullet(transform));
        btCollisionShape* collisionShape = new btBoxShape(to_bullet(mScale));
        mRigidBody = new btRigidBody(mass, motionState, collisionShape);
        mWorld->addRigidBody(mRigidBody, collisionGroup, collisionMask);

        getRigidBody()->setLinearFactor(btVector3(1,0,1));
        getRigidBody()->setAngularFactor(btVector3(0,0,0));

        mDayTexture = glow::Texture2D::createFromFile("../../data/textures/Question.png", glow::ColorSpace::sRGB);
        mNightTexture = glow::Texture2D::createFromFile("../../data/textures/Circuit.png", glow::ColorSpace::sRGB);
        mActiveTexture = mDayTexture;

        setAlpha(0.0);
        setColorRatio(0.0);
        setColor(glm::vec3(0.5));

        MessageBus::getInstance()->addSceneEventReceiver([=](SceneEventMessage message) {
            this->notifySceneEvent(message);
        });
    }

    void update(float elapsedSeconds) override
    {
        const float x = getRigidBody()->getCenterOfMassPosition().x();
        const float y = getRigidBody()->getCenterOfMassPosition().y();
        const float z = getRigidBody()->getCenterOfMassPosition().z();

        // nearest grid center
        const float cx = glm::round(x + 0.5f) - 0.5f;
        const float cz = glm::round(z);

        // snap to grid
        if (glm::length(glm::vec2(cx, cz) - glm::vec2(x, z)) < 0.02) {
            const glm::mat4 transform = glm::translate(glm::mat4(1), glm::vec3(cx, y, cz));
            getRigidBody()->setWorldTransform(to_bullet(transform));
        }
        else {
            getRigidBody()->applyCentralImpulse(btVector3(cx - x, 0, cz - z) * 0.5f);
        }

        PhysicsNode::update(elapsedSeconds);
        setLocalTransformation(glm::scale(mLocalTransformation, mScale));
    }

    void render(glow::UsedProgram& shader, glm::mat4& projection, glm::mat4& view, bool shadowPass) override
    {
        shader.setTexture("uTexture", mActiveTexture);
        shader.setUniform("uTexOffset", mTexOffset);
        PhysicsNode::render(shader, projection, view, shadowPass);
    }

    void notifySceneEvent(SceneEventMessage message) {
        if (message.eventId == SceneEventId::Day) {
            mActiveTexture = mDayTexture;
        }
        if (message.eventId == SceneEventId::Night) {
            mActiveTexture = mNightTexture;
        }
    }

private:
    glm::vec3 mScale;
    glm::vec2 mTexOffset;
    glow::SharedTexture2D mDayTexture;
    glow::SharedTexture2D mNightTexture;
    glow::SharedTexture2D mActiveTexture;
};
