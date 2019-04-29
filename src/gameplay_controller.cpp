#include "gameplay_controller.h"
#include <glm/gtc/matrix_transform.hpp>
#include <btBulletDynamicsCommon.h>
#include <GLFW/glfw3.h>
#include "magnet.h"
#include "bullet_helper.hh"


GameplayController::GameplayController(MessageBus* messageBus, SceneManager* scene, PhysicsManager* physics) :
    mScene(scene),
    mPhysics(physics),
    mIsActive(false)
{
    messageBus->addGameModeReceiver([=](GameModeMessage message) {
        this->notifyGameModeChange(message);
    });
    messageBus->addMouseClickReceiver([=](MouseClickMessage message) {
        if (mIsActive) this->notifyMouseClickInput(message);
    });
}

void GameplayController::notifyGameModeChange(GameModeMessage message)
{
    mIsActive = message.mode == GameMode::Gameplay;
}

void GameplayController::notifyMouseClickInput(MouseClickMessage message)
{
    if (message.getInput() == GLFW_MOUSE_BUTTON_LEFT ||
        message.getInput() == GLFW_MOUSE_BUTTON_RIGHT)
    {
        Camera* cam = mScene->getCamera();
        glm::vec3 pos = cam->getCameraPosition();
        glm::vec3 front = cam->getCameraFront();
        glm::vec3 isect;
        btRigidBody* body = mPhysics->pickBody(pos, pos + 200.0f*front, isect);
        if (body != nullptr) {
            glm::mat4 transform = glm::mat4(1.0f);
            transform = glm::translate(transform, isect);
            const bool red = message.getInput() == GLFW_MOUSE_BUTTON_LEFT;
            const glm::vec3 color = red ? glm::vec3(1,0,0) : glm::vec3(0,0,1);
            const float scale = 0.1;
            const int id = mPhysics->addMagnet(transform, scale, body->getUserIndex(), red);
            mScene->appendNode(new Magnet(mPhysics, id, scale, color));
            if (!body->isStaticObject()) {
                body->activate();
                body->applyCentralImpulse(3*to_bullet(front));
            }
        }
    }
    else if(message.getInput() == GLFW_MOUSE_BUTTON_MIDDLE) {
        mScene->removeNode([](AbstractNode* node) {
            return dynamic_cast<Magnet*>(node) != nullptr;
        });
        mPhysics->clearMagnets();
    }
}
