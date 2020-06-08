
#include "bt.h"

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
        root = node;

    if (groups.size() && groups.back().childrenLeftToAdd <= 0)
        groups.pop_back();
    if (!groups.size())
        return;

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
    }
}


std::shared_ptr<BehaviorTree> BehaviorTreeBuilder::end()
{
    if (!root)
        return nullptr;
    BehaviorTree* treePtr = memory->allocate<BehaviorTree>(root, memory);
    std::shared_ptr<BehaviorTree> tree(treePtr, [](BehaviorTree* t) { t->~BehaviorTree(); });
    root = nullptr;
    groups.clear();
    return tree;
}


std::ostream& operator<<(std::ostream& os, const BehaviorTree& tree)
{
    TextSerializer serializer(os, true);
    tree.traverse(serializer);
    return os;
}

}