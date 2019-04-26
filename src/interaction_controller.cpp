#include "interaction_controller.h"
#include <GLFW/glfw3.h>
#include <btBulletDynamicsCommon.h>
#include "io_manager.h"


InteractionController::InteractionController(MessageBus* messageBus, SceneManager* scene, PhysicsManager* physics, GLFWwindow* window) :
    mScene(scene),
    mPhysics(physics),
    mWindow(window),
    mCursorOn(true)
{
    messageBus->addKeyReceiver(this->getNotifyFuncKey());
    messageBus->addMouseClickReceiver([=](MouseClickMessage message) {
        this->notifyMouseClickInput(message);
    });
    messageBus->addMouseMoveReceiver(this->getNotifyFuncMouseMove());
    // this->cam = renderer->getScene()->getCamera();
}


InteractionController::~InteractionController() {}

void InteractionController::notifyKeyInput(KeyMessage message)
{
    GLFWwindow* window = mWindow;
    Camera* cam = mScene->getCamera();

    // Close Windows
    if (message.getAction() == GLFW_PRESS)
    {
        /*if (message.getInput() == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(window, true);*/

        float cameraSpeed = message.getSpeed();
        // TODO: Speed dependence on FPS
        // Move Camera (Position)

        if (message.getInput() == GLFW_KEY_W)
            cam->moveFront(cameraSpeed);
        if (message.getInput() == GLFW_KEY_S)
            cam->moveFront(-cameraSpeed);
        if (message.getInput() == GLFW_KEY_A)
            cam->moveRight(-cameraSpeed);
        if (message.getInput() == GLFW_KEY_D)
            cam->moveRight(cameraSpeed);
        if (message.getInput() == GLFW_KEY_LEFT_SHIFT)
            cam->moveUp(-cameraSpeed);
        if (message.getInput() == GLFW_KEY_SPACE)
            cam->moveUp(cameraSpeed);

        // Move Camera (Rotation)
        if (message.getInput() == GLFW_KEY_LEFT)
            cam->rotateYaw(cameraSpeed / 2);
        if (message.getInput() == GLFW_KEY_UP)
            cam->rotatePitch(-cameraSpeed / 2);
        if (message.getInput() == GLFW_KEY_RIGHT)
            cam->rotateYaw(-cameraSpeed / 2);
        if (message.getInput() == GLFW_KEY_DOWN)
            cam->rotatePitch(cameraSpeed / 2);
        if (message.getInput() == GLFW_KEY_Q)
            cam->rotateRoll(-cameraSpeed / 2);
        if (message.getInput() == GLFW_KEY_E)
            cam->rotateRoll(cameraSpeed / 2);

        // Render Control
        if (message.getInput() == GLFW_KEY_R)
            cam->resetPosition();
        /*if (message.getInput() == GLFW_KEY_F1)
            renderer->setRenderMode(RenderSystem::RenderMode::NORMAL);
        if (message.getInput() == GLFW_KEY_F2)
            renderer->setRenderMode(RenderSystem::RenderMode::MAP_VIEW);
        if (message.getInput() == GLFW_KEY_F3)
            renderer->setRenderMode(RenderSystem::RenderMode::CUBE);
        if (message.getInput() == GLFW_KEY_F4)
            renderer->setRenderMode(RenderSystem::RenderMode::SPHERICAL);*/

        // IOControl

    }
}

void InteractionController::notifyMouseClickInput(MouseClickMessage message)
{
    if (message.getInput() == GLFW_MOUSE_BUTTON_RIGHT) {
        Camera* cam = mScene->getCamera();
        glm::vec3 pos = cam->getCameraPosition();
        glm::vec3 front = cam->getCameraFront();
        btRigidBody* body = mPhysics->pickBody(pos, pos + 200.0f*front);
        if (body != nullptr) {
            body->activate(true);
            body->applyCentralImpulse(btVector3(0, 1, 0));
        }
    }
}

void InteractionController::notifyMouseMoveInput(MouseMoveMessage message)
{
    Camera* cam = mScene->getCamera();
    float speed = 0.002;
    int mode = glfwGetInputMode(mWindow, GLFW_CURSOR);
    if (mode == GLFW_CURSOR_DISABLED)
    {
        cam->rotateYaw(-message.getDeltaPosition().x * speed);
        cam->rotatePitch(-message.getDeltaPosition().y * speed);
    }
}

std::function<void(KeyMessage)> InteractionController::getNotifyFuncKey()
{
    auto messageListener = [=](KeyMessage message) -> void { this->notifyKeyInput(message); };
    return messageListener;
}

std::function<void(MouseMoveMessage)> InteractionController::getNotifyFuncMouseMove()
{
    auto messageListener = [=](MouseMoveMessage message) -> void { this->notifyMouseMoveInput(message); };
    return messageListener;
}
