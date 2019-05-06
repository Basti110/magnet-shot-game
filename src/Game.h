#pragma once
#include <glow-extras/camera/Camera.hh>
#include <glow-extras/glfw/GlfwApp.hh>
#include <glow/fwd.hh>
#include "message_bus.h"
#include "starter.h"
#include "mesh_node.h"


// bullet forward declarations
class btCollisionShape;
class btRigidBody;
struct btDefaultMotionState;
class btBroadphaseInterface;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btDiscreteDynamicsWorld;
class btSequentialImpulseConstraintSolver;

class Game : public glow::glfw::GlfwApp
{
    // gfx settings
private:
    glm::vec4 mBackgroundColor1;
    glm::vec4 mBackgroundColor2;
    bool mShowWireframe = false;
    bool mShowPostProcess = false;
    float mBounceF = 0;
    Starter* mStartManager;
    SceneManager* mScene;
    PhysicsManager* mPhysics;

    // gfx objects
private:
    // shaders
    glow::SharedProgram mShaderOutput;
    glow::SharedProgram mShaderObject;
    glow::SharedProgram mShaderSkybox;
    glow::SharedProgram mShaderCrosshair;

    // meshes
    glow::SharedVertexArray mMeshQuad;
    glow::SharedVertexArray mSkybox;
    glow::SharedVertexArray mCrosshair;
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
    void notifyGameModeChange(GameModeMessage message);
    void notifyGuiInput(Message* message);

    float mLastFrame = 0;
};
