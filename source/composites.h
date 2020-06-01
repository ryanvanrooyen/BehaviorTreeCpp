
#pragma once

#include <iostream>
#include <iomanip>
#include <vector>
#include <initializer_list>
#include "nodes.h"

namespace bt
{

class Composite : public Node
{
public:
    Composite(const char* name) : Node(name) {}
    Composite(const char* name, std::initializer_list<Node*> children) : Node(name), children(children) {}

    virtual void initialize() override
    {
        currentChild = children.begin();
    }

    void addChild(Node* child)
    {
        if (child) children.push_back(child);
    }
    void removeChild(Node* child)
    {
        if (child) children.push_back(child);
    }
    void clearChildren();

    virtual void traverse(class NodeVisitor& visitor) override;

protected:
    std::vector<Node*> children;
    std::vector<Node*>::iterator currentChild;
};


class Sequence : public Composite
{
public:
    Sequence() : Composite("Sequence") {}
    Sequence(std::initializer_list<Node*> children) : Composite("Sequence", children) {}
protected:
    virtual Status update() override;
};


class Selector : public Composite
{
public:
    Selector() : Composite("Selector") {}
    Selector(std::initializer_list<Node*> children) : Composite("Selector", children) {}
protected:
    virtual Status update() override;
};

}
