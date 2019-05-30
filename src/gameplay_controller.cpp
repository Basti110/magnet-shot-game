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
    if ((message.getAction() == GLFW_PRESS && message.getInput() == GLFW_MOUSE_BUTTON_LEFT) ||
        (message.getAction() == GLFW_PRESS && message.getInput() == GLFW_MOUSE_BUTTON_RIGHT))
    {
        Camera* cam = mScene->getCamera();
        glm::vec3 pos = cam->getCameraPosition();
        glm::vec3 front = cam->getCameraFront();
        glm::vec3 isect;
        btRigidBody* body = mPhysics->pickBody(pos, pos + 200.0f*front, isect);
        if (body != nullptr) {
            const bool red = message.getInput() == GLFW_MOUSE_BUTTON_LEFT;
            const float radius = 0.1;
            mScene->appendNode(new Magnet(isect, radius, mPhysics, body, red));
            if (!body->isStaticObject()) {
                const auto direction = to_bullet(front);
                body->activate();
                body->applyCentralImpulse(3 * direction);
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
