#pragma once
#include "message_bus.h"
#include "scene_manager.h"
#include "camera_path.h"


class CinematicController
{
public:
    CinematicController(MessageBus* messageBus, SceneManager* scene);
    void update(float elapsedSeconds);
    void notifyGameModeChange(GameModeMessage message);


private:
    Camera* mCamera;
    std::vector<CameraPath*> mCameraPaths;
    size_t mCurrentPathIdx;
    bool mIsActive;
};
