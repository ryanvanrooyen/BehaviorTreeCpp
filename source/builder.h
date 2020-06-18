
#ifndef BEHAVIOR_TREE_BUILDER_H
#define BEHAVIOR_TREE_BUILDER_H

#include <vector>
#include "nodes.h"
#include "decorators.h"
#include "composites.h"
#include "memory.h"
#include "scheduler.h"
#include "tree.h"

namespace bt
{

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
    BehaviorTreeBuilder& action(const char* name, ActionFunction action) { return create<Action>(name, action); }
    BehaviorTreeBuilder& action(const char* name, AsyncActionFunction onStart, AsyncActionFunction onTerminate = nullptr) { return create<AsyncAction>(name, onStart, onTerminate); }
    BehaviorTreeBuilder& action(const char* name, const std::shared_ptr<BehaviorTree>& tree) { return create<SubTree>(name, tree); }
    BehaviorTreeBuilder&  check(const char* name, ConditionFunction check) { return create<Condition>(name, check); }

    // Composites:
    BehaviorTreeBuilder& selector(uint16_t childCount) { return composite<Selector>(childCount); }
    BehaviorTreeBuilder& sequence(uint16_t childCount) { return composite<Sequence>(childCount); }
    BehaviorTreeBuilder& parallel(uint16_t childCount, Parallel::Policy success, Parallel::Policy failure) { return composite<Parallel>(childCount, success, failure); }

    // Decorators:
    BehaviorTreeBuilder& negate() { return group<Negate>(1); }

    template<typename T, typename... Args>
    BehaviorTreeBuilder& create(Args&&... args) { return group<T>(0, std::forward<Args>(args)...); }

    std::shared_ptr<BehaviorTree> end()
    {
        if (!root)
            return nullptr;
        if (groups.size())
            throw std::runtime_error("Invalid BehaviorTree definition. Number of child nodes does not match group node child counts.");
        BehaviorTree* treePtr = memory->allocate<BehaviorTree>(root, memory, scheduler);
        std::shared_ptr<BehaviorTree> tree(treePtr, [](BehaviorTree* t) { t->~BehaviorTree(); });
        root = nullptr;
        return tree;
    }

protected:
    template<typename T, typename... Args>
    BehaviorTreeBuilder& composite(uint16_t childCount, Args&&... args)
    {
        Node** children = memory->allocateArray<Node*>(childCount);
        return group<T>(childCount, children, childCount, std::forward<Args>(args)...);
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

    void addNode(Node* node)
    {
        if (!root)
        {
            root = node;
            return;
        }
        if (!groups.size())
            throw std::runtime_error("Invalid BehaviorTree definition. Number of child nodes does not match group node child counts.");

        Group& group = groups.back();
        if (group.parent && group.childrenLeftToAdd > 0)
        {
            if (Composite* parent = dynamic_cast<Composite*>(group.parent))
            {
                parent->addChild(node);
                group.childrenLeftToAdd -= 1;
            }
            else if (Decorator* parent = dynamic_cast<Decorator*>(group.parent))
            {
                parent->setChild(node);
                group.childrenLeftToAdd -= 1;
            }

            if (group.childrenLeftToAdd <= 0)
                groups.pop_back();
        }
    }

    Node* root = nullptr;
    std::shared_ptr<Memory> memory;
    std::shared_ptr<Scheduler> scheduler;
    std::vector<Group> groups = std::vector<Group>();
};

}

#endif
