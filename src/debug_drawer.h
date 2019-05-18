#pragma once
#include <LinearMath/btIDebugDraw.h>
#include <glow/objects/Program.hh>

class DebugDrawer : public btIDebugDraw
{
public:
    DebugDrawer();
    ~DebugDrawer();
    void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
    void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) {}
    void reportErrorWarning(const char* warningString) {}
    void draw3dText(const btVector3& location, const char* textString) {}
    void setDebugMode(int debugMode) {}
    int getDebugMode() const { return 1; }

    void setLineBuffer(const btVector3& from, const btVector3& to);
    void draw(glm::mat4& projection, glm::mat4& view, bool loadNew);
    void resetLines();

private:
    glow::SharedProgram mShader;
    std::vector<float> mVertices;
    unsigned int VBO = 0;
    unsigned int VAO = 0;
    bool mInit = false;
};