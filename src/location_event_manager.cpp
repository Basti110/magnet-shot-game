#include "location_event_manager.h"


LocationEventManager::LocationEventManager(MessageBus* messageBus, Camera* camera) :
    mMessageBus(messageBus),
    mCamera(camera),
    mPrevCameraPos(camera->getPos())
{

}

void LocationEventManager::registerEvent(const glm::vec3& min, const glm::vec3& max, LocationEventId eventId)
{
    mBoundingBoxes.push_back({min, max});
    mEventIds.push_back(eventId);
}

void LocationEventManager::update()
{
    const glm::vec3& cameraPos = mCamera->getPos();
    for (size_t i = 0; i < mBoundingBoxes.size(); i++)
    {
        const BoundingBox& bb = mBoundingBoxes[i];
        const LocationEventId& id = mEventIds[i];
        if (!bb.contains(mPrevCameraPos) && bb.contains(cameraPos)) {
            mMessageBus->sendMessage(new LocationEventMessage(id, LocationEventType::Enter));
        }
        if (bb.contains(mPrevCameraPos) && !bb.contains(cameraPos)) {
            mMessageBus->sendMessage(new LocationEventMessage(id, LocationEventType::Exit));
        }
    }
    mPrevCameraPos = cameraPos;
}
