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
private:
    void sendVec3Message(std::pair<glm::vec3, glm::vec3>& v, GuiSettings s);
    void sendFloatMessage(std::pair<float, float>& v, GuiSettings s);

    GLFWwindow* mWindow;
    MessageBus* mMessageBus;

    std::pair<float, float> mCubeSize;
    std::pair<glm::vec3, glm::vec3> mCubeColor;
    std::pair<glm::vec3, glm::vec3> mBackgroundColor;
    std::pair<glm::vec3, glm::vec3> mLightColor;
};

