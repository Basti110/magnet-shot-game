#include "starter.h"
#include "gui_manager.h"
#include "io_manager.h"
#include "editor.h"
#include "interaction_controller.h"
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
    mInteractionController = new InteractionController(mBusManager, mScene, mPhysics, window);
    mIoManager = new IOManager(window, mBusManager);
//    mEditor = new Editor(mBusManager, mScene, mPhysics);
    mGuiManager = new GuiManager(mBusManager, window);
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
