#pragma once
class InteractionController;
class GameplayController;
class CinematicController;
class GuiManager;
// class RenderSystem;
class IOManager;
class SceneManager;
class LocationEventManager;
class PuzzleBoxWatcher;
class MessageBus;
class Editor;
class PhysicsManager;
struct GLFWwindow;

class Starter
{
public:
    Starter(GLFWwindow* window);
    ~Starter();
    IOManager* getIOManager();
    MessageBus* getMessageBus();
    GuiManager* getGuiManager();
    SceneManager* getScene();
    PhysicsManager* getPhysicsManager();
    LocationEventManager* getLocationEventManager();
    PuzzleBoxWatcher* getPuzzleBoxWatcher();
    CinematicController* getCinematicController();

private:
    InteractionController* mInteractionController;
    GameplayController* mGameplayController;
    CinematicController* mCinematicController;
    GuiManager* mGuiManager;
    SceneManager* mScene;
    IOManager* mIoManager;
    MessageBus* mBusManager;
    Editor* mEditor;
    PhysicsManager* mPhysics;
    LocationEventManager* mLocationEventManager;
    PuzzleBoxWatcher* mPuzzleBoxWatcher;
};
