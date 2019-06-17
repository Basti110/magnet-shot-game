#pragma once
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
struct GLFWwindow;
class MessageBus;
enum class GameMode;


class IOManager
{
public:
    IOManager(GLFWwindow* window, MessageBus* messageBus);
    ~IOManager();
    void processInput();
private:
    void sendMessageGameMode(GameMode mode);
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

    GameMode mPrevGameMode;
    GameMode mCurrGameMode;
    int mPrevF6KeyState;
    int mPrevF7KeyState;
    int mPrevF8KeyState;
    int mPrevSpaceKeyState;
    int mPrevEscKeyState;
    int mPrevLeftButtonState;
    int mPrevRightButtonState;
    int mPrevMiddleButtonState;

    bool mPressW = false;
    bool mPressS = false;
    bool mPressA = false;
    bool mPressD = false;
    bool mPress1 = false;
    bool mPress2 = false;
    bool mPress3 = false;
};

