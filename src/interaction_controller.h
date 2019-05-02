#pragma once
//#include "render_system.h"
#include "message_bus.h"
#include "physics_manager.h"
#include "scnene_manager.h"
struct GLFWwindow;
class Cube;

class InteractionController
{
public:
    InteractionController(MessageBus* messageBus, SceneManager* scene, GLFWwindow* window, PhysicsManager* physics);
    void notifyGameModeChange(GameModeMessage message);
    void notifyKeyInput(KeyMessage message);
    void notifyMouseMoveInput(MouseMoveMessage message);

private:
    SceneManager* mScene;
    GLFWwindow* mWindow;
    PhysicsManager* mPhysics;
    Cube* mCube;
    Cube* mCubeX;
    bool mIsActive;
};
