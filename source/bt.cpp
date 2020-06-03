
#include "bt.h"

namespace bt
{

BehaviorTree::~BehaviorTree()
{
    delete root;
    root = nullptr;
}


void BehaviorTreeBuilder::addNode(Node* node)
{
    if (!root)
        root = node;

    auto current = groups.size() ? groups.back() : nullptr;
    if (current && current->childrenLeftToAdd <= 0)
    {
        delete current;
        if (groups.size() > 0)
        {
            groups.pop_back();
            current = groups.back();
        }
        else
        {
            current = nullptr;
        }
    }

    if (current && current->parent && current->childrenLeftToAdd > 0)
    {
        if (Composite* parent = dynamic_cast<Composite*>(current->parent))
        {
            parent->addChild(node);
            current->childrenLeftToAdd -= 1;
        }
        else if (Decorator* parent = dynamic_cast<Decorator*>(current->parent))
        {
            parent->setChild(node);
            current->childrenLeftToAdd -= 1;
        }
    }
}


BehaviorTree* BehaviorTreeBuilder::end()
{
    if (!root)
        return nullptr;
    BehaviorTree* tree = new BehaviorTree(root);
    createdTrees.push_back(tree);
    root = nullptr;
    for (Group* group : groups)
        delete group;
    groups.clear();
    return tree;
}


BehaviorTreeBuilder::~BehaviorTreeBuilder()
{
    for (BehaviorTree* tree : createdTrees)
        delete tree;
    createdTrees.clear();
}


std::ostream& operator<<(std::ostream& os, const BehaviorTree& tree)
{
    Serializer serializer(os);
    tree.traverse(serializer);
    return os;
}

}