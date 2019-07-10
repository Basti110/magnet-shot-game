#pragma once
//#include "render_system.h"
#include "message_bus.h"
#include "physics_manager.h"
#include "scene_manager.h"
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
    GameMode mGameMode;
    PhysicsManager* mPhysics;
    Cube* mCube;
    Node* mNode;
    Cube* mCubeX;
    glm::mat4 mStartPosition;
    glm::vec3 mFronMove;
    glm::vec3 mRightMove;
    int mCubeIndex;
    bool lineOn = false;
    bool mMovement = false;
    float mJumpTime = 0;
};
