#pragma once
#include "abstract_node.h"
#include "physics_manager.h"

class Cube : public AbstractNode
{
public:
public:
    Cube(glm::mat4& transformation, Color color, PhysicsManager* physik);
    ~Cube();
    virtual void createBuffer() override;
    virtual void deleteBuffer() override;
    virtual void render(const glow::UsedProgram& shader, glm::mat4& model, glm::mat4& view) override;
    virtual void update(float elapsedSeconds) override;
    void setVisible(bool value);
    void setColorRatio(float ratio);
    void setColor(glm::vec3 color);
    bool setTexture(std::string path);
    void addPhysics(glm::vec3 shape, const RigidBodyInfo& info);

private:
    void setVertices();

    static bool isInit;
    static unsigned int VBO;
    static unsigned int VAO;

    bool mIsPhysicsOn = false;
    int mPhysicsID;
    unsigned int mTexture;
    std::vector<float> mVertices;
    bool mIsVisible;
    float mColorRatio;
    glm::vec3 mColor;
    glm::vec3 mScale;
    PhysicsManager* mPhysics;
};
