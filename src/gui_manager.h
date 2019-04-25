#pragma once
#include <utility>
#include <glm/glm.hpp>
#include "message_bus.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class GuiManager
{
public:
    GuiManager(MessageBus* messageBus, GLFWwindow* window);
    ~GuiManager();

    void init();
    void render();
    void update();
    void notifyKeyInput(KeyMessage message);

private:
    void sendVec3Message(std::pair<glm::vec3, glm::vec3>& v, GuiSettings s);
    void sendFloatMessage(std::pair<float, float>& v, GuiSettings s);
    std::function<void(KeyMessage)> getNotifyFuncKey();


    GLFWwindow* mWindow;
    MessageBus* mMessageBus;

    bool mGuiOn = true;
    std::pair<float, float> mCubeSize;
    std::pair<glm::vec3, glm::vec3> mCubeColor;
    std::pair<glm::vec3, glm::vec3> mBackgroundColor;
    std::pair<glm::vec3, glm::vec3> mLightColor;

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
};

