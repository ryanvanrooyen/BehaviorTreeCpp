
#include "bt.h"

namespace bt
{

std::ostream& operator<<(std::ostream& os, const Status& s)
{
    if (s == Status::Success) os << "Success";
    if (s == Status::Running) os << "Running";
    if (s == Status::Failure) os << "Failure";
    if (s == Status::Initial) os << "Initial";
    return os;
}

void Node::traverse(Visitor& visitor) const
{
    visitor.visit(*this);
}

void SubTree::initialize(Scheduler& scheduler)
{
    if (tree && tree->root)
        scheduler.start(*tree->root, this);
}

void SubTree::onComplete(Scheduler& scheduler, const Node& node, Status status)
{
    scheduler.stop(*this, status);
}

void SubTree::traverse(Visitor& visitor) const
{
    visitor.visit(*this);
}

void SubTree::traverseSubTree(Visitor& visitor) const
{
    if (tree && tree->root)
        tree->root->traverse(visitor);
}

}