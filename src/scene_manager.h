#pragma once
#include <functional>
#include <glow/objects/Program.hh>
#include "camera.h"
#include "light.h"
#include "node.h"
#include "message_bus.h"
// class Camera;
class PointLight;

class SceneManager
{
public:
    SceneManager();
    ~SceneManager();
    void RenderScene();
    void addDynamicObject(AbstractNode& node);
    void addPointLight(PointLight* light);
    void render(glow::UsedProgram& shader, glm::mat4& projection, glm::mat4& view, bool shadowPass);
    void setLightInShader(glow::UsedProgram& shader);
    void appendNode(AbstractNode* node);
    void removeNode(std::function<bool(AbstractNode*)> predicate);
    void update(float elapsedSeconds);
    void setSunAngle(float angle);
    //void setSunColor(glm::vec3 sunColor);
    glm::vec3 getSunPos();
    void toggleDayNight();

    // Setter
    void setCamera(Camera* camera);
    void notifyLocationEvent(LocationEventMessage message);
    void setSceneRoot(Node* node);
    void setSun(Light* light);
    void setSunColor(glm::vec3 v) { mSunSetColor = v; }

    // Getter
    glm::vec3 getSunColor() { return mSunColor; }
    float getSunAngle() {return mSunAngle;}
    Node* getSceneRoot();
    Camera* getCamera();
    Light* getSun();

private:
    // std::vector<AbstractNode&> nodes;
    std::vector<PointLight*> mPointLights;
    std::vector<AbstractNode*> mDynamicObjects;
    Node* mRoot;
    Camera* mViewCamera;
    Light* mSun;
    glm::vec3 mSunColor;
    glm::vec3 mSunSetColor;
    float mSunAngle;
    bool isNight = false;
    float mAdvanceSunAngle = 0;
};
