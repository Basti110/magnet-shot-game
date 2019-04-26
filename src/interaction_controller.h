#pragma once
//#include "render_system.h"
#include "scnene_manager.h"
#include "message_bus.h"
struct GLFWwindow;

class InteractionController
{
public:
    InteractionController(SceneManager* scene, GLFWwindow* window, MessageBus* messageBus);
    ~InteractionController();
	void notifyKeyInput(KeyMessage message);
    void notifyMouseMoveInput(MouseMoveMessage message);
private:
    std::function<void(KeyMessage)> getNotifyFuncKey();
    std::function<void(MouseMoveMessage)> getNotifyFuncMouseMove();
    
    SceneManager* mScene;
    GLFWwindow* mWindow;
    bool mCursorOn;
    float mBounceMouse;
    float mBounceF;
};

