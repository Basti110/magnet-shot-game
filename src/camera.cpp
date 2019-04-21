#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
Camera::Camera()
{
    resetPosition();
}

void Camera::resetPosition()
{
    mCameraPos = glm::vec3(-2.0f, 2.0f, 3.0f);
    mCameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    mCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    mPitch = 0;
    mYaw = -90;
    buildTransform();
}

void Camera::rotateLocalAxis(glm::vec3 angle, float speed)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, speed, angle);
    mCameraFront = glm::vec3(model * glm::vec4(mCameraFront, 1));
    mCameraUp = glm::vec3(model * glm::vec4(mCameraUp, 1));
    buildTransform();
}

void Camera::moveFront(float speed)
{
    mCameraPos += mCameraFront * speed;
    buildTransform();
}

void Camera::moveRight(float speed)
{
    mCameraPos += glm::normalize(glm::cross(mCameraFront, mCameraUp)) * speed;
    buildTransform();
}

void Camera::moveUp(float speed)
{
    mCameraPos += mCameraUp * speed;
    buildTransform();
}

void Camera::rotatePitch(float angleRadians)
{
    glm::vec3 right = glm::cross(mCameraFront, mCameraUp);
    this->rotateLocalAxis(right, angleRadians);
    /*this->pitch += angleDegree;
    this->cameraFront.x = cos(glm::radians(this->pitch)) * cos(glm::radians(this->yaw));
    this->cameraFront.y = sin(glm::radians(this->pitch));
    this->cameraFront.z = cos(glm::radians(this->pitch)) * sin(glm::radians(this->yaw));
    this->cameraUp.x = cos(glm::radians(this->pitch + 90)) * cos(glm::radians(this->yaw));
    this->cameraUp.y = sin(glm::radians(this->pitch + 90));
    this->cameraUp.z = cos(glm::radians(this->pitch + 90)) * sin(glm::radians(this->yaw));*/
    // buildTransform();
}

void Camera::rotateYaw(float angleRadians)
{
    float yawX = acos(mCameraFront.x);
    this->rotateLocalAxis(mCameraUp, angleRadians);
    /*this->yaw += angleDegree;
    this->cameraFront.x = cos(glm::radians(this->pitch)) * cos(glm::radians(this->yaw));
    this->cameraFront.y = sin(glm::radians(this->pitch));
    this->cameraFront.z = cos(glm::radians(this->pitch)) * sin(glm::radians(this->yaw));

    this->cameraUp.x = cos(glm::radians(this->pitch + 90)) * cos(glm::radians(this->yaw));
    this->cameraUp.y = sin(glm::radians(this->pitch + 90));
    this->cameraUp.z = cos(glm::radians(this->pitch + 90)) * sin(glm::radians(this->yaw));
    this->cameraFront = glm::normalize(this->cameraFront);
    buildTransform();*/
}

void Camera::rotateRoll(float angleRadians)
{
    this->rotateLocalAxis(mCameraFront, angleRadians);
}

void Camera::buildTransform()
{
    glm::mat4 t = glm::lookAt(mCameraPos, mCameraPos + mCameraFront, mCameraUp);
    this->setLocalTransformation(t);
}

glm::mat4 Camera::getViewMatrix()
{
    return mGlobalTransformation;
}

glm::vec3 Camera::getCameraFront()
{
    return mCameraFront;
}

glm::vec3 Camera::getCameraPosition()
{
    return mCameraPos;
}

glm::mat4 Camera::getProjectionMatrix()
{
    return mProjectionMatrix;
}

void Camera::setViewportSize(int w, int h) 
{
    glm::mat4 projection = glm::mat4(1.0f);
    mProjectionMatrix = glm::perspective(glm::radians(90.0f), (float)w / (float)h, 0.1f, 100.0f);
}
