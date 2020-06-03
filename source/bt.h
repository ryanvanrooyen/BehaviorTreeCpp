
#pragma once

#include "nodes.h"
#include "composites.h"
#include "decorators.h"
#include "visitors.h"


namespace bt
{

class BehaviorTree
{
public:
    void tick() { if (root) root->tick(); }
    void traverse(BehaviorTreeVisitor& visitor) const
    {
        visitor.begin();
        if (root) root->traverse(visitor);
        visitor.end();
    }
    ~BehaviorTree();
    friend class BehaviorTreeBuilder;
private:
    BehaviorTree(Node* root) : root(root) {}
    Node* root;
};

std::ostream& operator<<(std::ostream& os, const BehaviorTree& tree);


class BehaviorTreeBuilder
{
public:
    // Nodes:
    BehaviorTreeBuilder& action(const char* name, std::function<Status()> action) { return create<Action>(name, action); }
    BehaviorTreeBuilder& check(const char* name, std::function<bool()> check) { return create<Check>(name, check); }
    BehaviorTreeBuilder& action(const char* name, BehaviorTree& tree) { return create<SubTree>(name, tree.root); }

    // Composites:
    BehaviorTreeBuilder& selector(int childCount) { return group<Selector>(childCount); }
    BehaviorTreeBuilder& sequence(int childCount) { return group<Sequence>(childCount); }

    // Decorators:
    BehaviorTreeBuilder& negate() { return group<Negate>(1); }

    template<typename T, typename... Args>
    BehaviorTreeBuilder& create(Args... args) { return group<T>(0, args...); }

    BehaviorTree* end();
    ~BehaviorTreeBuilder();

protected:
    template<typename T, typename... Args>
    BehaviorTreeBuilder& group(int childCount, Args... args)
    {
        T* node = new T(args...);
        addNode(node);
        if (childCount > 0)
            groups.push_back(new Group(node, childCount));
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
    std::vector<Group*> groups = std::vector<Group*>();
    std::vector<BehaviorTree*> createdTrees = std::vector<BehaviorTree*>();
};

}
