
#include "composites.h"
#include "visitors.h"

namespace bt
{

void Composite::addChild(Node* child)
{
    if (child && currentIndex < childCount)
    {
        children[currentIndex] = child;
        ++currentIndex;
    }
}


void Composite::traverse(Visitor& visitor) const
{
    visitor.visit(*this);
    traverseChildren(visitor);
}


void Composite::traverseChildren(Visitor& visitor) const
{
    visitor.beforeChildNodes(*this);
    for (int i = 0; i < childCount; ++i)
        children[i]->traverse(visitor);
    visitor.afterChildNodes(*this);
}


Composite::~Composite()
{
    if (children)
    {
        for (int i = 0; i < childCount; ++i)
            children[i]->~Node();
        children = nullptr;
    }
}


Status Sequence::update()
{
    // Keep going until a child behavior says it’s running or failed:
    while (true)
    {
        Status s = children[currentIndex]->tick();
        // If child fails or keeps running, do the same:
        if (s != bt::Status::Success)
            return s;
        // Move on until we hit the end of the array:
        if (++currentIndex == childCount)
            return bt::Status::Success;
    }
}


Status Selector::update()
{
    // Keep going until a child behavior says it’s running.
    while (true)
    {
        Status s = children[currentIndex]->tick();
        //If child succeeds or keeps running, do the same.
        if (s != Status::Failure)
            return s;
        //Continue search for fallback until the last child.
        if (++currentIndex == childCount)
            return Status::Failure;
    }
}

}
