
#include "decorators.h"
#include "visitors.h"

namespace bt
{

void Decorator::traverse(Visitor& visitor) const
{
    visitor.visit(*this);
    if (Composite* composite = dynamic_cast<Composite*>(childNode))
        composite->traverseChildren(visitor);
}


Decorator::~Decorator()
{
    childNode->~Node();
    childNode = nullptr;
}


Status Negate::update()
{
    if (!childNode)
        return Status::Failure;

    Status status = childNode->tick();
    if (status == Status::Success) return Status::Failure;
    if (status == Status::Failure) return Status::Success;
    return status;
}

}
