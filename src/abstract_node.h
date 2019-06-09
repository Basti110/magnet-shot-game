#pragma once
#include <vector>
#include <glow/objects/Program.hh>
//#include <glm/gtc/matrix_transform.hpp>
using std::vector;

struct Color
{
    float red;
    float green;
    float blue;
};

class AbstractNode
{
public:
    AbstractNode();
    virtual ~AbstractNode();
    void addChild(AbstractNode* child);
    void removeChild(AbstractNode* child);
    void isAddedTo(AbstractNode* parent);
    void isRemoved();
    void updateGlobalTransformation();
    int getNodeId();
    AbstractNode* getChild(unsigned int index);
    glm::vec3 getPos();

    const glm::mat4& getLocalTransformation();
    const glm::mat4& getGlobalTransformation();

    virtual void createBuffer();
    virtual void setLocalTransformation(glm::mat4 matrix);
    virtual void deleteBuffer();
    virtual void render(glow::UsedProgram& shader, glm::mat4& projection, glm::mat4& view, bool shadowPass);
    virtual void update(float elapsedSeconds);

protected:
    virtual void setGlobalTransformation(glm::mat4 matrix);
    vector<AbstractNode*> mChilds;
    AbstractNode* mParent;
    glm::mat4 mGlobalTransformation;
    glm::mat4 mLocalTransformation;

private:
    int mNodeId;
    static int mNodeNumber;
};