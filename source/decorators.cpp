
#include "bt.h"

namespace bt
{

void Decorator::traverse(Visitor& visitor) const
{
    visitor.visit(*this);
    if (Composite* composite = dynamic_cast<Composite*>(childNode))
        composite->traverseChildren(visitor);
}

void Decorator::initialize(Scheduler& scheduler)
{
    scheduler.start(*childNode, this);
}

Decorator::~Decorator()
{
    childNode->~Node();
    childNode = nullptr;
}

void Negate::onComplete(Scheduler& scheduler, const Node& child, Status status)
{
    if (status == Status::Success)
        status = Status::Failure;
    else if (status == Status::Failure)
        status = Status::Success;
    scheduler.stop(*this, status);
}

}
