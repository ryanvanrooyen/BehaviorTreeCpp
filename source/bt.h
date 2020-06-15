
#pragma once

#include <vector>
#include <deque>
#include <memory>
#include <utility>
#include "nodes.h"
#include "composites.h"
#include "decorators.h"
#include "visitors.h"
#include "memory.h"

namespace bt
{

class Scheduler
{
public:
    explicit Scheduler(size_t initialSize)
        : runningNodes(std::deque<Node*>()) {}

    void tick()
    {
        // Insert an end-of-update marker into the list of tasks.
        runningNodes.push_back(nullptr);

        // Keep going updating tasks until we encounter the nullptr marker:
        while (true)
        {
            Node* current = runningNodes.front();
            runningNodes.pop_front();

            if (current == nullptr)
                return;

            current->tick(*this);

            // If currently running, drop it into the queue for next tick:
            if (current->nodeStatus == Status::Running)
            {
                runningNodes.push_back(current);
            }
            else if (current->nodeStatus != Status::Suspended)
            {
                // Notify observer if task completed:
                if (current->observer)
                    current->observer->onComplete(*this, *current, current->nodeStatus);
            }
        }
    }

    void start(Node& node, Observer* observer = nullptr)
    {
        if (observer != nullptr)
            node.observer = observer;
        runningNodes.push_front(&node);
    }

    void stop(Node& node, Status result)
    {
        node.nodeStatus = result;
        if (node.observer)
            node.observer->onComplete(*this, node, result);
    }

    bool empty() { return runningNodes.size() == 0; }
private:
    std::deque<Node*> runningNodes;
};


class BehaviorTree
{
public:
    void traverse(Visitor& visitor) const;
    void tick()
    {
        if (scheduler->empty())
            scheduler->start(*root);
        scheduler->tick();
    }
    ~BehaviorTree();
    friend class Memory;
    friend class SubTree;
private:
    BehaviorTree(Node* root,
        const std::shared_ptr<Memory>& memory,
        const std::shared_ptr<Scheduler>& scheduler)
        : root(root), memory(memory), scheduler(scheduler) {}

    Node* root;
    std::shared_ptr<Memory> memory;
    std::shared_ptr<Scheduler> scheduler;
};

std::ostream& operator<<(std::ostream& os, const BehaviorTree& tree);


class BehaviorTreeBuilder
{
public:
    explicit BehaviorTreeBuilder(const size_t maxBytes = 1024, const size_t initSchedulerSize = 10)
        : memory(std::make_shared<Memory>(maxBytes))
        , scheduler(std::make_shared<Scheduler>(initSchedulerSize)) {}
        // , scheduler(std::shared_ptr<Scheduler>(memory->allocate<Scheduler>(initSchedulerSize))) {}

    BehaviorTreeBuilder(const std::shared_ptr<Memory>& memory, const std::shared_ptr<Scheduler>& scheduler)
        : memory(memory), scheduler(scheduler) {}

    // Nodes:
    BehaviorTreeBuilder& action(const char* name, ActionPtr action) { return create<ActionFunction>(name, action); }
    BehaviorTreeBuilder& action(const char* name, const std::shared_ptr<BehaviorTree>& tree) { return create<SubTree>(name, tree); }
    BehaviorTreeBuilder& check(const char* name, ConditionPtr check) { return create<Condition>(name, check); }

    // Composites:
    BehaviorTreeBuilder& selector(uint16_t childCount) { return composite<Selector>(childCount); }
    BehaviorTreeBuilder& sequence(uint16_t childCount) { return composite<Sequence>(childCount); }

    // Decorators:
    BehaviorTreeBuilder& negate() { return group<Negate>(1); }

    template<typename T, typename... Args>
    BehaviorTreeBuilder& create(Args&&... args) { return group<T>(0, std::forward<Args>(args)...); }

    template<typename T, typename... Args>
    BehaviorTreeBuilder& create2(Args... args) { return group<T>(0, args...); }

    std::shared_ptr<BehaviorTree> end();

protected:
    template<typename T>
    BehaviorTreeBuilder& composite(uint16_t childCount)
    {
        Node** children = memory->allocateArray<Node*>(childCount);
        return group<T>(childCount, children, childCount);
    }

    template<typename T, typename... Args>
    BehaviorTreeBuilder& group(uint16_t childCount, Args&&... args)
    {
        T* node = memory->allocate<T>(std::forward<Args>(args)...);
        addNode(node);
        if (childCount > 0)
            groups.push_back(Group(node, childCount));
        return *this;
    }

private:
    struct Group
    {
        Node* parent;
        int childrenLeftToAdd;
        Group(Node* parent, int children)
            : parent(parent), childrenLeftToAdd(children) {}
    };

    void addNode(Node* node);

    Node* root = nullptr;
    std::shared_ptr<Memory> memory;
    std::shared_ptr<Scheduler> scheduler;
    std::vector<Group> groups = std::vector<Group>();
};

}
