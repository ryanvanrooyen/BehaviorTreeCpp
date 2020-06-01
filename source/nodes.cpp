
#include "nodes.h"
#include "visitors.h"

namespace bt
{

std::ostream& operator<<(std::ostream& os, const Status& s)
{
    if (s == Status::Success) os << "Success";
    if (s == Status::Running) os << "Running";
    if (s == Status::Failure) os << "Failure";
    if (s == Status::Invalid) os << "Invalid";
    return os;
}


void Node::traverse(NodeVisitor& visitor)
{
    visitor.visit(*this);
}


MockNode::MockNode(const char* name, std::initializer_list<Status> status) : Node(name), mockStatus{status}
{
    currentStatus = mockStatus.begin();
}


Status MockNode::update()
{
    Status status = *currentStatus;
    if (currentStatus == mockStatus.end())
        currentStatus = mockStatus.begin();
    else
        ++currentStatus;
    return status;
}

}