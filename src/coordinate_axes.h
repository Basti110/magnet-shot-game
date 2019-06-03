#pragma once
#include "abstract_node.h"
#include "message_bus.h"
#include <btBulletDynamicsCommon.h>
class PhysicsManager;
class CoordinateAxes: public AbstractNode
{
public:
    CoordinateAxes(const glm::vec3& position, PhysicsManager* physics);
    ~CoordinateAxes();

    void render(const glow::UsedProgram& shader, glm::mat4& projection, glm::mat4& view, bool shadowPass) override;
    void update(float elapsedSeconds) override;

    bool hitArrow(const btRigidBody* body);

    void notifyMouseMoveInput(MouseMoveMessage message);
    void notifyMouseClickInput(MouseClickMessage message);

    static void setScale(float scale);
   
private:
    void createConeBuffer(float length, float radius, int fineness);
    void createLineBuffer(float length);
    void render(const glow::UsedProgram& shader, glm::mat4& model, glm::vec3 color, glm::mat4& lineTranslate);
    glm::mat4 getPhysicsArrowModelX();
    glm::mat4 getPhysicsArrowModelY();
    glm::mat4 getPhysicsArrowModelZ();

    
    unsigned int coneVAO;
    unsigned int coneVBO;
    int mHitBody = -1;
    float mLastScale = 1;

    glow::SharedVertexArray mLineSVA;
    glow::SharedVertexArray mVertexArray;
    glm::vec3 mColor;
    
    btRigidBody* mArrowX;
    btRigidBody* mArrowY;
    btRigidBody* mArrowZ;

    bool mHit = false;
    bool mInit = false;
   
    static float mScale;
};
