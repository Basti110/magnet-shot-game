#include "interaction_controller.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <btBulletDynamicsCommon.h>
#include "io_manager.h"
#include "magnet.h"
#include "bullet_helper.hh"


InteractionController::InteractionController(MessageBus* messageBus, SceneManager* scene, GLFWwindow* window) :
    mScene(scene),
    mWindow(window),
    mIsActive(false)
{
    messageBus->addGameModeReceiver([=](GameModeMessage message) {
        this->notifyGameModeChange(message);
    });
    messageBus->addKeyReceiver([=](KeyMessage message) {
        if (mIsActive) this->notifyKeyInput(message);
    });
    messageBus->addMouseMoveReceiver([=](MouseMoveMessage message) {
        if (mIsActive) this->notifyMouseMoveInput(message);
    });
}

void InteractionController::notifyGameModeChange(GameModeMessage message)
{
    mIsActive = (message.mode == GameMode::Gameplay || message.mode == GameMode::Editor);
}

void InteractionController::notifyKeyInput(KeyMessage message)
{
    GLFWwindow* window = mWindow;
    Camera* cam = mScene->getCamera();

    // Close Windows
    if (message.getAction() == GLFW_PRESS)
    {
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
