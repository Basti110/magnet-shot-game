#include "magnet.h"
#include <glow-extras/geometry/UVSphere.hh>


Magnet::Magnet(PhysicsManager* physics, int physicsId, float scale, glm::vec3 color) :
    mVertexArray(glow::geometry::make_uv_sphere()),
    mShader(glow::Program::createFromFile("../../data/shaders/simple")),
    mPhysics(physics),
    mPhysicsId(physicsId),
    mScale(scale),
    mColor(color)
{

}

Magnet::~Magnet()
{

}

void Magnet::render(const glow::UsedProgram& currentShader, glm::mat4& projection, glm::mat4& view)
{
    AbstractNode::render(currentShader, projection, view);
    auto shader = mShader->use();
    shader.setUniform("uProjection", projection);
    shader.setUniform("uView", view);
    shader.setUniform("uModel", mGlobalTransformation);
    shader.setUniform("uScale", mScale);
    shader.setUniform("uColor", mColor);
    mVertexArray->bind().draw();
}

void Magnet::update(float elapsedSeconds)
{
    glm::mat4 transform;
    if (mPhysics->getTransformation(mPhysicsId, transform)) {
        mGlobalTransformation = transform;
    }
    AbstractNode::update(elapsedSeconds);
}
