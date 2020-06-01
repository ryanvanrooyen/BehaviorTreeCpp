
#include "visitors.h"

namespace bt
{

void NodeSerializer::print(const Node& node)
{
    // out << "Printing from serializer" << std::endl;
    for (int i = 0; i < depth; i++)
        out << "\t";

    out << node.getName() << ": " << node.getStatus() << std::endl;
}


void NodeSerializer::visit(const Node& node)
{
    print(node);
}


void NodeSerializer::visit(const Composite& node)
{
    print(node);
    ++depth;
}

}
