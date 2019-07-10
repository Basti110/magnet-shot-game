#include "scene_manager.h"
#include "point_light.h"
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>


SceneManager::SceneManager()
{
    mRoot = new Node();
    mSun = nullptr;
    mViewCamera = new Camera();
}


SceneManager::~SceneManager()
{

}

void SceneManager::RenderScene()
{

}

void SceneManager::addDynamicObject(AbstractNode& node)
{

}

void SceneManager::addPointLight(PointLight* light) 
{
    mPointLights.push_back(light);
}

Node* SceneManager::getSceneRoot()
{
    return mRoot;
}

void SceneManager::setSceneRoot(Node* node)
{
    mRoot = node;
}

Camera * SceneManager::getCamera()
{
    return mViewCamera;
}

Light * SceneManager::getSun()
{
    return mSun;
}

void SceneManager::setSun(Light * light)
{
    glm::mat4 transform = glm::translate(glm::mat4(1), glm::vec3(0.f, 99.f, 0.f));
    light->setLocalTransformation(transform);
    mSun = light;
}

void SceneManager::setSunAngle(float angle) 
{ 
    float pi = glm::pi<float>();
    float sunSetRatio = -0.8 * pow(angle - pi / 2, 2) + 2;
    sunSetRatio = glm::max(glm::min(sunSetRatio, 1.0f), 0.0f);

    mSunColor = sunSetRatio * glm::vec3(3.0) + (1 - sunSetRatio) * mSunSetColor * 5.0f;
    mHdrExploration = sunSetRatio * 2.5 + (1 - sunSetRatio) * 0.5;

    if (angle > 3.65 && !isNight)
    {
        isNight = true;
        SceneEventMessage m(SceneEventId::Night);
        MessageBus::getInstance()->sendMessage(&m);
    }
    else if (angle < 3.65 && isNight)
    {
        isNight = false;
        SceneEventMessage m(SceneEventId::Day);
        MessageBus::getInstance()->sendMessage(&m);
    }

    mSunAngle = angle;
    if (angle > pi)
    {
        float ratio = -0.8 * pow(mSunAngle - 3 * pi / 2, 2) + 2;
        if (ratio >= 1)
            angle = angle - pi;
    }
    
    float theta = -angle - 3 * pi / 2;
    float phi = pi / 4;
    float r = 98;
    
    float y = r * cos(theta);
    float z = r * sin(theta) * cos(phi);
    float x = r * sin(theta) * sin(phi);

    glm::mat4 transform = glm::translate(glm::mat4(1), glm::vec3(x, y, z));
    mSun->setLocalTransformation(transform);
}

float SceneManager::getMoonVisibility()
{
    float pi = glm::pi<float>();
    float moonVisibility = 0;
    if (mSunAngle > 2.8) // define in common
    {
        moonVisibility = -0.6 * pow(mSunAngle - 3 * pi / 2, 2) + 1.8;
        moonVisibility = glm::max(glm::min(moonVisibility, 1.f), 0.f);
    }
    return moonVisibility;
}

float SceneManager::getSunVisibility()
{
    float pi = glm::pi<float>();
    float sunVisibility = 0;
    if (mSunAngle < 3.4) // definde in common
    {
        sunVisibility = -0.6 * pow(mSunAngle - pi / 2, 2) + 1.8;
        sunVisibility = glm::max(glm::min(sunVisibility, 1.f), 0.f);
    }
    return sunVisibility;
}

glm::vec3 SceneManager::getSunPos()
{
    return glm::vec3(mSun->getGlobalTransformation()[3]);
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
    for (size_t i = 0; i < mDynamicObjects.size(); ++i)
    {
        mDynamicObjects[i]->update(elapsedSeconds);
    }
    mRoot->update(elapsedSeconds);

    if (mAdvanceSunAngle > 0)
    {
        const float delta = + 0.2 * elapsedSeconds;
        setSunAngle(glm::mod(mSunAngle + delta, glm::two_pi<float>()));
        mAdvanceSunAngle -= delta;
    }
}

void SceneManager::render(glow::UsedProgram& shader, glm::mat4& projection, glm::mat4& view, bool shadowPass)
{
    glm::vec3 camPos = this->getCamera()->getPos();

	shader.setUniform("viewPos", camPos);
    
    for (size_t i = 0; i < mDynamicObjects.size(); ++i)
    {
        mDynamicObjects[i]->render(shader, projection, view, shadowPass);
	}
    mRoot->render(shader, projection, view, shadowPass);
}

void SceneManager::setLightInShader(glow::UsedProgram& shader)
{
    glm::vec3 sunAmbient;
    glm::vec3 sunDiffuse;
    glm::vec3 sunSpecular;
    glm::vec3 backAmbient;
    glm::vec3 backDiffuse;
    glm::vec3 backSpecular;
    float sunSetRatio;
    float pi = glm::pi<float>();

    sunAmbient = glm::vec3(1.0, 220 / 255., 199 / 255.) * mSun->getIntensity(); //Define in common
    sunDiffuse = glm::vec3(251 / 255., 155 / 255., 79 / 255.) * mSun->getIntensity();
    sunSpecular = glm::vec3(248 / 255., 125 / 255., 18 / 255.) * mSun->getIntensity();

    /*sunAmbient = mSun->getAmbient() * mSun->getIntensity();
    sunAmbient = mSun->getAmbient();
    sunAmbient *= mSun->getIntensity();
    sunDiffuse = mSun->getDiffuse() * mSun->getIntensity();
    sunSpecular = mSun->getSpecular() * mSun->getIntensity();*/

    if (mSunAngle < pi)
    {
        sunSetRatio = -0.8 * pow(mSunAngle - pi / 2, 2) + 2;
        sunSetRatio = glm::max(glm::min(sunSetRatio, 1.0f), 0.0f);
        backAmbient = mSun->getAmbient() * mSun->getIntensity();
        backDiffuse = mSun->getDiffuse() * mSun->getIntensity();
        backSpecular = mSun->getSpecular() * mSun->getIntensity();
        shader.setUniform("sunShadowOn", true);
    }
    else
    {
        sunSetRatio = -0.8 * pow(mSunAngle - 3 * pi / 2, 2) + 2;
        sunSetRatio = glm::max(glm::min(sunSetRatio, 1.f), 0.f);
        backAmbient = glm::vec3(3 / 255., 3 / 255., 3 / 255.); //Define in common
        backDiffuse = glm::vec3(0 / 255., 0 / 255., 0 / 255.);
        backSpecular = glm::vec3(0., 0., 0.);
        shader.setUniform("sunShadowOn", false);
    }

    sunAmbient = sunSetRatio * backAmbient + (1 - sunSetRatio) * sunAmbient;
    sunDiffuse = sunSetRatio * backDiffuse + (1 - sunSetRatio) * sunDiffuse;
    sunSpecular = sunSetRatio * backSpecular + (1 - sunSetRatio) * sunSpecular;
    
    glm::vec3 camPos = this->getCamera()->getPos();
    if (mSun)
    {
        glm::vec3 lightPos;
        lightPos = glm::vec3(mViewCamera->getViewMatrix() * glm::vec4(mSun->getPos(), 1));
        shader.setUniform("light.position", lightPos);
        shader.setUniform("light.ambient", sunAmbient);
        shader.setUniform("light.diffuse", sunDiffuse);
        shader.setUniform("light.specular", sunSpecular);
    }
    shader.setUniform("viewPos", camPos);

    
    GLint id;
    glGetIntegerv(GL_CURRENT_PROGRAM, &id);
    
    int lightCounter = 0;
    for (unsigned int i = 0; i < mPointLights.size(); i++)
    {
        if (!mPointLights[i]->getOn())
            continue;
        
        glm::vec3 lightPos = glm::vec3(mViewCamera->getViewMatrix() * glm::vec4(mPointLights[i]->getPos(), 1));
        glUniform3fv(glGetUniformLocation(id, ("pointLights[" + std::to_string(lightCounter) + "].Position").c_str()), 1, &lightPos[0]);
        glUniform3fv(glGetUniformLocation(id, ("pointLights[" + std::to_string(lightCounter) + "].Color").c_str()), 1, &(mPointLights[i]->getAmbient() * 5.0f)[0]);
        glUniform1f(glGetUniformLocation(id, ("pointLights[" + std::to_string(lightCounter) + "].Linear").c_str()), mPointLights[i]->getLinear());
        glUniform1f(glGetUniformLocation(id, ("pointLights[" + std::to_string(lightCounter) + "].Quadratic").c_str()), mPointLights[i]->getQuadratic());
        glUniform1f(glGetUniformLocation(id, ("pointLights[" + std::to_string(lightCounter) + "].Radius").c_str()), mPointLights[i]->getRadius());
        lightCounter++;
    }
    shader.setUniform("sizePointLight", lightCounter);
}

void SceneManager::setCamera(Camera * camera)
{
    mViewCamera = camera;
}

void SceneManager::toggleDayNight()
{
    if (mAdvanceSunAngle <= 0) {
        mAdvanceSunAngle = glm::pi<float>();
    }
}
