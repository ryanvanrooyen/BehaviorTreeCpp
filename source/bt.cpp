
#include "bt.h"
#include <stdexcept>

namespace bt
{

void BehaviorTree::traverse(Visitor& visitor) const
{
    visitor.begin();
    if (root)
        root->traverse(visitor);
    visitor.end();
}


BehaviorTree::~BehaviorTree()
{
    root->~Node();
    root = nullptr;
}


void BehaviorTreeBuilder::addNode(Node* node)
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


std::shared_ptr<BehaviorTree> BehaviorTreeBuilder::end()
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


std::ostream& operator<<(std::ostream& os, const BehaviorTree& tree)
{
    TextSerializer serializer(os, true);
    tree.traverse(serializer);
    return os;
}

}