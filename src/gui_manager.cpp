#include "gui_manager.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"


GuiManager::GuiManager(MessageBus* messageBus, GLFWwindow* window) : 
    mMessageBus(messageBus),
    mWindow(window)
{
    mCubeColor.first = glm::vec3(1.0);
    mBackgroundColor.first = glm::vec3(0.2, 0.6, 0.7);
    mLightColor.first = glm::vec3(1.0);
    mCubeSize.first = 1.0;

    sendVec3Message(mCubeColor, GuiSettings::CUBE_COLOR);
    sendVec3Message(mBackgroundColor, GuiSettings::BACKGROUND_COLOR);
    sendVec3Message(mLightColor, GuiSettings::LIGHT_COLOR);
    sendFloatMessage(mCubeSize, GuiSettings::CUBE_SIZE);
}


GuiManager::~GuiManager()
{

}

void GuiManager::init()
{
    const char* glsl_version = "#version 130";
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); 
    (void)io;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
}

void GuiManager::render()
{
    if (ImGui::Begin("GameDev Project"))
    {
        ImGui::Text("Editor:");
        {
            ImGui::SliderFloat("Cube Size", &mCubeSize.first, 0.0f, 10.0f);
            ImGui::ColorEdit3("Cube Color", &mCubeColor.first.r);
            ImGui::ColorEdit3("Background Color", &mBackgroundColor.first.r);
            ImGui::ColorEdit3("Light Color", &mLightColor.first.r);
        }
    }
    ImGui::End();
}

void GuiManager::update()
{
    if (mCubeColor.first != mCubeColor.second)
        sendVec3Message(mCubeColor, GuiSettings::CUBE_COLOR);
  
     
    if (mBackgroundColor.first != mBackgroundColor.second)
        sendVec3Message(mBackgroundColor, GuiSettings::BACKGROUND_COLOR);


    if (mCubeSize.first != mCubeSize.second)
        sendFloatMessage(mCubeSize, GuiSettings::CUBE_SIZE);

    
    
    if (mLightColor.first != mLightColor.second)   
        sendVec3Message(mLightColor, GuiSettings::LIGHT_COLOR);
    
}

void GuiManager::sendVec3Message(std::pair<glm::vec3, glm::vec3>& v, GuiSettings s) {
    GuiVec3Message m(v.first, s);
    mMessageBus->sendMessage(&m);
    v.second = v.first;
}

void GuiManager::sendFloatMessage(std::pair<float, float>& v, GuiSettings s) {
    GuiFloatMessage m(v.first, s);
    mMessageBus->sendMessage(&m);
    v.second = v.first;
}
