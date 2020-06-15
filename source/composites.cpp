
#include "bt.h"

namespace bt
{

void Composite::initialize(Scheduler& scheduler)
{
    currentIndex = 0;
    scheduler.start(*children[0], this);
}

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

void Sequence::onComplete(Scheduler& scheduler, const Node& child, Status status)
{
    if (status == Status::Failure)
    {
        scheduler.stop(*this, status);
        return;
    }

    if (++currentIndex == childCount)
        scheduler.stop(*this, Status::Success);
    else
        scheduler.start(*children[currentIndex], this);
}

void Selector::onComplete(Scheduler& scheduler, const Node& child, Status status)
{
    if (status == Status::Success)
    {
        scheduler.stop(*this, status);
        return;
    }

    if (++currentIndex == childCount)
        scheduler.stop(*this, Status::Failure);
    else
        scheduler.start(*children[currentIndex], this);
}

}
