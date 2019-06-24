#pragma once
#include "light.h"

#define GETSET(Type, Name) \
    Type get##Name() const { return m##Name; }; \
    void set##Name(Type value) { m##Name = value; }

#define GETSETR(Type, Name) \
    const Type& get##Name() const { return m##Name; }; \
    void set##Name(const Type& value) { m##Name = value; }

class PointLight : public AbstractNode
{
public:
    PointLight(const glm::mat4& transformation, Color color);
    ~PointLight();
    GETSETR(glm::vec3, Ambient)
    GETSETR(glm::vec3, Diffuse)
    GETSETR(glm::vec3, Specular)
    GETSET(float, Constant)
    GETSET(float, Linear)
    GETSET(float, Quadratic)
    GETSET(bool, On)
    float getRadius() { return mRadius; }
    void setFunc(float q, float l, float c);

private:
    void computeRadius();

    glm::vec3 mAmbient;
    glm::vec3 mDiffuse;
    glm::vec3 mSpecular;
    float mConstant;
    float mLinear;
    float mQuadratic;
    bool mOn = true;
    float mRadius; 
};

