#include "cinematic_controller.h"
#include <glm/gtc/matrix_transform.hpp>


CinematicController::CinematicController(MessageBus *messageBus, SceneManager *scene) :
    mCamera(scene->getCamera()),
    mCurrentPathIdx(0),
    mIsActive(false)
{
    glm::vec3 center;

    // bird
    center = glm::vec3(9.09171, 2.35247 + 0.1, -7.21369);
    mCameraPaths.push_back(new ClimbingCameraPath(mCamera, center, 0.75, 67, 8.5));

    // screens
    center = glm::vec3(1.34631f + 1.525f, 1.2, -18.5735f);
    mCameraPaths.push_back(new CircularCameraPath(mCamera, center, 2.75, -12, -30, 30, 0.8));

    messageBus->addGameModeReceiver([=](GameModeMessage message) {
        this->notifyGameModeChange(message);
    });
}


void CinematicController::update(float elapsedSeconds)
{
    auto currentPath = mCameraPaths[mCurrentPathIdx];

    if (mIsActive) {
        currentPath->update(elapsedSeconds);
        if (currentPath->isDone()) {
            currentPath->reset();
            mCurrentPathIdx = (mCurrentPathIdx + 1) % mCameraPaths.size();
        }
    }
}

void CinematicController::notifyGameModeChange(GameModeMessage message)
{
    if (message.mode == GameMode::Cinematic) {
        mIsActive = true;
        mCamera->detatchFromParent(true);
    }
    else {
        mIsActive = false;
        mCamera->detatchFromParent(false);
        mCamera->setAperture(0.0f);
    }
}
