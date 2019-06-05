#include "coordinate_axes.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

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

    RigidBodyInfo info;
    info.mass = 0;

    mArrowX = physics->addCoordinateAxisCone({0.1, 0.2}, getPhysicsArrowModelX(), info);
    mArrowY = physics->addCoordinateAxisCone({0.1, 0.2}, getPhysicsArrowModelY(), info);
    mArrowZ = physics->addCoordinateAxisCone({0.1, 0.2}, getPhysicsArrowModelZ(), info);

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

void CoordinateAxes::render(glow::UsedProgram& shader, glm::mat4& projection, glm::mat4& view, bool shadowPass)
{
    glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(mScale, mScale, mScale));
    glLineWidth(10 * mScale);

    glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3({0, 0, mScale * .5}));
    glm::mat4 model = getPhysicsArrowModelZ() * s;
    render(shader, model, {1, 0, 0}, t);

    model = getPhysicsArrowModelX() * s;
    t = glm::translate(glm::mat4(1.0f), glm::vec3({-mScale * .5, 0, 0}));
    render(shader, model, {0, 1, 0}, t);

    model = getPhysicsArrowModelY() * s;
    t = glm::translate(glm::mat4(1.0f), glm::vec3({0, -mScale * .5, 0}));
    render(shader, model, {0, 0, 1}, t);

    glLineWidth(1);
    AbstractNode::render(shader, projection, view, shadowPass);
}

void CoordinateAxes::render(glow::UsedProgram& shader, glm::mat4& model, glm::vec3 color, glm::mat4& lineTranslate)
{
    shader.setUniform("model", model);
    shader.setUniform("colorRatio", 1.0f);
    shader.setUniform("color", color);
    mVertexArray->bind().draw();
    model = lineTranslate * model;
    shader.setUniform("model", model);
    shader.setUniform("colorRatio", 1.0f);
    shader.setUniform("color", color);
    mLineSVA->bind().draw();
}

void CoordinateAxes::setScale(float scale) 
{
    mScale = scale;
}

void CoordinateAxes::update(float elapsedSeconds)
{
    AbstractNode::update(elapsedSeconds);

    mArrowX->setWorldTransform(to_bullet(getPhysicsArrowModelX()));
    mArrowY->setWorldTransform(to_bullet(getPhysicsArrowModelY()));
    mArrowZ->setWorldTransform(to_bullet(getPhysicsArrowModelZ()));

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
    float d = (delta.x - delta.y) / 30;
    glm::mat4 t;
    if (mHitBody == 1)
    {
        t = glm::translate(glm::mat4(1.0f), {d, 0, 0});
    }
    else if (mHitBody == 2)
    {
        t = glm::translate(glm::mat4(1.0f), {0, d, 0});
    }
    else
    {
        t = glm::translate(glm::mat4(1.0f), {0, 0, -d});
    }
    this->setGlobalTransformation(t * this->getGlobalTransformation());
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

glm::mat4 CoordinateAxes::getPhysicsArrowModelX()
{
    glm::vec3 pos = glm::vec3(mGlobalTransformation[3]) + glm::vec3({mScale, 0, 0});
    glm::mat4 r = glm::rotate(glm::mat4(1.0f), -0.5f * glm::pi<float>(), glm::vec3({0, 0, 1}));
    glm::mat4 t = glm::translate(glm::mat4(1.0f), pos);
    return t * r;
}

glm::mat4 CoordinateAxes::getPhysicsArrowModelY()
{
    glm::vec3 pos = glm::vec3(mGlobalTransformation[3]) + glm::vec3({0, mScale, 0});
    return glm::translate(glm::mat4(1.0f), pos);
}

glm::mat4 CoordinateAxes::getPhysicsArrowModelZ()
{
    glm::vec3 pos = glm::vec3(mGlobalTransformation[3]) + glm::vec3({0, 0, -mScale});
    glm::mat4 t = glm::translate(glm::mat4(1.0f), pos);
    glm::mat4 r = glm::rotate(glm::mat4(1.0f), -0.5f * glm::pi<float>(), glm::vec3({1, 0, 0}));
    return t * r;
}
