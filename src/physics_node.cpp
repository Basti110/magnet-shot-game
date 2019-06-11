#include "physics_node.h"
#include "bullet_helper.hh"
#include "coordinate_axes.h"
#include "physics_manager.h"


PhysicsNode::PhysicsNode(PhysicsManager* physics) : 
    mPhysics(physics),
    mWorld(physics->getDynamicsWorld()), 
    mRigidBody(nullptr),
    mUseVertexColors(false),
    mUseTexture(false),
    mIsVisible(true),
    mDisableShadows(false),
    mAlpha(1.0f)
{
    mMessageBus = MessageBus::getInstance();
    mMessageBus->addPickBodyReceiver([=](PickBodyMessage message) { this->notifyPickBody(message); });
    mMessageBus->addGuiReceiver([=](Message* message) { this->notifyGuiInput(message); });

	mProperty.nodeID = this->getNodeId();
    mProperty.ambient = 0.2f * glm::vec3(1);
    mProperty.diffuse = glm::vec3(1);
    mProperty.specular = glm::vec3(1);
    mProperty.shininess = 32;
}

PhysicsNode::~PhysicsNode()
{
    for (auto c : mConstraints)
    {
        mWorld->removeConstraint(c);
    }
    if (mRigidBody)
    {
        mWorld->removeRigidBody(mRigidBody);
        delete mRigidBody->getMotionState();
        delete mRigidBody->getCollisionShape();
        delete mRigidBody;
    }
}

void PhysicsNode::update(float elapsedSeconds)
{
    if (mRigidBody)
    {
        btTransform transform;
        mRigidBody->getMotionState()->getWorldTransform(transform);
        mGlobalTransformation = to_glm(transform);
    }

    if (mIsPicked && mCoordinateAxes && mRigidBody)
    {
        mCoordinateAxes->update(elapsedSeconds);
        glm::vec3 pos = glm::vec3(mCoordinateAxes->getGlobalTransformation()[3]);
        glm::mat4 trans = getGlobalTransformation();
        trans[3].x = pos.x;
        trans[3].y = pos.y;
        trans[3].z = pos.z;
        setGlobalTransformation(trans);
        mRigidBody->setWorldTransform(to_bullet(trans));
        mRigidBody->getMotionState()->setWorldTransform(to_bullet(trans));
     }
        
    AbstractNode::update(elapsedSeconds);
}

void PhysicsNode::render(glow::UsedProgram& shader, glm::mat4& projection, glm::mat4& view, bool shadowPass)
{
    if (!mIsVisible) return;
    if (shadowPass && mDisableShadows) return;

    //glm::vec3 color = false ? glm::vec3(1, 0, 0) : mColor;
    shader.setUniform("material.ambient", mProperty.ambient);
    shader.setUniform("material.diffuse", mProperty.diffuse);
    shader.setUniform("material.specular", mProperty.specular); // specular lighting doesn't have full effect on this object's material
    shader.setUniform("material.shininess", mProperty.shininess);
    shader.setUniform("model", mGlobalTransformation);
    shader.setUniform("colorRatio", 1.0f);
    //shader.setUniform("color", color);
    shader.setUniform("uAlpha", mAlpha);
    shader.setUniform("uUseVertexColors", mUseVertexColors);
    shader.setUniform("uUseTexture", mUseTexture);
    mVertexArray->bind().draw();
    AbstractNode::render(shader, projection, view, shadowPass);
    if (mCoordinateAxes && mIsPicked)
    {
        mCoordinateAxes->render(shader, projection, view, shadowPass);
    }
}

void PhysicsNode::setColor(const glm::vec3& color) 
{
    mProperty.nodeID = this->getNodeId();
    mProperty.ambient = 0.2f * color;
    mProperty.diffuse = color;
    mProperty.specular = color;
}

void PhysicsNode::notifyPickBody(PickBodyMessage message)
{
    if (message.body == nullptr)
    {
        mIsPicked = false;
        return;
    }



    if (mIsPicked && mCoordinateAxes != nullptr)
    {
        //bool hit = mCoordinateAxes->hitArrow(message.body);
        if (mCoordinateAxes->hitArrow(message.body))
            return;
    }
    mIsPicked = message.body == mRigidBody ? true : false;
    if (mIsPicked)
    {

        GuiPropertyMessage m(true, mProperty);
        mMessageBus->sendMessage(&m);
        if (mCoordinateAxes == nullptr)
            mCoordinateAxes = new CoordinateAxes(glm::vec3(mGlobalTransformation[3]), mPhysics);
    }
}

void PhysicsNode::notifyGuiInput(Message* message) 
{
    if (message->getType() == MType::BODY_PROPERTIES_IN)
    {
        GuiPropertyMessage* m = dynamic_cast<GuiPropertyMessage*>(message);
        if (m == nullptr)
            return;

		if (m->mProperty.nodeID == getNodeId())
		{
            mProperty = m->mProperty;
		}
    }
}

void PhysicsNode::setShininess(float shininess)
{
    mProperty.shininess = shininess;
}
