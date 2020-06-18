
#include "nodes.h"
#include "visitors.h"
#include "scheduler.h"
#include "tree.h"

namespace bt
{

inline void Node::traverse(Visitor& visitor) const
{
    visitor.visit(*this);
}

inline void SubTree::start(Scheduler& scheduler) noexcept
{
    if (tree && tree->root)
        scheduler.start(*tree->root, *this);
}

inline void SubTree::stop(Scheduler& scheduler) noexcept
{
    if (tree && tree->root)
        scheduler.stop(*tree->root);
}

inline void SubTree::onComplete(Scheduler& scheduler, const Node& root, Status status) noexcept
{
    scheduler.completed(*this, status);
}

inline void SubTree::traverse(Visitor& visitor) const
{
    visitor.visit(*this);
}

inline void SubTree::traverseSubTree(Visitor& visitor) const
{
    if (tree && tree->root)
        tree->root->traverse(visitor);
}

inline void AsyncNode::start(class Scheduler& scheduler) noexcept
{
    this->scheduler = &scheduler;
    this->start();
}

inline void AsyncNode::succeeded() noexcept
{
    if (status() == Status::Suspended && this->scheduler)
        this->scheduler->completed(*this, Status::Success);
}

inline void AsyncNode::failed() noexcept
{
    if (status() == Status::Suspended && this->scheduler)
        this->scheduler->completed(*this, Status::Failure);
}

}
