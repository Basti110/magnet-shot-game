#pragma once
#include "camera.h"
#include <glm/gtx/rotate_vector.hpp>


class CameraPath
{
public:
    CameraPath(Camera* camera) : mCamera(camera), mAnimationParam(0) {}
    virtual void reset() { mAnimationParam = 0; }
    virtual void update(float elapsedSeconds) {}
    virtual bool isDone() { return mAnimationParam >= 1; }

protected:
    Camera* mCamera;
    float mAnimationParam;
};


class CircularCameraPath : public CameraPath
{
public:
    CircularCameraPath(Camera* camera, const glm::vec3& center, float radius, float angle, float start, float end, float aperture) :
        CameraPath(camera),
        mCenter(center),
        mRadius(radius),
        mAngle(glm::radians(angle)),
        mStart(glm::radians(start)),
        mEnd(glm::radians(end)),
        mAperture(aperture)
    {

    }

    void update(float elapsedSeconds) override
    {
        mAnimationParam += elapsedSeconds * 0.1;
        glm::vec3 vec = glm::rotateX(glm::vec3(0,0,1), mAngle);
        vec = glm::rotateY(vec, (1 - mAnimationParam) * mStart + mAnimationParam * mEnd);
        const glm::vec3 eye = mCenter + mRadius * vec;
        const glm::vec3 up = glm::vec3(0,1,0);
        mCamera->lookAt(eye, mCenter, up);
        mCamera->setAperture(mAperture);
        mCamera->setFocus(mRadius);
    }

private:
    const glm::vec3 mCenter;
    const float mRadius;
    const float mAngle;
    const float mStart;
    const float mEnd;
    const float mAperture;
};


class ClimbingCameraPath : public CameraPath
{
public:
    ClimbingCameraPath(Camera* camera, const glm::vec3& center, float distance, float angle, float aperture) :
        CameraPath(camera),
        mCenter(center),
        mDistance(distance),
        mAngle(glm::radians(angle)),
        mAperture(aperture)
    {

    }

    void update(float elapsedSeconds) override
    {
        mAnimationParam += elapsedSeconds * 0.1;
        const glm::vec3 vec = glm::rotateY(glm::vec3(0,0,1), mAngle);
        const glm::vec3 offset(0, -0.7 + mAnimationParam, 0);
        const glm::vec3 eye = mCenter + offset + mDistance * vec;
        const glm::vec3 up = glm::vec3(0,1,0);
        mCamera->lookAt(eye, mCenter, up);
        mCamera->setAperture(mAperture);
        mCamera->setFocus(glm::length(mCenter - eye));
    }

private:
    const glm::vec3 mCenter;
    const float mDistance;
    const float mAngle;
    const float mAperture;
};
