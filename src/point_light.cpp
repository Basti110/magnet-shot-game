#include "point_light.h"

PointLight::PointLight(glm::mat4& transformation, Color color) 
{
    mAmbient = {color.red, color.green, color.blue};
    mDiffuse = {color.red, color.green, color.blue};
    mSpecular = {color.red, color.green, color.blue};

    mLocalTransformation = transformation;
    mGlobalTransformation = transformation;
}

PointLight::~PointLight() {}

void PointLight::setFunc(float q, float l, float c) 
{
    mQuadratic = q;
    mLinear = l;
    mConstant = c;
}
