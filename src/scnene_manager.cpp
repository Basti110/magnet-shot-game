#include "scnene_manager.h"
#include <algorithm>


SceneManager::SceneManager()
{
	//this->root.createBuffer();
	mRoot = new Node();
    mLight = nullptr;
    mViewCamera = new Camera();
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

void SceneManager::removeNode(std::function<bool(AbstractNode*)> predicate)
{
    auto invertedPredicate = std::not1(predicate);
    auto firstToRemove = std::stable_partition(mDynamicObjects.begin(), mDynamicObjects.end(), invertedPredicate);
    std::for_each(firstToRemove, mDynamicObjects.end(), [](AbstractNode* n){ delete n; });
    mDynamicObjects.erase(firstToRemove, mDynamicObjects.end());
}

void SceneManager::update(float elapsedSeconds) 
{
    for (int i = 0; i < mDynamicObjects.size(); ++i)
    {
        mDynamicObjects[i]->update(elapsedSeconds);
    }
    mRoot->update(elapsedSeconds);
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

