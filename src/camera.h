#ifndef CAMERA_H
#define CAMERA_H

#include "abstract_node.h"


class Message;


class Camera : public AbstractNode
{
public:
    Camera();
    void rotateLocalAxis(glm::vec3 angle, float speed);
    void rotatePitch(float angleRadians);
    void rotateYaw(float angleRadians);
    void rotateRoll(float angleRadians);
    void moveFront(float speed);
    void moveRight(float speed);
    void moveUp(float speed);
    void resetPosition();
    glm::vec3 getCameraFront();
    glm::vec3 getCameraRight();
    glm::vec3 getCameraUp();
    glm::vec3 getCameraPosition();
    glm::mat4 getProjectionMatrix();
    glm::mat4 getViewMatrix();
    int getWidth();
    int getHeight();
    void setViewportSize(int w, int h);

    void setLocalTransformation(glm::mat4 transformation) override;
    void render(glow::UsedProgram& shader, glm::mat4& projection, glm::mat4& view, bool shadowPass) override;

    void detatchFromParent(bool value);
    void lookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up);

    void setAperture(float value);
    void setFocus(float value);

    float getAperture();
    float getFocus();

    void notifyGuiInput(Message* message);


private:
    void buildTransform();

    int mWidth = 0;
    int mHeight = 0;
    glm::mat4 mProjectionMatrix;
    glm::vec3 mCameraPos;
    glm::vec3 mCameraFront;
    glm::vec3 mCameraUp;

    bool mDetatchFromParent;
    float mAperture;
    float mFocus;
};

#endif // CAMERA_H
