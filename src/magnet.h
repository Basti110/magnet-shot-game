#pragma once
#include "abstract_node.h"
#include "physics_manager.h"


class Magnet : public AbstractNode
{
public:
    Magnet(PhysicsManager* physics, int physicsId, float scale, glm::vec3 color);
    ~Magnet() override;
    void render(const glow::UsedProgram& currentShader, glm::mat4& model, glm::mat4& view) override;
    void update(float elapsedSeconds) override;

private:
    glow::SharedVertexArray mVertexArray;
    glow::SharedProgram mShader;

    PhysicsManager* mPhysics;
    int mPhysicsId;
    float mScale;
    glm::vec3 mColor;
};
