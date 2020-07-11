
#include "decorators.hpp"
#include "composites.hpp"
#include "visitors.hpp"
#include "scheduler.hpp"

namespace bt
{

inline void Decorator::traverse(Visitor& visitor) const
{
    visitor.visit(*this);
    if (Composite* composite = dynamic_cast<Composite*>(childNode))
        composite->traverseChildren(visitor);
}

inline void Decorator::start(Scheduler& scheduler) noexcept
{
    scheduler.start(*childNode, *this);
}

inline void Decorator::stop(Scheduler& scheduler) noexcept
{
    if (childNode)
        scheduler.stop(*childNode);
}

inline Decorator::~Decorator()
{
    if (childNode)
    {
        childNode->~Node();
        childNode = nullptr;
    }
}

inline void Negate::onComplete(Scheduler& scheduler, const Node& child, Status status) noexcept
{
    if (status == Status::Success)
        status = Status::Failure;
    else if (status == Status::Failure)
        status = Status::Success;
    scheduler.completed(*this, status);
}

}
