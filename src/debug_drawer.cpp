#include "debug_drawer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

DebugDrawer::DebugDrawer() 
{
    mShader = glow::Program::createFromFile("../../data/shaders/line");
    bool test = mShader->isLinked();
}

DebugDrawer::~DebugDrawer() 
{
    if (mInit)
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
}

void DebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) 
{
    setLineBuffer(from, to);
}

void DebugDrawer::setLineBuffer(const btVector3& from, const btVector3& to)
{
    mVertices.push_back(from.x());
    mVertices.push_back(from.y());
    mVertices.push_back(from.z());
    mVertices.push_back(to.x());
    mVertices.push_back(to.y());
    mVertices.push_back(to.z());

}

void DebugDrawer::draw(glm::mat4& projection, glm::mat4& view, bool loadNew)
{   
    if (mInit && loadNew)
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

    if (loadNew)
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(float), &mVertices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

    auto shader = mShader->use();
    shader.setUniform("projection", projection);
    shader.setUniform("view", view);
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, mVertices.size() / 3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    mInit = true;
}

void DebugDrawer::resetLines() 
{
    mVertices.clear();
}

