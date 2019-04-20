#pragma once
#include <glow-extras/camera/Camera.hh>
#include <glow-extras/glfw/GlfwApp.hh>
#include <glow/fwd.hh>
#include "message_bus.h"
#include "starter.h"
#include "mesh_node.h"


class Game : public glow::glfw::GlfwApp
{
    // gfx settings
private:
    glm::vec3 mBackgroundColor = {.10f, .46f, .83f};
    bool mShowWireframe = false;
    bool mShowPostProcess = false;
    float mBounceF = 0;
    Starter* mStartManager;
    SceneManager* mScene;

    // gfx objects
private:
    // shaders
    glow::SharedProgram mShaderOutput;
    glow::SharedProgram mShaderObject;

    // meshes
    glow::SharedVertexArray mMeshQuad;
    MeshNode mFirstLevel;

    // intermediate framebuffer with color and depth texture
    glow::SharedFramebuffer mFramebuffer;
    glow::SharedTextureRectangle mTargetColor;
    glow::SharedTextureRectangle mTargetDepth;

    std::vector<glow::SharedTextureRectangle> mTargets;

    // ctor
public:
    Game();

    // events
public:
    void init() override;                       // called once after OpenGL is set up
    void update(float elapsedSeconds) override; // called in 60 Hz fixed timestep
    void render(float elapsedSeconds) override; // called once per frame (variable timestep)
    void onGui() override;                      // called once per frame to set up UI

    void onResize(int w, int h) override; // called when window is resized

    void updateCamera(float elapsedSeconds);
    std::function<void(KeyMessage)> getNotifyFuncKey();
    std::function<void(Message*)> getNotifyFuncGui();
    void notifyKeyInput(KeyMessage message);
    void notifyGuiInput(Message* message);
    void setBackgroundColor(glm::vec3 color);
};
