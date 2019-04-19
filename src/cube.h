#pragma once
#include "abstract_node.h"

class Cube : public AbstractNode
{
public:

public:
    Cube(glm::mat4& transformation, Color color);
    ~Cube();
    virtual void createBuffer() override;
    virtual void deleteBuffer() override;
    virtual void render(const glow::UsedProgram& shader, glm::mat4& model, glm::mat4& view) override;
    void setColorRatio(float ratio);
    void setColor(glm::vec3 color);
    bool setTexture(std::string path);

private:
    void setVertices();

    static bool isInit;
    static unsigned int VBO;
    static unsigned int VAO;

    unsigned int mTexture;
    std::vector<float> mVertices;
    float mColorRatio;
    glm::vec3 mColor;
};
