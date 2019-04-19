#include "editor.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "light.h"


Editor::Editor(MessageBus* mB, SceneManager* scene) : mScene(scene)
{
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(0.0f, 0.0f, 0.0f));
    mCube = new Cube(trans, Color{1.0f, 1.0f, 1.0f});
    mCube->createBuffer();
    mScene->appendNode(mCube);
    mB->addMouseClickReceiver(getNotifyFuncMouseClick());
    mB->addMouseMoveReceiver(getNotifyFuncMouseMove());
    mB->addKeyReceiver(getNotifyFuncKey());
    mB->addGuiReceiver(getNotifyFuncGui());
    mBounceClick = 0;
    mCursorOn = true;
    mCubeDistance = 2.0;
}

Editor::~Editor() {}

std::function<void(MouseClickMessage)> Editor::getNotifyFuncMouseClick()
{
    auto messageListener = [=](MouseClickMessage message) -> void { this->notifyMouseClickInput(message); };
    return messageListener;
}

std::function<void(MouseMoveMessage)> Editor::getNotifyFuncMouseMove()
{
    auto messageListener = [=](MouseMoveMessage message) -> void { this->notifyMouseMoveInput(message); };
    return messageListener;
}

std::function<void(KeyMessage)> Editor::getNotifyFuncKey()
{
    auto messageListener = [=](KeyMessage message) -> void { this->notifyKeyInput(message); };
    return messageListener;
}

std::function<void(Message*)> Editor::getNotifyFuncGui()
{
    auto messageListener = [=](Message* message) -> void { this->notifyGuiInput(message); };
    return messageListener;
}

void Editor::refreshCube()
{
    Camera* cam = mScene->getCamera();
    glm::vec3 front = cam->getCameraFront();
    glm::vec3 pos = cam->getCameraPosition();
    glm::vec3 movePos = front * glm::vec3(mCubeDistance) + pos;
    int x = (int)(movePos.x + 0.5);
    int y = (int)(movePos.y + 0.5);
    int z = (int)(movePos.z + 0.5);
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(x, y, z));
    mCube->setLocalTransformation(trans);
}

void Editor::notifyMouseClickInput(MouseClickMessage message)
{
    if (!mCursorOn && message.getInput() == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (glfwGetTime() - mBounceClick > 0.2)
        {
            std::cout << "click\n";
            mBounceClick = glfwGetTime();
            Cube* newCube = new Cube(*mCube);
            mScene->appendNode(newCube);
        }
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
    return;
}

void Editor::notifyMouseMoveInput(MouseMoveMessage message)
{
    refreshCube();
}

void Editor::notifyKeyInput(KeyMessage message)
{
    // Movement
    if (message.getAction() == GLFW_PRESS)
    {
        refreshCube();

        if (message.getInput() == GLFW_KEY_F)
        {
            if (glfwGetTime() - mBounceF > 1)
            {
                if (mCursorOn == true)
                    mCursorOn = false;
                else
                    mCursorOn = true;
                mBounceF = glfwGetTime();
            }
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

        if (m->getSetting() == GuiSettings::LIGHT_COLOR)
        {
            Light* light = mScene->getLight();
            if (light)
                light->setColor(m->getValue());
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
    }
}
