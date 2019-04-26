#include "mesh_node.h"
#include "load_mesh.h"



MeshNode::MeshNode() {}

MeshNode::MeshNode(std::string path, bool interpolate_tangents)
{
    init(path, interpolate_tangents);
}

void MeshNode::init(std::string path, bool interpolate_tangents)
{
    mMesh = load_mesh_from_obj(path, interpolate_tangents);
    mColorRatio = 1;
    mColor = {0.5, 0.5, 0.5};
    mShader = glow::Program::createFromFile("../../data/shaders/object_test");
}

MeshNode::~MeshNode()
{
}

void MeshNode::createBuffer()
{
}

void MeshNode::deleteBuffer()
{
}

void MeshNode::render(const glow::UsedProgram & shader, glm::mat4 & projection, glm::mat4 & view)
{
    {
        //auto shader = mShader->use();
        shader.setUniform("model", mGlobalTransformation);
        shader.setUniform("colorRatio", mColorRatio);
        shader.setUniform("color", mColor);
        mMesh->bind().draw();
    }
}

void MeshNode::setColorRatio(float ratio)
{
    mColorRatio = ratio;
}

void MeshNode::setColor(glm::vec3 color)
{
    mColor = color;
}


