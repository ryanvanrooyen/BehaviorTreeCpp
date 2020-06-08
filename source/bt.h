
#pragma once

#include <vector>
#include <memory>
#include "nodes.h"
#include "composites.h"
#include "decorators.h"
#include "visitors.h"
#include "memory.h"

namespace bt
{

class BehaviorTree
{
public:
    Status tick() { return root ? root->tick() : Status::Failure; }
    void traverse(Visitor& visitor) const;
    ~BehaviorTree();
    friend class Memory;
private:
    BehaviorTree(Node* root, const std::shared_ptr<Memory>& memory) : root(root), memory(memory) {}
    Node* root;
    std::shared_ptr<Memory> memory;
};

std::ostream& operator<<(std::ostream& os, const BehaviorTree& tree);


class BehaviorTreeBuilder
{
public:
    BehaviorTreeBuilder() : BehaviorTreeBuilder(1024) { }
    BehaviorTreeBuilder(const size_t maxBytes) : memory(std::make_shared<Memory>(maxBytes)) { }
    BehaviorTreeBuilder(const std::shared_ptr<Memory>& existingMemory) : memory(existingMemory) { }

    // Nodes:
    BehaviorTreeBuilder& action(const char* name, std::function<Status()> action) { return create<ActionFunction>(name, action); }
    BehaviorTreeBuilder& check(const char* name, std::function<bool()> check) { return create<Condition>(name, check); }
    BehaviorTreeBuilder& action(const char* name, const std::shared_ptr<BehaviorTree>& tree) { return create<SubTree>(name, tree); }

    // Composites:
    BehaviorTreeBuilder& selector(uint16_t childCount) { return composite<Selector>(childCount); }
    BehaviorTreeBuilder& sequence(uint16_t childCount) { return composite<Sequence>(childCount); }

    // Decorators:
    BehaviorTreeBuilder& negate() { return group<Negate>(1); }

    template<typename T, typename... Args>
    BehaviorTreeBuilder& create(Args... args) { return group<T>(0, args...); }

    std::shared_ptr<BehaviorTree> end();

protected:
    template<typename T>
    BehaviorTreeBuilder& composite(uint16_t childCount)
    {
        Node** children = memory->allocateArray<Node*>(childCount);
        return group<T>(childCount, children, childCount);
    }

    template<typename T, typename... Args>
    BehaviorTreeBuilder& group(uint16_t childCount, Args... args)
    {
        T* node = memory->allocate<T>(args...);
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
        Group(Node* parent, int children) : parent(parent), childrenLeftToAdd(children) {}
    };

    void addNode(Node* node);

    Node* root = nullptr;
    std::shared_ptr<Memory> memory;
    std::vector<Group> groups = std::vector<Group>();
};

}
