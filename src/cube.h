#pragma once
#include "physics_node.h"
#include "physics_manager.h"

class Cube : public PhysicsNode
{
public:
public:
    Cube(const glm::mat4& transformation, Color color, PhysicsManager* physics);
    ~Cube();
    virtual void createBuffer() override;
    virtual void deleteBuffer() override;
    virtual void render(glow::UsedProgram& shader, glm::mat4& model, glm::mat4& view, bool shadowPass) override;
    virtual void update(float elapsedSeconds) override;
    void setColorRatio(float ratio);
    void setColor(glm::vec3 color);
    void setScale(glm::vec3 scale);
    bool setTexture(std::string path);
    void addPhysics(glm::vec3 scale, const RigidBodyInfo& info);
    glm::vec3 getScale();
    glm::vec3 getColor();
    const RigidBodyInfo& getRigidBodyInfo();

private:
    void setVertices();

    static bool isInit;
    static unsigned int VBO;
    static unsigned int VAO;

    bool mIsPhysicsOn = false;
    unsigned int mTexture;
    std::vector<float> mVertices;
    float mColorRatio;
    glm::vec3 mScale;
    RigidBodyInfo mRigidBodyinfo;
};
