#pragma once
//#include "render_system.h"
#include "message_bus.h"
#include "physics_manager.h"
#include "scnene_manager.h"
struct GLFWwindow;
class btGeneric6DofConstraint;
class Cube;

class InteractionController
{
public:
    InteractionController(MessageBus* messageBus, SceneManager* scene, GLFWwindow* window, PhysicsManager* physics);
    void notifyGameModeChange(GameModeMessage message);
    void notifyKeyInput(KeyMessage message);
    void notifyMouseMoveInput(MouseMoveMessage message);

private:
    btGeneric6DofConstraint* mConstraint;
    SceneManager* mScene;
    GLFWwindow* mWindow;
    PhysicsManager* mPhysics;
    Cube* mCube;
    Node* mNode;
    int mCubeIndex;
    Cube* mCubeX;
    GameMode mGameMode;
    bool lineOn = false;
    bool mMovement = false;
};
