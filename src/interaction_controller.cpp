#include "interaction_controller.h"

#include "Cube.h"
#include "magnet.h"
#include "physics_manager.h"

#include <btBulletDynamicsCommon.h>
#include "bullet_helper.hh"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

InteractionController::InteractionController(MessageBus* messageBus, SceneManager* scene, GLFWwindow* window, PhysicsManager* physics) :
    mScene(scene),
    mWindow(window),
    mIsActive(false), 
    mPhysics(physics)
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

    RigidBodyInfo info;
    info.mass = 70.0;
    info.friction = 0.5;
 
    glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(-3.0f, 2.0f, 0.0f));
    mCube = new Cube(trans, Color{1.0f, 1.0f, 1.0f}, mPhysics);
    mScene->appendNode(mCube);
    mCube->addPhysics(glm::vec3(1.0f), info);
    mCube->addChild(mScene->getCamera());

    
    trans = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 2.0f, 0.0f));
    mCubeX = new Cube(trans, Color{1.0f, 1.0f, 1.0f}, mPhysics);
    //mCube->addChild(mCubeX);
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
