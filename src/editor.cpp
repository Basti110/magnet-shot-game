#include "editor.h"
#include "common.h"
#include "light.h"
#include "coordinate_axes.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <btBulletDynamicsCommon.h>

#include <json/json.hpp>

#include <iostream>
#include <fstream>


namespace
{
    struct CubeStorage
    {
        std::vector<float> translation;
        std::vector<float> color;
        float scaleX;
        float scaleY;
        float scaleZ;
        float angular_damping;
        float friction;
        float linear_damping;
        float mass;
        float restitution;
    };

    void to_json(nlohmann::json& j, const CubeStorage& p)
    {
        j = nlohmann::json
        {
            {"translation", p.translation},
            {"color", p.color},
            {"scaleX", p.scaleX}, 
            {"scaleY", p.scaleY}, 
            {"scaleZ", p.scaleZ},
            {"angular_damping", p.angular_damping},
            {"friction", p.friction},
            {"linear_damping", p.linear_damping},
            {"mass", p.mass},
            {"restitution", p.restitution},
        };
    }

    void from_json(const nlohmann::json& j, CubeStorage& p)
    {
        j.at("translation").get_to(p.translation);
        j.at("color").get_to(p.color);
        j.at("scaleX").get_to(p.scaleX);
        j.at("scaleY").get_to(p.scaleY);
        j.at("scaleZ").get_to(p.scaleZ);
        j.at("angular_damping").get_to(p.angular_damping);
        j.at("friction").get_to(p.friction);
        j.at("linear_damping").get_to(p.linear_damping);
        j.at("mass").get_to(p.mass);
        j.at("restitution").get_to(p.restitution);
    }

}

Editor::Editor(MessageBus* mB, SceneManager* scene, PhysicsManager* physics) : 
    mScene(scene), 
    mPhysics(physics),
    mIsActive(false)
{
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(0.0f, 0.0f, 0.0f));
    mCube = new Cube(trans, Color{1.0f, 1.0f, 1.0f}, mPhysics);
    mCube->createBuffer();
    mScene->appendNode(mCube);

    // Test Axes

    //CoordinateAxes* axes = new CoordinateAxes({4.75, 8, 4.0}, mPhysics);

    //mScene->appendNode(axes);
    // ADD BRIDGE ------------------------------------------------
    trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(-3.45f, -0.5f, -27.75f));
    Cube* newCube = new Cube(trans, {red.r, red.g, red.b}, mPhysics);
    mScene->appendNode(newCube);

    RigidBodyInfo info;
    info.mass = 1.0;
    info.friction = 0.5;
    newCube->addPhysics({4.0, 1.0, 6.0}, info);
    btRigidBody* body = newCube->getRigidBody();
    body->setActivationState(DISABLE_DEACTIVATION);
    const btVector3 btPivotA(0.0, -0.5f, -3.0f); // right next to the door slightly outside
    btVector3 btAxisA(1.0f, 0.0f, 0.0f);       // pointing upwards, aka Y-axis

    btHingeConstraint* spDoorHinge = new btHingeConstraint(*body, btPivotA, btAxisA);
    spDoorHinge->setLimit(-SIMD_PI * 0.5f, SIMD_PI * 0.5f);
    //spDoorHinge->
    mPhysics->getDynamicsWorld()->addConstraint(spDoorHinge);
    spDoorHinge->setDbgDrawSize(btScalar(5.f));
    // -----------------------------------------------------------

    mB->addGameModeReceiver([=](GameModeMessage message) {
        this->notifyGameModeChange(message);
    });
    mB->addMouseClickReceiver([=](MouseClickMessage message) {
        if (mIsActive) this->notifyMouseClickInput(message);
    });
    mB->addMouseMoveReceiver([=](MouseMoveMessage message) {
        if (mIsActive) this->notifyMouseMoveInput(message);
    });
    mB->addKeyReceiver([=](KeyMessage message) {
        if (mIsActive) this->notifyKeyInput(message);
    });
    mB->addGuiReceiver([=](Message* message) {
        this->notifyGuiInput(message);
    });

    mCubeDistance = 2.0;
    // load();
}

void Editor::refreshCube()
{
    if (mGameMode != GameMode::Editor)
        return;

    Camera* cam = mScene->getCamera();
    glm::vec3 front = cam->getCameraFront();
    glm::vec3 pos = cam->getCameraPosition();
    glm::vec3 movePos = front * glm::vec3(mCubeDistance) + pos;
    float x = movePos.x; //(int)(movePos.x + 0.5);
    float y = movePos.y; //(int)(movePos.y + 0.5);
    float z = movePos.z; //(int)(movePos.z + 0.5);
    glm::mat4 transform = glm::translate(glm::mat4(1.0), glm::vec3(x, y, z));
    transform *= glm::scale(glm::mat4(1.0), mCubeScale);
    
    mCube->setLocalTransformation(transform);
}

void Editor::notifyGameModeChange(GameModeMessage message)
{
    mIsActive = message.mode == GameMode::Editor || message.mode == GameMode::Editor2;
    mCube->setVisible(message.mode == GameMode::Editor);
    mGameMode = message.mode;

	if (mGameMode != GameMode::Editor2)
    {
            PickBodyMessage* m = new PickBodyMessage(nullptr);
            MessageBus::getInstance()->sendMessage(m);
    }

}

void Editor::notifyMouseClickInput(MouseClickMessage message)
{
    if (message.getInput() == GLFW_MOUSE_BUTTON_LEFT && mGameMode == GameMode::Editor)
    {
        if (message.getAction() == GLFW_PRESS)
        {
            Cube* newCube = new Cube(*mCube);
            auto trans = mCube->getLocalTransformation();
            trans -= glm::scale(glm::mat4(1.0), mCubeScale);
            trans += glm::mat4(1.0);
            newCube->setLocalTransformation(trans);
            mScene->appendNode(newCube);
            newCube->addPhysics(mCubeScale, mCubeInfo);
            mCubes.push_back(newCube);
        }
    }
    if (message.getInput() == GLFW_MOUSE_BUTTON_MIDDLE && message.getAction() == 1000)
    {
        if (mGameMode == GameMode::Editor)
        {
            mCubeDistance += message.getPostion().y;
            if (mCubeDistance > 10)
                mCubeDistance = 10;
            if (mCubeDistance < 1)
                mCubeDistance = 1;
            refreshCube();
        }
        if (mGameMode == GameMode::Editor2)
        {
            mCoordinateAxesScale += message.getPostion().y;
            if (mCoordinateAxesScale > 10)
                mCoordinateAxesScale = 10;
            if (mCoordinateAxesScale < 1)
                mCoordinateAxesScale = 1;
            CoordinateAxes::setScale(mCoordinateAxesScale);
        }
    }
    if (mGameMode == GameMode::Editor2 && message.getInput() == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (message.getAction() == GLFW_PRESS)
        {
            glm::vec3 direction = getRayTo(message.getPostion().x, message.getPostion().y);
            direction = glm::normalize(direction);
            std::cout << direction.x << ", " << direction.y << ", " << direction.z << "\n";
        
            Camera* cam = mScene->getCamera();
            glm::vec3 pos = cam->getCameraPosition();
            std::cout << pos.x << ", " << pos.y << ", " << pos.z << "\n";
            pos = pos + 2.f * glm::normalize(direction);
            btRigidBody* test = mPhysics->pickBody(pos, direction * 200.f);
            //test->
            /*if (test != nullptr)
                std::cout << "Hit Object!: " << test->getUserIndex() << "\n";*/
            PickBodyMessage* m = new PickBodyMessage(test);
            MessageBus::getInstance()->sendMessage(m);
        }
    }
    std::cout << "click\n";
}

void Editor::notifyMouseMoveInput(MouseMoveMessage message)
{
    if (mGameMode == GameMode::Editor)
    {
        refreshCube();
        Camera* cam = mScene->getCamera();
        glm::vec3 front = cam->getCameraFront();
        glm::vec3 pos = cam->getCameraPosition();
        glm::mat4 projection = mScene->getCamera()->getProjectionMatrix();
        glm::vec4 v = glm::vec4(front.x, front.y, front.z, 1) * glm::vec4(10);
        v = projection * v;
        btRigidBody* test = mPhysics->pickBody(pos, glm::vec3(v.x, v.y, v.z));
        //if (test != nullptr)
            //std::cout << "Hit Object!: " << test->getUserIndex() << "\n";
    }
}

void Editor::notifyKeyInput(KeyMessage message)
{
    // Movement
    if (message.getAction() == GLFW_PRESS)
    {
        refreshCube();
        if (message.getInput() == GLFW_KEY_G)
        {
            if (glfwGetTime() - mLastDelete <= 0.5)
                return;
            mLastDelete = glfwGetTime();
          
            if (mCubes.size() <= 0)
                return;
            auto cube = mCubes[mCubes.size() - 1];
            mCubes.pop_back();
            mScene->removeNode([=](AbstractNode* node) { return dynamic_cast<Cube*>(node) == cube; });
        }
    }
}

void Editor::notifyGuiInput(Message* message)
{
    if (message->getType() == MType::GUI_VEC3)
    {
        GuiVec3Message* m = dynamic_cast<GuiVec3Message*>(message);
        if (m == nullptr)
            return;

        if (m->getSetting() == GuiSettings::CUBE_COLOR)
        {
            mCube->setColor(m->getValue());
        }

        if (m->getSetting() == GuiSettings::LIGHT_AMBIENT)
        {
            Light* light = mScene->getSun();
            if (light)
                light->setAmbient(m->getValue());
        }

		if (m->getSetting() == GuiSettings::LIGHT_DIFFUSE)
        {
            Light* light = mScene->getSun();
            if (light)
                light->setDiffuse(m->getValue());
        }

		if (m->getSetting() == GuiSettings::LIGHT_SPECULAR)
        {
            Light* light = mScene->getSun();
            if (light)
                light->setSpecular(m->getValue());
        }

        if (m->getSetting() == GuiSettings::CUBE_SHAPE)
        {
            auto transformation = mCube->getLocalTransformation();
            glm::vec3 translation = glm::vec3(transformation[3]);
            transformation = glm::translate(glm::mat4(1.0), translation);
            transformation *= glm::scale(glm::mat4(1.0f), m->getValue());
            mCube->setLocalTransformation(transformation);
            mCubeScale = m->getValue();
        }
    }

    if (message->getType() == MType::GUI_FLOAT)
    {
        GuiFloatMessage* m = dynamic_cast<GuiFloatMessage*>(message);
        if (m == nullptr)
            return;

        if (m->getSetting() == GuiSettings::CUBE_SIZE)
        {
            
        }

        if (m->getSetting() == GuiSettings::CUBE_MASS)
        {
            mCubeInfo.mass = m->getValue();
        }
        if (m->getSetting() == GuiSettings::CUBE_FRICTION)
        {
            mCubeInfo.friction = m->getValue(); 
        }
        if (m->getSetting() == GuiSettings::CUBE_RESTITUTION)
        {
            mCubeInfo.restitution = m->getValue(); 
        }
        if (m->getSetting() == GuiSettings::CUBE_LINEAR_DAMPING)
        {
            mCubeInfo.linearDamping = m->getValue(); 
        }
        if (m->getSetting() == GuiSettings::CUBE_ANGULAR_DAMPING)
        {
            mCubeInfo.angularDamping = m->getValue();
        }
        if (m->getSetting() == GuiSettings::SAVE)
        {
            save();
        }
    }
}

void Editor::save() 
{
    nlohmann::json json;
    for (auto cube : mCubes)
    {
        glm::vec3 scale = cube->getScale();
        glm::vec3 color = cube->getColor();
        glm::vec3 trans = glm::vec3(cube->getGlobalTransformation()[3]);

        CubeStorage s;
        s.translation = {trans.x, trans.y, trans.z};
        s.color = {color.x, color.y, color.z};
        s.scaleX = scale.x;
        s.scaleY = scale.y;
        s.scaleZ = scale.z;
     
        s.angular_damping = cube->getRigidBodyInfo().angularDamping;
        s.friction = cube->getRigidBodyInfo().friction;
        s.linear_damping = cube->getRigidBodyInfo().linearDamping;
        s.mass = cube->getRigidBodyInfo().mass;
        s.restitution = cube->getRigidBodyInfo().restitution;
        nlohmann::json j = s;
        //j["shape"] = cube->getRigidBodyInfo().shape;
        json += j;
    }
    //std::cout << json;

    std::ofstream myfile;
    myfile.open("../../data/world.json");
    myfile << json;
    myfile.close();
}

void Editor::load() {
    std::ifstream infile;
    infile.open("../../data/world.json");
    std::string worldString;

    infile >> worldString;
    std::cout << worldString;

    nlohmann::json newJson = nlohmann::json::parse(worldString);
    for (auto j : newJson)
    {
        auto storage = j.get<CubeStorage>();
        auto t = storage.translation;
        auto c = storage.color;
        glm::vec3 scale = {storage.scaleX, storage.scaleY, storage.scaleX};
        RigidBodyInfo info;
        info.friction = storage.friction;
        info.mass = storage.mass;

        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::translate(trans, glm::vec3(t[0], t[1], t[2]));
        trans = glm::scale(trans, glm::vec3(1));
        Cube* newCube = new Cube(trans, Color{c[0], c[1], c[2]}, mPhysics);
        newCube->addPhysics(scale, info);
        newCube->setScale(scale);
        mScene->appendNode(newCube);
        //newCube->addPhysics(mCubeScale, mCubeInfo);
        mCubes.push_back(newCube);
    }
}

//TODO: Do not use hard coded camera Settings 
glm::vec3 Editor::getRayTo(int x, int y)
{
    float top = 1.f;
    float bottom = -1.f;
    float nearPlane = 0.1f;
    float tanFov = (top - bottom) * 0.5f / nearPlane;
    float fov = btScalar(2.0) * btAtan(tanFov);

    glm::vec3 camPos, camTarget;

    camPos = mScene->getCamera()->getCameraPosition();

    glm::vec3 rayFrom = camPos;
    glm::vec3 rayForward = glm::normalize(mScene->getCamera()->getCameraFront());
    float farPlane = 500.f;
    rayForward *= farPlane;

    btVector3 rightOffset;
    glm::vec3 vertical = mScene->getCamera()->getCameraUp();


    glm::vec3 hor;
    hor = -glm::normalize(glm::cross(vertical, rayForward));

    float tanfov = tanf(0.5f * glm::radians(75.0f));

    hor *= 2.f * farPlane * tanfov;
    vertical *= 2.f * farPlane * tanfov;

    float aspect;
    float width = float(mScene->getCamera()->getWidth());
    float height = float(mScene->getCamera()->getHeight());

    aspect = width / height;

    hor *= aspect;

    glm::vec3 rayToCenter = rayFrom + rayForward;
    glm::vec3 dHor = hor * 1.f / width;
    glm::vec3 dVert = vertical * 1.f / height;

    glm::vec3 rayTo = rayToCenter - 0.5f * hor + 0.5f * vertical;
    rayTo += float(x) * dHor;
    rayTo -= float(y) * dVert;
    return rayTo;
}
