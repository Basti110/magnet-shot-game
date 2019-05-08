#pragma once
#include "abstract_node.h"

#include <btBulletDynamicsCommon.h>
#include <glow/objects/VertexArray.hh>


class PhysicsNode : public AbstractNode
{
public:
    PhysicsNode(btDynamicsWorld* world);
    ~PhysicsNode() override;
    void update(float elapsedSeconds) override;
    void render(const glow::UsedProgram& shader, glm::mat4& projection, glm::mat4& view) override;

    btRigidBody* getRigidBody() { return mRigidBody; }

protected:
    btDynamicsWorld* mWorld;
    btRigidBody* mRigidBody;
    std::vector<btTypedConstraint*> mConstraints;

    glow::SharedVertexArray mVertexArray;
    glm::vec3 mColor;
};
