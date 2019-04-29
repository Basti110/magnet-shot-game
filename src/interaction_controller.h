#pragma once
//#include "render_system.h"
#include "message_bus.h"
#include "scnene_manager.h"
#include "physics_manager.h"
struct GLFWwindow;

class InteractionController
{
public:
    InteractionController(MessageBus* messageBus, SceneManager* scene, GLFWwindow* window);
    void notifyGameModeChange(GameModeMessage message);
    void notifyKeyInput(KeyMessage message);
    void notifyMouseMoveInput(MouseMoveMessage message);
private:
    SceneManager* mScene;
    GLFWwindow* mWindow;
    bool mIsActive;
};

