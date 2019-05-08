#pragma once
//#include <utility>
#include "cube.h"
#include "message_bus.h"
#include "scnene_manager.h"

class Editor
{
public:
    Editor(MessageBus* mB, SceneManager* scene, PhysicsManager* physics);
    void notifyGameModeChange(GameModeMessage message);
    void notifyMouseClickInput(MouseClickMessage message);
    void notifyMouseMoveInput(MouseMoveMessage message);
    void notifyKeyInput(KeyMessage message);
    void notifyGuiInput(Message* message);
    void save();

    void load();

private:
    void refreshCube();

    SceneManager* mScene;
    PhysicsManager* mPhysics;
    Cube* mCube;
    glm::vec3 mCubeScale;
    RigidBodyInfo mCubeInfo;
    float mCubeDistance;
    bool mIsActive;
    std::vector<Cube*> mCubes;
    float mLastDelete = 0;
};
