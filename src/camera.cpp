#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <glm/gtc/matrix_inverse.hpp>
Camera::Camera()
{
    resetPosition();
}

void Camera::resetPosition()
{
    mCameraPos = glm::vec3(0.0f, 2.0f, 0.0f);
    mCameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    mCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
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
    glm::vec3 up(0, 1, 0);
    this->rotateLocalAxis(up, angleRadians);
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
    AbstractNode::setLocalTransformation(glm::inverse(t));
    //mCameraPos = glm::vec3(mGlobalTransformation[3]);
    /*glm::mat3 model = glm::mat3(mGlobalTransformation);
    mCameraFront = model * glm::vec3(0.0f, 0.0f, -1.0f);
    mCameraUp = model * glm::vec3(0.0f, 1.0f, 0.0f);*/
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::inverse(mGlobalTransformation);
}

glm::vec3 Camera::getCameraFront()
{
    return glm::mat3(mGlobalTransformation) * glm::vec3(0.0f, 0.0f, -1.0f);
}

glm::vec3 Camera::getCameraPosition()
{
    return glm::vec3(mGlobalTransformation[3]);
}

glm::mat4 Camera::getProjectionMatrix()
{
    return mProjectionMatrix;
}

void Camera::setViewportSize(int w, int h) 
{
    glm::mat4 projection = glm::mat4(1.0f);
    mProjectionMatrix = glm::perspective(glm::radians(90.0f), (float)w / (float)h, 0.1f, 500.0f);
}

void Camera::setLocalTransformation(glm::mat4 transformation)
{
    //AbstractNode::setLocalTransformation(transformation);
    mCameraPos = glm::vec3(transformation[3]);
    glm::mat3 model = glm::mat3(transformation);
    mCameraFront = model * glm::vec3(0.0f, 0.0f, -1.0f);
    mCameraUp = model * glm::vec3(0.0f, 1.0f, 0.0f);
    buildTransform();
}

void Camera::render(const glow::UsedProgram& shader, glm::mat4& projection, glm::mat4& view)
{
    AbstractNode::render(shader, projection, view);
}
