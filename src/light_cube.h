#pragma once
#include "cube.h"
#include "point_light.h"
class SceneManager;

class LightCube : public Cube
{
public:
    LightCube(const glm::mat4& transformation, Color color, PhysicsManager* physics, SceneManager* m);
    ~LightCube();
    void notifySceneEvent(SceneEventMessage message);
    void activateControl(bool v) { mControlActivated = v; }
    void lightOn(bool v) { mLight->setOn(v); }

private:
    bool mControlActivated;
    PointLight* mLight;
    glm::vec3 mNightColor;
};
