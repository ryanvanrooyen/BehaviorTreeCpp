
#include "nodes.h"
#include "visitors.h"
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


Status SubTree::update()
{
    return tree->tick();
}


void SubTree::traverse(Visitor& visitor) const
{
    visitor.visit(*this);
}


void SubTree::traverseSubTree(Visitor& visitor) const
{
    tree->traverse(visitor);
}

}