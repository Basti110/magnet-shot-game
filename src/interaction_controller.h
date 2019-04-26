#pragma once
//#include "render_system.h"
#include "message_bus.h"
#include "scnene_manager.h"
#include "physics_manager.h"
struct GLFWwindow;

class InteractionController
{
public:
    InteractionController(MessageBus* messageBus, SceneManager* scene, PhysicsManager* physics, GLFWwindow* window);
    ~InteractionController();
    void notifyKeyInput(KeyMessage message);
    void notifyMouseClickInput(MouseClickMessage message);
    void notifyMouseMoveInput(MouseMoveMessage message);
private:
    std::function<void(KeyMessage)> getNotifyFuncKey();
    std::function<void(MouseMoveMessage)> getNotifyFuncMouseMove();
    
    SceneManager* mScene;
    PhysicsManager* mPhysics;
    GLFWwindow* mWindow;
    bool mCursorOn;
    float mBounceMouse;
    float mBounceF;
};

