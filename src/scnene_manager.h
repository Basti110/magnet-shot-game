#pragma once
#include "node.h"
#include "camera.h"
#include "light.h"
#include <glow/objects/Program.hh>
//class Camera;

class SceneManager
{
public:
    SceneManager();
    ~SceneManager();
    void RenderScene();
    void addDynamicObject(AbstractNode& node);
    void render(const glow::UsedProgram& shader, glm::mat4& projection, glm::mat4& view);
    void appendNode(AbstractNode* node);
    void update(float elapsedSeconds);

    //Setter
    void setCamera(Camera* camera);
    void setSceneRoot(Node* node);
    void setLight(Light* light);

    //Getter
	Node* getSceneRoot();
    Camera* getCamera();
    Light* getLight();

private:

	//std::vector<AbstractNode&> nodes;
	std::vector<AbstractNode*> mDynamicObjects;
	Node* mRoot;
    Camera* mViewCamera;
    Light* mLight;
};

