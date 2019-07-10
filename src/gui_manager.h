#pragma once
#include <utility>
#include <glm/glm.hpp>
#include "message_bus.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
class SceneManager;

class GuiManager
{
public:
    GuiManager(MessageBus* messageBus, GLFWwindow* window, SceneManager* scene);
    ~GuiManager();

    void init();
    void render();
    void update();

    void notifyGameModeChange(GameModeMessage message);
    void notifyGuiOut(Message* message);

private:
    void sendVec3Message(std::pair<glm::vec3, glm::vec3>& v, GuiSettings s);
    void sendFloatMessage(std::pair<float, float>& v, GuiSettings s);
    void renderOverlay();
    void renderWorldSettings();
    void renderEditorSettings();
    void renderPropertyView();

    MessageBus* mMessageBus;
    GLFWwindow* mWindow;
    SceneManager* mScene;

    GameMode mGameMode = GameMode::Gameplay;
    std::pair<float, float> mCubeSize;
    std::pair<glm::vec3, glm::vec3> mCubeColor;
    std::pair<glm::vec3, glm::vec3> mBackgroundColor1;
    std::pair<glm::vec3, glm::vec3> mBackgroundColor2;
    std::pair<glm::vec3, glm::vec3> mLightAmbient;
    std::pair<glm::vec3, glm::vec3> mLightDiffuse;
    std::pair<glm::vec3, glm::vec3> mLightSpecular;
    std::pair<glm::vec3, glm::vec3> mSunColor;
    std::pair<glm::vec3, glm::vec3> mSunsetColor1;
    std::pair<glm::vec3, glm::vec3> mSunsetColor2;
    //std::pair<bool, bool> mSSOA;
    std::pair<float, float> mShadowOffset;
    std::pair<float, float> mSunAngle;
    std::pair<float, float> mSunIntensity;
    std::pair<float, float> mShadowSmoothness;
    std::pair<float, float> mHdrExposure;

    //------------------------------------------------
    // ------------- Object Selection ----------------
    //------------------------------------------------

    std::vector<std::string> mObjects = {
        "Simple Cube", 
        "Simple Sphere", 
        "TestObject"};

    int mSelectedObject = 0;

    //Cube
    std::pair<glm::vec3, glm::vec3> mCubeShape;
    std::pair<float, float> mCubeMass;
    std::pair<float, float> mCubeFriction;
    std::pair<float, float> mCubeRestitution;
    std::pair<float, float> mCubeLinearDamping;
    std::pair<float, float> mCubeAngularDamping;

    bool mSave = false;

	//Property Output
    bool mPropertyViewIsActive = false;
    std::pair<BodyProperties, BodyProperties> mBodyProperty;
};

