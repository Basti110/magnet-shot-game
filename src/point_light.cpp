#include "point_light.h"

PointLight::PointLight(const glm::mat4& transformation, Color color) 
{
    mAmbient = {color.red, color.green, color.blue};
    mDiffuse = {color.red, color.green, color.blue};
    mSpecular = {color.red, color.green, color.blue};

    mLocalTransformation = transformation;
    mGlobalTransformation = transformation;
    setFunc(1.8, 0.7, 1);
}

PointLight::~PointLight() 
{

}

void PointLight::setFunc(float q, float l, float c) 
{
    mQuadratic = q;
    mLinear = l;
    mConstant = c;
    computeRadius();
}

void PointLight::computeRadius() 
{
    float lightMax = std::fmaxf(std::fmaxf(mDiffuse.x, mDiffuse.y), mDiffuse.z);
    mRadius = (-mLinear + std::sqrt(mLinear * mLinear - 4 * mQuadratic * (mConstant - (256.0 / 5.0) * lightMax))) / (2 * mQuadratic);
}
