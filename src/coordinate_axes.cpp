#include "coordinate_axes.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

#include <glow/objects/ArrayBuffer.hh>
#include <glow/objects/ElementArrayBuffer.hh>
#include <glow/objects/VertexArray.hh>


//#include <glm/gtc/matrix_inverse.hpp>

CoordinateAxes::CoordinateAxes(const glm::vec3& position, float radius, float lenght, btDynamicsWorld* world) : PhysicsNode(world)
{
    glm::mat4 translation = glm::translate(glm::mat4(1), position);
    this->setGlobalTransformation(translation);
    mColor = glm::vec3({0.5, 0.5, 0.5});
    createConeBuffer(0.2, 0.1, 10);
    createLineBuffer(1);
    // glm::mat4 mat = mGlobalTransformation;
}

CoordinateAxes::~CoordinateAxes()
{
    if (!mInit)
        return;

    glDeleteVertexArrays(1, &coneVAO);
    glDeleteBuffers(1, &coneVBO);
}

void CoordinateAxes::render(const glow::UsedProgram& shader, glm::mat4& projection, glm::mat4& view)
{
    glm::mat4 r = glm::rotate(glm::mat4(1.0f), -0.5f * glm::pi<float>(), glm::vec3({1, 0, 0}));
    glm::vec3 pos = glm::vec3(mGlobalTransformation[3]) + glm::vec3({0, 0, -1});
    glm::mat4 t = glm::translate(glm::mat4(1.0f), pos);
    glm::mat4 model = t * r;
    glLineWidth(10);
    shader.setUniform("model", model);
    shader.setUniform("colorRatio", 1.0f);
    shader.setUniform("color", glm::vec3({1, 0, 0}));
    mVertexArray->bind().draw();
    t = glm::translate(glm::mat4(1.0f), glm::vec3({0, 0, 0.5}));
    model = t * model;
    shader.setUniform("model", model);
    shader.setUniform("colorRatio", 1.0f);
    shader.setUniform("color", glm::vec3({1, 0, 0}));
    mLineSVA->bind().draw();

    r = glm::rotate(glm::mat4(1.0f), -0.5f * glm::pi<float>(), glm::vec3({0, 0, 1}));
    pos = glm::vec3(mGlobalTransformation[3]) + glm::vec3({1, 0, 0});
    t = glm::translate(glm::mat4(1.0f), pos);
    model = t * r;
    shader.setUniform("model", model);
    shader.setUniform("colorRatio", 1.0f);
    shader.setUniform("color", glm::vec3({0, 1, 0}));
    mVertexArray->bind().draw();
    t = glm::translate(glm::mat4(1.0f), glm::vec3({-0.5, 0, 0}));
    model = t * model;
    shader.setUniform("model", model);
    shader.setUniform("colorRatio", 1.0f);
    shader.setUniform("color", glm::vec3({0, 1, 0}));
    mLineSVA->bind().draw();

    pos = glm::vec3(mGlobalTransformation[3]) + glm::vec3({0, 1, 0});
    t = glm::translate(glm::mat4(1.0f), pos);
    model = t;
    shader.setUniform("model", model);
    shader.setUniform("colorRatio", 1.0f);
    shader.setUniform("color", glm::vec3({0, 0, 1}));
    mVertexArray->bind().draw();
    t = glm::translate(glm::mat4(1.0f), glm::vec3({0, -0.5, 0}));
    model = t * model;
    shader.setUniform("model", model);
    shader.setUniform("colorRatio", 1.0f);
    shader.setUniform("color", glm::vec3({0, 0, 1}));
    mLineSVA->bind().draw();

    glLineWidth(1);
    AbstractNode::render(shader, projection, view);
}

/*void CoordinateAxes::render(const glow::UsedProgram& shader, glm::mat4& projection, glm::mat4& view)
{
    //auto shader = shader->use();
    shader.setUniform("model", mGlobalTransformation);
    shader.setUniform("projection", projection);
    shader.setUniform("view", view);
    glBindVertexArray(coneVAO);
    glDrawArrays(GL_TRIANGLES, 0, coneTriangles);
}*/

void CoordinateAxes::createConeBuffer(float length, float radius, int fineness)
{
    std::vector<glm::vec3> aPos;
    std::vector<glm::vec3> aNormal;
    std::vector<glm::vec3> aTangent;
    std::vector<glm::vec2> aTexCoord;

    std::vector<float> coneVertices;
    float LenHalf = length / 2;
    float circleStep = (glm::pi<float>() * 2) / fineness;
    glm::vec2 prevCoord = {glm::cos(0) * radius, glm::sin(0) * radius};
    glm::vec3 center = {0, -LenHalf, 0};
    float PrevAngle = 0;

    for (int i = 1; i <= fineness; ++i)
    {
        aPos.push_back({0, LenHalf, 0});
        aNormal.push_back({0, 1, 0});
        aPos.push_back({prevCoord.x, -LenHalf, prevCoord.y});
        aNormal.push_back(glm::normalize(glm::vec3({prevCoord.x, 0, prevCoord.y})));

        float newX = glm::cos(i * circleStep) * radius;
        float newZ = glm::sin(i * circleStep) * radius;
        prevCoord = {newX, newZ};

        aPos.push_back({prevCoord.x, -LenHalf, prevCoord.y});
        aNormal.push_back(glm::normalize(glm::vec3({prevCoord.x, 0, prevCoord.y})));

        for (int i = 0; i < 3; ++i)
        {
            aTangent.push_back({0, 0, 0});
            aTexCoord.push_back({0, 0});
        }
    }

    /*coneVertices.push_back(0);
    coneVertices.push_back(LenHalf);
    coneVertices.push_back(0);

    coneVertices.push_back(prevCoord.x);
    coneVertices.push_back(-LenHalf);
    coneVertices.push_back(prevCoord.y);

    coneVertices.push_back(glm::cos(0) * radius);
    coneVertices.push_back(LenHalf);
    coneVertices.push_back(glm::sin(0) * radius);

    glGenVertexArrays(1, &coneVAO);
    glGenBuffers(1, &coneVBO);

    glBindVertexArray(coneVAO);

    glBindBuffer(GL_ARRAY_BUFFER, coneVBO);
    glBufferData(GL_ARRAY_BUFFER, coneVertices.size() * sizeof(float), &coneVertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); */
    auto abPos = glow::ArrayBuffer::create("aPos", aPos);
    auto abNormal = glow::ArrayBuffer::create("aNormal", aNormal);
    auto abTangent = glow::ArrayBuffer::create("aTangent", aTangent);
    auto abTexCoord = glow::ArrayBuffer::create("aTexCoord", aTexCoord);

    glow::SharedElementArrayBuffer eab;
    mVertexArray = glow::VertexArray::create({abPos, abNormal, abTangent, abTexCoord}, eab, GL_TRIANGLES);
    mInit = true;
}

void CoordinateAxes::createLineBuffer(float length)
{
    std::vector<glm::vec3> aPos;
    std::vector<glm::vec3> aNormal;
    std::vector<glm::vec3> aTangent;
    std::vector<glm::vec2> aTexCoord;

    aPos.push_back({0, length / 2, 0});
    aPos.push_back({0, -length / 2, 0});
    aNormal.push_back({0, 1, 0});
    aNormal.push_back({0, 1, 0});
    aTangent.push_back({0, 0, 0});
    aTangent.push_back({0, 0, 0});
    aTexCoord.push_back({0, 0});
    aTexCoord.push_back({0, 0});

    auto abPos = glow::ArrayBuffer::create("aPos", aPos);
    auto abNormal = glow::ArrayBuffer::create("aNormal", aNormal);
    auto abTangent = glow::ArrayBuffer::create("aTangent", aTangent);
    auto abTexCoord = glow::ArrayBuffer::create("aTexCoord", aTexCoord);

    glow::SharedElementArrayBuffer eab;
    mLineSVA = glow::VertexArray::create({abPos, abNormal, abTangent, abTexCoord}, eab, GL_LINES);
}
