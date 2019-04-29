#pragma once
class InteractionController;
class GameplayController;
class GuiManager;
// class RenderSystem;
class IOManager;
class SceneManager;
class MessageBus;
class Editor;
class PhysicsManager;
struct GLFWwindow;

class Starter
{
public:
    Starter(GLFWwindow* window);
    ~Starter();
    // RenderSystem* getRenderSystem();
    IOManager* getIOManager();
    MessageBus* getMessageBus();
    GuiManager* getGuiManager();
    SceneManager* getScene();
    PhysicsManager* getPhysicsManager();

private:
    // RenderSystem* mRenderSystem;
    InteractionController* mInteractionController;
    GameplayController* mGameplayController;
    GuiManager* mGuiManager;
    SceneManager* mScene;
    IOManager* mIoManager;
    MessageBus* mBusManager;
    Editor* mEditor;
    PhysicsManager* mPhysics;
};
