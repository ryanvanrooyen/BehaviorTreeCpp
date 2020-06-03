
#include "decorators.h"
#include "visitors.h"

namespace bt
{

void Decorator::traverse(BehaviorTreeVisitor& visitor) const
{
    visitor.visit(*this);
    if (Composite* composite = dynamic_cast<Composite*>(childNode))
        composite->traverseChildren(visitor);
}


Decorator::~Decorator()
{
    delete childNode;
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
