
#pragma once

#include <memory>
#include <iostream>

namespace bt
{

enum class Status
{
    Initial,
    Running,
    Success,
    Failure,
    Suspended,
};

std::ostream& operator<<(std::ostream& os, const Status& status);


class Node
{
public:
    virtual void initialize(class Scheduler& scheduler) {}
    virtual Status update() noexcept = 0;

    void tick(class Scheduler& scheduler)
    {
        std::cout << "Tick Node: " << name() << " " << nodeStatus << std::endl;
        if (nodeStatus != Status::Running) initialize(scheduler);
        nodeStatus = update();
    }

    Status status() const noexcept { return nodeStatus; }
    virtual const char* name() const noexcept { return "Node"; }

    virtual void traverse(class Visitor& visitor) const;

    virtual ~Node() {}

    friend class Scheduler;
private:
    Status nodeStatus = Status::Initial;
    class Observer* observer = nullptr;
};


class Observer
{
public:
    virtual void onComplete(class Scheduler& scheduler, const Node& node, Status status) = 0;
};


class Action: public Node
{
public:
    Action(const char* name) : nodeName(name) {}
    virtual const char* name() const noexcept override { return nodeName; }
private:
    const char* nodeName;
};


class SubTree : public Action, public Observer
{
public:
    SubTree(const char* name, const std::shared_ptr<class BehaviorTree>& tree)
        : Action(name), tree(tree) {}

    virtual void initialize(class Scheduler& scheduler) override;
    virtual Status update() noexcept override { return Status::Suspended; }
    virtual void traverse(Visitor& visitor) const override;
    void traverseSubTree(Visitor& visitor) const;
    virtual void onComplete(class Scheduler& scheduler, const Node& node, Status status) override;
private:
    std::shared_ptr<class BehaviorTree> tree;
};


typedef bool (*ConditionPtr) ();

class Condition: public Action
{
public:
    Condition(const char* name, ConditionPtr check)
        : Action(name), check(check) {}
    virtual Status update() noexcept override
    {
        try
        {
            return check() ? Status::Success : Status::Failure;
        }
        catch (...)
        {
            return Status::Failure;
        }
    }
private:
    ConditionPtr check;
};


typedef Status (*ActionPtr) ();

class ActionFunction: public Action
{
public:
    ActionFunction(const char* name, ActionPtr action)
        : Action(name), action(action) {}
    virtual Status update() noexcept override
    {
        try
        {
            return action();
        }
        catch (...)
        {
            return Status::Failure;
        }
    }
private:
    ActionPtr action;
};

}
