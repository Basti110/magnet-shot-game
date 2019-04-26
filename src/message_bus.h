#pragma once
#include <iostream>
#include <functional>
#include <queue>
#include <vector>
#include <glm/glm.hpp>

enum MType {
    M_KEY,
    M_MOUSE_CLICK,
    M_MOUSE_MOVE,
    GUI_FLOAT,
    GUI_VEC3,
    SCENE,
    RENDERSYSTEM
};

enum GuiSettings {
    BACKGROUND_COLOR,
    LIGHT_COLOR,
    CUBE_COLOR,
    CUBE_SIZE,
    CUBE_SHAPE,
    CUBE_MASS,
    CUBE_FRICTION,
    CUBE_RESTITUTION,
    CUBE_LINEAR_DAMPING,
    CUBE_ANGULAR_DAMPING,
};

class Message
{
    public:


    public:
        Message() {}
        virtual ~Message();

        MType getType() {
            return type;
        }
    protected:
        enum MType type;
};

class KeyMessage: public Message
{

public:
    KeyMessage(int input, int action, float speed)
    {
        this->type = MType::M_KEY;
        this->input = input;
        this->action = action;
        this->speed = speed;
    }

    int getInput()
    {
        return input;
    }

    int getAction()
    {
        return action;
    }

    float getSpeed()
    {
        return speed;
    }
private:
    int input;
    int action;
    float speed;
};

class MouseClickMessage: public Message
{

public:
    MouseClickMessage(float x, float y, int input, int action)
	{
		this->type = MType::M_MOUSE_CLICK;
		this->input = input;
		this->action = action;
        this->position = { x, y };
	}

	int getInput()
	{
		return input;
	}

	int getAction()
	{
		return action;
	}

    glm::vec2 getPostion()
    {
        return position;
    }
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
        this->position = { x, y };
        this->deltaPosition = { dX, dY };
    }

    glm::vec2 getPosition()
    {
        return position;
    }

    glm::vec2 getDeltaPosition()
    {
        return deltaPosition;
    }
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

    float getValue()
    {
        return value;
    }

    GuiSettings getSetting()
    {
        return setting;
    }
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

    glm::vec3 getValue()
    {
        return value;
    }

    GuiSettings getSetting()
    {
        return setting;
    }
private:
    glm::vec3 value;
    GuiSettings setting;
};

class MessageBus
{
    public:
        MessageBus();
        ~MessageBus();
        void addKeyReceiver(std::function<void(KeyMessage)> messageReceiver)
        {
            this->keyReceiver.push_back(messageReceiver);
        }

		void addMouseClickReceiver(std::function<void(MouseClickMessage)> messageReceiver)
		{
			this->mouseClickReceiver.push_back(messageReceiver);
		}

        void addMouseMoveReceiver(std::function<void(MouseMoveMessage)> messageReceiver)
        {
            this->mouseMoveReceiver.push_back(messageReceiver);
        }

        void addGuiReceiver(std::function<void(Message*)> messageReceiver)
        {
            this->guiReceiver.push_back(messageReceiver);
        }

        void sendMessage(Message* message)
        {
            //Store KeyMessages
            if (message->getType() == MType::M_KEY) {
                KeyMessage* keyMessage = dynamic_cast<KeyMessage*>(message);
                if (!keyMessage)
                    return; //TODO: Error
                keyMessage = new KeyMessage(*keyMessage);
                this->keyMessages.push(keyMessage);
            }
			else if (message->getType() == MType::M_MOUSE_CLICK) {
                MouseClickMessage* mouseMessage = dynamic_cast<MouseClickMessage*>(message);
				if (!mouseMessage)
					return; //TODO: Error
				mouseMessage = new MouseClickMessage(*mouseMessage);
				this->mouseClickMessages.push(mouseMessage);
			}
            else if (message->getType() == MType::M_MOUSE_MOVE) {
                MouseMoveMessage* mouseMessage = dynamic_cast<MouseMoveMessage*>(message);
                if (!mouseMessage)
                    return; //TODO: Error
                mouseMessage = new MouseMoveMessage(*mouseMessage);
                this->mouseMoveMessages.push(mouseMessage);
            }
            else if (message->getType() == MType::GUI_FLOAT) {
                GuiFloatMessage* m = dynamic_cast<GuiFloatMessage*>(message);
                if (!m)
                    return; //TODO: Error
                m = new GuiFloatMessage(*m);
                this->guiMessages.push(m);
            }
            else if (message->getType() == MType::GUI_VEC3) {
                GuiVec3Message* m = dynamic_cast<GuiVec3Message*>(message);
                if (!m)
                    return; //TODO: Error
                m = new GuiVec3Message(*m);
                this->guiMessages.push(m);
            }
        }

        void notify()
        {
            //Send KeyMessages
            while (!keyMessages.empty()) {
                for (auto iter = keyReceiver.begin(); iter != keyReceiver.end(); iter++) {
                    (*iter)(KeyMessage(*keyMessages.front()));
                }
                KeyMessage* m = keyMessages.front();
                keyMessages.pop();
                delete m;
            }

			while (!mouseClickMessages.empty()) {
				for (auto iter = mouseClickReceiver.begin(); iter != mouseClickReceiver.end(); iter++) {
					(*iter)(MouseClickMessage(*mouseClickMessages.front()));
				}
                MouseClickMessage* m = mouseClickMessages.front();
                mouseClickMessages.pop();
				delete m;
			}

            while (!mouseMoveMessages.empty()) {
                for (auto iter = mouseMoveReceiver.begin(); iter != mouseMoveReceiver.end(); iter++) {
                    (*iter)(MouseMoveMessage(*mouseMoveMessages.front()));
                }
                MouseMoveMessage* m = mouseMoveMessages.front();
                mouseMoveMessages.pop();
                delete m;
            }

            while (!guiMessages.empty()) {
                for (auto iter = guiReceiver.begin(); iter != guiReceiver.end(); iter++) {
                    (*iter)(guiMessages.front());
                }
                Message* m = guiMessages.front();
                guiMessages.pop();
                delete m;
            }
        }

        private:
            std::vector<std::function<void(KeyMessage)>> keyReceiver;
			std::vector<std::function<void(MouseClickMessage)>> mouseClickReceiver;
            std::vector<std::function<void(MouseMoveMessage)>> mouseMoveReceiver; 
            std::vector<std::function<void(Message*)>> guiReceiver;
            std::queue<KeyMessage*> keyMessages;
			std::queue<MouseClickMessage*> mouseClickMessages;
            std::queue<MouseMoveMessage*> mouseMoveMessages;
            std::queue<Message*> guiMessages;

};      

