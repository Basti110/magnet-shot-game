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

    void setNightIntensity(float value) { mNightIntensity = value; };

private:
    bool mControlActivated = true;
    PointLight* mLight;
    glm::vec3 mColor;
    float mDayIntensity;
    float mNightIntensity;
};
