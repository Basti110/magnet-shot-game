#pragma once
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
struct GLFWwindow;
class MessageBus;
class IOManager
{
public:
    IOManager(GLFWwindow* window, MessageBus* messageBus);
    ~IOManager();
    void processInput();
private:
    void sendMessageKey(int key, int action, float speed);
    void sendMessageMouseMove();
    void sendMessageMouseClick(int input, int action);

    GLFWwindow* window;
    MessageBus* messageBus;
    float mDeltaTime;
    float mLastFrame;
    double mMouseLastDeltaX;
    double mMouseLastDeltaY;
    double mMousePositionX;
    double mMousePositionY;

    float mBounceF;
};

