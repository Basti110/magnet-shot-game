#include "scnene_manager.h"
//#include "abstract_node.h"



SceneManager::SceneManager()
{
	//this->root.createBuffer();
	mRoot = new Node();
    mLight = nullptr;
}


SceneManager::~SceneManager()
{

}

void SceneManager::RenderScene()
{

}

void SceneManager::addDynamicObject(AbstractNode & node)
{

}

Node* SceneManager::getSceneRoot()
{
	//this->root = Node();
    return mRoot;
}

void SceneManager::setSceneRoot(Node* node)
{
	//this->root = Node();
    mRoot = node;
}

Camera * SceneManager::getCamera()
{
    return mViewCamera;
}

Light * SceneManager::getLight()
{
    return mLight;
}

void SceneManager::setLight(Light * light)
{
    mLight = light;
}

void SceneManager::appendNode(AbstractNode* node)
{
	mDynamicObjects.push_back(node);
}

void SceneManager::render(const glow::UsedProgram& shader, glm::mat4& projection, glm::mat4& view)
{
    glm::vec3 camPos = this->getCamera()->getPos();
    glm::vec3 lightPos;
    glm::vec3 lightColor;

    if (mLight)
    {
        lightPos = mLight->getPos();
        lightColor = mLight->getColor();
    }
    shader.setUniform("lightPos", lightPos);
    shader.setUniform("viewPos", camPos);
    shader.setUniform("lightColor", lightColor);
    
    for (int i = 0; i < mDynamicObjects.size(); ++i)
    {
        mDynamicObjects[i]->render(shader, projection, view);
	}
    mRoot->render(shader, projection, view);
}

void SceneManager::setCamera(Camera * camera)
{
    mViewCamera = camera;
}

