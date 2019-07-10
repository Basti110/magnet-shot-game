#include "light_cube.h"
#include "scene_manager.h"
#include <glm/gtc/matrix_transform.hpp>

LightCube::LightCube(const glm::mat4& transformation, Color color, PhysicsManager* physics, SceneManager* m) : 
    Cube(transformation, {0.9, 0.9, 0.9}, physics), 
    mLight(nullptr)
{
    PointLight* light = new PointLight(glm::translate(glm::mat4(1), glm::vec3(0.0f, 0.0f, 0.0f)), color);
    this->addChild(light);
    
    if (m != nullptr)
        m->addPointLight(light);
    
    light->setFunc(1.8, 0.7, 1);
    mLight = light;

    mAlpha = 1.0;
    mDayIntensity = mProperty.intensity;
    mNightIntensity = mProperty.intensity;

    //mProperty.ambient = 0.9f * glm::vec3(color.red, color.green, color.blue);
    mNightColor = glm::vec3(color.red, color.green, color.blue);
    mLight->setOn(false);

    MessageBus::getInstance()->addSceneEventReceiver([=](SceneEventMessage message) {
        if (mControlActivated)
            this->notifySceneEvent(message);
    });
}

LightCube::~LightCube() 
{
    this->removeChild(mLight);
}

void LightCube::notifySceneEvent(SceneEventMessage message) 
{
    if (message.eventId == SceneEventId::Night)
    {
        setColor(mNightColor);
        mProperty.ambient = 0.9f * mNightColor;
        mAlpha = 0.0;
        mLight->setOn(true);
        mProperty.intensity = mNightIntensity;
    }
    else if (message.eventId == SceneEventId::Day)
    {
        setColor({0.9, 0.9, 0.9});
        mAlpha = 1.0;
        mLight->setOn(false);
        mProperty.intensity = mDayIntensity;
    }
}
