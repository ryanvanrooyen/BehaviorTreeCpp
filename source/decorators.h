
#pragma once

#include "nodes.h"

namespace bt
{

class Decorator : public Node
{
public:
    void setChild(Node* child) { childNode = child; }
    virtual void traverse(class Visitor& visitor) const override;

    Node* child() const { return childNode; }
    virtual ~Decorator() override;
protected:
    Node* childNode = nullptr;
};


class Negate : public Decorator
{
public:
    virtual const char* name() const override { return "Not"; }

protected:
    virtual Status update() override;
};

}
