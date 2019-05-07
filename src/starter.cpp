#include "starter.h"
#include "gui_manager.h"
#include "io_manager.h"
#include "editor.h"
#include "interaction_controller.h"
#include "gameplay_controller.h"
#include "message_bus.h"
#include "scnene_manager.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>


//initializes all required classes
Starter::Starter(GLFWwindow* window)
{
    mBusManager = new MessageBus();
    mScene = new SceneManager();
    mPhysics = new PhysicsManager();
    mInteractionController = new InteractionController(mBusManager, mScene, window, mPhysics);
    mGameplayController = new GameplayController(mBusManager, mScene, mPhysics);
    mEditor = new Editor(mBusManager, mScene, mPhysics);
    mIoManager = new IOManager(window, mBusManager);
    mGuiManager = new GuiManager(mBusManager, window, mScene);
    std::cout << "Startup finished\n";
}

Starter::~Starter() {}

IOManager* Starter::getIOManager()
{
    return mIoManager;
}

MessageBus* Starter::getMessageBus()
{
    return mBusManager;
}

GuiManager* Starter::getGuiManager()
{
    return mGuiManager;
}

SceneManager* Starter::getScene()
{
    return mScene;
}

PhysicsManager* Starter::getPhysicsManager()
{
    return mPhysics;
}
