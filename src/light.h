#pragma once
#include "abstract_node.h"

class Light :
    public AbstractNode
{
public:
    Light(glm::mat4 & transformation, Color color);
    ~Light();
    virtual void createBuffer() override;
    virtual void deleteBuffer() override;
    virtual void render(glow::UsedProgram& shader, glm::mat4& model, glm::mat4& view, bool shadowPass) override;
    void setColor(glm::vec3 color);
    glm::vec3 getColor();

private:
    static bool mIsInit;
    static glow::SharedProgram mShader;
    static unsigned int VBO;
    static unsigned int VAO;
    std::vector<float> mVertices;
    glm::vec3 mColor;
};

