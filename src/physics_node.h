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
    PhysicsNode(PhysicsManager* physics, bool receiveMessages = true);
    ~PhysicsNode() override;

    void update(float elapsedSeconds) override;
    void render(glow::UsedProgram& shader, glm::mat4& projection, glm::mat4& view, bool shadowPass) override;

    btRigidBody* getRigidBody() { return mRigidBody; }
    void addConstraint(btTypedConstraint* constraint);
    void setAmbient(const glm::vec3& color);
    void setDiffuse(const glm::vec3& color) { mProperty.diffuse = color; }
    void setSpecular(const glm::vec3& color) { mProperty.specular = color; }
    void setIntensity(float intensity) { mProperty.intensity = intensity; }
    void setColor(const glm::vec3& color);
    void setUseVertexColors(bool value) { mUseVertexColors = value; }
    bool getUseVertexColors() { return mUseVertexColors; }
    void setColorRatio(float value) { mColorRatio = value; }
    float getColorRatio() { return mColorRatio; }
    void setVisible(bool value) { mIsVisible = value; }
    bool getVisible() { return mIsVisible; }
    void setDisableShadows(bool value) { mDisableShadows = value; }
    bool getDisableShadows() { return mDisableShadows; }
    void setAlpha(float value) { mAlpha = value; }
    void notifyPickBody(PickBodyMessage message);
    void notifyGuiInput(Message* message);
    void setShininess(float shininess);

protected:
    BodyProperties mProperty;
    PhysicsManager* mPhysics;
    btDynamicsWorld* mWorld;
    btRigidBody* mRigidBody;
    std::vector<btTypedConstraint*> mConstraints;
    bool mUseVertexColors;
    bool mIsVisible;
    bool mDisableShadows;
    float mAlpha;
    float mColorRatio;

    CoordinateAxes* mCoordinateAxes = nullptr;
    glow::SharedVertexArray mVertexArray;
    bool mIsPicked = false;
    MessageBus* mMessageBus;
};
