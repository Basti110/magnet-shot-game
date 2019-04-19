#pragma once
#include "scnene_manager.h"
#include "message_bus.h"
#include "cube.h"

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

