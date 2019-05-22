#include "Game.h"

#include <glm/ext.hpp>

#include "gui_manager.h"
#include "io_manager.h"
#include "scnene_manager.h"
#include "physics_manager.h"

#include "node.h"
#include "light.h"
#include "mesh_node.h"
#include "dispenser.h"
#include "cube.h"

// glow OpenGL wrapper
#include <glow/common/log.hh>
#include <glow/common/scoped_gl.hh>
#include <glow/objects/ArrayBuffer.hh>
#include <glow/objects/Framebuffer.hh>
#include <glow/objects/Program.hh>
#include <glow/objects/Texture2D.hh>

// extra functionality of glow
#include <GLFW/glfw3.h>  // window/input framework
#include <imgui/imgui.h> // UI framework
#include <glm/glm.hpp>   // math library
#include <glm/gtc/matrix_transform.hpp>
#include <glow-extras/geometry/Quad.hh>
#include <glow-extras/geometry/UVSphere.hh>


Game::Game() : GlfwApp(Gui::ImGui) {}

void Game::init()
{
    setVSync(true);
    GlfwApp::init();
    setTitle("Game Development 2019");
    mStartManager = new Starter(window());
    mScene = mStartManager->getScene();
    mPhysics = mStartManager->getPhysicsManager();
    auto dynamicsWorld = mPhysics->getDynamicsWorld();

    mStartManager->getMessageBus()->addGameModeReceiver([=](GameModeMessage message) {
        this->notifyGameModeChange(message);
    });
    mStartManager->getMessageBus()->addKeyReceiver([=](KeyMessage message) { 
        this->notifyKeyInput(message); 
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
        mTargets.push_back(mTargetColor = glow::Texture2D::create(1, 1, GL_RGB16F));
        mTargets.push_back(mTargetDepth = glow::Texture2D::create(1, 1, GL_DEPTH_COMPONENT32));
        mTargetColor->bind().setFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
        mTargetDepth->bind().setFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
        mTargetColor->bind().generateMipmaps();
        mTargetDepth->bind().generateMipmaps();
        mFramebuffer = glow::Framebuffer::create("fColor", mTargetColor, mTargetDepth);
    }
    {
        mShadowMap = glow::Texture2D::create(8192, 8192, GL_DEPTH_COMPONENT32);
        mShadowMap->bind().generateMipmaps();
        mShadowMap->bind().setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        mShadowMap->bind().setFilter(GL_LINEAR, GL_LINEAR);
        mShadowFramebuffer = glow::Framebuffer::createDepthOnly(mShadowMap);
    }

    {

        mMeshQuad = glow::geometry::make_quad();
        mSkybox = glow::geometry::make_uv_sphere();

        auto crosshairBuffer = glow::ArrayBuffer::create();
        crosshairBuffer->defineAttribute<glm::vec4>("aPosition");
        crosshairBuffer->bind().setData({
            glm::vec4(0.0f, -0.1f, 0.0f, 1.f), glm::vec4(0.0f, 0.1f, 0.0f, 1.0f),
            glm::vec4(-0.1f, 0.0f, 0.0f, 1.f), glm::vec4(0.1f, 0.0f, 0.0f, 1.0f),
        });
        mCrosshair = glow::VertexArray::create(crosshairBuffer, GL_LINES);

        mShaderShadow = glow::Program::createFromFile("../../data/shaders/shadow");
        mShaderObject = glow::Program::createFromFile("../../data/shaders/shader_node");
        mShaderOutput = glow::Program::createFromFile("../../data/shaders/output");
        mShaderSkybox = glow::Program::createFromFile("../../data/shaders/skybox");
        mShaderCrosshair = glow::Program::createFromFile("../../data/shaders/crosshair");

        Node* root = new Node;

        // add light
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::translate(trans, glm::vec3(30.0f, 25.0f, 25.0f));
        Light* light = new Light(trans, Color{1.0f, 1.0f, 1.0f});
        root->addChild(light);
        mScene->setLight(light);

        // add level
        MeshNode* level = new MeshNode(
            glm::mat4(1), "../../data/meshes/FirstRoom.obj", mPhysics,
            GROUP_STATIC_OBJECTS, GROUP_DYNAMIC_OBJECTS, 0.0f
        );
        root->addChild(level);

        // add dynamic objects
        MeshNode* obstacle = new MeshNode(
            glm::mat4(1), "../../data/meshes/Obstacle.obj", mPhysics,
            GROUP_DYNAMIC_OBJECTS, GROUP_STATIC_OBJECTS | GROUP_DYNAMIC_OBJECTS, 1.0f
        );
        obstacle->setColor(glm::vec3(234/255.0f, 22/255.0f, 22/255.0f));
        obstacle->getRigidBody()->setLinearFactor(btVector3(1,0,0));
        obstacle->getRigidBody()->setAngularFactor(btVector3(0,0,0));
        obstacle->getRigidBody()->setFriction(0.5f);
        root->addChild(obstacle);

        // add trees
        auto treeParams = {
            std::make_pair(glm::vec3(-4.35f, 0.27f, -6.2f), -45.0f),
            std::make_pair(glm::vec3(-4.35f, 0.27f, -9.5f), -45.0f),
            std::make_pair(glm::vec3(11.25f, 0.27f, -12.18f), 0.0f)
        };
        for (const auto& param : treeParams) {
            glm::mat4 transform = glm::translate(glm::mat4(1), param.first);
            transform = glm::rotate(transform, glm::radians(param.second), glm::vec3(0,1,0));
            MeshNode* base = new MeshNode(
                transform, "../../data/meshes/TreeBase.obj", mPhysics,
                GROUP_STATIC_OBJECTS, GROUP_DYNAMIC_OBJECTS, 0.0f
            );
            base->setColor(glm::vec3(151/255.0f, 103/255.0f, 48/255.0f));
            root->addChild(base);
            MeshNode* top = new MeshNode(
                transform, "../../data/meshes/TreeTop.obj", mPhysics,
                GROUP_STATIC_OBJECTS, GROUP_DYNAMIC_OBJECTS, 0.0f
            );
            top->setColor(glm::vec3(99/255.0f, 194/255.0f, 47/255.0f));
            root->addChild(top);
        }

        // add dispenser
        root->addChild(new Dispenser(glm::vec3(2.5f, 0.0f, -14.0f), mPhysics));

        root->createBuffer();
        mScene->setSceneRoot(root);

        // add a bunch of cubes for testing
        RigidBodyInfo info;
        info.mass = 5.0;
        info.friction = 0.5;
        for (int i = 0; i < 4; i++) {
            glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(1,i*5+5, -7));
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
        //std::cout << "Update: " << time << "ms\n";
        mLastFrame = glfwGetTime();
    }
}

void Game::render(float elapsedSeconds)
{
    // shadow pass
    glm::vec3 camPos = mStartManager->getScene()->getCamera()->getPos();
    glm::vec3 lightPos = camPos + glm::vec3(10);
    glm::mat4 shadowView = glm::lookAt(lightPos, camPos, glm::vec3(0, 1, 0));
    glm::mat4 shadowProj = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, 1.0f, 50.0f);
    glm::mat4 shadowTransform = shadowProj * shadowView;
    {
        GLOW_SCOPED(enable, GL_DEPTH_TEST);
        GLOW_SCOPED(cullFace, GL_FRONT);

        auto fb = mShadowFramebuffer->bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        auto shader = mShaderShadow->use();
        shader.setUniform("projection", shadowProj);
        shader.setUniform("view", shadowView);
        mScene->render(shader, shadowProj, shadowView);
    }

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


        GLOW_SCOPED(clearColor, mBackgroundColor1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = mScene->getCamera()->getViewMatrix();
        glm::mat4 projection = mScene->getCamera()->getProjectionMatrix();
        if (!mShowPhysicsDebug)
        {
            auto shader = mShaderObject->use();
            shader.setUniform("projection", projection);
            shader.setUniform("view", view);
            shader.setUniform("shadowTransform", shadowTransform);
            shader.setTexture("shadowMap", mShadowMap);
            mScene->render(shader, projection, view);
        }
        else
        {
            float updateRate = 0.0;
            mPhysics->renderDebug(projection, view, updateRate);
        }
        

        // draw skybox
        view = glm::mat4(glm::mat3(view));
        GLOW_SCOPED(depthFunc, GL_LEQUAL);
        auto shaderSkybox = mShaderSkybox->use();
        shaderSkybox.setUniform("uTransform", projection * view);
        shaderSkybox.setUniform("uColor1", mBackgroundColor1);
        shaderSkybox.setUniform("uColor2", mBackgroundColor2);
        mSkybox->bind().draw();
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
        t->bind().resize(2 * w, 2 * h);
}

void Game::updateCamera(float elapsedSeconds) {}


std::function<void(Message*)> Game::getNotifyFuncGui()
{
    auto messageListener = [=](Message* message) -> void { this->notifyGuiInput(message); };
    return messageListener;
}

void Game::notifyGameModeChange(GameModeMessage message)
{
    if (message.mode == GameMode::Menu || message.mode == GameMode::Editor2)
    {
        setCursorMode(glow::glfw::CursorMode::Normal);
    }
    else {
        setCursorMode(glow::glfw::CursorMode::Disabled);
    }
}

void Game::notifyGuiInput(Message* message)
{
    if (auto m = dynamic_cast<GuiVec3Message*>(message))
    {
        if (m->getSetting() == GuiSettings::BACKGROUND_COLOR1)
        {
            mBackgroundColor1 = glm::vec4(m->getValue(), 1.0f);
        }
        if (m->getSetting() == GuiSettings::BACKGROUND_COLOR2)
        {
            mBackgroundColor2 = glm::vec4(m->getValue(), 1.0f);
        }
    }
    if (auto m = dynamic_cast<GuiFloatMessage*>(message))
    {
        if (m->getSetting() == GuiSettings::SHADOW_OFFSET)
        {
            auto shader = mShaderObject->use();
            shader.setUniform("shadowOffset", m->getValue());
        }
        if (m->getSetting() == GuiSettings::SHADOW_SMOOTHNESS)
        {
            auto shader = mShaderObject->use();
            shader.setUniform("shadowSmoothness", m->getValue());
        }
    }
}

void Game::notifyKeyInput(KeyMessage message) 
{
    if (message.getAction() == GLFW_PRESS)
    {
        if (message.getInput() == GLFW_KEY_F7)
        {
            mShowPhysicsDebug = mShowPhysicsDebug ? false : true;
        }
    }
}
