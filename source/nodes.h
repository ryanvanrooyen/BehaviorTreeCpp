
#pragma once

#include <vector>
#include <initializer_list>
#include <iostream>
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
        if (nodeStatus != Status::Running) initialize();
        nodeStatus = update();
        if (nodeStatus != Status::Running) terminate(nodeStatus);
        return nodeStatus;
    }

    Status status() const { return nodeStatus; }
    virtual const char* name() const { return "Node"; }

    virtual void traverse(class BehaviorTreeVisitor& visitor) const;

    virtual ~Node() {}

private:
    Status nodeStatus = Status::Initial;
};


class SubTree : public Node
{
public:
    SubTree(const char* name, Node* root) : treeName(name), root(root) {}
    virtual const char* name() const override { return treeName; }

    virtual void traverse(BehaviorTreeVisitor& visitor) const override;
    void traverseSubTree(BehaviorTreeVisitor& visitor) const;

protected:
    virtual Status update() override { return root->tick(); }

private:
    const char* treeName;
    Node* root;
};


class Action: public Node
{
public:
    Action(const char* name, std::function<Status()> action) : nodeName(name), action(action) {}
    virtual Status update() override { return action(); }
    virtual const char* name() const override { return nodeName; }
private:
    const char* nodeName;
    std::function<Status()> action;
};


class Check: public Node
{
public:
    Check(const char* name, std::function<bool()> check) : nodeName(name), check(check) {}
    virtual Status update() override { return check() ? Status::Success : Status::Failure; }
    virtual const char* name() const override { return nodeName; }
private:
    const char* nodeName;
    std::function<bool()> check;
};


}
