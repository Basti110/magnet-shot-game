#include "editor.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <btBulletDynamicsCommon.h>
#include "light.h"


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

    // ADD BRIDGE ------------------------------------------------
    trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(-3.25f, -0.5f, -27.75f));
    Cube* newCube = new Cube(trans, Color{1.0f, 1.0f, 1.0f}, mPhysics);
    mScene->appendNode(newCube);

    RigidBodyInfo info;
    info.mass = 1.0;
    info.friction = 0.5;
    newCube->addPhysics({4.0, 1.0, 6.0}, info);
    btRigidBody* body = mPhysics->getRigidBody(newCube->getPhysicsID());
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
}

void Editor::refreshCube()
{
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
    mIsActive = message.mode == GameMode::Editor;
    mCube->setVisible(mIsActive);
}

void Editor::notifyMouseClickInput(MouseClickMessage message)
{
    if (message.getInput() == GLFW_MOUSE_BUTTON_LEFT)
    {
        Cube* newCube = new Cube(*mCube);
        auto trans = mCube->getLocalTransformation();
        trans -= glm::scale(glm::mat4(1.0), mCubeScale);
        trans += glm::mat4(1.0);
        newCube->setLocalTransformation(trans);
        mScene->appendNode(newCube);
        newCube->addPhysics(mCubeScale, mCubeInfo);
    }
    if (message.getInput() == GLFW_MOUSE_BUTTON_MIDDLE && message.getAction() == 1000)
    {
        mCubeDistance += message.getPostion().y;
        if (mCubeDistance > 10)
            mCubeDistance = 10;
        if (mCubeDistance < 1)
            mCubeDistance = 1;
        refreshCube();
    }
}

void Editor::notifyMouseMoveInput(MouseMoveMessage message)
{
    refreshCube();
    Camera* cam = mScene->getCamera();
    glm::vec3 front = cam->getCameraFront();
    glm::vec3 pos = cam->getCameraPosition();
    glm::mat4 projection = mScene->getCamera()->getProjectionMatrix();
    glm::vec4 v = glm::vec4(front.x, front.y, front.z, 1) * glm::vec4(10);
    v = projection * v;
    btRigidBody* test = mPhysics->pickBody(pos, glm::vec3(v.x, v.y, v.z));
    if (test != nullptr)
        std::cout << "Hit Object!: " << test->getUserIndex() << "\n";
}

void Editor::notifyKeyInput(KeyMessage message)
{
    // Movement
    if (message.getAction() == GLFW_PRESS)
    {
        refreshCube();
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

        if (m->getSetting() == GuiSettings::LIGHT_COLOR)
        {
            Light* light = mScene->getLight();
            if (light)
                light->setColor(m->getValue());
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
    }
}
