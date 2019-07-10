#pragma once
#include "mesh_node.h"
#include "physics_manager.h"
#include "bullet_helper.hh"
#include "light_cube.h"

#include <glm/gtc/matrix_transform.hpp>

class SceneManager;

class WindTurbine : public AbstractNode
{
public:
    WindTurbine(const glm::mat4& transform, float bladesAngle, PhysicsManager* physics, SceneManager* m) :
        mBladesAngle(bladesAngle)
    {
        MeshNode* base = new MeshNode(
            transform,
            "../../data/meshes/WindTurbineBase.obj",
            physics,
            GROUP_STATIC_OBJECTS,
            GROUP_DYNAMIC_OBJECTS,
            0.0f
        );

        base->setColor({150 / 255., 150 / 255., 150 / 255.});

        const glm::vec3 bladesOffset(0.72530f, 8.73904f, 0.0f);
        mBladesTransform = glm::translate(transform, bladesOffset);

        mBlades = new MeshNode(
            mBladesTransform,
            "../../data/meshes/WindTurbineBlades.obj",
            physics,
            GROUP_STATIC_OBJECTS,
            GROUP_DYNAMIC_OBJECTS,
            0.0f
        );

        glm::mat4 t = transform * glm::translate(glm::mat4(1), glm::vec3(-0.1f, 9.15f, 0.0f)) * glm::scale(glm::mat4(1), glm::vec3(0.2, 0.2, 0.2));
        mLight = new LightCube(t, {1, 0, 0}, physics, m); 
        mLight->activateControl(false);
        this->addChild(mLight);
        addChild(base);
        addChild(mBlades);

        MessageBus::getInstance()->addSceneEventReceiver([=](SceneEventMessage message) {
            this->notifySceneEvent(message);
        });
    }

    void update(float elapsedSeconds)
    {
        mAnimationTimer += elapsedSeconds;
        mBladesAngle += elapsedSeconds * 0.75f;
        const glm::mat4 rotation = glm::rotate(glm::mat4(1), mBladesAngle, glm::vec3(1,0,0));
        mBlades->getRigidBody()->setCenterOfMassTransform(to_bullet(mBladesTransform * rotation));
        mBlades->setLocalTransformation(mBladesTransform * rotation);

        if (!mDay)
        {
            if ((int)(mAnimationTimer * 1.5f) % 2 == 0)
            {
                mLight->lightOn(true);
                mLight->setAlpha(0.0);
                mLight->setColor({1, 0, 0});
                mLight->setAmbient({1, 0, 0});
            }
            else
            {
                mLight->lightOn(false);
                mLight->setAlpha(1.0);
                mLight->setColor({0.9, 0.9, 0.9});
            }          
        }
    }

    LightCube* getLight()
    { 
        return mLight;
    }

    void notifySceneEvent(SceneEventMessage message)
    {
        if (message.eventId == SceneEventId::Night)
        {
            mDay = false;
        }        
        else if (message.eventId == SceneEventId::Day)
        {
            mDay = true;
            mLight->lightOn(false);
            mLight->setAlpha(1.0);
            mLight->setColor({0.9, 0.9, 0.9});
        }      
    }

private:
    bool mDay;
    float mAnimationTimer = 0;
    LightCube* mLight;
    MeshNode* mBlades;
    glm::mat4 mBladesTransform;
    float mBladesAngle;
};
