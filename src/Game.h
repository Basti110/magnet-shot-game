#pragma once
#include <glow-extras/camera/Camera.hh>
#include <glow-extras/glfw/GlfwApp.hh>
#include <glow/fwd.hh>
#include "message_bus.h"
#include "starter.h"


// bullet forward declarations
class btCollisionShape;
class btRigidBody;
struct btDefaultMotionState;
class btBroadphaseInterface;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btDiscreteDynamicsWorld;
class btSequentialImpulseConstraintSolver;
class MagnetGun;

class Game : public glow::glfw::GlfwApp
{
    // gfx settings
private:
    glm::vec4 mBackgroundColor1;
    glm::vec4 mBackgroundColor2;
    glm::vec3 mSunColor;
    glm::vec3 mSunsetBackground1;
    glm::vec3 mSunsetBackground2;
    glm::vec2 mWindowSize;
    bool mShowWireframe = false;
    bool mShowPostProcess = false;
    bool mShowPhysicsDebug = false;
    bool mDebugLine = false;
    bool mUseFXAA = true;
    float mBounceF = 0;
    int mSuperSampling = 1;
    Starter* mStartManager;
    SceneManager* mScene;
    PhysicsManager* mPhysics;
    MagnetGun* mMagnetGun;

    // gfx objects
private:

    void initSSOA();
    void initShader();
    void initLevel();

    // shaders
    glow::SharedProgram mShaderShadow;
    glow::SharedProgram mShaderOutput;
    glow::SharedProgram mShaderGeometry;
    glow::SharedProgram mShaderLighting;
    glow::SharedProgram mShaderSkybox;
    glow::SharedProgram mShaderCrosshair;

    // meshes
    glow::SharedVertexArray mMeshQuad;
    glow::SharedVertexArray mSkybox;
    glow::SharedVertexArray mCrosshair;

    // intermediate framebuffer with color and depth texture
    glow::SharedFramebuffer mFramebuffer;
    glow::SharedTexture2D mTargetColor;
    glow::SharedTexture2D mTargetDepth;

    std::vector<glow::SharedTexture2D> mTargets;
	
	//SSAO
    bool mSSOA_On = true;
    glow::SharedFramebuffer mGBuffer;
    glow::SharedTexture2D mGDepth;

    glow::SharedTexture2D mGPosition;
    glow::SharedTexture2D mGNormal;
    glow::SharedTexture2D mGAmbient;
    glow::SharedTexture2D mGDiffuse;
    glow::SharedTexture2D mGSpecular;

	glow::SharedFramebuffer mSSAO_Buffer;
	glow::SharedTexture2D mSSAO_Color;
    glow::SharedTexture2D mSSAO_Noise;
    glow::SharedFramebuffer mSSAO_BufferBlur;
    glow::SharedTexture2D mSSAO_ColorBlur;

	glow::SharedProgram mShaderSSAO;
    glow::SharedProgram mShaderBlurSSAO;
	std::vector<glm::vec3> ssaoKernel;
    std::vector<glm::vec3> ssaoNoise;


    // shadow map
    glow::SharedFramebuffer mShadowFramebuffer;
    glow::SharedTexture2D mShadowMap;

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
    std::function<void(Message*)> getNotifyFuncGui();
    void notifyGameModeChange(GameModeMessage message);
    void notifyGuiInput(Message* message);
    void notifyKeyInput(KeyMessage message);

    float mLastFrame = 0;
};
