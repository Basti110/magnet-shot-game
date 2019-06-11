#pragma once
#include "sphere_node.h"
#include "physics_manager.h"


class Magnet : public SphereNode
{
public:
    Magnet(const glm::vec3& position, float radius, PhysicsManager* physics, btRigidBody* parent, bool red) :
        SphereNode(position, radius, physics, GROUP_NONE, GROUP_NONE, parent->isStaticObject() ? 0.0f : 0.1f),
        mParent(parent)
    {
        //mColor = red ? glm::vec3(1,0,0) : glm::vec3(0,0,1);
        physics->addMagnet(mRigidBody, red);
        if (!parent->isStaticObject()) {
            auto magnetCenter = mRigidBody->getCenterOfMassPosition();
            auto pivotInParent = parent->getWorldTransform().inverse() * magnetCenter;
            auto pivotInMagnet = btVector3(0,0,0);
            auto constraint = new btPoint2PointConstraint(*parent, *mRigidBody, pivotInParent, pivotInMagnet);
            constraint->m_setting.m_tau = 0.9;
            constraint->m_setting.m_impulseClamp = 50;
            mWorld->addConstraint(constraint, true);
            mConstraints.push_back(constraint);
        }
    }

    ~Magnet()
    {
        if (!mParent->isStaticObject()) {
            mParent->activate();
        }
    }

private:
    btRigidBody* mParent;
};
