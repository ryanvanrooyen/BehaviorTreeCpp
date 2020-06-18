
#include "composites.h"
#include "visitors.h"
#include "scheduler.h"

namespace bt
{

inline void Composite::start(Scheduler& scheduler) noexcept
{
    currentIndex = 0;
    scheduler.start(*children[0], *this);
}

inline void Composite::traverse(Visitor& visitor) const
{
    visitor.visit(*this);
    traverseChildren(visitor);
}

inline void Composite::traverseChildren(Visitor& visitor) const
{
    visitor.beforeChildNodes(*this);
    for (int i = 0; i < childCount; ++i)
        children[i]->traverse(visitor);
    visitor.afterChildNodes(*this);
}

inline void Composite::stop(Scheduler& scheduler) noexcept
{
    if (children)
    {
        for (uint16_t i = 0; i < childCount; ++i)
            if (Node* child = children[i])
                scheduler.stop(*child);
    }
}

inline void Sequence::onComplete(Scheduler& scheduler, const Node& child, Status status) noexcept
{
    if (status == Status::Failure)
    {
        scheduler.completed(*this, status);
        return;
    }

    if (++currentIndex == childCount)
        scheduler.completed(*this, Status::Success);
    else
        scheduler.start(*children[currentIndex], *this);
}

inline void Selector::onComplete(Scheduler& scheduler, const Node& child, Status status) noexcept
{
    if (status == Status::Success)
    {
        scheduler.completed(*this, status);
        return;
    }

    if (++currentIndex == childCount)
        scheduler.completed(*this, Status::Failure);
    else
        scheduler.start(*children[currentIndex], *this);
}

inline void Parallel::start(Scheduler& scheduler) noexcept
{
    successCount = 0;
    failureCount = 0;
    currentIndex = 0;

    for (uint16_t i = 0; i < childCount; ++i)
        scheduler.start(*children[i], *this);
}

inline void Parallel::onComplete(Scheduler& scheduler, const Node& child, Status status) noexcept
{
    // TODO: Handle shutting down other children in the RequireOne policy cases:
    if (status == Status::Success)
    {
        ++successCount;
        if (successPolicy == Policy::RequireOne)
        {
            stop(scheduler);  // Stop the other running children
            scheduler.completed(*this, Status::Success);
            return;
        }
    }
    else if (status == Status::Failure)
    {
        ++failureCount;
        if (failurePolicy == Policy::RequireOne)
        {
            stop(scheduler);  // Stop the other running children
            scheduler.completed(*this, Status::Failure);
            return;
        }
    }

    if (failurePolicy == Policy::RequireAll && failureCount == childCount)
        scheduler.completed(*this, Status::Failure);
    else if (successPolicy == Policy::RequireAll && successCount == childCount)
        scheduler.completed(*this, Status::Success);
}

}
