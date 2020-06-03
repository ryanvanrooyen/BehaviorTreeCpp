
#include "nodes.h"
#include "visitors.h"

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


void Node::traverse(BehaviorTreeVisitor& visitor) const
{
    visitor.visit(*this);
}


void SubTree::traverse(BehaviorTreeVisitor& visitor) const
{
    visitor.visit(*this);
}


void SubTree::traverseSubTree(BehaviorTreeVisitor& visitor) const
{
    if (root)
        root->traverse(visitor);
}

}