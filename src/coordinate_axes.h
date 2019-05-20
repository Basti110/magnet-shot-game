#pragma once
#include "physics_node.h"

class CoordinateAxes: public PhysicsNode
{
public:
    CoordinateAxes(const glm::vec3& position, float radius, float lenght, btDynamicsWorld* world);
    ~CoordinateAxes();
    //void render(const glow::UsedProgram& shader, glm::mat4& projection, glm::mat4& view) override;
    void render(const glow::UsedProgram& shader, glm::mat4& projection, glm::mat4& view);
    

private:
    void createConeBuffer(float length, float radius, int fineness);
    void createLineBuffer(float length);

    bool mInit = false;
    unsigned int coneVAO;
    unsigned int coneVBO;

    glow::SharedVertexArray mLineSVA;

    //int coneTriangles;
    // create
};