
#pragma once

#include <memory>
#include <functional>

namespace bt
{

enum class Status
{
    Success,
    Running,
    Failure,
    Initial,
};

std::ostream& operator<<(std::ostream& os, const Status& status);


class Node
{
public:
    virtual void initialize() {}
    virtual Status update() = 0;
    virtual void terminate(Status) {}

    Status tick()
    {
        // std::cout << "Tick Node: " << name() << " " << nodeStatus << std::endl;
        if (nodeStatus != Status::Running) initialize();
        nodeStatus = update();
        if (nodeStatus != Status::Running) terminate(nodeStatus);
        return nodeStatus;
    }

    Status status() const { return nodeStatus; }
    virtual const char* name() const { return "Node"; }

    virtual void traverse(class Visitor& visitor) const;

    virtual ~Node() {}

private:
    Status nodeStatus = Status::Initial;
};


class Action: public Node
{
public:
    Action(const char* name) : nodeName(name) {}
    virtual const char* name() const override { return nodeName; }
private:
    const char* nodeName;
};


class SubTree : public Action
{
public:
    SubTree(const char* name, const std::shared_ptr<class BehaviorTree>& tree)
        : Action(name), tree(tree) {}

    virtual void traverse(Visitor& visitor) const override;
    void traverseSubTree(Visitor& visitor) const;
protected:
    virtual Status update() override;

private:
    std::shared_ptr<class BehaviorTree> tree;
};


class Condition: public Action
{
public:
    Condition(const char* name, std::function<bool()> check) : Action(name), check(check) {}
    virtual Status update() override { return check() ? Status::Success : Status::Failure; }
private:
    std::function<bool()> check;
};


class ActionFunction: public Action
{
public:
    ActionFunction(const char* name, std::function<Status()> action) : Action(name), action(action) {}
    virtual Status update() override { return action(); }
private:
    std::function<Status()> action;
};

}
