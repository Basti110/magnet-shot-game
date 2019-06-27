#pragma once
#include "mesh_node.h"
//#include <btSoftBodyInternals.h>
//#include <btSoftBodyHelpers.h>
#include <BulletSoftBody/btSoftBody.h>
#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>
#include <glow/objects/ArrayBuffer.hh>
#include <glow/objects/ElementArrayBuffer.hh>
#include <glow/objects/VertexArray.hh>
#include "bullet_helper.hh"

class Cable : public MeshNode
{
public:
    Cable(int id, PhysicsManager* physics, MessageBus* messageBus)
      : MeshNode(glm::mat4(), getFilename(id), physics, GROUP_NONE, GROUP_NONE, 0.0f), mId(id), mStartAnimation(false), mAnimationTimer(0)
    {
        messageBus->addActivateScreenReceiver([=](ActivateScreenMessage message) { this->notifyActivateScreen(message); });

        btSoftBody* psb = createRope(*(physics->getSoftWorldInfo()), btVector3(-9.5, 4, -20.5), btVector3(8, 4, -20.5), 16, 1 + 2);
        psb->m_cfg.piterations = 4;
        psb->m_materials[0]->m_kLST = 0.9 + (1 / (btScalar)(15 - 1)) * 0.9;
        psb->setTotalMass(1);
        physics->getSoftWorld()->addSoftBody(psb);
        mSoftbodies.push_back(psb);

        psb = createRope(*(physics->getSoftWorldInfo()), btVector3(-9.5, 4, -21), btVector3(8, 4, -21), 16, 1 + 2);
        psb->m_cfg.piterations = 4;
        psb->m_materials[0]->m_kLST = 0.9 + (1 / (btScalar)(15 - 1)) * 0.9;
        psb->setTotalMass(1);
        physics->getSoftWorld()->addSoftBody(psb);
        mSoftbodies.push_back(psb);

        psb = createRope(*(physics->getSoftWorldInfo()), btVector3(-9.5, 4, -19.5), btVector3(8, 4, -19.5), 16, 1 + 2);
        psb->m_cfg.piterations = 4;
        psb->m_materials[0]->m_kLST = 0.9 + (1 / (btScalar)(15 - 1)) * 0.9;
        psb->setTotalMass(1);
        physics->getSoftWorld()->addSoftBody(psb);
        mSoftbodies.push_back(psb);

        psb = createRope(*(physics->getSoftWorldInfo()), btVector3(-9.5, 4, -19), btVector3(8, 4, -19), 16, 1 + 2);
        psb->m_cfg.piterations = 4;
        psb->m_materials[0]->m_kLST = 0.9 + (1 / (btScalar)(15 - 1)) * 0.9;
        psb->setTotalMass(1);
        physics->getSoftWorld()->addSoftBody(psb);
        mSoftbodies.push_back(psb);

        
        generateData();
        generateVertexArray();
    }

    static const std::string getFilename(int id) { return "../../data/meshes/Cable" + std::to_string(id) + ".obj"; }

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
            glow::info() << "Cable Update Time: " << (mUpdateTime / 100) * 1000.f << " ms";
            mUpdateTime = 0;
            mUpdates = 0;
        }

        mSecondCount += elapsedSeconds;
        if (mSecondCount >= 3)
        {
            mSecondCount = 0;
            for (int i = 0; i < mSoftbodies.size(); ++i)
            {
                mSoftbodies[i]->addForce({0, 0, -2});
            }

        }
    }

    void generateData()
    {
        aPos = {};
        aNormal = {};
        aTangent = {};
        aTexCoord = {};
        for (int i = 0; i < mSoftbodies.size(); ++i)
        {
            generateData(mSoftbodies[i]);
        }
    }

    void generateDataPosNormal()
    {
        for (int i = 0; i < mSoftbodies.size(); ++i)
        {
            generateDataPosNormal(mSoftbodies[i], i);
        }
    }

    void generateDataPosNormal(btSoftBody* psb, int offset)
    {
        float e = 0.5;
        int p = 7;
        float r = 0.03;
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
            psb->setMass(0, 0);
        if (fixeds & 2)
            psb->setMass(r - 1, 0);
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
