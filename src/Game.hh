#pragma once

#include <glow/fwd.hh>

#include <glow-extras/camera/Camera.hh>
#include <glow-extras/glfw/GlfwApp.hh>

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
    // logic
private:
    glm::vec3 mSpherePosition = {2, 0, 0};
    float mSphereSize = 1.0f;

    // cube
private:
    /// Collision shape
    btCollisionShape* mCubeCollisionShape = nullptr;
    /// Physics body
    btRigidBody* mCubeRigidBody = nullptr;
    /// Physics motion state
    btDefaultMotionState* mCubeMotionState = nullptr;
    /// Cube transformation
    glm::mat4 mCubeTransform = glm::translate(glm::vec3(-2, 3, 0)) * glm::rotate(glm::radians(45.f), normalize(glm::vec3(1, 1, 1)));

    // ground plane
private:
    btCollisionShape* mPlaneCollisionShape = nullptr;
    btRigidBody* mPlaneRigidBody = nullptr;
    btDefaultMotionState* mPlaneMotionState = nullptr;

private: // bullet physics
    btBroadphaseInterface* mBulletBroadphase = nullptr;
    btDefaultCollisionConfiguration* mBulletCollisionConfig = nullptr;
    btCollisionDispatcher* mBulletCollisionDispatcher = nullptr;
    btSequentialImpulseConstraintSolver* mBulletSolver = nullptr;
    btDiscreteDynamicsWorld* mBulletWorld = nullptr;

    // gfx settings
private:
    glm::vec3 mBackgroundColor = {.10f, .46f, .83f};
    bool mShowWireframe = false;
    bool mShowPostProcess = false;

    // gfx objects
private:
    glow::camera::SharedCamera mCamera;

    // shaders
    glow::SharedProgram mShaderOutput;
    glow::SharedProgram mShaderObject;

    // meshes
    glow::SharedVertexArray mMeshQuad;
    glow::SharedVertexArray mMeshCube;
    glow::SharedVertexArray mMeshSphere;

    // textures
    glow::SharedTexture2D mTexCubeAlbedo;
    glow::SharedTexture2D mTexCubeNormal;

    // intermediate framebuffer with color and depth texture
    glow::SharedFramebuffer mFramebuffer;
    glow::SharedTextureRectangle mTargetColor;
    glow::SharedTextureRectangle mTargetDepth;

    std::vector<glow::SharedTextureRectangle> mTargets;

    // ctor
public:
    Game();
    ~Game();

    // events
public:
    void init() override;                       // called once after OpenGL is set up
    void update(float elapsedSeconds) override; // called in 60 Hz fixed timestep
    void render(float elapsedSeconds) override; // called once per frame (variable timestep)
    void onGui() override;                      // called once per frame to set up UI

    void onResize(int w, int h) override; // called when window is resized

    void updateCamera(float elapsedSeconds);
};
