#pragma once
#include "message_bus.h"
#include "scene_manager.h"
#include "physics_manager.h"


class GameplayController
{
public:
    GameplayController(MessageBus* messageBus, SceneManager* scene, PhysicsManager* physics);
    void notifyGameModeChange(GameModeMessage message);
    void notifyMouseClickInput(MouseClickMessage message);

private:
    SceneManager* mScene;
    PhysicsManager* mPhysics;
    bool mIsActive;
};

