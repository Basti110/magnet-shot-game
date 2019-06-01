#pragma once
#include "message_bus.h"
#include "camera.h"


struct BoundingBox {
    const glm::vec3 min;
    const glm::vec3 max;

    bool contains(const glm::vec3& p) const
    {
        return p.x > min.x && p.x < max.x &&
               p.y > min.y && p.y < max.y &&
               p.z > min.z && p.z < max.z;
    }
};


class LocationEventManager {
public:
    LocationEventManager(MessageBus* messageBus, Camera* camera);
    void registerEvent(const glm::vec3& min, const glm::vec3& max, LocationEventId eventId);
    void update();

private:
    MessageBus* mMessageBus;
    Camera* mCamera;

    glm::vec3 mPrevCameraPos;
    std::vector<BoundingBox> mBoundingBoxes;
    std::vector<LocationEventId> mEventIds;
};

