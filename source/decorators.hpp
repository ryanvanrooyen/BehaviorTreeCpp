
#ifndef BEHAVIOR_TREE_DECORATORS_H
#define BEHAVIOR_TREE_DECORATORS_H

#include "nodes.hpp"

namespace bt
{

class Decorator : public Node, public Observer
{
public:
    void setChild(Node* child) { childNode = child; }
    Node* child() const { return childNode; }
    virtual void traverse(class Visitor& visitor) const override;
    virtual ~Decorator() override;
protected:
    virtual void start(class Scheduler& scheduler) noexcept override;
    virtual Status update() noexcept override { return Status::Suspended; }
    virtual void stop(class Scheduler& scheduler) noexcept override;
private:
    Node* childNode = nullptr;
};


class Negate : public Decorator
{
public:
    virtual const char* name() const noexcept override { return "Not"; }
protected:
    virtual void onComplete(class Scheduler& scheduler, const Node& child, Status status) noexcept override;
};

}

#endif
