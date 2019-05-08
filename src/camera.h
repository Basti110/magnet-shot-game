#ifndef CAMERA_H
#define CAMERA_H
#include "abstract_node.h"

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
    glm::vec3 getCameraPosition();
    glm::mat4 getProjectionMatrix();
    glm::mat4 getViewMatrix();
    void setViewportSize(int w, int h);

    void setLocalTransformation(glm::mat4 transformation) override;
    void render(const glow::UsedProgram& shader, glm::mat4& projection, glm::mat4& view) override;


private:
    void buildTransform();

    glm::mat4 mProjectionMatrix;
    glm::vec3 mCameraPos;
    glm::vec3 mCameraFront;
    glm::vec3 mCameraUp;
};

#endif // CAMERA_H
