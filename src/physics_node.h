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
    void render(const glow::UsedProgram& shader, glm::mat4& projection, glm::mat4& view) override;

    btRigidBody* getRigidBody() { return mRigidBody; }
    void setColor(const glm::vec3& color) { mColor = color; }
    void notifyPickBody(PickBodyMessage message);

protected:
    PhysicsManager* mPhysics;
    btDynamicsWorld* mWorld;
    btRigidBody* mRigidBody;
    std::vector<btTypedConstraint*> mConstraints;

    CoordinateAxes* mCoordinateAxes = nullptr;
    glow::SharedVertexArray mVertexArray;
    glm::vec3 mColor;
    bool mIsPicked = false;
    MessageBus* mMessageBus;
};
