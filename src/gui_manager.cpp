#include "gui_manager.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"


GuiManager::GuiManager(MessageBus* messageBus, GLFWwindow* window) : 
    mMessageBus(messageBus),
    mWindow(window)
{
    messageBus->addGameModeReceiver([=](GameModeMessage message) {
        this->notifyGameModeChange(message);
    });

    //Common Settings

    mBackgroundColor.first = glm::vec3(0.2, 0.6, 0.7);
    mLightColor.first = glm::vec3(1.0);
    
    sendVec3Message(mBackgroundColor, GuiSettings::BACKGROUND_COLOR);
    sendVec3Message(mLightColor, GuiSettings::LIGHT_COLOR);
    
    //Cube Settings

    mCubeSize.first = 1.0;
    mCubeColor.first = glm::vec3(1.0);
    mCubeShape.first = glm::vec3(1.0, 1.0, 1.0);
    mCubeMass.first = 1;
    mCubeFriction.first = 0.1;
    mCubeRestitution.first = 0;
    mCubeLinearDamping.first = 0.01;
    mCubeAngularDamping.first = 0.01;

    sendFloatMessage(mCubeSize, GuiSettings::CUBE_SIZE);
    sendVec3Message(mCubeColor, GuiSettings::CUBE_COLOR);
    sendVec3Message(mCubeShape, GuiSettings::CUBE_SHAPE);
    sendFloatMessage(mCubeMass, GuiSettings::CUBE_MASS);
    sendFloatMessage(mCubeFriction, GuiSettings::CUBE_FRICTION);
    sendFloatMessage(mCubeRestitution, GuiSettings::CUBE_RESTITUTION);
    sendFloatMessage(mCubeLinearDamping, GuiSettings::CUBE_LINEAR_DAMPING);
    sendFloatMessage(mCubeAngularDamping, GuiSettings::CUBE_ANGULAR_DAMPING);
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
    if (!mGuiOn)
        return;

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
    //bool show_demo_window = true;
    //ImGui::ShowDemoWindow(&show_demo_window);

    bool* p_open = new bool(false);
    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Layout", p_open, ImGuiWindowFlags_MenuBar))
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Close"))
                    *p_open = false;
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // left
        ImGui::BeginChild("left pane", ImVec2(150, 0), true);
        for (int i = 0; i < mObjects.size(); i++)
        {
            //char label[128];
            //sprintf(label, "MyObject %d", i);
            if (ImGui::Selectable(mObjects[i].c_str(), mSelectedObject == i))
                mSelectedObject = i;
        }
        ImGui::EndChild();
        ImGui::SameLine();

        // right
        ImGui::BeginGroup();
        ImGui::BeginChild("Item Option", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
        ImGui::Text(mObjects[mSelectedObject].c_str());
        ImGui::Separator();
        if(mSelectedObject == 0) 
        {
            ImGui::SliderFloat("Cube Size X", &mCubeShape.first.x, 0.0f, 10.0f);
            ImGui::SliderFloat("Cube Size Y", &mCubeShape.first.y, 0.0f, 10.0f);
            ImGui::SliderFloat("Cube Size Z", &mCubeShape.first.z, 0.0f, 10.0f);
            ImGui::ColorEdit3("Cube Color", &mCubeColor.first.r);
            //ImGui::SameLine();
            ImGui::Spacing();
            ImGui::Text("Physical Properties");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::SliderFloat("Mass", &mCubeMass.first, 0.0f, 10.0f);
            ImGui::SliderFloat("Friction", &mCubeFriction.first, 0.0f, 1.0f);
            ImGui::SliderFloat("Restitution", &mCubeRestitution.first, 0.0f, 1.0f);
            ImGui::SliderFloat("Linear Damping", &mCubeLinearDamping.first, 0.0f, 1.0f);
            ImGui::SliderFloat("Angular Damping", &mCubeAngularDamping.first, 0.0f, 1.0f);
        }
        else if (mSelectedObject == 1) 
        {
            ImGui::TextWrapped("coming soon");
        }
        else if (mSelectedObject == 2)
        {
            ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna "
                               "aliqua. ");
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    ImGui::End();
}

void GuiManager::update()
{
    if (!mGuiOn)
        return;

    if (mCubeColor.first != mCubeColor.second)
        sendVec3Message(mCubeColor, GuiSettings::CUBE_COLOR);
  
     
    if (mBackgroundColor.first != mBackgroundColor.second)
        sendVec3Message(mBackgroundColor, GuiSettings::BACKGROUND_COLOR);


    if (mCubeSize.first != mCubeSize.second)
        sendFloatMessage(mCubeSize, GuiSettings::CUBE_SIZE);

    
    //Cube
    
    if (mLightColor.first != mLightColor.second)   
        sendVec3Message(mLightColor, GuiSettings::LIGHT_COLOR);

    if (mCubeShape.first != mCubeShape.second)
        sendVec3Message(mCubeShape, GuiSettings::CUBE_SHAPE);

    if (mCubeMass.first != mCubeMass.second)
        sendFloatMessage(mCubeMass, GuiSettings::CUBE_MASS);

    if (mCubeFriction.first != mCubeFriction.second)
        sendFloatMessage(mCubeFriction, GuiSettings::CUBE_FRICTION);

    if (mCubeRestitution.first != mCubeRestitution.second)
        sendFloatMessage(mCubeRestitution, GuiSettings::CUBE_RESTITUTION);

    if (mCubeLinearDamping.first != mCubeLinearDamping.second)
        sendFloatMessage(mCubeLinearDamping, GuiSettings::CUBE_LINEAR_DAMPING);

    if (mCubeAngularDamping.first != mCubeAngularDamping.second)
        sendFloatMessage(mCubeAngularDamping, GuiSettings::CUBE_ANGULAR_DAMPING);
    
}

void GuiManager::notifyGameModeChange(GameModeMessage message)
{
    mGuiOn = message.mode == GameMode::Menu;
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
