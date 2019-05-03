#include "io_manager.h"
#include "message_bus.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace {
    MessageBus* bus;
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        std::cout << "Scroll: " << yoffset << std::endl;
        MouseClickMessage m(xoffset, yoffset, GLFW_MOUSE_BUTTON_MIDDLE, 1000);
        bus->sendMessage(&m);
    }
}

IOManager::IOManager(GLFWwindow* window, MessageBus* messageBus) :
    window(window),
    messageBus(messageBus)
{
    bus = messageBus;
    mDeltaTime = 0;
    mLastFrame = 0;
    mMouseLastDeltaX = 0;
    mMouseLastDeltaY = 0;
    glfwSetScrollCallback(window, scroll_callback);

    mPrevGameMode = GameMode::Gameplay;
    mCurrGameMode = GameMode::Gameplay;
    mPrevEscKeyState = GLFW_RELEASE;
    mPrevLeftButtonState = GLFW_RELEASE;
    mPrevRightButtonState = GLFW_RELEASE;
    mPrevMiddleButtonState = GLFW_RELEASE;
    mPrevSpaceKeyState = GLFW_RELEASE;
}


IOManager::~IOManager()
{
}

void IOManager::processInput()
{
    float currentFrame = glfwGetTime();
    mDeltaTime = currentFrame - mLastFrame;
    mLastFrame = currentFrame;
    //TODO: sendMessageKey with deltaTime not speed
    float cameraSpeed = 5.0f * mDeltaTime;

    //TODO: Speed dependence on FPS
    //Move Camera (Position)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        sendMessageKey(GLFW_KEY_W, GLFW_PRESS, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        sendMessageKey(GLFW_KEY_S, GLFW_PRESS, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        sendMessageKey(GLFW_KEY_A, GLFW_PRESS, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        sendMessageKey(GLFW_KEY_D, GLFW_PRESS, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        sendMessageKey(GLFW_KEY_LEFT_SHIFT, GLFW_PRESS, cameraSpeed);
    if (mCurrGameMode == GameMode::Editor)
    {
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            sendMessageKey(GLFW_KEY_SPACE, GLFW_PRESS, cameraSpeed);
    }
    else if (mCurrGameMode == GameMode::Gameplay)
    {
        const int spaceKeyState = glfwGetKey(window, GLFW_KEY_SPACE);
        if (spaceKeyState == GLFW_PRESS && mPrevSpaceKeyState == GLFW_RELEASE)
        {
            sendMessageKey(GLFW_KEY_SPACE, GLFW_PRESS, cameraSpeed);
            mPrevSpaceKeyState = spaceKeyState;
        }
        if (spaceKeyState == GLFW_RELEASE && mPrevSpaceKeyState == GLFW_PRESS)
            mPrevSpaceKeyState = GLFW_RELEASE;
    }

    //Move Camera (Rotation)
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        sendMessageKey(GLFW_KEY_LEFT, GLFW_PRESS, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        sendMessageKey(GLFW_KEY_UP, GLFW_PRESS, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        sendMessageKey(GLFW_KEY_RIGHT, GLFW_PRESS, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        sendMessageKey(GLFW_KEY_DOWN, GLFW_PRESS, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        sendMessageKey(GLFW_KEY_Q, GLFW_PRESS, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        sendMessageKey(GLFW_KEY_E, GLFW_PRESS, cameraSpeed);

    // Render Control
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        sendMessageKey(GLFW_KEY_R, GLFW_PRESS, cameraSpeed);

    // Switch GameMode
    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
        sendMessageGameMode(GameMode::Gameplay);
        mPrevGameMode = mCurrGameMode;
        mCurrGameMode = GameMode::Gameplay;
    }
    if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) {
        sendMessageGameMode(GameMode::Editor);
        mPrevGameMode = mCurrGameMode;
        mCurrGameMode = GameMode::Editor;
    }

    const int F8KeyState = glfwGetKey(window, GLFW_KEY_F8);
    if (F8KeyState == GLFW_RELEASE && mPrevF8KeyState == GLFW_PRESS)
        sendMessageKey(GLFW_KEY_F8, GLFW_PRESS, cameraSpeed);
    mPrevF8KeyState = glfwGetKey(window, GLFW_KEY_F8);

    // Toggle menu
    const int escKeyState = glfwGetKey(window, GLFW_KEY_ESCAPE);
    if (escKeyState == GLFW_RELEASE && mPrevEscKeyState == GLFW_PRESS) {
        if (mCurrGameMode == GameMode::Menu) {
            sendMessageGameMode(mPrevGameMode);
            mCurrGameMode = mPrevGameMode;
        }
        else {
            sendMessageGameMode(GameMode::Menu);
            mPrevGameMode = mCurrGameMode;
            mCurrGameMode = GameMode::Menu;
        }
    }
    mPrevEscKeyState = escKeyState;

    // Mouse movement
    auto lastMouseX = mMousePositionX;
    auto lastMouseY = mMousePositionY;

    glfwGetCursorPos(window, &mMousePositionX, &mMousePositionY);

    mMouseLastDeltaX = mMousePositionX - lastMouseX;
    mMouseLastDeltaY = mMousePositionY - lastMouseY;

    sendMessageMouseMove();

    // Mouse buttons
    const int leftButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    const int rightButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    const int middleButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
    if (leftButtonState == GLFW_RELEASE && mPrevLeftButtonState == GLFW_PRESS) {
        sendMessageMouseClick(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS);
    }
    if (rightButtonState == GLFW_RELEASE && mPrevRightButtonState == GLFW_PRESS) {
        sendMessageMouseClick(GLFW_MOUSE_BUTTON_RIGHT, GLFW_PRESS);
    }
    if (middleButtonState == GLFW_RELEASE && mPrevMiddleButtonState == GLFW_PRESS) {
        sendMessageMouseClick(GLFW_MOUSE_BUTTON_MIDDLE, GLFW_PRESS);
    }
    mPrevLeftButtonState = leftButtonState;
    mPrevRightButtonState = rightButtonState;
    mPrevMiddleButtonState = middleButtonState;
}

void IOManager::sendMessageGameMode(GameMode mode) {
	GameModeMessage m(mode);
	this->messageBus->sendMessage(&m);
}

void IOManager::sendMessageKey(int key, int action, float speed) {
	KeyMessage m(key, action, speed);
	this->messageBus->sendMessage(&m);
}

void IOManager::sendMessageMouseMove()
{
    float deltaX = mMouseLastDeltaX;
    float deltaY = mMouseLastDeltaY;
    if (deltaX >= 9999 | deltaY >= 9999)
        return;
    MouseMoveMessage m(mMousePositionX, mMousePositionY, mMouseLastDeltaX, mMouseLastDeltaY);
    this->messageBus->sendMessage(&m);
}

void IOManager::sendMessageMouseClick(int input, int action)
{
    MouseClickMessage m(mMousePositionX, mMousePositionY, input, action);
    this->messageBus->sendMessage(&m);
}
