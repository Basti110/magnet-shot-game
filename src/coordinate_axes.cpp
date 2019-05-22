#include "coordinate_axes.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>

#include <glow/objects/ArrayBuffer.hh>
#include <glow/objects/ElementArrayBuffer.hh>
#include <glow/objects/VertexArray.hh>

#include "bullet_helper.hh"
#include "physics_manager.h"


//#include <glm/gtc/matrix_inverse.hpp>
float CoordinateAxes::mScale = 1.0f;
CoordinateAxes::CoordinateAxes(const glm::vec3& position, PhysicsManager* physics)
{
    glm::mat4 translation = glm::translate(glm::mat4(1), position);
    this->setGlobalTransformation(translation);
    mColor = glm::vec3({0.5, 0.5, 0.5});
    createConeBuffer(0.2, 0.1, 10);
    createLineBuffer(1);
    // glm::mat4 mat = mGlobalTransformation;
    RigidBodyInfo info;
    info.mass = 0;

    glm::vec3 pos = glm::vec3(mGlobalTransformation[3]) + glm::vec3({0, 0, -mScale});
    glm::mat4 t = glm::translate(glm::mat4(1.0f), pos);
    glm::mat4 r = glm::rotate(glm::mat4(1.0f), -0.5f * glm::pi<float>(), glm::vec3({1, 0, 0}));
    glm::mat4 model = t * r;
    mArrowX = physics->addCoordinateAxisCone({0.1, 0.2}, model, info);

    pos = glm::vec3(mGlobalTransformation[3]) + glm::vec3({mScale, 0, 0});
    r = glm::rotate(glm::mat4(1.0f), -0.5f * glm::pi<float>(), glm::vec3({0, 0, 1}));
    t = glm::translate(glm::mat4(1.0f), pos);
    model = t * r;
    mArrowY = physics->addCoordinateAxisCone({0.1, 0.2}, model, info);

    pos = glm::vec3(mGlobalTransformation[3]) + glm::vec3({0, mScale, 0});
    t = glm::translate(glm::mat4(1.0f), pos);
    model = t;
    mArrowZ = physics->addCoordinateAxisCone({0.1, 0.2}, model, info);

    btCollisionShape* collisionShape = mArrowX->getCollisionShape();
    collisionShape->setLocalScaling({3.0, 3.0, 3.0});

    MessageBus::getInstance()->addMouseMoveReceiver([=](MouseMoveMessage message) {
        if (mHit)
            this->notifyMouseMoveInput(message);
    });

    MessageBus::getInstance()->addMouseClickReceiver([=](MouseClickMessage message) {
        if (mHit)
            this->notifyMouseClickInput(message);
    });
}

CoordinateAxes::~CoordinateAxes()
{
    if (!mInit)
        return;

    glDeleteVertexArrays(1, &coneVAO);
    glDeleteBuffers(1, &coneVBO);
}

bool CoordinateAxes::hitArrow(const btRigidBody* body)
{
    if (body == mArrowX)
    {
        mHitBody = 1;
        mHit = true;
        return true;
    }
    else if (body == mArrowY)
    {
        mHitBody = 2;
        mHit = true;
        return true;
    }
    else if (body == mArrowZ)
    {
        mHitBody = 3;
        mHit = true;
        return true;
    }
    return false;
}

void CoordinateAxes::render(const glow::UsedProgram& shader, glm::mat4& projection, glm::mat4& view)
{
    glm::mat4 r = glm::rotate(glm::mat4(1.0f), -0.5f * glm::pi<float>(), glm::vec3({1, 0, 0}));
    glm::vec3 pos = glm::vec3(mGlobalTransformation[3]) + glm::vec3({0, 0, -mScale});
    glm::mat4 t = glm::translate(glm::mat4(1.0f), pos);
    glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(mScale, mScale, mScale));
    glm::mat4 model = t * r * s;
    glLineWidth(10 * mScale);
    shader.setUniform("model", model);
    shader.setUniform("colorRatio", 1.0f);
    shader.setUniform("color", glm::vec3({1, 0, 0}));
    mVertexArray->bind().draw();
    t = glm::translate(glm::mat4(1.0f), glm::vec3({0, 0, mScale * .5}));
    model = t * model;
    shader.setUniform("model", model);
    shader.setUniform("colorRatio", 1.0f);
    shader.setUniform("color", glm::vec3({1, 0, 0}));
    mLineSVA->bind().draw();

    r = glm::rotate(glm::mat4(1.0f), -0.5f * glm::pi<float>(), glm::vec3({0, 0, 1}));
    pos = glm::vec3(mGlobalTransformation[3]) + glm::vec3({mScale, 0, 0});
    t = glm::translate(glm::mat4(1.0f), pos);
    model = t * r * s;
    shader.setUniform("model", model);
    shader.setUniform("colorRatio", 1.0f);
    shader.setUniform("color", glm::vec3({0, 1, 0}));
    mVertexArray->bind().draw();
    t = glm::translate(glm::mat4(1.0f), glm::vec3({-mScale * .5, 0, 0}));
    model = t * model;
    shader.setUniform("model", model);
    shader.setUniform("colorRatio", 1.0f);
    shader.setUniform("color", glm::vec3({0, 1, 0}));
    mLineSVA->bind().draw();

    pos = glm::vec3(mGlobalTransformation[3]) + glm::vec3({0, mScale, 0});
    t = glm::translate(glm::mat4(1.0f), pos);
    model = t * s;
    shader.setUniform("model", model);
    shader.setUniform("colorRatio", 1.0f);
    shader.setUniform("color", glm::vec3({0, 0, 1}));
    mVertexArray->bind().draw();
    t = glm::translate(glm::mat4(1.0f), glm::vec3({0, -mScale * .5, 0}));
    model = t * model;
    shader.setUniform("model", model);
    shader.setUniform("colorRatio", 1.0f);
    shader.setUniform("color", glm::vec3({0, 0, 1}));
    mLineSVA->bind().draw();

    glLineWidth(1);
    AbstractNode::render(shader, projection, view);
}

void CoordinateAxes::setScale(float scale) 
{
    mScale = scale;
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
void CoordinateAxes::update(float elapsedSeconds)
{
    AbstractNode::update(elapsedSeconds);
    glm::vec3 pos = glm::vec3(mGlobalTransformation[3]) + glm::vec3({0, 0, -mScale});
    glm::mat4 t = glm::translate(glm::mat4(1.0f), pos);
    glm::mat4 r = glm::rotate(glm::mat4(1.0f), -0.5f * glm::pi<float>(), glm::vec3({1, 0, 0}));
    glm::mat4 model = t * r;
    mArrowX->setWorldTransform(to_bullet(model));

    pos = glm::vec3(mGlobalTransformation[3]) + glm::vec3({mScale, 0, 0});
    r = glm::rotate(glm::mat4(1.0f), -0.5f * glm::pi<float>(), glm::vec3({0, 0, 1}));
    t = glm::translate(glm::mat4(1.0f), pos);
    model = t * r;
    mArrowY->setWorldTransform(to_bullet(model));

    pos = glm::vec3(mGlobalTransformation[3]) + glm::vec3({0, mScale, 0});
    t = glm::translate(glm::mat4(1.0f), pos);
    model = t;
    mArrowZ->setWorldTransform(to_bullet(model));

    if (mLastScale == mScale)
        return;

    btCollisionShape* collisionShape = mArrowX->getCollisionShape();
    collisionShape->setLocalScaling({mScale, mScale, mScale});

    collisionShape = mArrowY->getCollisionShape();
    collisionShape->setLocalScaling({mScale, mScale, mScale});

    collisionShape = mArrowZ->getCollisionShape();
    collisionShape->setLocalScaling({mScale, mScale, mScale});

    mLastScale = mScale;
}

void CoordinateAxes::notifyMouseMoveInput(MouseMoveMessage message) 
{
    glm::vec2 delta = message.getDeltaPosition();
    float d = (delta.x - delta.y) / 10; // glm::distance(glm::vec2(0,0), delta) / 10;
    glm::mat4 t;
    if (mHitBody == 1)
    {
        t = glm::translate(glm::mat4(1.0f), {0, 0, -d});
    }
    else if (mHitBody == 2)
    {
        t = glm::translate(glm::mat4(1.0f), {d, 0, 0});
    }
    else
    {
        t = glm::translate(glm::mat4(1.0f), {0, d, 0});
    }
    this->setGlobalTransformation(t * this->getGlobalTransformation());
    //glm::mat4 transformation = this->getGlobalTransformation();
}

void CoordinateAxes::notifyMouseClickInput(MouseClickMessage message) 
{
    if (message.getInput() == 0)
    {
        if (message.getAction() == 0)
        {
            mHit = false;
        }
    }
}

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
