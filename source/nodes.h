
#ifndef BEHAVIOR_TREE_NODES_H
#define BEHAVIOR_TREE_NODES_H

#include <memory>
#include "status.h"

namespace bt
{

class Node
{
public:
    virtual const char* name() const noexcept { return "Node"; }
    Status status() const noexcept { return nodeStatus; }
    virtual void traverse(class Visitor& visitor) const;
    virtual ~Node() {}
    friend class Scheduler;
protected:
    virtual void start(class Scheduler& scheduler) noexcept {}
    virtual Status update() noexcept = 0;
    virtual void stop(class Scheduler& scheduler) noexcept {}
private:
    void tick(class Scheduler& scheduler) noexcept
    {
        if (nodeStatus != Status::Running)
            start(scheduler);
        nodeStatus = update();
        std::cout << "Tick Node: " << name() << " " << nodeStatus << std::endl;
    }
    Status nodeStatus = Status::Initial;
    class Observer* observer = nullptr;
};


class Observer
{
public:
    friend class Scheduler;
protected:
    virtual void onComplete(class Scheduler& scheduler, const Node& node, Status status) noexcept = 0;
};


class NamedNode: public Node
{
public:
    NamedNode(const char* name) : nodeName(name) {}
    virtual const char* name() const noexcept override { return nodeName; }
private:
    const char* nodeName;
};


class SubTree : public NamedNode, public Observer
{
public:
    SubTree(const char* name, const std::shared_ptr<class BehaviorTree>& tree)
        : NamedNode(name), tree(tree) {}

    virtual void traverse(Visitor& visitor) const override;
    void traverseSubTree(Visitor& visitor) const;
protected:
    virtual void start(class Scheduler& scheduler) noexcept override;
    virtual Status update() noexcept override { return Status::Suspended; }
    virtual void stop(class Scheduler& scheduler) noexcept override;
    virtual void onComplete(class Scheduler& scheduler, const Node& root, Status status) noexcept override;
private:
    std::shared_ptr<class BehaviorTree> tree;
};


typedef bool (*ConditionFunction) ();

class Condition: public NamedNode
{
public:
    Condition(const char* name, ConditionFunction check)
        : NamedNode(name), check(check) {}
protected:
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
    ConditionFunction check;
};


typedef Status (*ActionFunction) ();

class Action : public NamedNode
{
public:
    Action(const char* name, ActionFunction action)
        : NamedNode(name), action(action) {}
protected:
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
    ActionFunction action;
};


class AsyncNode: public Node
{
public:
    virtual const char* name() const noexcept override { return "Async Node"; }
    void succeeded() noexcept;
    void failed() noexcept;
protected:
    virtual void start() noexcept = 0;
    virtual void start(class Scheduler& scheduler) noexcept override;
    virtual Status update() noexcept override { return Status::Suspended; }
private:
    class Scheduler* scheduler;
};


typedef void (*AsyncActionFunction) (class AsyncAction&);

class AsyncAction : public AsyncNode
{
public:
    AsyncAction(const char* name, AsyncActionFunction onStart, AsyncActionFunction onStop = nullptr)
        : nodeName(name), onStart(onStart), onStop(onStop) {}
    virtual const char* name() const noexcept override { return nodeName; }
protected:
    virtual void start() noexcept override
    {
        try
        {
            onStart(*this);
        }
        catch (...)
        {
            failed();
        }
    }

    virtual void stop(class Scheduler& scheduler) noexcept override
    {
        try
        {
            if (onStop)
                onStop(*this);
        }
        catch (...)
        {
        }
    }
private:
    const char* nodeName;
    AsyncActionFunction onStart;
    AsyncActionFunction onStop;
};

}

#endif
