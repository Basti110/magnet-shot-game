#pragma once
#include "abstract_node.h"
#include "message_bus.h"

#include <btBulletDynamicsCommon.h>
#include <glow/objects/VertexArray.hh>

class CoordinateAxes;
class PhysicsManager;


class PhysicsNode : public AbstractNode
{
public:
    PhysicsNode(PhysicsManager* physics);
    ~PhysicsNode() override;

    void update(float elapsedSeconds) override;
    void render(glow::UsedProgram& shader, glm::mat4& projection, glm::mat4& view, bool shadowPass) override;

    btRigidBody* getRigidBody() { return mRigidBody; }
    void setColor(const glm::vec3& color);
    void setUseVertexColors(bool value) { mUseVertexColors = value; }
    bool getUseVertexColors() { return mUseVertexColors; }
    void setUseTexture(bool value) { mUseTexture = value; }
    bool getUseTexture() { return mUseTexture; }
    void setVisible(bool value) { mIsVisible = value; }
    bool getVisible() { return mIsVisible; }
    void setDisableShadows(bool value) { mDisableShadows = value; }
    bool getDisableShadows() { return mDisableShadows; }
    void setAlpha(float value) { mAlpha = value; }
    void notifyPickBody(PickBodyMessage message);
    void notifyGuiInput(Message* message);

protected:
    BodyProperties mProperty;
    PhysicsManager* mPhysics;
    btDynamicsWorld* mWorld;
    btRigidBody* mRigidBody;
    std::vector<btTypedConstraint*> mConstraints;

    glm::vec3 mColor;
    bool mUseVertexColors;
    bool mUseTexture;
    bool mIsVisible;
    bool mDisableShadows;
    float mAlpha;

    CoordinateAxes* mCoordinateAxes = nullptr;
    glow::SharedVertexArray mVertexArray;
    bool mIsPicked = false;
    MessageBus* mMessageBus;
};
