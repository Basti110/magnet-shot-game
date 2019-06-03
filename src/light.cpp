#include "light.h"
#include <glow/objects/Program.hh>

bool Light::mIsInit = false;
unsigned int Light::VBO = 0;
unsigned int Light::VAO = 0;
glow::SharedProgram Light::mShader = glow::SharedProgram(); // glow::Program::createFromFile("../data/shaders/shader_light");

Light::Light(glm::mat4& transformation, Color color)
{
    mShader = glow::Program::createFromFile("../../data/shaders/shader_light");
    mColor = {color.red, color.green, color.blue};

    mLocalTransformation = transformation;
    mGlobalTransformation = transformation;

    if (mIsInit)
        return;

    this->mVertices = {
       -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f, 
       -0.5f,  0.5f, -0.5f,  
       -0.5f, -0.5f, -0.5f,  

       -0.5f, -0.5f,  0.5f, 
        0.5f, -0.5f,  0.5f,  
        0.5f,  0.5f,  0.5f,  
        0.5f,  0.5f,  0.5f, 
       -0.5f,  0.5f,  0.5f, 
       -0.5f, -0.5f,  0.5f, 

       -0.5f,  0.5f,  0.5f,  
       -0.5f,  0.5f, -0.5f,  
       -0.5f, -0.5f, -0.5f,  
       -0.5f, -0.5f, -0.5f,  
       -0.5f, -0.5f,  0.5f, 
       -0.5f,  0.5f,  0.5f,  

        0.5f,  0.5f,  0.5f,  
        0.5f,  0.5f, -0.5f, 
        0.5f, -0.5f, -0.5f,  
        0.5f, -0.5f, -0.5f,  
        0.5f, -0.5f,  0.5f,  
        0.5f,  0.5f,  0.5f,  

       -0.5f, -0.5f, -0.5f,  
        0.5f, -0.5f, -0.5f,  
        0.5f, -0.5f,  0.5f, 
        0.5f, -0.5f,  0.5f,  
       -0.5f, -0.5f,  0.5f, 
       -0.5f, -0.5f, -0.5f, 

       -0.5f,  0.5f, -0.5f,  
        0.5f,  0.5f, -0.5f, 
        0.5f,  0.5f,  0.5f,  
        0.5f,  0.5f,  0.5f,
       -0.5f,  0.5f,  0.5f,
       -0.5f,  0.5f, -0.5f,
    };
    createBuffer();
    mIsInit = true;
}


Light::~Light() {}

void Light::createBuffer()
{
    if (mIsInit)
        return;

    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);

    glBindVertexArray(this->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(float), &mVertices[0], GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Light::deleteBuffer() {}

void Light::render(const glow::UsedProgram& shader, glm::mat4& projection, glm::mat4& view, bool shadowPass)
{
    if (this->mIsInit)
    {
        auto shader = mShader->use();
        shader.setUniform("model", mGlobalTransformation);
        shader.setUniform("projection", projection);
        shader.setUniform("view", view);
        shader.setUniform("color", mColor);
        glBindVertexArray(this->VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

void Light::setColor(glm::vec3 color)
{
    mColor = color;
}

glm::vec3 Light::getColor()
{
    return mColor;
}
