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
    void setAmbient(const glm::vec3& color);
    void setDiffuse(const glm::vec3& color);
    void setSpecular(const glm::vec3& color);
    void setRenderColor(const glm::vec3& color);
    glm::vec3 getAmbient();
    glm::vec3 getDiffuse();
    glm::vec3 getSpecular();

private:
    static bool mIsInit;
    static glow::SharedProgram mShader;
    static unsigned int VBO;
    static unsigned int VAO;
    std::vector<float> mVertices;
    glm::vec3 mAmbient;
    glm::vec3 mDiffuse;
    glm::vec3 mSpecular;
    glm::vec3 mRenderColor;
};

