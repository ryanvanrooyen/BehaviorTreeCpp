
#pragma once

#include "nodes.h"

namespace bt
{

class Decorator : public Node, public Observer
{
public:
    void setChild(Node* child) { childNode = child; }
    virtual void initialize(class Scheduler& scheduler) override;
    virtual void traverse(class Visitor& visitor) const override;
    Node* child() const { return childNode; }
    virtual ~Decorator() override;
protected:
    virtual Status update() noexcept override { return Status::Suspended; }
private:
    Node* childNode = nullptr;
};


class Negate : public Decorator
{
public:
    virtual const char* name() const noexcept override { return "Not"; }
    virtual void onComplete(class Scheduler& scheduler, const Node& child, Status status) override;
};

}
