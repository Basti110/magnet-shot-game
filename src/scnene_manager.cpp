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

void SceneManager::render(glow::UsedProgram& shader, glm::mat4& projection, glm::mat4& view, bool shadowPass)
{
    glm::vec3 camPos = this->getCamera()->getPos();

	shader.setUniform("viewPos", camPos);
    
    for (int i = 0; i < mDynamicObjects.size(); ++i)
    {
        mDynamicObjects[i]->render(shader, projection, view, shadowPass);
	}
    mRoot->render(shader, projection, view, shadowPass);
}

void SceneManager::setLight(glow::UsedProgram& shader) 
{
    glm::vec3 camPos = this->getCamera()->getPos();
    if (mLight)
    {
        glm::vec3 lightPos;
        lightPos = mLight->getPos();
        shader.setUniform("light.position", lightPos);
        shader.setUniform("light.ambient", mLight->getAmbient());
        shader.setUniform("light.diffuse", mLight->getDiffuse());
        shader.setUniform("light.specular", mLight->getSpecular());
    }
    shader.setUniform("viewPos", camPos);
}

void SceneManager::setCamera(Camera * camera)
{
    mViewCamera = camera;
}

