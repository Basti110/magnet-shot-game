#pragma once
#include "abstract_node.h"

class Node : public AbstractNode
{
public:
    Node();
    ~Node();
    void createBuffer() { AbstractNode::createBuffer(); }
    void deleteBuffer() { AbstractNode::deleteBuffer(); }
    void render(const glow::UsedProgram& shader, glm::mat4& projection, glm::mat4& view) { AbstractNode::render(shader, projection, view); }
};
