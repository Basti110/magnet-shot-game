#pragma once
#include <functional>
#include <glow/objects/Program.hh>
#include "camera.h"
#include "light.h"
#include "node.h"
// class Camera;

class SceneManager
{
public:
    SceneManager();
    ~SceneManager();
    void RenderScene();
    void addDynamicObject(AbstractNode& node);
    void render(glow::UsedProgram& shader, glm::mat4& projection, glm::mat4& view, bool shadowPass);
    void setLight(glow::UsedProgram& shader);
    void appendNode(AbstractNode* node);
    void removeNode(std::function<bool(AbstractNode*)> predicate);
    void update(float elapsedSeconds);
    void setSunAngle(float angle);
    void setSunColor(glm::vec3 sunColor);
    glm::vec3 getSunPos();

        // Setter
        void setCamera(Camera* camera);
    void setSceneRoot(Node* node);
    void setSun(Light* light);

    // Getter
    Node* getSceneRoot();
    Camera* getCamera();
    Light* getSun();

private:
    // std::vector<AbstractNode&> nodes;
    std::vector<AbstractNode*> mDynamicObjects;
    Node* mRoot;
    Camera* mViewCamera;
    Light* mSun;
    float mSunAngle;
};
