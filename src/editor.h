#pragma once
#include "cube.h"
#include "message_bus.h"
#include "scnene_manager.h"

class Editor
{
public:
    Editor(MessageBus* mB, SceneManager* scene);
    ~Editor();
    void notifyMouseClickInput(MouseClickMessage message);
    void notifyMouseMoveInput(MouseMoveMessage message);
    void notifyKeyInput(KeyMessage message);
    void notifyGuiInput(Message* message);

private:
    std::function<void(MouseClickMessage)> getNotifyFuncMouseClick();
    std::function<void(MouseMoveMessage)> getNotifyFuncMouseMove();
    std::function<void(KeyMessage)> getNotifyFuncKey();
    std::function<void(Message*)> getNotifyFuncGui();
    void refreshCube();

    SceneManager* mScene;
    Cube* mCube;
    float mBounceClick;
    float mBounceF;
    float mCubeDistance;
    bool mCursorOn;
};
