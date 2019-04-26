#pragma once
#include "abstract_node.h"
#include <glow/objects/VertexArray.hh>

class MeshNode : public AbstractNode
{
public:
    MeshNode();
    MeshNode(std::string path, bool interpolate);
    ~MeshNode();

    virtual void createBuffer() override;
    virtual void deleteBuffer() override;
    virtual void render(const glow::UsedProgram& shader, glm::mat4& model, glm::mat4& view) override;
    void setColorRatio(float ratio);
    void setColor(glm::vec3 color);
    void init(std::string path, bool interpolate);
    glow::SharedProgram mShader;
    //bool setTexture(std::string path);
private:
    glow::SharedVertexArray mMesh;
    float mColorRatio;
    glm::vec3 mColor;
};

