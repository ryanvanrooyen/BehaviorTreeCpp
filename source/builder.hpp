
#ifndef BEHAVIOR_TREE_BUILDER_H
#define BEHAVIOR_TREE_BUILDER_H

#include <vector>
#include "nodes.hpp"
#include "decorators.hpp"
#include "composites.hpp"
#include "memory.hpp"
#include "scheduler.hpp"
#include "tree.hpp"

namespace bt
{

class Builder
{
public:
    explicit Builder(const size_t maxBytes = 1024, const size_t initSchedulerSize = 10)
        : memory(std::make_shared<Memory>(maxBytes))
        , scheduler(std::make_shared<Scheduler>(initSchedulerSize)) {}
        // , scheduler(std::shared_ptr<Scheduler>(memory->allocate<Scheduler>(initSchedulerSize))) {}

    Builder(const std::shared_ptr<Memory>& memory, const std::shared_ptr<Scheduler>& scheduler)
        : memory(memory), scheduler(scheduler) {}

    // Nodes:
    Builder& action(const char* name, ActionFunction action) { return create<Action>(name, action); }
    Builder& action(const char* name, AsyncActionFunction onStart, AsyncActionFunction onTerminate = nullptr) { return create<AsyncAction>(name, onStart, onTerminate); }
    Builder& action(const char* name, const std::shared_ptr<BehaviorTree>& tree) { return create<SubTree>(name, tree); }
    Builder&  check(const char* name, ConditionFunction check) { return create<Condition>(name, check); }

    // Composites:
    Builder& selector(uint16_t childCount) { return composite<Selector>(childCount); }
    Builder& sequence(uint16_t childCount) { return composite<Sequence>(childCount); }
    Builder& parallel(uint16_t childCount, Parallel::Policy success) { return composite<Parallel>(childCount, success); }
    Builder& parallel(uint16_t childCount, Parallel::Policy success, Parallel::Policy failure) { return composite<Parallel>(childCount, success, failure); }

    // Decorators:
    Builder& negate() { return group<Negate>(1); }

    template<typename T, typename... Args>
    Builder& create(Args&&... args) { return group<T>(0, std::forward<Args>(args)...); }

    std::shared_ptr<BehaviorTree> end();

protected:
    template<typename T, typename... Args>
    Builder& composite(uint16_t childCount, Args&&... args)
    {
        Node** children = memory->allocateArray<Node*>(childCount);
        return group<T>(childCount, children, childCount, std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    Builder& group(uint16_t childCount, Args&&... args)
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

#endif
