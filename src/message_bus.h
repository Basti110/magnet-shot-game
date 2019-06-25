#pragma once
#include <functional>
#include <glm/glm.hpp>
#include <iostream>
#include <queue>
#include <vector>
#include "singelton.h"

class btRigidBody;

enum MType
{
    M_GAME_MODE,
    M_KEY,
    M_MOUSE_CLICK,
    M_MOUSE_MOVE,
    M_LOCATION_EVENT,
    M_ACTIVATE_SCREEN,
    GUI_FLOAT,
    GUI_VEC3,
    BODY_PROPERTIES_OUT,
	BODY_PROPERTIES_IN,
    SCENE_EVENT,
    RENDERSYSTEM,
    PICK_BODY,
};

enum GuiSettings
{
    BACKGROUND_COLOR1,
    BACKGROUND_COLOR2,
    LIGHT_AMBIENT,
    LIGHT_DIFFUSE,
    LIGHT_SPECULAR,
    SUN_ANGLE, 
    SHADOW_OFFSET,
    SHADOW_SMOOTHNESS,
    CUBE_COLOR,
    CUBE_SIZE,
    CUBE_SHAPE,
    CUBE_MASS,
    CUBE_FRICTION,
    CUBE_RESTITUTION,
    CUBE_LINEAR_DAMPING,
    CUBE_ANGULAR_DAMPING,
    SAVE,
    SUN_COLOR,
    SUNSET_BACKGROUND1,
    SUNSET_BACKGROUND2
};

enum class GameMode
{
    Menu,
    Gameplay,
    Editor,
    Editor2
};

enum class LocationEventId
{
    GameOver,
    MagnetGunPickUp,
    ActivateStairs,
    DeactivateStairs,
    Island2
};

enum class SceneEventId
{
    Night,
    Day
};

enum class LocationEventType
{
    Enter,
    Exit
};

class BodyProperties
{
public:
    int nodeID;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;

	bool compare(const BodyProperties& v) 
	{
        return (ambient == v.ambient) && diffuse == v.diffuse && specular == v.specular && shininess == v.shininess && nodeID == v.nodeID;
	}
};

class Message
{
public:
    Message() {}
    virtual ~Message();

    MType getType() { return type; }

protected:
    enum MType type;
};

class GameModeMessage : public Message
{
public:
    GameModeMessage(GameMode m) : mode(m) { this->type = M_GAME_MODE; }
    const GameMode mode;
};

class PickBodyMessage : public Message
{
public:
    PickBodyMessage(btRigidBody* m) : body(m) { this->type = PICK_BODY; }
    const btRigidBody* body;
};

class KeyMessage : public Message
{
public:
    KeyMessage(int input, int action, float speed)
    {
        this->type = MType::M_KEY;
        this->input = input;
        this->action = action;
        this->speed = speed;
    }

    int getInput() { return input; }

    int getAction() { return action; }

    float getSpeed() { return speed; }

private:
    int input;
    int action;
    float speed;
};

class MouseClickMessage : public Message
{
public:
    MouseClickMessage(float x, float y, int input, int action)
    {
        this->type = MType::M_MOUSE_CLICK;
        this->input = input;
        this->action = action;
        this->position = {x, y};
    }

    int getInput() { return input; }

    int getAction() { return action; }

    glm::vec2 getPostion() { return position; }

private:
    glm::vec2 position;
    int input;
    int action;
};

class MouseMoveMessage : public Message
{
public:
    MouseMoveMessage(float x, float y, float dX, float dY)
    {
        this->type = MType::M_MOUSE_MOVE;
        this->position = {x, y};
        this->deltaPosition = {dX, dY};
    }

    glm::vec2 getPosition() { return position; }

    glm::vec2 getDeltaPosition() { return deltaPosition; }

private:
    glm::vec2 position;
    glm::vec2 deltaPosition;
};

class GuiFloatMessage : public Message
{
public:
    GuiFloatMessage(float value, GuiSettings setting)
    {
        this->type = MType::GUI_FLOAT;
        this->setting = setting;
        this->value = value;
    }

    float getValue() { return value; }

    GuiSettings getSetting() { return setting; }

private:
    float value;
    GuiSettings setting;
};

class GuiVec3Message : public Message
{
public:
    GuiVec3Message(glm::vec3 value, GuiSettings setting)
    {
        this->type = MType::GUI_VEC3;
        this->setting = setting;
        this->value = value;
    }

    glm::vec3 getValue() { return value; }

    GuiSettings getSetting() { return setting; }

private:
    glm::vec3 value;
    GuiSettings setting;
};

class LocationEventMessage : public Message
{
public:
    LocationEventMessage(LocationEventId eventId, LocationEventType eventType) : eventId(eventId), eventType(eventType)
    {
        this->type = M_LOCATION_EVENT;
    }
    const LocationEventId eventId;
    const LocationEventType eventType;
};

class SceneEventMessage : public Message
{
public:
    SceneEventMessage(SceneEventId eventId) : eventId(eventId)
    {
        this->type = SCENE_EVENT;
    }
    const SceneEventId eventId;
};

class ActivateScreenMessage : public Message
{
public:
    ActivateScreenMessage(int screenId) : screenId(screenId) { this->type = M_ACTIVATE_SCREEN; }
    const int screenId;
};

class GuiPropertyMessage : public Message
{
public:
    GuiPropertyMessage(bool out, BodyProperties& p) : mProperty(p) { this->type = out ? BODY_PROPERTIES_OUT : BODY_PROPERTIES_IN; }
    const BodyProperties mProperty;
};

class MessageBus : public Singleton<MessageBus>
{
public:
    MessageBus();
    ~MessageBus();

    void addGameModeReceiver(std::function<void(GameModeMessage)> messageReceiver) { this->gameModeReceiver.push_back(messageReceiver); }

    void addKeyReceiver(std::function<void(KeyMessage)> messageReceiver) { this->keyReceiver.push_back(messageReceiver); }

    void addMouseClickReceiver(std::function<void(MouseClickMessage)> messageReceiver) { this->mouseClickReceiver.push_back(messageReceiver); }

    void addMouseMoveReceiver(std::function<void(MouseMoveMessage)> messageReceiver) { this->mouseMoveReceiver.push_back(messageReceiver); }

    void addPickBodyReceiver(std::function<void(PickBodyMessage)> messageReceiver) { this->pickBodyReceiver.push_back(messageReceiver); }

    void addGuiReceiver(std::function<void(Message*)> messageReceiver) { this->guiReceiver.push_back(messageReceiver); }

	void addGuiOutReceiver(std::function<void(Message*)> messageReceiver) { this->guiOutReceiver.push_back(messageReceiver); }

    void addLocationEventReceiver(std::function<void(LocationEventMessage)> messageReceiver)
    {
        this->locationEventReceiver.push_back(messageReceiver);
    }

    void addSceneEventReceiver(std::function<void(SceneEventMessage)> messageReceiver)
    {
        this->sceneEventReceiver.push_back(messageReceiver);
    }

    void addActivateScreenReceiver(std::function<void(ActivateScreenMessage)> messageReceiver)
    {
        this->activateScreenReceiver.push_back(messageReceiver);
    }

    void sendMessage(Message* message)
    {
        if (message->getType() == MType::M_GAME_MODE)
        {
            GameModeMessage* gameModeMessage = dynamic_cast<GameModeMessage*>(message);
            if (!gameModeMessage)
                return; // TODO: Error
            gameModeMessage = new GameModeMessage(*gameModeMessage);
            this->gameModeMessages.push(gameModeMessage);
        }
        else if (message->getType() == MType::M_KEY)
        {
            KeyMessage* keyMessage = dynamic_cast<KeyMessage*>(message);
            if (!keyMessage)
                return; // TODO: Error
            keyMessage = new KeyMessage(*keyMessage);
            this->keyMessages.push(keyMessage);
        }
        else if (message->getType() == MType::M_MOUSE_CLICK)
        {
            MouseClickMessage* mouseMessage = dynamic_cast<MouseClickMessage*>(message);
            if (!mouseMessage)
                return; // TODO: Error
            mouseMessage = new MouseClickMessage(*mouseMessage);
            this->mouseClickMessages.push(mouseMessage);
        }
        else if (message->getType() == MType::M_MOUSE_MOVE)
        {
            MouseMoveMessage* mouseMessage = dynamic_cast<MouseMoveMessage*>(message);
            if (!mouseMessage)
                return; // TODO: Error
            mouseMessage = new MouseMoveMessage(*mouseMessage);
            this->mouseMoveMessages.push(mouseMessage);
        }
        else if (message->getType() == MType::GUI_FLOAT)
        {
            GuiFloatMessage* m = dynamic_cast<GuiFloatMessage*>(message);
            if (!m)
                return; // TODO: Error
            m = new GuiFloatMessage(*m);
            this->guiMessages.push(m);
        }
        else if (message->getType() == MType::GUI_VEC3)
        {
            GuiVec3Message* m = dynamic_cast<GuiVec3Message*>(message);
            if (!m)
                return; // TODO: Error
            m = new GuiVec3Message(*m);
            this->guiMessages.push(m);
        }
        else if (message->getType() == MType::BODY_PROPERTIES_OUT)
        {
            GuiPropertyMessage* m = dynamic_cast<GuiPropertyMessage*>(message);
            if (!m)
                return; // TODO: Error
            m = new GuiPropertyMessage(*m);
            this->guiOutMessages.push(m);
        }
        else if (message->getType() == MType::BODY_PROPERTIES_IN)
        {
            GuiPropertyMessage* m = dynamic_cast<GuiPropertyMessage*>(message);
            if (!m)
                return; // TODO: Error
            m = new GuiPropertyMessage(*m);
            this->guiMessages.push(m);
        }
        else if (message->getType() == MType::PICK_BODY)
        {
            PickBodyMessage* mouseMessage = dynamic_cast<PickBodyMessage*>(message);
            if (!mouseMessage)
                return; // TODO: Error
            mouseMessage = new PickBodyMessage(*mouseMessage);
            this->pickBodyMessages.push(mouseMessage);
        }
        else if (message->getType() == MType::M_LOCATION_EVENT)
        {
            LocationEventMessage* locationEventMessage = dynamic_cast<LocationEventMessage*>(message);
            if (!locationEventMessage)
                return; // TODO: Error
            locationEventMessage = new LocationEventMessage(*locationEventMessage);
            this->locationEventMessages.push(locationEventMessage);
        }
        else if (message->getType() == MType::SCENE_EVENT)
        {
            SceneEventMessage* m = dynamic_cast<SceneEventMessage*>(message);
            if (!m)
                return; // TODO: Error
            m = new SceneEventMessage(*m);
            this->sceneEventMessages.push(m);
        }
        else if (message->getType() == MType::M_ACTIVATE_SCREEN)
        {
            ActivateScreenMessage* activateScreenMessage = dynamic_cast<ActivateScreenMessage*>(message);
            if (!activateScreenMessage)
                return; // TODO: Error
            activateScreenMessage = new ActivateScreenMessage(*activateScreenMessage);
            this->activateScreenMessages.push(activateScreenMessage);
        }
    }

    void notify()
    {
        while (!gameModeMessages.empty())
        {
            for (auto iter = gameModeReceiver.begin(); iter != gameModeReceiver.end(); iter++)
            {
                (*iter)(GameModeMessage(*gameModeMessages.front()));
            }
            GameModeMessage* m = gameModeMessages.front();
            gameModeMessages.pop();
            delete m;
        }

        while (!keyMessages.empty())
        {
            for (auto iter = keyReceiver.begin(); iter != keyReceiver.end(); iter++)
            {
                (*iter)(KeyMessage(*keyMessages.front()));
            }
            KeyMessage* m = keyMessages.front();
            keyMessages.pop();
            delete m;
        }

        while (!mouseClickMessages.empty())
        {
            for (auto iter = mouseClickReceiver.begin(); iter != mouseClickReceiver.end(); iter++)
            {
                (*iter)(MouseClickMessage(*mouseClickMessages.front()));
            }
            MouseClickMessage* m = mouseClickMessages.front();
            mouseClickMessages.pop();
            delete m;
        }

        while (!mouseMoveMessages.empty())
        {
            for (auto iter = mouseMoveReceiver.begin(); iter != mouseMoveReceiver.end(); iter++)
            {
                (*iter)(MouseMoveMessage(*mouseMoveMessages.front()));
            }
            MouseMoveMessage* m = mouseMoveMessages.front();
            mouseMoveMessages.pop();
            delete m;
        }

        while (!pickBodyMessages.empty())
        {
            for (auto iter = pickBodyReceiver.begin(); iter != pickBodyReceiver.end(); iter++)
            {
                (*iter)(PickBodyMessage(*pickBodyMessages.front()));
            }
            PickBodyMessage* m = pickBodyMessages.front();
            pickBodyMessages.pop();
            delete m;
        }

        while (!guiMessages.empty())
        {
            for (auto iter = guiReceiver.begin(); iter != guiReceiver.end(); iter++)
            {
                (*iter)(guiMessages.front());
            }
            Message* m = guiMessages.front();
            guiMessages.pop();
            delete m;
        }

		while (!guiOutMessages.empty())
        {
            for (auto iter = guiOutReceiver.begin(); iter != guiOutReceiver.end(); iter++)
            {
                (*iter)(guiOutMessages.front());
            }
            Message* m = guiOutMessages.front();
            guiOutMessages.pop();
            delete m;
        }

        while (!locationEventMessages.empty())
        {
            for (auto iter = locationEventReceiver.begin(); iter != locationEventReceiver.end(); iter++)
            {
                (*iter)(LocationEventMessage(*locationEventMessages.front()));
            }
            LocationEventMessage* m = locationEventMessages.front();
            locationEventMessages.pop();
            delete m;
        }

        while (!sceneEventMessages.empty())
        {
            for (auto iter = sceneEventReceiver.begin(); iter != sceneEventReceiver.end(); iter++)
            {
                (*iter)(SceneEventMessage(*sceneEventMessages.front()));
            }
            SceneEventMessage* m = sceneEventMessages.front();
            sceneEventMessages.pop();
            delete m;
        }

        while (!activateScreenMessages.empty())
        {
            for (auto iter = activateScreenReceiver.begin(); iter != activateScreenReceiver.end(); iter++)
            {
                (*iter)(ActivateScreenMessage(*activateScreenMessages.front()));
            }
            ActivateScreenMessage* m = activateScreenMessages.front();
            activateScreenMessages.pop();
            delete m;
        }
    }

private:
    std::vector<std::function<void(GameModeMessage)>> gameModeReceiver;
    std::vector<std::function<void(KeyMessage)>> keyReceiver;
    std::vector<std::function<void(MouseClickMessage)>> mouseClickReceiver;
    std::vector<std::function<void(MouseMoveMessage)>> mouseMoveReceiver;
    std::vector<std::function<void(Message*)>> guiReceiver;
    std::vector<std::function<void(Message*)>> guiOutReceiver;
    std::vector<std::function<void(PickBodyMessage)>> pickBodyReceiver;
    std::vector<std::function<void(LocationEventMessage)>> locationEventReceiver;
    std::vector<std::function<void(SceneEventMessage)>> sceneEventReceiver;
    std::vector<std::function<void(ActivateScreenMessage)>> activateScreenReceiver;
    std::queue<GameModeMessage*> gameModeMessages;
    std::queue<KeyMessage*> keyMessages;
    std::queue<MouseClickMessage*> mouseClickMessages;
    std::queue<MouseMoveMessage*> mouseMoveMessages;
    std::queue<Message*> guiMessages;
    std::queue<Message*> guiOutMessages;
    std::queue<PickBodyMessage*> pickBodyMessages;
    std::queue<LocationEventMessage*> locationEventMessages;
    std::queue<SceneEventMessage*> sceneEventMessages;
    std::queue<ActivateScreenMessage*> activateScreenMessages;
};
