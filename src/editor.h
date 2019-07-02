#pragma once
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
    glm::vec3 getRayTo(int x, int y);

    SceneManager* mScene;
    PhysicsManager* mPhysics;
    Cube* mCube;
    glm::vec3 mCubeScale;
    RigidBodyInfo mCubeInfo;
    float mCubeDistance;
    float mCoordinateAxesScale = 1;
    bool mIsActive;
    GameMode mGameMode = GameMode::Gameplay;
    std::vector<Cube*> mCubes;
    float mLastDelete = 0;
};
