
#include "composites.h"
#include "visitors.h"

namespace bt
{

void Composite::traverse(BehaviorTreeVisitor& visitor) const
{
    visitor.visit(*this);
    traverseChildren(visitor);
}


void Composite::traverseChildren(BehaviorTreeVisitor& visitor) const
{
    visitor.beforeChildNodes(*this);
    for (Node* child : children) child->traverse(visitor);
    visitor.afterChildNodes(*this);
}


Composite::~Composite()
{
    for (Node* child : children)
        delete child;
    children.clear();
}


Status Sequence::update()
{
    // Keep going until a child behavior says it’s running or failed:
    while (true)
    {
        Status s = (*currentChild)->tick();
        // If child fails or keeps running, do the same:
        if (s != bt::Status::Success)
            return s;
        // Move on until we hit the end of the array:
        if (++currentChild == children.end())
            return bt::Status::Success;
    }
}


Status Selector::update()
{
    // Keep going until a child behavior says it’s running.
    while (true)
    {
        Status s = (*currentChild)->tick();
        //If child succeeds or keeps running, do the same.
        if (s != Status::Failure)
            return s;
        //Continue search for fallback until the last child.
        if (++currentChild == children.end())
            return Status::Failure;
    }
}

}
