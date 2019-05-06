#include "interaction_controller.h"

#include "cube.h"
#include "bullet_helper.hh"
#include "magnet.h"
#include "physics_manager.h"

#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Character/btCharacterControllerInterface.h>
#include "bullet_helper.hh"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#define CHARACTER_HEIGHT 1.5
#define CHARACTER_EPSILON 0.1
#define JUMP_TIMEOUT 0.2

InteractionController::InteractionController(MessageBus* messageBus, SceneManager* scene, GLFWwindow* window, PhysicsManager* physics)
  : mScene(scene), mWindow(window), mGameMode(GameMode::Gameplay), mPhysics(physics)
{
    messageBus->addGameModeReceiver([=](GameModeMessage message) { this->notifyGameModeChange(message); });
    messageBus->addKeyReceiver([=](KeyMessage message) {
        if (mGameMode == GameMode::Editor || mGameMode == GameMode::Gameplay)
            this->notifyKeyInput(message);
    });
    messageBus->addMouseMoveReceiver([=](MouseMoveMessage message) {
        if (mGameMode == GameMode::Editor || mGameMode == GameMode::Gameplay)
            this->notifyMouseMoveInput(message);
    });

    RigidBodyInfo info;
    info.mass = 70.0;
    info.friction = 0.5;

    //auto controller = new btKinematik
    glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(-3.0f, 10.0f, 0.0f));

    mNode = new Node();
    mNode->setLocalTransformation(trans);
    
    int bodyID = physics->addCapsule({0.25, 1.0}, trans, info);
    mNode->setRigidBody(bodyID, physics);
    //mCube = new Cube(trans, Color{1.0f, 1.0f, 1.0f}, mPhysics);

    mScene->appendNode(mNode);
    //mCube->addPhysics(glm::vec3(1.0f), info);
    mNode->addChild(mScene->getCamera());

    btRigidBody* body = mPhysics->getRigidBody(mNode->getPhysicsID());
    bool useLinearReferenceFrameA = true;
    btTransform frameInB;
    frameInB = btTransform::getIdentity();
    frameInB.setOrigin(btVector3(0., 0., 0.));
    mConstraint = new btGeneric6DofConstraint(*body, frameInB, useLinearReferenceFrameA);
    mConstraint->setLinearLowerLimit(btVector3(-50, -50, -50));
    mConstraint->setLinearUpperLimit(btVector3(50, 50, 50));
    mConstraint->setAngularLowerLimit(btVector3(0, 0, 0));
    mConstraint->setAngularUpperLimit(btVector3(0, 0, 0));
    physics->addConstraint(mConstraint);

    trans = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 2.0f, 0.0f));
    mCubeX = new Cube(trans, Color{1.0f, 1.0f, 1.0f}, mPhysics);
}

void InteractionController::notifyGameModeChange(GameModeMessage message)
{
    mGameMode = message.mode;
}

void InteractionController::notifyKeyInput(KeyMessage message)
{
    GLFWwindow* window = mWindow;
    Camera* cam = mScene->getCamera();

    
    btRigidBody* body = mPhysics->getRigidBody(mNode->getPhysicsID());
    btTransform t;
    body->getMotionState()->getWorldTransform(t);
    glm::vec3 pos = glm::vec3(to_glm(t)[3]);
    //pos.z += CHARACTER_HEIGHT / 2; // half of character height

    glm::vec3 to(0.0f, -10.0f, 0.0f);
    bool onGround = mPhysics->bodyWithinReach(pos, to, CHARACTER_HEIGHT + CHARACTER_EPSILON, body);
    
    if (message.getAction() == GLFW_RELEASE)
    {
        if (mGameMode == GameMode::Gameplay)
        {
            if (!onGround && (message.getInput() == GLFW_KEY_W ||
                message.getInput() == GLFW_KEY_S ||
                message.getInput() == GLFW_KEY_A ||
                message.getInput() == GLFW_KEY_D))
            {
                body->setLinearVelocity(to_bullet(glm::vec3(0, 0, 0)));
            }
        }
    }

    if (message.getAction() == GLFW_PRESS)
    {    
        body->activate(true);
        glm::mat3 rotation = glm::mat3(cam->getGlobalTransformation());
        float cameraSpeed = message.getSpeed();
        glm::mat4 trans = glm::translate(mNode->getGlobalTransformation(), glm::vec3(0.0f, 0.0f, -0.1f));

        if (mGameMode == GameMode::Editor)
        {
            if (message.getInput() == GLFW_KEY_W)
                cam->moveFront(cameraSpeed);
            if (message.getInput() == GLFW_KEY_S)
                cam->moveFront(-cameraSpeed);
            if (message.getInput() == GLFW_KEY_A)
                cam->moveRight(-cameraSpeed);
            if (message.getInput() == GLFW_KEY_D)
                cam->moveRight(cameraSpeed);
            if (message.getInput() == GLFW_KEY_LEFT_SHIFT)
                cam->moveUp(-cameraSpeed);
            if (message.getInput() == GLFW_KEY_SPACE)
                cam->moveUp(cameraSpeed);

            // body->a

            // Move Camera (Rotation)
            if (message.getInput() == GLFW_KEY_LEFT)
                cam->rotateYaw(cameraSpeed / 2);
            if (message.getInput() == GLFW_KEY_UP)
                cam->rotatePitch(-cameraSpeed / 2);
            if (message.getInput() == GLFW_KEY_RIGHT)
                cam->rotateYaw(-cameraSpeed / 2);
            if (message.getInput() == GLFW_KEY_DOWN)
                cam->rotatePitch(cameraSpeed / 2);
            if (message.getInput() == GLFW_KEY_Q)
                cam->rotateRoll(-cameraSpeed / 2);
            if (message.getInput() == GLFW_KEY_E)
                cam->rotateRoll(cameraSpeed / 2);
        }
        else if (mGameMode == GameMode::Gameplay)
        {
            if (onGround && (glfwGetTime() - mJumpTime) > JUMP_TIMEOUT)
            {
                if (message.getInput() == GLFW_KEY_W)
                    body->setLinearVelocity(to_bullet(rotation * glm::vec3(0, 0, -5)));
                if (message.getInput() == GLFW_KEY_S)
                    body->setLinearVelocity(to_bullet(rotation * glm::vec3(0, 0, 5)));
                if (message.getInput() == GLFW_KEY_A)
                    body->setLinearVelocity(to_bullet(rotation * glm::vec3(-5, 0, 0)));
                if (message.getInput() == GLFW_KEY_D)
                    body->setLinearVelocity(to_bullet(rotation * glm::vec3(5, 0, 0)));
                if (message.getInput() == GLFW_KEY_SPACE) 
                {
                    body->applyCentralImpulse({0, 800, 0});
                    mJumpTime = glfwGetTime();
                }
            }
        }


        // Render Control
        if (message.getInput() == GLFW_KEY_R)
            cam->resetPosition();

        if (message.getInput() == GLFW_KEY_F8)
        {
            if (this->lineOn)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                this->lineOn = false;
            }
            else
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                this->lineOn = true;
            }
        }
    }
}

void InteractionController::notifyMouseMoveInput(MouseMoveMessage message)
{
    Camera* cam = mScene->getCamera();
    float speed = 0.002;
    int mode = glfwGetInputMode(mWindow, GLFW_CURSOR);
    if (mGameMode == GameMode::Editor || mGameMode == GameMode::Gameplay)
    {
        cam->rotateYaw(-message.getDeltaPosition().x * speed);
        cam->rotatePitch(-message.getDeltaPosition().y * speed);
    }
    /*else if (mGameMode == GameMode::Gameplay)
    {
        btRigidBody* body = mPhysics->getRigidBody(mCube->getPhysicsID());
        body->applyTorque({0, -message.getDeltaPosition().x * 2, 0});
    }*/

}
