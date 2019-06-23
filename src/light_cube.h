#pragma once
#include "cube.h"
#include "point_light.h"
class SceneManager;

class LightCube : public Cube
{
public:
    LightCube(const glm::mat4& transformation, Color color, PhysicsManager* physics, SceneManager* m);
    ~LightCube();

private:
    PointLight* mLight;
};
