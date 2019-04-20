#include "gui_manager.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"


GuiManager::GuiManager(MessageBus* messageBus, GLFWwindow* window) : 
    mMessageBus(messageBus),
    mWindow(window)
{
    mCubeColor.first = glm::vec3(1.0);
    mCubeColor.second = glm::vec3(1.0);
    mBackgroundColor.first = glm::vec3(1.0);
    mBackgroundColor.second = glm::vec3(1.0);
    mCubeSize.first = 1.0;
    mCubeSize.second = 1.0;
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
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
}

void GuiManager::render()
{
    //ImGui_ImplOpenGL3_NewFrame();
    //ImGui_ImplGlfw_NewFrame();
    //ImGui::NewFrame();
    //ImGui::Begin("GameDev Project", &show_another_window);
    if (ImGui::Begin("GameDev Project"))
    {
        ImGui::Text("Editor:");
        {
            //ImGui::Indent();
            ImGui::SliderFloat("Cube Size", &mCubeSize.first, 0.0f, 10.0f);
            ImGui::ColorEdit3("Cube Color", &mCubeColor.first.r);
            ImGui::ColorEdit3("Background Color", &mBackgroundColor.first.r);
            ImGui::ColorEdit3("Light Color", &mLightColor.first.r);
            //ImGui::Unindent();
        }
    }
    /*ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
    ImGui::Text("Hello from another window!");
    if (ImGui::Button("Close Me"))
        show_another_window = false;*/
    ImGui::End();


    //ImGui::Render();
    //int display_w, display_h;
    //glfwMakeContextCurrent(mWindow);
    //glfwGetFramebufferSize(mWindow, &display_w, &display_h);
    //glViewport(0, 0, display_w, display_h);
    //glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    //glClear(GL_COLOR_BUFFER_BIT);
    //ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GuiManager::update()
{
    if (mCubeColor.first != mCubeColor.second)
    {
        GuiVec3Message m(mCubeColor.first, GuiSettings::CUBE_COLOR);
        mMessageBus->sendMessage(&m);
        mCubeColor.second = mCubeColor.first;
    }
     
    if (mBackgroundColor.first != mBackgroundColor.second)
    {
        GuiVec3Message m(mBackgroundColor.first, GuiSettings::BACKGROUND_COLOR);
        mMessageBus->sendMessage(&m);
        mBackgroundColor.second = mBackgroundColor.first;
    }

    if (mCubeSize.first != mCubeSize.second)
    {
        GuiFloatMessage m(mCubeSize.first, GuiSettings::CUBE_SIZE);
        mMessageBus->sendMessage(&m);
        mCubeSize.second = mCubeSize.first;
    }
    
    if (mLightColor.first != mLightColor.second)
    {
        GuiVec3Message m(mLightColor.first, GuiSettings::LIGHT_COLOR);
        mMessageBus->sendMessage(&m);
        mLightColor.second = mLightColor.first;
    }
}
