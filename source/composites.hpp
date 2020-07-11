
#ifndef BEHAVIOR_TREE_COMPOSITES_H
#define BEHAVIOR_TREE_COMPOSITES_H

#include "nodes.hpp"

namespace bt
{

class Composite : public Node, public Observer
{
public:
    Composite(Node** children, uint16_t childCount)
        : children(children), childCount(childCount) {}

    void addChild(Node* child);
    virtual void traverse(class Visitor& visitor) const override;
    void traverseChildren(class Visitor& visitor) const;
    virtual ~Composite() override;
protected:
    virtual void start(class Scheduler& scheduler) noexcept override;
    virtual Status update() noexcept override { return Status::Suspended; }
    virtual void stop(class Scheduler& scheduler) noexcept override;
    Node** children;
    const uint16_t childCount;
    uint16_t currentIndex = 0;
};


class Sequence : public Composite
{
public:
    using Composite::Composite;
    virtual const char* name() const noexcept override { return "Sequence"; }
protected:
    virtual void onComplete(class Scheduler& scheduler, const Node& child, Status status) noexcept override;
};


class Selector : public Composite
{
public:
    using Composite::Composite;
    virtual const char* name() const noexcept override { return "Selector"; }
protected:
    virtual void onComplete(class Scheduler& scheduler, const Node& child, Status status) noexcept override;
};


class Parallel : public Composite {
public:
    enum class Policy { RequireOne, RequireAll };

    Parallel(Node** children, uint16_t childCount, Policy success, Policy failure)
        : Composite(children, childCount), successPolicy(success), failurePolicy(failure) {}

    virtual const char* name() const noexcept override { return "Parallel"; }
protected:
    virtual void start(class Scheduler& scheduler) noexcept override;
    virtual void onComplete(class Scheduler& scheduler, const Node& child, Status status) noexcept override;
private:
    uint16_t successCount = 0;
    uint16_t failureCount = 0;
    Policy successPolicy;
    Policy failurePolicy;
};

}

#endif
