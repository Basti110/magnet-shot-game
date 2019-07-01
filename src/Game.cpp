#include "Game.h"

#include <glm/ext.hpp>

#include "common.h"
#include "gui_manager.h"
#include "io_manager.h"
#include "location_event_manager.h"
#include "physics_manager.h"
#include "scnene_manager.h"

#include "dynamic_cable.h"
#include "static_cable.h"
#include "cube.h"
#include "dispenser.h"
#include "floating_bridge.h"
#include "light.h"
#include "magnet_gun.h"
#include "mesh_node.h"
#include "node.h"
#include "screen.h"
#include "solar_panel.h"
#include "stairs.h"
#include "wind_turbine.h"
#include "light_cube.h"

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

#include <random>

int SCR_WIDTH = 1080;
int SCR_HEIGHT = 800;

#define SAMPLES 32


Game::Game() : GlfwApp(Gui::ImGui) {}

void Game::init()
{
    setVSync(true);
    GlfwApp::init();
    setTitle("Game Development 2019");
    mStartManager = new Starter(window());
    mScene = mStartManager->getScene();
    mPhysics = mStartManager->getPhysicsManager();
    auto messageBus = mStartManager->getMessageBus();

    messageBus->addGameModeReceiver([=](GameModeMessage message) { this->notifyGameModeChange(message); });
    messageBus->addKeyReceiver([=](KeyMessage message) { this->notifyKeyInput(message); });

    messageBus->addGuiReceiver(getNotifyFuncGui());

    GameModeMessage m(GameMode::Gameplay);
    messageBus->sendMessage(&m);
    setCursorMode(glow::glfw::CursorMode::Disabled);

    {
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
            glm::vec4(0.0f, -0.1f, 0.0f, 1.f),
            glm::vec4(0.0f, 0.1f, 0.0f, 1.0f),
            glm::vec4(-0.1f, 0.0f, 0.0f, 1.f),
            glm::vec4(0.1f, 0.0f, 0.0f, 1.0f),
        });
        mCrosshair = glow::VertexArray::create(crosshairBuffer, GL_LINES);

        mShaderShadow = glow::Program::createFromFile("../../data/shaders/shadow");
        mShaderGeometry = glow::Program::createFromFile("../../data/shaders/node_geometry");
        mShaderLighting = glow::Program::createFromFile("../../data/shaders/node_lighting");
        mShaderOutput = glow::Program::createFromFile("../../data/shaders/output");
        mShaderSkybox = glow::Program::createFromFile("../../data/shaders/skybox");
        mShaderCrosshair = glow::Program::createFromFile("../../data/shaders/crosshair");


        // register location events
        auto locationEventManager = mStartManager->getLocationEventManager();
        locationEventManager->registerEvent(glm::vec3(2, 0, 1), glm::vec3(3, 5, 2), LocationEventId::MagnetGunPickUp);
        locationEventManager->registerEvent(glm::vec3(-23.6, 0, -17.8), glm::vec3(-18.6, 5, -14.8), LocationEventId::Island2);
        locationEventManager->registerEvent(glm::vec3(-6, 4, -39), glm::vec3(-1, 7, -35), LocationEventId::ActivateStairs);
        locationEventManager->registerEvent(glm::vec3(-6, 0, -38), glm::vec3(-1, 7, -23), LocationEventId::DeactivateStairs);
    }

    initSSOA();
    initLevel();
}

void Game::update(float elapsedSeconds)
{
    // update game in 60 Hz fixed timestep
    float currentFrame = glfwGetTime();
    float deltaTime = currentFrame - mLastFrame;

    float time = glfwGetTime();
    mStartManager->getPhysicsManager()->update(elapsedSeconds);
    mStartManager->getGuiManager()->update();
    mStartManager->getIOManager()->processInput();
    mStartManager->getLocationEventManager()->update();
    mStartManager->getMessageBus()->notify();
    updateCamera(elapsedSeconds);
    mScene->update(elapsedSeconds);
    time = glfwGetTime() - time;

    if (deltaTime > 1)
    {
        // std::cout << "Update: " << time << "ms\n";
        mLastFrame = glfwGetTime();
    }
}

// TODO: Add window size to parameter
void Game::render(float elapsedSeconds)
{
    // Data
    glm::mat4 view = mScene->getCamera()->getViewMatrix();
    glm::vec3 lightPos = mScene->getSunPos();
    glm::mat4 shadowView = glm::lookAt(lightPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 shadowProj = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, 1.0f, 190.0f);
    glm::mat4 shadowTransform = shadowProj * shadowView * glm::inverse(view);

    // shadow pass
    {
        GLOW_SCOPED(enable, GL_DEPTH_TEST);
        GLOW_SCOPED(cullFace, GL_FRONT);

        auto fb = mShadowFramebuffer->bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        auto shader = mShaderShadow->use();
        shader.setUniform("projection", shadowProj);
        shader.setUniform("view", shadowView);
        mScene->render(shader, shadowProj, shadowView, true);
    }


    // Render World in 4 Phases
    {
        auto fb = mFramebuffer->bind();

        glm::mat4 projection = mScene->getCamera()->getProjectionMatrix();
        GLOW_SCOPED(clearColor, glm::vec4(0.5, 0.5, 0.5, 1));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (!mShowPhysicsDebug)
        {


            // Phase 1 : render scene's geometry data into Gbuffer
            // 5 Texture Buffer for each pixel: gPosition, gNormal, gAmbient, gDiffuse, gSpecular
            {
                //GLOW_SCOPED(polygonMode, GL_LINE);
                // GLOW_SCOPED(depthFunc, GL_LEQUAL);
                auto gBuffer = mGBuffer->bind();
                GLOW_SCOPED(clearColor, glm::vec4(0.5,0.5,0.5, 1));
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                GLOW_SCOPED(enable, GL_DEPTH_TEST);
                {           
                    auto shaderSkybox = mShaderSkybox->use();
                    float sunAngle = mScene->getSunAngle();
                    glm::vec3 back1;
                    glm::vec3 back2;
                    if (sunAngle < 3.141592653)
                    {
                        float sunSetRatio = -0.8 * pow(sunAngle - 1.5707963, 2) + 2;
                        sunSetRatio = glm::max(glm::min(sunSetRatio, 1.f), 0.f);
                        back1 = sunSetRatio * glm::vec3(mBackgroundColor1) + (1 - sunSetRatio) * mSunsetBackground1;
                        back2 = sunSetRatio * glm::vec3(mBackgroundColor2) + (1 - sunSetRatio) * mSunsetBackground2;
                    }
                    else
                    {
                        glm::vec3 dark1 = glm::vec3(0, 3 / 255., 8 / 255.) * glm::vec3(9 / 255. , 9 / 255., 9 / 255.);
                        glm::vec3 dark2 = glm::vec3(2 / 255., 7 / 255., 48 / 255.) * glm::vec3(9 / 255. , 9 / 255., 9 / 255.);
                        float sunSetRatio = -0.8 * pow(sunAngle - 4.712388, 2) + 2;
                        sunSetRatio = glm::max(glm::min(sunSetRatio, 1.f), 0.f);
                        back1 = sunSetRatio * glm::vec3(dark1) + (1 - sunSetRatio) * mSunsetBackground1;
                        back2 = sunSetRatio * glm::vec3(dark2) + (1 - sunSetRatio) * mSunsetBackground2;
                    }

                    float angle =  mScene->getSunAngle();
                    if (angle > 3.141592653)
                        angle -= 3.141592653;

                    shaderSkybox.setUniform("uTransform", projection * view); // * glm::mat4(glm::mat3(view)));
                    shaderSkybox.setUniform("uColor1", back1);
                    shaderSkybox.setUniform("uColor2", back2);
                    shaderSkybox.setUniform("sunAngle", angle);
                    shaderSkybox.setUniform("sunColor", mScene->getSunColor());
                    mSkybox->bind().draw();
                }
                

                
                GLOW_SCOPED(enable, GL_CULL_FACE);
                GLOW_SCOPED(cullFace, GL_BACK);

                if (mDebugLine)
                    GLOW_SCOPED(polygonMode, GL_LINE);
                else
                    GLOW_SCOPED(polygonMode, GL_FILL);

                auto shader = mShaderGeometry->use();
                shader.setUniform("projection", projection);
                shader.setUniform("view", view);
                mScene->render(shader, projection, view, false);
                
                //GLOW_SCOPED(polygonMode, GL_FILL);
            }

            // Phase 2 : Generate SSAO texture from gPosition
            {
                auto ssaoBuffer = mSSAO_Buffer->bind();
                auto shader = mShaderSSAO->use();
                GLOW_SCOPED(clearColor, glm::vec4(1));
                glClear(GL_COLOR_BUFFER_BIT);

                GLint id;
                glGetIntegerv(GL_CURRENT_PROGRAM, &id);
                for (unsigned int i = 0; i < SAMPLES; ++i)
                    glUniform3fv(glGetUniformLocation(id, ("samples[" + std::to_string(i) + "]").c_str()), 1, &(ssaoKernel[i])[0]);
                shader.setUniform("projection", projection);

                shader.setTexture("gPosition", mGPosition);
                shader.setTexture("gNormal", mGNormal);
                shader.setTexture("texNoise", mSSAO_Noise);
                if (mSSOA_On)
                    mMeshQuad->bind().draw();
            }

            // Phase 3: Blur the SSOA texture
            {
                auto ssaoBuffer = mSSAO_BufferBlur->bind();
                auto shader = mShaderBlurSSAO->use();
                GLOW_SCOPED(clearColor, glm::vec4(1));
                glClear(GL_COLOR_BUFFER_BIT);

                shader.setTexture("ssaoInput", mSSAO_Color);

                if (mSSOA_On)
                    mMeshQuad->bind().draw();
            }

            // Phase 4: Traditional lighting with SSOA. Uses the texture Information from the phases before.
            {
                GLOW_SCOPED(disable, GL_DEPTH_TEST);

                GLOW_SCOPED(enable, GL_BLEND);
                GLOW_SCOPED(blendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                auto shader = mShaderLighting->use();
                shader.setUniform("shadowTransform", shadowTransform);
                shader.setTexture("shadowMap", mShadowMap);
                shader.setTexture("gPosition", mGPosition);
                shader.setTexture("gNormal", mGNormal);
                shader.setTexture("gAmbient", mGAmbient);
                shader.setTexture("gDiffuse", mGDiffuse);
                shader.setTexture("gSpecular", mGSpecular);
                shader.setTexture("ssao", mSSAO_Color);
                mScene->setLightInShader(shader);
                mMeshQuad->bind().draw();
            }
        }
        else
        {
            float updateRate = 0.0;
            mPhysics->renderDebug(projection, view, updateRate);
        }

        // draw skybox
        /*view = glm::mat4(glm::mat3(view));
        GLOW_SCOPED(depthFunc, GL_LEQUAL);
        auto shaderSkybox = mShaderSkybox->use();
        shaderSkybox.setUniform("uTransform", projection * view);
        shaderSkybox.setUniform("uColor1", mBackgroundColor1);
        shaderSkybox.setUniform("uColor2", mBackgroundColor2);
        mSkybox->bind().draw();*/
    }
    /*GLOW_SCOPED(enable, GL_BLEND);
    GLOW_SCOPED(blendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLOW_SCOPED(enable, GL_CULL_FACE);
    GLOW_SCOPED(cullFace, GL_BACK);

    GLOW_SCOPED(clearColor, mBackgroundColor1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);*/


    GLOW_SCOPED(disable, GL_CULL_FACE);

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
    else
    {
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
        else if (m->getSetting() == GuiSettings::BACKGROUND_COLOR2)
        {
            mBackgroundColor2 = glm::vec4(m->getValue(), 1.0f);
        }
        else if (m->getSetting() == GuiSettings::SUN_COLOR)
        {
            auto shader = mShaderSkybox->use();
            mScene->getSun()->setRenderColor(m->getValue());       
            mScene->setSunColor(m->getValue());
        }
        else if (m->getSetting() == GuiSettings::SUNSET_BACKGROUND1)
        {
            auto shader = mShaderSkybox->use();
            mSunsetBackground1 = m->getValue();
        }
        else if (m->getSetting() == GuiSettings::SUNSET_BACKGROUND2)
        {
            auto shader = mShaderSkybox->use();
            mSunsetBackground2 = m->getValue();
        }
    }
    if (auto m = dynamic_cast<GuiFloatMessage*>(message))
    {
        if (m->getSetting() == GuiSettings::SHADOW_OFFSET)
        {
            auto shader = mShaderLighting->use();
            shader.setUniform("shadowOffset", m->getValue());
        }
        else if (m->getSetting() == GuiSettings::SHADOW_SMOOTHNESS)
        {
            auto shader = mShaderLighting->use();
            shader.setUniform("shadowSmoothness", m->getValue());
        }
        else if (m->getSetting() == GuiSettings::SUN_ANGLE)
        {
            mScene->setSunAngle(m->getValue());        
            //float sunAngle = m->getValue();

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

        if (message.getInput() == GLFW_KEY_F6)
        {
            mSSOA_On = mSSOA_On ? false : true;
        }

        if (message.getInput() == GLFW_KEY_F8)
        {
            mDebugLine = mDebugLine ? false : true;
        }
    }
}

void Game::initSSOA()
{
    // GBuffer Textures and Framebuffer
    mTargets.push_back(mGDepth = glow::Texture2D::create(1, 1, GL_DEPTH_COMPONENT32));
    mTargets.push_back(mGPosition = glow::Texture2D::create(1, 1, GL_RGB16F));
    mGPosition->bind().setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    mGPosition->bind().setFilter(GL_NEAREST, GL_NEAREST);

    mTargets.push_back(mGNormal = glow::Texture2D::create(1, 1, GL_RGB16F));
    mGNormal->bind().setFilter(GL_NEAREST, GL_NEAREST);

    mTargets.push_back(mGAmbient = glow::Texture2D::create(1, 1, GL_RGBA16F));
    mGAmbient->bind().setFilter(GL_NEAREST, GL_NEAREST);

    mTargets.push_back(mGDiffuse = glow::Texture2D::create(1, 1, GL_RGB));
    mGDiffuse->bind().setFilter(GL_NEAREST, GL_NEAREST);

    mTargets.push_back(mGSpecular = glow::Texture2D::create(1, 1, GL_RGBA));
    mGSpecular->bind().setFilter(GL_NEAREST, GL_NEAREST);

    mGBuffer = glow::Framebuffer::createDepthOnly(mGDepth);
    mGBuffer->bind().attachColor("gPosition", mGPosition);
    mGBuffer->bind().attachColor("gNormal", mGNormal);
    mGBuffer->bind().attachColor("gAmbient", mGAmbient);
    mGBuffer->bind().attachColor("gDiffuse", mGDiffuse);
    mGBuffer->bind().attachColor("gSpecular", mGSpecular);

    // SSAO Buffer
    mTargets.push_back(mSSAO_Color = glow::Texture2D::create(1, 1, GL_RED));
    mSSAO_Color->bind().setFilter(GL_NEAREST, GL_NEAREST);
    mSSAO_Buffer = glow::Framebuffer::create("FragColor", mSSAO_Color);

    // SSOA Blur Buffer
    mTargets.push_back(mSSAO_ColorBlur = glow::Texture2D::create(1, 1, GL_RED));
    mSSAO_ColorBlur->bind().setFilter(GL_NEAREST, GL_NEAREST);
    mSSAO_BufferBlur = glow::Framebuffer::create("FragColor", mSSAO_ColorBlur);

    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
    std::default_random_engine generator;

    // SSAO Kernelfunction
    for (unsigned int i = 0; i < SAMPLES; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / SAMPLES;
        scale = 0.1f + scale * scale * (1.0f - 0.1f);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }

    // SSAO Noise for random values
    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }

    mSSAO_Noise = glow::Texture2D::create(4, 4, GL_RGB32F);
    mSSAO_Noise->bind().setData(GL_RGB32F, 4, 4, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    mSSAO_Noise->bind().setWrap(GL_REPEAT, GL_REPEAT);
    mSSAO_Noise->bind().setFilter(GL_NEAREST, GL_NEAREST);

    // SSAO Shader
    mShaderSSAO = glow::Program::createFromFile("../../data/shaders/ssao");
    mShaderBlurSSAO = glow::Program::createFromFile("../../data/shaders/ssao_blur");
}

void Game::initLevel()
{
    auto messageBus = mStartManager->getMessageBus();
    Node* root = new Node;

    // add light
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(30.0f, 25.0f, 25.0f));
    Light* light = new Light(trans, Color{1.0f, 1.0f, 1.0f});
    root->addChild(light);
    mScene->setSun(light);

    // add level
    MeshNode* level = new MeshNode(glm::mat4(1), "../../data/meshes/FirstRoom.obj", mPhysics, GROUP_STATIC_OBJECTS, GROUP_DYNAMIC_OBJECTS, 0.0f);
    root->addChild(level);

    // add screens & cables
    for (int i = 0; i < 3; i++)
    {
        const glm::vec3 position(1.34631f + i * 1.525f, 0.0f, -18.5735f);
        const glm::mat4 transform = glm::translate(glm::mat4(1), position);
        root->addChild(new Screen(i, transform, mPhysics, messageBus));
        root->addChild(new StaticCable(i, mPhysics, messageBus));
    }
    root->addChild(new DynamicCable(1, mPhysics, messageBus));
    MeshNode* activatedCable = new MeshNode(
        glm::mat4(), "../../data/meshes/ActivatedCable.obj",
        mPhysics, GROUP_NONE, GROUP_NONE, 0.0f
    );
    activatedCable->setColor(red);
    root->addChild(activatedCable);

    // add left bridge
    glm::mat4 bridgeTransform = glm::translate(glm::mat4(1), glm::vec3(-26.0f, -0.75f, -16.0f));
    FloatingBridge* bridge = new FloatingBridge(bridgeTransform, mPhysics);
    bridge->setColor(red);
    root->addChild(bridge);

    // add right bridge
    bridgeTransform = glm::translate(glm::mat4(1), glm::vec3(29.0f, -0.75f, -17.6296f));
    bridge = new FloatingBridge(bridgeTransform, mPhysics);
    bridge->setColor(red);
    root->addChild(bridge);

    // add gun
    mMagnetGun = new MagnetGun(glm::vec3(2.5, 1.5, 1.5), mPhysics, messageBus, mScene->getCamera());
    root->addChild(mMagnetGun);

    // add obstacle
    glm::mat4 obstacleTransform = glm::translate(glm::mat4(1), glm::vec3(-1.5f, 1.5f, -40.0f));
    glm::vec3 obstacleScale(8.5f, 1.5f, 1.0f);
    BoxNode* obstacle = new BoxNode(obstacleTransform, obstacleScale, mPhysics, GROUP_DYNAMIC_OBJECTS, GROUP_STATIC_OBJECTS | GROUP_DYNAMIC_OBJECTS, 1.0f);
    obstacle->setColor(red);
    obstacle->getRigidBody()->setLinearFactor(btVector3(1, 0, 0));
    obstacle->getRigidBody()->setAngularFactor(btVector3(0, 0, 0));
    obstacle->getRigidBody()->setFriction(0.5f);
    root->addChild(obstacle);

    // add trees
    auto treeParams = {
        // position, rotation z
        std::make_pair(glm::vec3(- 4.35f, 0.27f, - 6.20f), -45.0f), // main island (left)
        std::make_pair(glm::vec3(- 4.35f, 0.27f, - 9.50f), -45.0f), // main island (left)
        std::make_pair(glm::vec3( 11.00f, 0.27f, -12.18f),   0.0f), // main island (right)
        std::make_pair(glm::vec3(-44.35f, 0.27f, -11.20f), -45.0f), // left island
        std::make_pair(glm::vec3(-44.35f, 0.27f, -14.50f), -45.0f)  // left island
    };
    for (const auto& param : treeParams)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1), param.first);
        transform = glm::rotate(transform, glm::radians(param.second), glm::vec3(0, 1, 0));
        MeshNode* base = new MeshNode(
            transform, "../../data/meshes/TreeBase.obj",
            mPhysics, GROUP_STATIC_OBJECTS, GROUP_DYNAMIC_OBJECTS, 0.0f
        );
        base->setColor(brown);
        root->addChild(base);
        MeshNode* top = new MeshNode(
            transform, "../../data/meshes/TreeTop.obj",
            mPhysics, GROUP_STATIC_OBJECTS, GROUP_DYNAMIC_OBJECTS, 0.0f
        );
        top->setColor(green);
        root->addChild(top);
    }

    // add solar panel
    glm::mat4 panelTransform = glm::translate(glm::mat4(1), glm::vec3(1.75, 4, -40.25));
    panelTransform = glm::rotate(panelTransform, glm::radians(45.0f), glm::vec3(0, 1, 0));
    root->addChild(new SolarPanel(panelTransform, glm::pi<float>(), mPhysics, messageBus));

    // add wind turbines
    auto turbineParams = {
        // position, rotation z, initial blade angle
        std::make_tuple(glm::vec3(-36.5f, 0.0f, 6.5f), 30.0f, 0.0f),
        std::make_tuple(glm::vec3(-44.0f, 0.0f, -1.0f), 30.0f, 22.5f)
    };
    for (const auto& param : turbineParams)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1), std::get<0>(param));
        transform = glm::rotate(transform, glm::radians(std::get<1>(param)), glm::vec3(0, 1, 0));
        WindTurbine* turbine = new WindTurbine(transform, glm::radians(std::get<2>(param)), mPhysics, mScene);
        //mScene->addPointLight(turbine->getLight());
        root->addChild(turbine);
    }

    // add electric box
    root->addChild(new MeshNode(
        glm::translate(glm::mat4(1), glm::vec3(-36.5f, 0.0f, 0.0f)),
        "../../data/meshes/ElectricBox.obj",
        mPhysics, GROUP_STATIC_OBJECTS, GROUP_DYNAMIC_OBJECTS, 0.0f
    ));

    // add clock tower
    root->addChild(new MeshNode(
        glm::translate(glm::mat4(1), glm::vec3(-40.0f, 0.0f, -33.0f)),
        "../../data/meshes/ClockTower.ply",
        mPhysics, GROUP_STATIC_OBJECTS, GROUP_DYNAMIC_OBJECTS, 0.0f
    ));
    BoxNode* box = new BoxNode(
        glm::translate(glm::mat4(1), glm::vec3(-40.0f, 2.5f, -33.0f)),
        glm::vec3(0.15f, 0.55f, 0.15f),
        mPhysics, GROUP_STATIC_OBJECTS, GROUP_DYNAMIC_OBJECTS, 0.0f
    );
    box->setColor(red);
    root->addChild(box);

    // add dispenser
    glm::mat4 dispenserTransform = glm::mat4(1);
    dispenserTransform = glm::translate(dispenserTransform, glm::vec3(12, 0, 5));
    dispenserTransform = glm::rotate(dispenserTransform, glm::pi<float>(), glm::vec3(0, 1, 0));
    root->addChild(new Dispenser(dispenserTransform, mPhysics));

    root->createBuffer();
    mScene->setSceneRoot(root);

    // add a bunch of cubes for testing
    RigidBodyInfo info;
    info.mass = 5.0;
    info.friction = 0.5;
    std::vector<Color> colors;
    colors.push_back({1, 1, 0});
    colors.push_back({0, 1, 0});
    colors.push_back({0, 0, 1});
    colors.push_back({0, 1, 1});

    for (int i = 0; i < 4; i++)
    {
        glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(1, i * 5 + 5, -7));
        LightCube* cubeLight = new LightCube(trans, colors[i], mStartManager->getPhysicsManager(), mScene);
        mScene->appendNode(cubeLight);
        cubeLight->createBuffer();
        cubeLight->addPhysics(glm::vec3(1.0f), info);
    }

    // add stairs
    glm::mat4 stairsTransform = glm::translate(glm::mat4(1), glm::vec3(-3.45f, 2.866667f, -34.6f));
    Stairs* stairs = new Stairs(stairsTransform, mPhysics, messageBus);
    root->addChild(stairs);


    glm::mat4 lightCubeTransform = glm::translate(glm::mat4(1), glm::vec3(0, 5, -10));
    LightCube* cubeLight = new LightCube(lightCubeTransform, {1, 0, 0}, mPhysics, mScene);
    cubeLight->addPhysics(glm::vec3(1.0f), info);
    root->addChild(cubeLight);

}
