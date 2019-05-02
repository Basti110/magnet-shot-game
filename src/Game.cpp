#include "Game.h"

#include <glm/ext.hpp>

#include "gui_manager.h"
#include "io_manager.h"
#include "light.h"
#include "node.h"
#include "scnene_manager.h"
#include "physics_manager.h"
#include "cube.h"

// glow OpenGL wrapper
#include <glow/common/log.hh>
#include <glow/common/scoped_gl.hh>
#include <glow/objects/ArrayBuffer.hh>
#include <glow/objects/Framebuffer.hh>
#include <glow/objects/Program.hh>
#include <glow/objects/Texture2D.hh>
#include <glow/objects/TextureRectangle.hh>
#include <glow/objects/VertexArray.hh>

// extra functionality of glow
#include <GLFW/glfw3.h>  // window/input framework
#include <imgui/imgui.h> // UI framework
#include <glm/glm.hpp>   // math library
#include <glm/gtc/matrix_transform.hpp>
#include <glow-extras/geometry/Quad.hh>
#include <glow-extras/geometry/UVSphere.hh>
#include "load_mesh.h" // helper function for loading .obj into VertexArrays


Game::Game() : GlfwApp(Gui::ImGui) {}

void Game::init()
{
    setVSync(true);
    GlfwApp::init();
    setTitle("Game Development 2019");
    mStartManager = new Starter(window());
    mScene = mStartManager->getScene();
    mPhysics = mStartManager->getPhysicsManager();

    mStartManager->getMessageBus()->addGameModeReceiver([=](GameModeMessage message) {
        this->notifyGameModeChange(message);
    });
    mStartManager->getMessageBus()->addGuiReceiver(getNotifyFuncGui());

    GameModeMessage m(GameMode::Gameplay);
    mStartManager->getMessageBus()->sendMessage(&m);
    setCursorMode(glow::glfw::CursorMode::Disabled);

    {
        //Camera* cam = new Camera();
        //mScene->setCamera(cam);

        // create framebuffer (16bit color + 32bit depth)
        // size is 1x1 for now and is changed onResize
        mTargets.push_back(mTargetColor = glow::TextureRectangle::create(1, 1, GL_RGB16F));
        mTargets.push_back(mTargetDepth = glow::TextureRectangle::create(1, 1, GL_DEPTH_COMPONENT32F));
        mFramebuffer = glow::Framebuffer::create("fColor", mTargetColor, mTargetDepth);
    }

    {

        mMeshQuad = glow::geometry::make_quad();

        auto crosshairBuffer = glow::ArrayBuffer::create();
        crosshairBuffer->defineAttribute<glm::vec4>("aPosition");
        crosshairBuffer->bind().setData({
            glm::vec4(0.0f, -0.1f, 0.0f, 1.f), glm::vec4(0.0f, 0.1f, 0.0f, 1.0f),
            glm::vec4(-0.1f, 0.0f, 0.0f, 1.f), glm::vec4(0.1f, 0.0f, 0.0f, 1.0f),
        });
        mCrosshair = glow::VertexArray::create(crosshairBuffer, GL_LINES);

        mFirstLevel.init("../../data/meshes/FirstRoom.obj", false);
        mPhysics->addMesh("../../data/meshes/FirstRoomCollision.obj");

        mShaderObject = glow::Program::createFromFile("../../data/shaders/shader_node");
        mShaderOutput = glow::Program::createFromFile("../../data/shaders/output");
        mShaderCrosshair = glow::Program::createFromFile("../../data/shaders/crosshair");

        Node* root = new Node;
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::translate(trans, glm::vec3(0.0f, 4.0f, -9.0f));
        Light* light = new Light(trans, Color{1.0f, 1.0f, 1.0f});
        root->addChild(light);
        root->addChild(&mFirstLevel);
        mStartManager->getScene()->setLight(light);
        root->createBuffer();
        mScene->setSceneRoot(root);


        // add a bunch of cubes for testing
        RigidBodyInfo info;
        info.mass = 1.0;
        for (int i = 0; i < 5; i++) {
            glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(1,i*5+5, 0));
            Cube* newCube = new Cube(trans, Color{1.0f, 1.0f, 1.0f}, mStartManager->getPhysicsManager());
            mScene->appendNode(newCube);
            newCube->createBuffer();
            newCube->addPhysics(glm::vec3(1.0f), info);
        }
    }
}

void Game::update(float elapsedSeconds)
{
    // update game in 60 Hz fixed timestep
    float currentFrame = glfwGetTime();
    float deltaTime = currentFrame - mLastFrame;

    float time = glfwGetTime();
    mStartManager->getPhysicsManager()->update(elapsedSeconds);
    mScene->update(elapsedSeconds);
    mStartManager->getGuiManager()->update();
    mStartManager->getIOManager()->processInput();
    mStartManager->getMessageBus()->notify();
    time = glfwGetTime() - time;

    if (deltaTime > 1)
    {
        std::cout << "Update: " << time << "ms\n";
        mLastFrame = glfwGetTime();
    }
}

void Game::render(float elapsedSeconds)
{
    // render game variable timestep

    // camera update here because it should be coupled tightly to rendering!
    updateCamera(elapsedSeconds);
    // TODO: Add window size to parameter
    int SCR_WIDTH = 1080;
    int SCR_HEIGHT = 800;

    {
        auto fb = mFramebuffer->bind();

        GLOW_SCOPED(enable, GL_DEPTH_TEST);
        //GLOW_SCOPED(enable, GL_CULL_FACE);

        //GLOW_SCOPED(polygonMode, GL_FILL);


        GLOW_SCOPED(clearColor, mBackgroundColor);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        {
            auto shader = mShaderObject->use();
            glm::mat4 view = mScene->getCamera()->getViewMatrix();

            // TODO: Add rojection matrix to Camera class
            glm::mat4 projection = mScene->getCamera()->getProjectionMatrix();

            shader.setUniform("projection", projection);
            shader.setUniform("view", view);
            mScene->render(shader, projection, view);
        }
    }

    GLOW_SCOPED(disable, GL_DEPTH_TEST);
    //GLOW_SCOPED(disable, GL_CULL_FACE);

    GLOW_SCOPED(polygonMode, GL_FILL);
    auto shaderOutput = mShaderOutput->use();
    shaderOutput.setTexture("uTexColor", mTargetColor);
    shaderOutput.setTexture("uTexDepth", mTargetDepth);
    shaderOutput.setUniform("uShowPostProcess", mShowPostProcess);
    mMeshQuad->bind().draw();

    // draw crosshair
    int w, h;
    glfwGetWindowSize(window(), &w, &h);
    const float aspectRatio = w / (float)h;
    auto shaderCrosshair = mShaderCrosshair->use();
    shaderCrosshair.setUniform("uAspectRatio", aspectRatio);
    shaderCrosshair.setUniform("uScale", 0.5f);
    mCrosshair->bind().draw();
}

// Update the GUI
void Game::onGui()
{
    mStartManager->getGuiManager()->render();
}

// Called when window is resized
void Game::onResize(int w, int h)
{
    mScene->getCamera()->setViewportSize(w, h);

    for (auto const& t : mTargets)
        t->bind().resize(w, h);
}

void Game::updateCamera(float elapsedSeconds) {}


std::function<void(Message*)> Game::getNotifyFuncGui()
{
    auto messageListener = [=](Message* message) -> void { this->notifyGuiInput(message); };
    return messageListener;
}

void Game::notifyGameModeChange(GameModeMessage message)
{
    if (message.mode == GameMode::Menu) {
        setCursorMode(glow::glfw::CursorMode::Normal);
    }
    else {
        setCursorMode(glow::glfw::CursorMode::Disabled);
    }
}

void Game::notifyGuiInput(Message* message)
{
    if (message->getType() != MType::GUI_VEC3)
        return;

    GuiVec3Message* m = dynamic_cast<GuiVec3Message*>(message);
    if (m == nullptr)
        return;

    if (m->getSetting() == GuiSettings::BACKGROUND_COLOR)
    {
        setBackgroundColor(m->getValue());
    }
}

void Game::setBackgroundColor(glm::vec3 color)
{
    mBackgroundColor = color;
}
