
#pragma once

#include "nodes.h"
#include <cstdint>

namespace bt
{

class Composite : public Node
{
public:
    Composite(Node** children, uint16_t childCount)
        : children(children), childCount(childCount) {}

    virtual void initialize() override { currentIndex = 0; }
    void addChild(Node* child);
    virtual void traverse(class Visitor& visitor) const override;
    void traverseChildren(class Visitor& visitor) const;
    virtual ~Composite() override;
protected:
    const uint16_t childCount;
    uint16_t currentIndex = 0;
    Node** children;
};


class Sequence : public Composite
{
public:
    using Composite::Composite;
    virtual const char* name() const override { return "Sequence"; }
protected:
    virtual Status update() override;
};


class Selector : public Composite
{
public:
    using Composite::Composite;
    virtual const char* name() const override { return "Selector"; }
protected:
    virtual Status update() override;
};

}
