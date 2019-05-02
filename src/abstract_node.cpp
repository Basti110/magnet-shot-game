#include "abstract_node.h"
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>

AbstractNode::AbstractNode() : mParent(nullptr)
{
    glm::mat4 trans = glm::mat4(1.0f);
    mLocalTransformation = glm::translate(trans, glm::vec3(0.0f, 0.0f, 0.0f));
    mGlobalTransformation = glm::translate(trans, glm::vec3(0.0f, 0.0f, 0.0f));
}


AbstractNode::~AbstractNode() {}

void AbstractNode::addChild(AbstractNode* child)
{
    mChilds.push_back(child);
    child->isAddedTo(this);
}

void AbstractNode::removeChild(AbstractNode* child)
{
    mChilds.push_back(child);
    child->isRemoved();
}

void AbstractNode::isAddedTo(AbstractNode* parent)
{
    mParent = parent;
    updateGlobalTransformation();
}

void AbstractNode::isRemoved()
{
    mParent = nullptr;
    mGlobalTransformation = mLocalTransformation;
}

AbstractNode* AbstractNode::getChild(unsigned int index)
{
    if (mChilds.size() >= index)
        return nullptr;
    return mChilds[index];
}

glm::vec3 AbstractNode::getPos()
{
    glm::vec4 pos = glm::column(mGlobalTransformation, 3);
    return glm::vec3(pos);
}

void AbstractNode::setLocalTransformation(glm::mat4 matrix)
{
    mLocalTransformation = matrix;
    updateGlobalTransformation();
}

void AbstractNode::updateGlobalTransformation()
{
    if (mParent)
    {
        mGlobalTransformation = mParent->getGlobalTransformation() * mLocalTransformation;
    }
    else
    {
        mGlobalTransformation = mLocalTransformation;
    }
    for (std::vector<AbstractNode*>::iterator it = mChilds.begin(); it != mChilds.end(); ++it)
    {
        (*it)->updateGlobalTransformation();
    }
}

const glm::mat4& AbstractNode::getLocalTransformation()
{
    return mLocalTransformation;
}

const glm::mat4& AbstractNode::getGlobalTransformation()
{
    return mGlobalTransformation;
}

void AbstractNode::render(const glow::UsedProgram& shader, glm::mat4& projection, glm::mat4& view)
{
    for (std::vector<AbstractNode*>::iterator it = mChilds.begin(); it != mChilds.end(); ++it)
    {
        (*it)->render(shader, projection, view);
    }
}

void AbstractNode::update(float elapsedSeconds) 
{
    for (std::vector<AbstractNode*>::iterator it = mChilds.begin(); it != mChilds.end(); ++it)
    {
        (*it)->update(elapsedSeconds);
    }
}

void AbstractNode::setGlobalTransformation(glm::mat4 matrix) 
{
    glm::mat4 local;
    if (!mParent)
        local = matrix;
    else
        local = glm::inverse(mParent->getGlobalTransformation()) * matrix;
    setLocalTransformation(local);
}

void AbstractNode::createBuffer()
{
    for (std::vector<AbstractNode*>::iterator it = mChilds.begin(); it != mChilds.end(); ++it)
    {
        (*it)->createBuffer();
    }
}

void AbstractNode::deleteBuffer()
{
    for (std::vector<AbstractNode*>::iterator it = mChilds.begin(); it != mChilds.end(); ++it)
    {
        (*it)->deleteBuffer();
    }
}
