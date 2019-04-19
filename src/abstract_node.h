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
    ~AbstractNode();
    void addChild(AbstractNode* child);
    void removeChild(AbstractNode* child);
    void isAddedTo(AbstractNode* parent);
    void isRemoved();
    void updateGlobalTransformation();
    AbstractNode* getChild(unsigned int index);
    glm::vec3 getPos();

    void setLocalTransformation(glm::mat4 matrix);
    const glm::mat4& getLocalTransformation();
    const glm::mat4& getGlobalTransformation();

    virtual void createBuffer();
    virtual void deleteBuffer();
    virtual void render(const glow::UsedProgram& shader, glm::mat4& projection, glm::mat4& view);

protected:
    vector<AbstractNode*> mChilds;
    AbstractNode* mParent;
    glm::mat4 mGlobalTransformation;
    glm::mat4 mLocalTransformation;
};
