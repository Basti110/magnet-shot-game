#pragma once
#include <utility>
#include "mesh_node.h"
#include "physics_manager.h"
//#include <btSoftBodyInternals.h>
//#include <btSoftBodyHelpers.h>
#include <BulletSoftBody/btSoftBody.h>
#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>
#include <glow/objects/ArrayBuffer.hh>
#include <glow/objects/ElementArrayBuffer.hh>
#include <glow/objects/VertexArray.hh>
#include "bullet_helper.hh"


class DynamicCable : public PhysicsNode
{
public:
    DynamicCable(int id, PhysicsManager* physics, MessageBus* messageBus) : PhysicsNode(physics), mId(id), mStartAnimation(false), mAnimationTimer(0)
    {
        messageBus->addActivateScreenReceiver([=](ActivateScreenMessage message) { this->notifyActivateScreen(message); });


        std::vector<std::pair<btVector3, btVector3>> cablePositions;

        if (id == 0)
        {
            cablePositions.push_back({btVector3(-33.0, 4.115, -20.49), btVector3(-9.54, 4.115, -20.49)});
            cablePositions.push_back({btVector3(-9.55, 4.115, -20.49), btVector3(7.85, 4.115, -20.49)});
        }

        if (id == 1)
            cablePositions.push_back({btVector3(8.13, 4.05, -20.1), btVector3(6.78, 4.6, -35.0)});

        if (id == 2)
            cablePositions.push_back({btVector3(7.95, 4.119, -19.76), btVector3(37.1, 4.119, -19.76)});


        for (size_t i = 0; i < cablePositions.size(); ++i)
        {
            btSoftBody* psb = createRope(*(physics->getSoftWorldInfo()), cablePositions[i].first, cablePositions[i].second, 16, 1 + 2);

            if (psb == nullptr)
                continue;

            psb->m_cfg.piterations = 4;
            psb->m_materials[0]->m_kLST = 0.99 + (1 / (btScalar)(15 - 1)) * 0.9;
            psb->setTotalMass(0.5);
            physics->getSoftWorld()->addSoftBody(psb, GROUP_NONE, GROUP_NONE);
            mSoftbodies.push_back(psb);
        }

        generateData();
        generateVertexArray();
        this->setColor({200 / 255., 200 / 255., 200 / 255.});
    }

    void notifyActivateScreen(ActivateScreenMessage message)
    {
        if (message.screenId == mId)
        {
            mStartAnimation = true;
            mAnimationTimer = -0.5f;
        }
    }

    void update(float elapsedSeconds) override
    {
        if (mStartAnimation)
        {
            mAnimationTimer += elapsedSeconds;
            const float t = glm::clamp(mAnimationTimer, 0.0f, 1.0f);
            setColor((1 - t) * white + t * red);
        }

        float updateTime = glfwGetTime();
        generateDataPosNormal();
        updateVertexArray();
        mUpdateTime += glfwGetTime() - updateTime;

        if (++mUpdates >= 100)
        {
            // glow::info() << "Cable Update Time: " << (mUpdateTime / 100) * 1000.f << " ms";
            mUpdateTime = 0;
            mUpdates = 0;
        }

        mSecondCount += elapsedSeconds;
        if (mSecondCount >= 4)
        {
            mSecondCount = 0;
            for (size_t i = 0; i < mSoftbodies.size(); ++i)
            {
                mSoftbodies[i]->addForce({0, 0, -0.2});
            }
        }
    }

    void generateData()
    {
        aPos = {};
        aNormal = {};
        aTangent = {};
        aTexCoord = {};
        for (size_t i = 0; i < mSoftbodies.size(); ++i)
        {
            generateData(mSoftbodies[i]);
        }
    }

    void generateDataPosNormal()
    {
        for (size_t i = 0; i < mSoftbodies.size(); ++i)
        {
            generateDataPosNormal(mSoftbodies[i], i);
        }
    }

    void generateDataPosNormal(btSoftBody* psb, int offset)
    {
        float e = 0.5;
        int p = 7;
        float r = 0.038;
        float stepSize = glm::radians(360.0f) / p;

        int nodeSize = psb->m_nodes.size();
        int o = offset * ((nodeSize - 1) * (p * 6));

        for (int i = 0; i < (nodeSize - 1); ++i)
        {
            const btSoftBody::Node& node = psb->m_nodes[i];
            const btSoftBody::Node& nodeNext = psb->m_nodes[i + 1];
            glm::vec3 pos1 = to_glm(node.m_x);
            glm::vec3 pos2 = to_glm(nodeNext.m_x);
            glm::vec3 direction = glm::normalize(pos2 - pos1);
            glm::vec3 direction2 = glm::normalize(glm::vec3(direction.x + e, direction.y + e, direction.z + e));
            // Need normalize?
            glm::vec3 up = glm::normalize(glm::cross(direction, direction2));

            glm::mat3 rotation1 = glm::rotate(glm::mat4(1), 0.0f, direction);
            glm::vec3 normal1 = rotation1 * up;
            glm::vec3 rn1 = r * normal1;

            for (int j = 0; j < p; ++j)
            {
                glm::mat3 rotation2 = glm::rotate(glm::mat4(1), -1 * stepSize * (j + 1), direction);
                glm::vec3 normal2 = rotation2 * up;
                glm::vec3 rn2 = r * normal2;
                int index = o + i * (p * 6) + (j * 6);
                aPos[index + 0] = pos1 + rn2;
                aNormal[index + 0] = normal2;
                aPos[index + 1] = pos2 + rn1;
                aNormal[index + 1] = normal1;
                aPos[index + 2] = pos2 + rn2;
                aNormal[index + 2] = normal2;

                aPos[index + 3] = pos1 + rn1;
                aNormal[index + 3] = normal1;
                aPos[index + 4] = pos2 + rn1;
                aNormal[index + 4] = normal1;
                aPos[index + 5] = pos1 + rn2;
                aNormal[index + 5] = normal2;

                rotation1 = rotation2;
                normal1 = normal2;
                rn1 = rn2;
            }
        }
    }

    void generateData(btSoftBody* psb)
    {
        float e = 0.5;
        int p = 7;
        float r = 0.03;
        float stepSize = glm::radians(360.0f) / p;


        int nodeSize = psb->m_nodes.size();

        for (int i = 0; i < (nodeSize - 1); ++i)
        {
            const btSoftBody::Node& node = psb->m_nodes[i];
            const btSoftBody::Node& nodeNext = psb->m_nodes[i + 1];
            glm::vec3 pos1 = to_glm(node.m_x);
            glm::vec3 pos2 = to_glm(nodeNext.m_x);
            glm::vec3 direction = glm::normalize(pos2 - pos1);
            glm::vec3 direction2 = glm::normalize(glm::vec3(direction.x + e, direction.y + e, direction.z + e));
            // Need normalize?
            glm::vec3 up = glm::normalize(glm::cross(direction, direction2));

            glm::mat3 rotation1 = glm::rotate(glm::mat4(1), 0.0f, direction);
            glm::vec3 normal1 = rotation1 * up;
            glm::vec3 rn1 = r * normal1;

            for (int j = 0; j < p; ++j)
            {
                glm::mat3 rotation2 = glm::rotate(glm::mat4(1), -1 * stepSize * (j + 1), direction);
                glm::vec3 normal2 = rotation2 * up;
                glm::vec3 rn2 = r * normal2;

                aPos.push_back(pos1 + rn2);
                aNormal.push_back(normal2);
                aPos.push_back(pos2 + rn1);
                aNormal.push_back(normal1);
                aPos.push_back(pos2 + rn2);
                aNormal.push_back(normal2);

                aPos.push_back(pos1 + rn1);
                aNormal.push_back(normal1);
                aPos.push_back(pos2 + rn1);
                aNormal.push_back(normal1);
                aPos.push_back(pos1 + rn2);
                aNormal.push_back(normal2);


                for (int k = 0; k < 6; ++k)
                {
                    aTangent.push_back({0, 0, 0});
                    aTexCoord.push_back({0, 0});
                }

                rotation1 = rotation2;
                normal1 = normal2;
                rn1 = rn2;
            }
        }
    }

    void generateVertexArray()
    {
        auto abPos = glow::ArrayBuffer::create("aPos", aPos);
        auto abNormal = glow::ArrayBuffer::create("aNormal", aNormal);
        auto abTangent = glow::ArrayBuffer::create("aTangent", aTangent);
        auto abTexCoord = glow::ArrayBuffer::create("aTexCoord", aTexCoord);

        glow::SharedElementArrayBuffer eab;

        mVertexArray = glow::VertexArray::create({abPos, abNormal, abTangent, abTexCoord}, eab, GL_TRIANGLES);
    }

    void updateVertexArray()
    {
        auto abPos = mVertexArray->getAttributeBuffer("aPos");
        abPos->bind().setData(aPos);
        auto abNormal = mVertexArray->getAttributeBuffer("aNormal");
        abNormal->bind().setData(aNormal);
    }

    btSoftBody* createRope(btSoftBodyWorldInfo& worldInfo, const btVector3& from, const btVector3& to, int res, int fixeds)
    {
        /* Create nodes	*/
        if (res < 2)
            return nullptr;

        const int r = res + 2;
        btVector3* x = new btVector3[r];
        btScalar* m = new btScalar[r];
        int i;

        for (i = 0; i < r; ++i)
        {
            const btScalar t = i / (btScalar)(r - 1);
            x[i] = lerp(from, to, t);
            m[i] = 1;
        }
        btSoftBody* psb = new btSoftBody(&worldInfo, r, x, m);
        if (fixeds & 1)
        {
            psb->setMass(0, 0);
            // psb->setMass(1, 0);
        }

        if (fixeds & 2)
        {
            psb->setMass(r - 1, 0);
            // psb->setMass(r - 2, 0);
        }

        delete[] x;
        delete[] m;
        /* Create links	*/
        for (i = 1; i < r; ++i)
        {
            psb->appendLink(i - 1, i);
        }
        /* Finished		*/
        return (psb);
    }

private:
    int mUpdates = 0;
    float mUpdateTime;
    float mSecondCount = 0;
    int mId;
    bool mStartAnimation;
    float mAnimationTimer;
    std::vector<btSoftBody*> mSoftbodies;
    std::vector<glm::vec3> aPos;
    std::vector<glm::vec3> aNormal;
    std::vector<glm::vec3> aTangent;
    std::vector<glm::vec2> aTexCoord;
};
