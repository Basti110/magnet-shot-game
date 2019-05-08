#include "gui_manager.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "scnene_manager.h"

GuiManager::GuiManager(MessageBus* messageBus, GLFWwindow* window, SceneManager* scene) : 
    mMessageBus(messageBus),
    mWindow(window), 
    mScene(scene)
{
    messageBus->addGameModeReceiver([=](GameModeMessage message) {
        this->notifyGameModeChange(message);
    });


    //Common Settings

    mBackgroundColor1.first = glm::vec3(  9/255.0,  65/255.0, 152/255.0);
    mBackgroundColor2.first = glm::vec3(102/255.0, 169/255.0, 222/255.0);
    mLightColor.first = glm::vec3(1.0);
    mShadowOffset.first = 0.0005;
    mShadowSmoothness.first = 0.0006;
    
    sendVec3Message(mBackgroundColor1, GuiSettings::BACKGROUND_COLOR1);
    sendVec3Message(mBackgroundColor2, GuiSettings::BACKGROUND_COLOR2);
    sendVec3Message(mLightColor, GuiSettings::LIGHT_COLOR);
    sendFloatMessage(mShadowOffset, GuiSettings::SHADOW_OFFSET);
    sendFloatMessage(mShadowSmoothness, GuiSettings::SHADOW_SMOOTHNESS);
    
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
    if (mGameMode == GameMode::Editor)
        renderOverlay();
    
    if (mGameMode == GameMode::Menu)
    {
        renderEditorSettings();
        renderWorldSettings();
        bool show_demo_window = true;
        //ImGui::ShowDemoWindow(&show_demo_window);
    }
}

void GuiManager::update()
{
    if (mGameMode != GameMode::Menu)
        return;

    if (mCubeColor.first != mCubeColor.second)
        sendVec3Message(mCubeColor, GuiSettings::CUBE_COLOR);
  
    if (mBackgroundColor1.first != mBackgroundColor1.second)
        sendVec3Message(mBackgroundColor1, GuiSettings::BACKGROUND_COLOR1);
    if (mBackgroundColor2.first != mBackgroundColor2.second)
        sendVec3Message(mBackgroundColor2, GuiSettings::BACKGROUND_COLOR2);

    if (mShadowOffset.first != mShadowOffset.second)
        sendFloatMessage(mShadowOffset, GuiSettings::SHADOW_OFFSET);
    if (mShadowSmoothness.first != mShadowSmoothness.second)
        sendFloatMessage(mShadowSmoothness, GuiSettings::SHADOW_SMOOTHNESS);

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
    mGameMode = message.mode;
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

void GuiManager::renderOverlay() 
{
    const float DISTANCE = 10.0f;
    bool* p_open = new bool(true);
    int corner = 0;

    glm::vec3 pos = mScene->getCamera()->getPos() + glm::vec3(2.0) * mScene->getCamera()->getCameraFront();
    ImVec2 window_pos
        = ImVec2((corner & 1) ? ImGui::GetIO().DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? ImGui::GetIO().DisplaySize.y - DISTANCE : DISTANCE);
    ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
    ImGui::SetNextWindowBgAlpha(0.3f);
    if (corner != -1)
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);

    if (ImGui::Begin("Example: Simple Overlay", p_open,
                     (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize
                         | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
    {
        ImGui::Text("Editor overlay");
        ImGui::Separator();
        if (ImGui::IsMousePosValid())
            ImGui::Text("Front Position: (%.1f,%.1f,%.1f)", pos.x, pos.y, pos.z);
        else
            ImGui::Text("Mouse Position: <invalid>");
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem("Custom", NULL, corner == -1))
                corner = -1;
            if (ImGui::MenuItem("Top-left", NULL, corner == 0))
                corner = 0;
            if (ImGui::MenuItem("Top-right", NULL, corner == 1))
                corner = 1;
            if (ImGui::MenuItem("Bottom-left", NULL, corner == 2))
                corner = 2;
            if (ImGui::MenuItem("Bottom-right", NULL, corner == 3))
                corner = 3;
            if (p_open && ImGui::MenuItem("Close"))
                *p_open = false;
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}

void GuiManager::renderWorldSettings() 
{
    if (ImGui::Begin("GameDev Project"))
    {
        ImGui::Text("Editor:");
        {
            ImGui::SliderFloat("Cube Size", &mCubeSize.first, 0.0f, 10.0f);
            ImGui::ColorEdit3("Cube Color", &mCubeColor.first.r);
            ImGui::ColorEdit3("Background Color 1", &mBackgroundColor1.first.r);
            ImGui::ColorEdit3("Background Color 2", &mBackgroundColor2.first.r);
            ImGui::ColorEdit3("Light Color", &mLightColor.first.r);
            ImGui::SliderFloat("Shadow Offset", &mShadowOffset.first, 0.0f, 0.0025f, "%.5f");
            ImGui::SliderFloat("Shadow Smoothness", &mShadowSmoothness.first, 0.0f, 0.0025f, "%.5f");
        }
    }
    ImGui::End();
}

void GuiManager::renderEditorSettings() 
{
    bool* p_open = new bool(true);
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
            // char label[128];
            // sprintf(label, "MyObject %d", i);
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
        if (mSelectedObject == 0)
        {
            ImGui::SliderFloat("Cube Size X", &mCubeShape.first.x, 0.0f, 10.0f);
            ImGui::SliderFloat("Cube Size Y", &mCubeShape.first.y, 0.0f, 10.0f);
            ImGui::SliderFloat("Cube Size Z", &mCubeShape.first.z, 0.0f, 10.0f);
            ImGui::ColorEdit3("Cube Color", &mCubeColor.first.r);
            // ImGui::SameLine();
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
