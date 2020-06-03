
#pragma once

#include <vector>
#include "nodes.h"

namespace bt
{

class Composite : public Node
{
public:
    virtual void initialize() override
    {
        currentChild = children.begin();
    }

    void addChild(Node* child)
    {
        if (child) children.push_back(child);
    }

    virtual void traverse(class BehaviorTreeVisitor& visitor) const override;
    void traverseChildren(class BehaviorTreeVisitor& visitor) const;
    virtual ~Composite() override;
protected:
    std::vector<Node*> children = std::vector<Node*>();
    std::vector<Node*>::iterator currentChild;
};


class Sequence : public Composite
{
public:
    virtual const char* name() const override { return "Sequence"; }
protected:
    virtual Status update() override;
};


class Selector : public Composite
{
public:
    virtual const char* name() const override { return "Selector"; }
protected:
    virtual Status update() override;
};

}
