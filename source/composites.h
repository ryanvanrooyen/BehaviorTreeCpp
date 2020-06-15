
#pragma once

#include "nodes.h"
#include <cstdint>

namespace bt
{

class Composite : public Node, public Observer
{
public:
    Composite(Node** children, uint16_t childCount)
        : children(children), childCount(childCount) {}

    virtual void initialize(class Scheduler& scheduler) override;
    void addChild(Node* child);
    virtual void traverse(class Visitor& visitor) const override;
    void traverseChildren(class Visitor& visitor) const;
    virtual ~Composite() override;
protected:
    virtual Status update() noexcept override { return Status::Suspended; }
    Node** children;
    const uint16_t childCount;
    uint16_t currentIndex = 0;
};

class Sequence : public Composite
{
public:
    using Composite::Composite;
    virtual const char* name() const noexcept override { return "Sequence"; }
    virtual void onComplete(class Scheduler& scheduler, const Node& child, Status status) override;
};

class Selector : public Composite
{
public:
    using Composite::Composite;
    virtual const char* name() const noexcept override { return "Selector"; }
    virtual void onComplete(class Scheduler& scheduler, const Node& child, Status status) override;
};

}
