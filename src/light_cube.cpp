#include "light_cube.h"
#include "scnene_manager.h"
#include <glm/gtc/matrix_transform.hpp>

LightCube::LightCube(const glm::mat4& transformation, Color color, PhysicsManager* physics, SceneManager* m) : 
    Cube(transformation, color, physics), 
    mLight(nullptr)
{
    PointLight* light = new PointLight(glm::translate(glm::mat4(1), glm::vec3(0.0f, 0.0f, 0.0f)), color);
    this->addChild(light);
    
    if (m != nullptr)
        m->addPointLight(light);
    
    light->setFunc(1.8, 0.7, 1);
    mLight = light;

    mAlpha = 0.0;

    mProperty.ambient = 0.9f * glm::vec3(color.red, color.green, color.blue);
}

LightCube::~LightCube() 
{
    this->removeChild(mLight);
}
